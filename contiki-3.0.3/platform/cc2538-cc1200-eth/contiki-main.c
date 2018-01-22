/*
 * Copyright (c) 2015, Weptech elektronik GmbH Germany
 * http://www.weptech.de
 *
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
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \addtogroup cc2538-platforms
 *
 * @{
 *
 * \defgroup cc2538-cc1200-eth The cc2538-cc1200-eth IoT development platform
 *
 * The cc2538-cc1200-eth is a platform sold and maintained by 
 * Weptech elektronik GmbH, Germany.
 * It is based on the cc2538 SoC supplemented with a cc1200 sub-GHz transceiver
 * allowing for dual-band operation in the 2.4 and the sub-GHz bands.
 * A Microchip enc28j60 ethernet controller connected via a separate
 * SPI provides a 10BASE-T interface allowing to build low-cost 6LoWPAN 
 * gateway solutions.
 *
 * @{
 *
 * \file Main module for the cc2538-cc1200-eth platform
 */

#include "contiki.h"

#include "dev/leds.h"
#include "dev/sys-ctrl.h"
#include "dev/scb.h"
#include "dev/nvic.h"
#include "dev/uart.h"
#include "dev/watchdog.h"
#include "dev/ioc.h"
#include "dev/button-sensor.h"
#include "dev/serial-line.h"
#include "dev/slip.h"
#include "dev/cc2538-rf.h"
#include "dev/udma.h"

#include "usb/usb-serial.h"
#include "lib/random.h"
#include "net/netstack.h"
#include "net/queuebuf.h"
#include "cpu.h"
#include "reg.h"
#include "ieee-addr.h"

#include "eui48.h"

#include "lpm.h"

#include "contiki.h"
#include "dev/leds.h"
#include "dev/sys-ctrl.h"
#include "dev/scb.h"
#include "dev/nvic.h"
#include "dev/uart.h"
#include "dev/watchdog.h"
#include "dev/ioc.h"
#include "dev/button-sensor.h"
#include "dev/serial-line.h"
#include "dev/slip.h"
#include "dev/cc2538-rf.h"
#include "dev/udma.h"
#include "usb/usb-serial.h"
#include "lib/random.h"
#include "net/netstack.h"
#include "net/rime/rime.h"
#include "net/queuebuf.h"
#include "net/ip/tcpip.h"
#include "net/ip/uip.h"
#include "net/mac/frame802154.h"
#include "cpu.h"
#include "reg.h"
#include "ieee-addr.h"
#include "lpm.h"

/* For verbose startup */
//#include "cc1200-conf.h"
//#include "cc1200-rf-cfg.h"

/* #include "netstack-aes.h" */
/* #include "simple-rpl.h" */

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define STARTUP_CONF_VERBOSE 1

/*---------------------------------------------------------------------------*/
/* #if STARTUP_CONF_VERBOSE */
/* #define PRINTF(...) printf(__VA_ARGS__) */
/* #else */
/* #define PRINTF(...) */
/* #endif */

#if UART_CONF_ENABLE
#define PUTS(s) puts(s)
#else
#define PUTS(s)
#endif
/*---------------------------------------------------------------------------*/
/* Import the rf configuration set by CC1200_RF_CFG */
#if PLATFORM_USE_CC1200 && STARTUP_CONF_VERBOSE 
extern const cc1200_rf_cfg_t CC1200_RF_CFG;
#endif /* PLATFORM_USE_CC1200 */
/*---------------------------------------------------------------------------*/
static void
fade(unsigned char l)
{

  volatile int i;
  int k, j;

#if (SYS_CTRL_CONF_SYS_DIV == SYS_CTRL_CLOCK_CTRL_SYS_DIV_32MHZ)
#define FADE_DELAY 1200
#else
#define FADE_DELAY 800
#endif

  for(k = 0; k < FADE_DELAY; ++k) {
    j = k > (FADE_DELAY >> 1) ? FADE_DELAY - k : k;

    leds_on(l);
    for(i = 0; i < j; ++i) {
      asm ("nop");
    }
    leds_off(l);
    for(i = 0; i < (FADE_DELAY >> 1) - j; ++i) {
      asm ("nop");
    }
  }

}
/*---------------------------------------------------------------------------*/
static void
set_rf_params(void)
{

  uint16_t short_addr;
  uint8_t ext_addr[8];

  ieee_addr_cpy_to(ext_addr, 8);

  short_addr = ext_addr[7];
  short_addr |= ext_addr[6] << 8;

  /* Populate linkaddr_node_addr. Maintain endianness */
  memcpy(&linkaddr_node_addr, &ext_addr[8 - LINKADDR_SIZE], LINKADDR_SIZE);

#if STARTUP_CONF_VERBOSE
  {
    int i;
    printf("EUI-64 (wireless interface):\n ");
    for(i = 0; i < LINKADDR_SIZE - 1; i++) {
      printf("%02x:", linkaddr_node_addr.u8[i]);
    }
    printf("%02x\n", linkaddr_node_addr.u8[i]);
  }
#endif

#if (!PLATFORM_USE_CC1200)

  NETSTACK_RADIO.set_value(RADIO_PARAM_PAN_ID, IEEE802154_PANID);
  NETSTACK_RADIO.set_value(RADIO_PARAM_16BIT_ADDR, short_addr);
  NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, CC2538_RF_CHANNEL);
  NETSTACK_RADIO.set_object(RADIO_PARAM_64BIT_ADDR, ext_addr, 8);
  
#else
  
  /* 
   * Default parameters are already set at compile time using the 
   * configuration parameters given in cc1200-conf.h.
   * No need to update any of them here.
   */

#endif

}
/*---------------------------------------------------------------------------*/
/* 
 * All pins used by the peripherals are configured by their respective 
 * drivers.We use this function to configure the remaining ones.
 */
static void
gpio_setup(void)
{

#if 0
  
  GPIO_SOFTWARE_CONTROL(GPIO_PORT_TO_BASE(ENC28J60_RESET_N_PORT_NUM),
                        GPIO_PIN_MASK(ENC28J60_RESET_N_PIN_NUM));
  ioc_set_over(ENC28J60_RESET_N_PORT_NUM, 
               ENC28J60_RESET_N_PIN_NUM, 
               IOC_OVERRIDE_DIS);
  GPIO_SET_OUTPUT(GPIO_PORT_TO_BASE(ENC28J60_RESET_N_PORT_NUM),
                  GPIO_PIN_MASK( ENC28J60_RESET_N_PIN_NUM));

  GPIO_CLR_PIN(GPIO_PORT_TO_BASE(ENC28J60_RESET_N_PORT_NUM),
               GPIO_PIN_MASK(ENC28J60_RESET_N_PIN_NUM));

  GPIO_SET_PIN(GPIO_PORT_TO_BASE(ENC28J60_RESET_N_PORT_NUM),
               GPIO_PIN_MASK(ENC28J60_RESET_N_PIN_NUM));

  /* GPIO_CLR_PIN(GPIO_PORT_TO_BASE(ENC28J60_RESET_N_PORT_NUM), */
  /*              GPIO_PIN_MASK(ENC28J60_RESET_N_PIN_NUM)); */

#endif

#if 0

  /* CTS# output low */
  GPIO_SOFTWARE_CONTROL(GPIO_PORT_TO_BASE(FTDI_CTS_N_PORT),
                        GPIO_PIN_MASK(FTDI_CTS_N_PIN));
  ioc_set_over(FTDI_CTS_N_PORT, FTDI_CTS_N_PIN, IOC_OVERRIDE_DIS);
  GPIO_SET_OUTPUT(GPIO_PORT_TO_BASE(FTDI_CTS_N_PORT),
                  GPIO_PIN_MASK(FTDI_CTS_N_PIN));
  GPIO_CLR_PIN(GPIO_PORT_TO_BASE(FTDI_CTS_N_PORT),
               GPIO_PIN_MASK(FTDI_CTS_N_PIN));
  
  /* RTS# input */
  GPIO_SOFTWARE_CONTROL(GPIO_PORT_TO_BASE(FTDI_RTS_N_PORT),
                        GPIO_PIN_MASK(FTDI_RTS_N_PIN));
  ioc_set_over(FTDI_RTS_N_PORT, FTDI_RTS_N_PIN, IOC_OVERRIDE_DIS);
  GPIO_SET_INPUT(GPIO_PORT_TO_BASE(FTDI_RTS_N_PORT), 
                 GPIO_PIN_MASK(FTDI_RTS_N_PIN));
  
  /* CBUS0 input */
  GPIO_SOFTWARE_CONTROL(GPIO_PORT_TO_BASE(FTDI_CBUS0_PORT),
                        GPIO_PIN_MASK(FTDI_CBUS0_PIN));
  ioc_set_over(GPIO_D_NUM, FTDI_CBUS0_PIN, IOC_OVERRIDE_DIS);
  GPIO_SET_INPUT(GPIO_PORT_TO_BASE(FTDI_CBUS0_PORT), 
                 GPIO_PIN_MASK(FTDI_CBUS0_PIN));

#endif

} /* gpio_setup(), uk 22.05.2015 */
/*---------------------------------------------------------------------------*/
/**
 * \brief Main routine for the cc2538dk platform
 */
int
main(void)
{

  nvic_init();
  ioc_init();
  sys_ctrl_init();
  clock_init();
  lpm_init();
  rtimer_init();
  gpio_init();

  /* Configure unused pins */
  gpio_setup();

  leds_init();
  fade(LEDS_RED);

  process_init();

  watchdog_init();
  /* button_sensor_init(); */

  /*
   * Character I/O Initialisation.
   * When the UART receives a character it will call serial_line_input_byte to
   * notify the core. The same applies for the USB driver.
   *
   * If slip-arch is also linked in afterwards (e.g. if we are a border router)
   * it will overwrite one of the two peripheral input callbacks. Characters
   * received over the relevant peripheral will be handled by
   * slip_input_byte instead
   */
#if UART_CONF_ENABLE
  uart_init(0);
  /* uart_init(1); */
  uart_set_input(SERIAL_LINE_CONF_UART, serial_line_input_byte);
#endif

#if USB_SERIAL_CONF_ENABLE
  usb_serial_init();
  usb_serial_set_input(serial_line_input_byte);
#endif

  serial_line_init();

  INTERRUPTS_ENABLE();
  fade(LEDS_YELLOW);

  PUTS(CONTIKI_VERSION_STRING);
  PUTS(BOARD_STRING);

#if STARTUP_CONF_VERBOSE 

  printf(" Net       : ");
  printf("%s\n", NETSTACK_NETWORK.name);
  printf(" MAC       : ");
  printf("%s\n", NETSTACK_MAC.name);
  printf(" RDC       : ");
  printf("%s\n", NETSTACK_RDC.name);

#if PLATFORM_USE_CC1200 
  printf(" RF channel: ");
  printf("%d\n", CC1200_DEFAULT_CHANNEL);
  printf(" RF cfg    : ");
  printf("%s\n",  CC1200_RF_CFG.cfg_descriptor);
#endif  /* PLATFORM_CONF_USE_CC1200 */

#endif  /* STARTUP_CONF_VERBOSE */

  /* Initialise the H/W RNG engine. */
  random_init(0);

  udma_init();

  process_start(&etimer_process, NULL);
  ctimer_init();

  set_rf_params();
  netstack_init();

#if NETSTACK_CONF_WITH_IPV6
  memcpy(&uip_lladdr.addr, &linkaddr_node_addr, sizeof(uip_lladdr.addr));
  queuebuf_init();
  process_start(&tcpip_process, NULL);
#endif /* NETSTACK_CONF_WITH_IPV6 */

  /* Start the sensor process */
  process_start(&sensors_process, NULL);

  energest_init();
  ENERGEST_ON(ENERGEST_TYPE_CPU);

  fade(LEDS_GREEN);

  autostart_start(autostart_processes);

  watchdog_start();

  while(1) {
    uint8_t r;
    do {
      /* Reset watchdog and handle polls and events */
      watchdog_periodic();
      r = process_run();
    } while(r > 0);

    /* We have serviced all pending events. Enter a Low-Power mode. */
    lpm_enter();
  }

}
/*---------------------------------------------------------------------------*/

/**
 * @}
 * @}
 */
