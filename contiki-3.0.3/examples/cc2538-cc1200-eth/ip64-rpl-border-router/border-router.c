/*
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
 *
 * This file is part of the Contiki operating system.
 *
 */
/**
 * \file
 *         border-router
 * \author
 *         Niclas Finne <nfi@sics.se>
 *         Joakim Eriksson <joakime@sics.se>
 *         Nicolas Tsiftes <nvt@sics.se>
 */

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/rpl/rpl.h"

#include "ip64.h"
/* Statistics only */
#include "ip64-addrmap.h"


#include "net/netstack.h"
#include "dev/button-sensor.h"
#include "dev/slip.h"
#include "dev/leds.h"

#include "stats.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

#define PUSH_MSG_PORT           61616
#define PUSH_MSG_BUFSIZE        400
/*---------------------------------------------------------------------------*/
#define ADD(...)                                                        \
  do                                                                    \
  {                                                                     \
    push_msg_len += snprintf((char*)&push_msg_buf[push_msg_len],        \
                             PUSH_MSG_BUFSIZE - push_msg_len, __VA_ARGS__); \
  } while(0)
/*---------------------------------------------------------------------------*/
static uint8_t push_msg_buf[PUSH_MSG_BUFSIZE];
static int push_msg_len;
static struct udp_socket udp_socket;
static uint32_t pkt_no;
static uip_ipaddr_t ripaddr;
/*---------------------------------------------------------------------------*/
/* Various configuration parameters */
/* The interval at which we toggle the "operating" LED (in seconds)*/
#define BLINK_INTERVAL                  2
/* The interval we use to dump statistics. Set to 0 to turn this feature off */
#define STAT_INTERVAL                   10
/*---------------------------------------------------------------------------*/

#define DEBUG_LEVEL_OFF                 0
#define DEBUG_LEVEL_ERROR               1
#define DEBUG_LEVEL_WARNING             2
#define DEBUG_LEVEL_INFO                3

#define DEBUG_LEVEL                     DEBUG_LEVEL_INFO
/*---------------------------------------------------------------------------*/
#if DEBUG_LEVEL >= DEBUG_LEVEL_ERROR
#define ERROR(...)                      printf(__VA_ARGS__)
#else
#define ERROR(...)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_WARNING
#define WARNING(...)                    printf(__VA_ARGS__)
#else
#define WARNING(...)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_INFO
#define INFO(...)                       printf(__VA_ARGS__)
#else
#define INFO(...)
#endif
/*---------------------------------------------------------------------------*/


/* static uip_ipaddr_t prefix; */
/* static uint8_t ip4addr_set; */

/* The Contiki prefix */
static const uint8_t contiki_pfx[8] = {0xfd,0x4d,0x42,0x67,0x5f,0x8c,0,0};

#if STAT_INTERVAL
static int stat_prescaler;
#endif

PROCESS(border_router_process, "Border router process");

#if WEBSERVER==0
/* No webserver */
AUTOSTART_PROCESSES(&border_router_process);
#elif WEBSERVER>1
/* Use an external webserver application */
#include "webserver-nogui.h"
AUTOSTART_PROCESSES(&border_router_process,&webserver_nogui_process);
#else
/* Use simple webserver with only one page for minimum footprint.
 * Multiple connections can result in interleaved tcp segments since
 * a single static buffer is used for all segments.
 */
#include "httpd-simple.h"
/* The internal webserver can provide additional information if
 * enough program flash is available.
 */
#define WEBSERVER_CONF_LOADTIME 0
#define WEBSERVER_CONF_FILESTATS 1
#define WEBSERVER_CONF_NEIGHBOR_STATUS 1
/* Adding links requires a larger RAM buffer. To avoid static allocation
 * the stack can be used for formatting; however tcp retransmissions
 * and multiple connections can result in garbled segments.
 * TODO:use PSOCk_GENERATOR_SEND and tcp state storage to fix this.
 */
#define WEBSERVER_CONF_ROUTE_LINKS 0
#if WEBSERVER_CONF_ROUTE_LINKS
#define BUF_USES_STACK 1
#endif

PROCESS(webserver_nogui_process, "Web server");
PROCESS_THREAD(webserver_nogui_process, ev, data)
{
  PROCESS_BEGIN();

  httpd_init();

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
    httpd_appcall(data);
  }

  PROCESS_END();
}
AUTOSTART_PROCESSES(&border_router_process,&webserver_nogui_process);

static const char *TOP = "<html><head><title>ContikiRPL</title></head><body>\n";
static const char *BOTTOM = "</body></html>\n";
#if BUF_USES_STACK
static char *bufptr, *bufend;
#define ADD(...) do {                                                   \
    bufptr += snprintf(bufptr, bufend - bufptr, __VA_ARGS__);      \
  } while(0)
#else
static char buf[256];
static int blen;
#define ADD(...) do {                                                   \
    blen += snprintf(&buf[blen], sizeof(buf) - blen, __VA_ARGS__);      \
  } while(0)
#endif

/*---------------------------------------------------------------------------*/
static void
ipaddr_add(const uip_ipaddr_t *addr)
{
  uint16_t a;
  int i, f;
  for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
    a = (addr->u8[i] << 8) + addr->u8[i + 1];
    if(a == 0 && f >= 0) {
      if(f++ == 0) ADD("::");
    } else {
      if(f > 0) {
        f = -1;
      } else if(i > 0) {
        ADD(":");
      }
      ADD("%x", a);
    }
  }
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(generate_routes(struct httpd_state *s))
{
  static uip_ds6_route_t *r;
  static uip_ds6_nbr_t *nbr;
#if BUF_USES_STACK
  char buf[256];
#endif
#if WEBSERVER_CONF_LOADTIME
  static clock_time_t numticks;
  numticks = clock_time();
#endif

  PSOCK_BEGIN(&s->sout);

  SEND_STRING(&s->sout, TOP);
#if BUF_USES_STACK
  bufptr = buf;bufend=bufptr+sizeof(buf);
#else
  blen = 0;
#endif
  ADD("Neighbors<pre>");

  for(nbr = nbr_table_head(ds6_neighbors);
      nbr != NULL;
      nbr = nbr_table_next(ds6_neighbors, nbr)) {

#if WEBSERVER_CONF_NEIGHBOR_STATUS
#if BUF_USES_STACK
{char* j=bufptr+25;
      ipaddr_add(&nbr->ipaddr);
      while (bufptr < j) ADD(" ");
      switch (nbr->state) {
      case NBR_INCOMPLETE: ADD(" INCOMPLETE");break;
      case NBR_REACHABLE: ADD(" REACHABLE");break;
      case NBR_STALE: ADD(" STALE");break;
      case NBR_DELAY: ADD(" DELAY");break;
      case NBR_PROBE: ADD(" NBR_PROBE");break;
      }
}
#else
{uint8_t j=blen+25;
      ipaddr_add(&nbr->ipaddr);
      while (blen < j) ADD(" ");
      switch (nbr->state) {
      case NBR_INCOMPLETE: ADD(" INCOMPLETE");break;
      case NBR_REACHABLE: ADD(" REACHABLE");break;
      case NBR_STALE: ADD(" STALE");break;
      case NBR_DELAY: ADD(" DELAY");break;
      case NBR_PROBE: ADD(" NBR_PROBE");break;
      }
}
#endif
#else
      ipaddr_add(&nbr->ipaddr);
#endif

      ADD("\n");
#if BUF_USES_STACK
      if(bufptr > bufend - 45) {
        SEND_STRING(&s->sout, buf);
        bufptr = buf; bufend = bufptr + sizeof(buf);
      }
#else
      if(blen > sizeof(buf) - 45) {
        SEND_STRING(&s->sout, buf);
        blen = 0;
      }
#endif
  }
  ADD("</pre>Routes<pre>");
  SEND_STRING(&s->sout, buf);
#if BUF_USES_STACK
  bufptr = buf; bufend = bufptr + sizeof(buf);
#else
  blen = 0;
#endif

  for(r = uip_ds6_route_head(); r != NULL; r = uip_ds6_route_next(r)) {

#if BUF_USES_STACK
#if WEBSERVER_CONF_ROUTE_LINKS
    ADD("<a href=http://[");
    ipaddr_add(&r->ipaddr);
    ADD("]/status.shtml>");
    ipaddr_add(&r->ipaddr);
    ADD("</a>");
#else
    ipaddr_add(&r->ipaddr);
#endif
#else
#if WEBSERVER_CONF_ROUTE_LINKS
    ADD("<a href=http://[");
    ipaddr_add(&r->ipaddr);
    ADD("]/status.shtml>");
    SEND_STRING(&s->sout, buf); //TODO: why tunslip6 needs an output here, wpcapslip does not
    blen = 0;
    ipaddr_add(&r->ipaddr);
    ADD("</a>");
#else
    ipaddr_add(&r->ipaddr);
#endif
#endif
    ADD("/%u (via ", r->length);
    ipaddr_add(uip_ds6_route_nexthop(r));
    if(1 || (r->state.lifetime < 600)) {
      ADD(") %lus\n", (unsigned long)r->state.lifetime);
    } else {
      ADD(")\n");
    }
    SEND_STRING(&s->sout, buf);
#if BUF_USES_STACK
    bufptr = buf; bufend = bufptr + sizeof(buf);
#else
    blen = 0;
#endif
  }
  ADD("</pre>");

#if WEBSERVER_CONF_FILESTATS
  static uint16_t numtimes;
  ADD("<br><i>This page sent %u times</i>",++numtimes);
#endif

#if WEBSERVER_CONF_LOADTIME
  numticks = clock_time() - numticks + 1;
  ADD(" <i>(%u.%02u sec)</i>",numticks/CLOCK_SECOND,(100*(numticks%CLOCK_SECOND))/CLOCK_SECOND));
#endif

  SEND_STRING(&s->sout, buf);
  SEND_STRING(&s->sout, BOTTOM);

  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
httpd_simple_script_t
httpd_simple_get_script(const char *name)
{

  return generate_routes;
}

#endif /* WEBSERVER */

/*---------------------------------------------------------------------------*/
static void
print_local_addresses(void)
{

  int i;
  uint8_t state;

  uip_ipaddr_t addr;

#if 0

  addr.u8[0] = 0x00;
  addr.u8[1] = 0x64;
  addr.u8[2] = 0xff;
  addr.u8[3] = 0x9b;

  addr.u8[4] = 0x00;
  addr.u8[5] = 0x00;
  addr.u8[6] = 0x00;
  addr.u8[7] = 0x00;

  addr.u8[8] = 0x00;
  addr.u8[9] = 0x00;
  addr.u8[10] = 0x00;
  addr.u8[11] = 0x00;

#else

  addr.u8[0] = 0;
  addr.u8[1] = 0;
  addr.u8[2] = 0;
  addr.u8[3] = 0;

  addr.u8[4] = 0;
  addr.u8[5] = 0;
  addr.u8[6] = 0;
  addr.u8[7] = 0;

  addr.u8[8] = 0;
  addr.u8[9] = 0;
  addr.u8[10] = 0xff;
  addr.u8[11] = 0xff;

#endif

  addr.u8[12] = 192;
  addr.u8[13] = 168;
  addr.u8[14] = 1;
  addr.u8[15] = 1;

  memcpy((void*)&ripaddr,
         (const void*)&addr,
         sizeof(uip_ipaddr_t));
    
  printf("IPv6 addresses:\n");
  
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      printf(" ");
      uip_debug_ipaddr_print(&uip_ds6_if.addr_list[i].ipaddr);
      printf("\n");
    }
  }

}
/*---------------------------------------------------------------------------*/
static void
create_rpl_dag(void)
{

  rpl_dag_t *dag;
  uip_ipaddr_t ipaddr;
  uip_ipaddr_t prefix;

  memset(&prefix, 0, 16);
  memcpy(&prefix, contiki_pfx, 8);
  memcpy(&ipaddr, contiki_pfx, 16);

  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

  dag = rpl_set_root(RPL_DEFAULT_INSTANCE, &ipaddr);
  if(dag != NULL) {
    rpl_set_prefix(dag, &prefix, 64);
    INFO("Created a new RPL dag using prefix:\n ");
    uip_debug_ipaddr_print(&prefix);
    INFO("\n");
  } else {
    ERROR("Failed to create a new RPL dag!\n");
  }

}
/*---------------------------------------------------------------------------*/
static void
ip64_print_stats(void)
{

  int i;

/* struct ip64_addrmap_entry { */
/*   struct ip64_addrmap_entry *next; */
/*   struct timer timer; */
/*   uip_ip6addr_t ip6addr; */
/*   uip_ip4addr_t ip4addr; */
/*   uint32_t ip6to4, ip4to6; */
/*   uint16_t mapped_port; */
/*   uint16_t ip6port; */
/*   uint16_t ip4port; */
/*   uint8_t protocol; */
/*   uint8_t flags; */
/* }; */

  struct ip64_addrmap_entry *m;

  m = ip64_addrmap_list();
  while(m != NULL) {
    i++;
    m = list_item_next(m);
  }
    
}
/*---------------------------------------------------------------------------*/
static void
generate_push_msg(void)
{
  push_msg_len = 0;

  ADD("{\n");

  ADD("\"pkt_no\":%lu,\n",
     ++pkt_no);

  ADD("\"ut\":{%lu,\"s\"},\n",
     clock_seconds());

  ADD("}\n");

  PRINTF("push: Sending %d bytes\n", push_msg_len);
  PRINTF("push:\n%s", push_msg_buf);

} /* generate_push_msg() */
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(border_router_process, ev, data)
{

  static struct etimer et;

  PROCESS_BEGIN();

  /* 
   * While waiting for the IPv4 address to be assigned via DHCP, the future
   * border router can join an existing DAG as a parent or child, 
   * or acquire a default router that will later take precedence over the 
   * fallback interface.
   * Prevent that by turning the radio off until we are initialized as a DAG 
   * root.
   */

  NETSTACK_MAC.off(0);

  /* 
   * Initialize the IP64 module so we'll start translating packets.
   * If IP64_CONF_DHCP is set, we will first try to optain an IPv4 
   * address via DHCP.
   */

#if 1

  ip64_init();

#if !IP64_CONF_DHCP
#error You have to set IP64_CONF_DHCP in order to obtain an IPv4 address! 
#endif

  /* Wait for IPv4 address to be assigned via DHCP */
  INFO("Requesting IPv4 address");
  for(;;) {

    if (ip64_hostaddr_is_configured()) {

      const uip_ip4addr_t *hostaddr = ip64_get_hostaddr();
      const uip_ip4addr_t *netmask = ip64_get_netmask();
      const uip_ip4addr_t *draddr = ip64_get_draddr();

      leds_off(LEDS_YELLOW);
      leds_off(LEDS_GREEN);
      leds_off(LEDS_RED);

      INFO("assigned via DHCP:\n");
      INFO(" IPv4 address  : %d.%d.%d.%d\n",
           hostaddr->u8[0], hostaddr->u8[1],
           hostaddr->u8[2], hostaddr->u8[3]);
      INFO(" netmask       : %d.%d.%d.%d\n",
           netmask->u8[0], netmask->u8[1],
           netmask->u8[2], netmask->u8[3]);
      INFO(" default router: %d.%d.%d.%d\n",
           draddr->u8[0], draddr->u8[1],
           draddr->u8[2], draddr->u8[3]);

      break;

    }

    INFO(".");
    leds_toggle(LEDS_YELLOW);
    leds_toggle(LEDS_GREEN);
    leds_toggle(LEDS_RED);

    etimer_set(&et, CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

  }

#endif

  /* Now turn the radio on, but disable radio duty cycling */
  NETSTACK_MAC.off(1);

  create_rpl_dag();

#if DEBUG_LEVEL >= DEBUG_LEVEL_INFO
  print_local_addresses();
#endif

  INFO("RPL border router up and running\n");

  udp_socket_register(&udp_socket, NULL, NULL);

  while(1) {

    PROCESS_YIELD();

    /* Handle BTN1 */
    if (ev == sensors_event && data == &button_btn1_sensor) {
      INFO("Initiating global repair\n");
      rpl_repair_root(RPL_DEFAULT_INSTANCE);
    }
    
    /* "Heartbeat" indication */
    if (ev == PROCESS_EVENT_TIMER) {

      etimer_set(&et, BLINK_INTERVAL * CLOCK_SECOND);

      leds_on(LEDS_YELLOW);
      clock_delay_usec(5000);
      leds_off(LEDS_YELLOW);
      
#if STAT_INTERVAL

      if (!stat_prescaler) {

        generate_push_msg();
        if (udp_socket_connect(&udp_socket,
                               &ripaddr,
                               PUSH_MSG_PORT) == 1) {
        
          printf("Pushing to: ");
          uip_debug_ipaddr_print(&ripaddr);
          printf("\n");
          
          udp_socket_send(&udp_socket,
                          (void const*)push_msg_buf,
                          push_msg_len);
          
        }

        printf("Usage statistics:\n");
        printf(" uptime         : %lu\n", clock_seconds());
        printf(" neighbors      : %i\n",  uip_ds6_nbr_num());
        printf(" routes         : %i\n",  uip_ds6_route_num_routes());

#if RIMESTATS_CONF_ENABLED   

        printf(" contentiondrop : %lu\n",  RIMESTATS_GET(contentiondrop));
        printf(" lltx           : %lu\n",  RIMESTATS_GET(lltx));
        printf(" llrx           : %lu\n",  RIMESTATS_GET(llrx));
        printf(" badcrc         : %lu\n",  RIMESTATS_GET(badcrc));
        printf(" toolong        : %lu\n",  RIMESTATS_GET(toolong));
        printf(" tooshort       : %lu\n",  RIMESTATS_GET(tooshort));

#endif

#if 1
        ip64_print_stats();
#endif

#if PLATFORM_CONF_STATS        
        printf(" eth_tx_pkts    : %lu\n", STATS_GET(eth_tx_pkts));
        printf(" eth_rx_pkts    : %lu\n", STATS_GET(eth_rx_pkts));
        printf(" eth_tx_bytes   : %lu\n", STATS_GET(eth_tx_bytes));
        printf(" eth_rx_bytes   : %lu\n", STATS_GET(eth_rx_bytes));
#endif

        stat_prescaler = STAT_INTERVAL / BLINK_INTERVAL;


      } 
      stat_prescaler--;

#endif /* STAT_INTERVAL */

    } /* Timer event */

  } /* while() */

  PROCESS_END();

}
/*---------------------------------------------------------------------------*/
