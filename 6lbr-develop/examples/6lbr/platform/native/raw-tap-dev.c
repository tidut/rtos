/*
 *  Copyright (c) 2013, CETIC.
 * Copyright (c) 2011, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \author
 *         Niclas Finne <nfi@sics.se>
 *         Joakim Eriksson <joakime@sics.se>
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "TAP"

#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#ifdef linux
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
//#include <net/if.h>  <- conflict with linux/if.h
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/ether.h>
#include <linux/if_packet.h>
#include <linux/if.h>
#include <linux/if_tun.h>
struct ifreq if_idx;
#endif

#ifdef __APPLE__
#include <net/if_dl.h>
#include <ifaddrs.h>
#endif

#include "net/netstack.h"
#include "net/packetbuf.h"
#include "eth-drv.h"
#include "raw-tap-dev.h"
#include "cetic-6lbr.h"
#include "slip-config.h"
#include "log-6lbr.h"

//Temporary, should be removed
#include "native-slip.h"
#include "native-rdc.h"
extern int slipfd;
extern void slip_flushbuf(int fd);
//End of temporary

extern void cetic_6lbr_clear_ip(void);

#ifndef __CYGWIN__
static int tunfd;

static int set_fd(fd_set * rset, fd_set * wset);
static void handle_fd(fd_set * rset, fd_set * wset);
static const struct select_callback tun_select_callback = {
  set_fd,
  handle_fd
};
#endif /* __CYGWIN__ */

int ssystem(const char *fmt, ...)
  __attribute__ ((__format__(__printf__, 1, 2)));

int
ssystem(const char *fmt, ...)
{
  char cmd[128];
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(cmd, sizeof(cmd), fmt, ap);
  va_end(ap);
  fflush(stdout);
  return system(cmd);
}

/*---------------------------------------------------------------------------*/
void
cleanup(void)
{
  if(slip_config_ifdown_script != NULL) {
    if(access(slip_config_ifdown_script, R_OK | X_OK) == 0) {
      LOG6LBR_INFO("Running 6lbr-ifdown script '%s'\n", slip_config_ifdown_script);
      int status = ssystem("%s %s %s 2>&1", slip_config_ifdown_script,
              use_raw_ethernet ? "raw" : "tap", slip_config_tundev);
      if(status != 0) {
        LOG6LBR_ERROR("6lbr-ifdown script returned an error\n");
      }
    } else {
      LOG6LBR_ERROR("Could not access %s : %s\n", slip_config_ifdown_script,
              strerror(errno));
    }
  } else {
    LOG6LBR_INFO("No 6lbr-ifdown script specified\n");
  }
  cetic_6lbr_clear_ip();
#if !CETIC_6LBR_ONE_ITF
  slip_set_mac(&linkaddr_null);
  slip_flushbuf(slipfd);
#endif
}

/*---------------------------------------------------------------------------*/
void
sigcleanup(int signo)
{
  LOG6LBR_FATAL("signal %d\n", signo);
  exit(1);                      /* exit(0) will call cleanup() */
}

/*---------------------------------------------------------------------------*/
void
ifconf(const char *tundev)
{
  if(slip_config_ifup_script != NULL) {
    if(access(slip_config_ifup_script, R_OK | X_OK) == 0) {
      LOG6LBR_INFO("Running 6lbr-ifup script '%s'\n", slip_config_ifup_script);
      int status = ssystem("%s %s %s 2>&1", slip_config_ifup_script,
              use_raw_ethernet ? "raw" : "tap", slip_config_tundev);
      if(status != 0) {
        LOG6LBR_FATAL("6lbr-ifup script returned an error, aborting...\n");
        exit(1);
      }
    } else {
      LOG6LBR_ERROR("Could not access %s : %s\n", slip_config_ifup_script,
              strerror(errno));
    }
  } else {
    LOG6LBR_INFO("No 6lbr-up script specified\n");
  }
}
/*---------------------------------------------------------------------------*/
int
devopen(const char *dev, int flags)
{
  char t[32];

  strcpy(t, "/dev/");
  strncat(t, dev, sizeof(t) - 5);
  return open(t, flags);
}
/*---------------------------------------------------------------------------*/

#ifdef linux
int
eth_alloc(const char *tundev)
{
  int sockfd;

  if((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
    LOG6LBR_FATAL("socket() : %s\n", strerror(errno));
    exit(1);
  }
  memset(&if_idx, 0, sizeof(struct ifreq));
  strncpy(if_idx.ifr_name, tundev, IFNAMSIZ - 1);
  if(ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0) {
    LOG6LBR_FATAL("ioctl() : %s\n", strerror(errno));
    exit(1);
  }
  struct sockaddr_ll sll;

  sll.sll_family = AF_PACKET;
  sll.sll_ifindex = if_idx.ifr_ifindex;
  sll.sll_protocol = htons(ETH_P_ALL);
  if(bind(sockfd, (struct sockaddr *)&sll, sizeof(sll)) < 0) {
    LOG6LBR_FATAL("bind() : %s\n", strerror(errno));
    exit(1);
  }
  struct packet_mreq mr;

  memset(&mr, 0, sizeof(mr));
  mr.mr_ifindex = if_idx.ifr_ifindex;
  mr.mr_type = PACKET_MR_PROMISC;
  if(setsockopt(sockfd, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) <
     0) {
    LOG6LBR_FATAL("setsockopt() : %s\n", strerror(errno));
    exit(1);
  }
  return sockfd;
}

int
tun_alloc(char *dev)
{
  struct ifreq ifr;
  int fd, err;

  if((fd = open("/dev/net/tun", O_RDWR)) < 0) {
    return -1;
  }

  memset(&ifr, 0, sizeof(ifr));

  /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
   *        IFF_NO_PI - Do not provide packet information
   */
  ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
  if(*dev != 0)
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);

  if((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
    close(fd);
    return err;
  }
  strcpy(dev, ifr.ifr_name);
  return fd;
}

void
fetch_mac(int fd, char *dev, ethaddr_t * eth_mac_addr)
{
  struct ifreq buffer;

  memset(&buffer, 0x00, sizeof(buffer));
  strcpy(buffer.ifr_name, dev);
  ioctl(fd, SIOCGIFHWADDR, &buffer);
  memcpy(eth_mac_addr, buffer.ifr_hwaddr.sa_data, 6);
}
#else
int
eth_alloc(const char *tundev)
{
  LOG6LBR_FATAL("RAW Ethernet mode not supported\n");
  exit(1);
}
int
tun_alloc(char *dev)
{
  return devopen(dev, O_RDWR);
}
void
fetch_mac(int fd, char *dev, ethaddr_t * eth_mac_addr)
{
  struct ifaddrs *ifap, *ifaptr;
  unsigned char *ptr;
  int found = 0;

  if(getifaddrs(&ifap) == 0) {
    for(ifaptr = ifap; ifaptr != NULL; ifaptr = (ifaptr)->ifa_next) {
      if(!strcmp((ifaptr)->ifa_name, dev)
         && (((ifaptr)->ifa_addr)->sa_family == AF_LINK)) {
        ptr =
          (unsigned char *)LLADDR((struct sockaddr_dl *)(ifaptr)->ifa_addr);
        memcpy(eth_mac_addr, ptr, 6);
        //(*eth_mac_addr)[5] = 7;
        found = 1;
        break;
      }
    }
    freeifaddrs(ifap);
  }
  if(!found) {
    LOG6LBR_FATAL("Could not find mac address for %s\n", dev);
    exit(1);
  }
}
#endif

#ifdef __CYGWIN__
/*wpcap process is used to connect to host interface */
void
tun_init()
{
  setvbuf(stdout, NULL, _IOLBF, 0);     /* Line buffered output. */

  slip_init();
}

#else

static uint16_t delaymsec = 0;
static uint32_t delaystartsec, delaystartmsec;

/*---------------------------------------------------------------------------*/

void
tun_init()
{
  setvbuf(stdout, NULL, _IOLBF, 0);     /* Line buffered output. */

  if(use_raw_ethernet) {
    tunfd = eth_alloc(slip_config_tundev);
  } else {
    tunfd = tun_alloc(slip_config_tundev);
  }
  if(tunfd == -1) {
    LOG6LBR_FATAL("tun_alloc() : %s\n", strerror(errno));
    exit(1);
  }

  select_set_callback(tunfd, &tun_select_callback);

  LOG6LBR_INFO("opened device /dev/%s\n", slip_config_tundev);

  atexit(cleanup);
  signal(SIGHUP, sigcleanup);
  signal(SIGTERM, sigcleanup);
  signal(SIGINT, sigcleanup);
  ifconf(slip_config_tundev);
#if !CETIC_6LBR_ONE_ITF
  if(use_raw_ethernet) {
#endif
    fetch_mac(tunfd, slip_config_tundev, &eth_mac_addr);
    LOG6LBR_ETHADDR(INFO, &eth_mac_addr, "Eth MAC address : ");
    eth_mac_addr_ready = 1;
#if !CETIC_6LBR_ONE_ITF
  }
#endif
}

/*---------------------------------------------------------------------------*/
void
tun_output(uint8_t * data, int len)
{
  if(write(tunfd, data, len) != len) {
    LOG6LBR_FATAL("write() : %s\n", strerror(errno));
    exit(1);
  }
  LOG6LBR_PRINTF(PACKET, TAP_OUT, "write: %d\n", len);
}
/*---------------------------------------------------------------------------*/
int
tun_input(unsigned char *data, int maxlen)
{
  int size;

  if((size = read(tunfd, data, maxlen)) == -1) {
    LOG6LBR_FATAL("read() : %s\n", strerror(errno));
    exit(1);
  }
  LOG6LBR_PRINTF(PACKET, TAP_IN, "read: %d\n", size);
  return size;
}

/*---------------------------------------------------------------------------*/
/* tun and slip select callback                                              */
/*---------------------------------------------------------------------------*/
static int
set_fd(fd_set * rset, fd_set * wset)
{
  FD_SET(tunfd, rset);
  return 1;
}

/*---------------------------------------------------------------------------*/

static void
handle_fd(fd_set * rset, fd_set * wset)
{
  /* Optional delay between outgoing packets */
  /* Base delay times number of 6lowpan fragments to be sent */
  /* delaymsec = 10; */
  if(delaymsec) {
    struct timeval tv;
    int dmsec;

    gettimeofday(&tv, NULL);
    dmsec =
      (tv.tv_sec - delaystartsec) * 1000 + tv.tv_usec / 1000 - delaystartmsec;
    if(dmsec < 0)
      delaymsec = 0;
    if(dmsec > delaymsec)
      delaymsec = 0;
  }

  if(delaymsec == 0) {
    int size;

    if(FD_ISSET(tunfd, rset)) {
      size = tun_input(ethernet_tmp_buf, ETHERNET_TMP_BUF_SIZE);
      eth_drv_input(ethernet_tmp_buf, size);

      if(slip_config_basedelay) {
        struct timeval tv;

        gettimeofday(&tv, NULL);
        delaymsec = slip_config_basedelay;
        delaystartsec = tv.tv_sec;
        delaystartmsec = tv.tv_usec / 1000;
      }
    }
  }
}
#endif /*  __CYGWIN_ */

/*---------------------------------------------------------------------------*/
