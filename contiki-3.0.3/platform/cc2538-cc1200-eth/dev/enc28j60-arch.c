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
 *
 * This file is part of the Contiki operating system.
 */

#include "enc28j60-arch.h"

#include "spi-arch.h"
#include "dev/gpio.h"
#include "dev/ioc.h"

#include <string.h>

/*---------------------------------------------------------------------------*/
/* Determine the vector to be used for the INT_N callback function */
#if (ENC28J60_INT_N_PORT_NUM == GPIO_A_NUM)
#define ENC28J60_INT_N_PORT_VECTOR    NVIC_INT_GPIO_PORT_A
#elif (ENC28J60_INT_N_PORT_NUM == GPIO_B_NUM)
#define ENC28J60_INT_N_PORT_VECTOR    NVIC_INT_GPIO_PORT_B
#elif (ENC28J60_INT_N_PORT_NUM == GPIO_C_NUM)
#define ENC28J60_INT_N_PORT_VECTOR    NVIC_INT_GPIO_PORT_C
#elif (ENC28J60_INT_N_PORT_NUM == GPIO_D_NUM)
#define ENC28J60_INT_N_PORT_VECTOR    NVIC_INT_GPIO_PORT_D
#endif
/*---------------------------------------------------------------------------*/
static void
int_n_callback(uint8_t port, uint8_t pin)
{
  enc28j60_interrupt_handler();
}
/*---------------------------------------------------------------------------*/
static void
gpio_set_input(uint8_t port_num, uint8_t pin_num)
{
  GPIO_SOFTWARE_CONTROL(GPIO_PORT_TO_BASE(port_num), GPIO_PIN_MASK(pin_num));
  ioc_set_over(port_num, pin_num, IOC_OVERRIDE_DIS);
  GPIO_SET_INPUT(GPIO_PORT_TO_BASE(port_num), GPIO_PIN_MASK(pin_num));
}
/*---------------------------------------------------------------------------*/
static void
gpio_set_output(uint8_t port_num, uint8_t pin_num)
{
  GPIO_SOFTWARE_CONTROL(GPIO_PORT_TO_BASE(port_num), GPIO_PIN_MASK(pin_num));
  ioc_set_over(port_num, pin_num, IOC_OVERRIDE_DIS);
  GPIO_SET_OUTPUT(GPIO_PORT_TO_BASE(port_num), GPIO_PIN_MASK(pin_num));
}
/*---------------------------------------------------------------------------*/
void
enc28j60_arch_spi_init(void)
{
  /* CS_N output high */
  gpio_set_output(ENC28J60_CS_N_PORT_NUM, ENC28J60_CS_N_PIN_NUM);
  enc28j60_arch_spi_deselect();
  /* INT_N input */
  gpio_set_input(ENC28J60_INT_N_PORT_NUM, ENC28J60_INT_N_PIN_NUM);
  /* PWR_ON output */
  gpio_set_output(ENC28J60_PWR_ON_PORT_NUM, ENC28J60_PWR_ON_PIN_NUM);
  /* Set up SPI */
  spix_init(ENC28J60_SPI_INSTANCE);
  spix_set_mode(ENC28J60_SPI_INSTANCE, SSI_CR0_FRF_MOTOROLA, 0, 0, 8);

}
/*---------------------------------------------------------------------------*/
void 
enc28j60_arch_setup_irq(void)
{
  /* Enable edge detection */
  GPIO_DETECT_EDGE(GPIO_PORT_TO_BASE(ENC28J60_INT_N_PORT_NUM),
                   GPIO_PIN_MASK(ENC28J60_INT_N_PIN_NUM));
  /* Single edge */
  GPIO_TRIGGER_SINGLE_EDGE(GPIO_PORT_TO_BASE(ENC28J60_INT_N_PORT_NUM),
                           GPIO_PIN_MASK(ENC28J60_INT_N_PIN_NUM));
   /* Trigger interrupt on falling edge */
  GPIO_DETECT_FALLING(GPIO_PORT_TO_BASE(ENC28J60_INT_N_PORT_NUM),
            GPIO_PIN_MASK(ENC28J60_INT_N_PIN_NUM));
  /* Enable corresponding interrupt */
  nvic_interrupt_enable(ENC28J60_INT_N_PORT_VECTOR);
  /* Register callback */
  gpio_register_callback(int_n_callback,
                         ENC28J60_INT_N_PORT_NUM,
                         ENC28J60_INT_N_PIN_NUM);
}
/*---------------------------------------------------------------------------*/
void 
enc28j60_arch_enable_irq(void)
{
  /* Reset interrupt trigger */
  GPIO_CLEAR_INTERRUPT(GPIO_PORT_TO_BASE(ENC28J60_INT_N_PORT_NUM),
                       GPIO_PIN_MASK(ENC28J60_INT_N_PIN_NUM));
  /* Enable interrupt on the INT_N pin */
  GPIO_ENABLE_INTERRUPT(GPIO_PORT_TO_BASE(ENC28J60_INT_N_PORT_NUM),
                        GPIO_PIN_MASK(ENC28J60_INT_N_PIN_NUM));
}
/*---------------------------------------------------------------------------*/
void 
enc28j60_arch_disable_irq(void)
{
  /* Disable interrupt on the INT_N pin */
  GPIO_DISABLE_INTERRUPT(GPIO_PORT_TO_BASE(ENC28J60_INT_N_PORT_NUM),
                         GPIO_PIN_MASK(ENC28J60_INT_N_PIN_NUM));
}
/*---------------------------------------------------------------------------*/
uint8_t
enc28j60_arch_spi_write(uint8_t c)
{
  ENC28J60_SPI_WAITFORTxREADY();
  ENC28J60_SPI_TXBUF = c;
  ENC28J60_SPI_WAITFOREORx();
  return ENC28J60_SPI_RXBUF;
}
/*---------------------------------------------------------------------------*/
uint8_t
enc28j60_arch_spi_read(void)
{
  ENC28J60_SPI_WAITFORTxREADY();
  ENC28J60_SPI_TXBUF = 0;
  ENC28J60_SPI_WAITFOREORx();
  return ENC28J60_SPI_RXBUF;
}
/*---------------------------------------------------------------------------*/
int
enc28j60_arch_spi_rw_byte(uint8_t c)
{
  ENC28J60_SPI_WAITFORTxREADY();
  ENC28J60_SPI_TXBUF = c;
  ENC28J60_SPI_WAITFOREORx();
  return ENC28J60_SPI_RXBUF;
}
/*---------------------------------------------------------------------------*/
int
enc28j60_arch_spi_rw(uint8_t *read_buf,
                     const uint8_t *write_buf,
                     uint16_t len)
{
  uint16_t i;

  if(read_buf == NULL && write_buf == NULL) {
    /* Allow also dummy read
     in order to flush receive buffer in case of invalid
     packets */
    for(i = 0; i < len; i++) {
      ENC28J60_SPI_WAITFORTxREADY();
      ENC28J60_SPI_TXBUF = 0;
      ENC28J60_SPI_WAITFOREORx();
      ENC28J60_SPI_RXBUF;
    }
  } else if(read_buf == NULL) {
    for(i = 0; i < len; i++) {
      ENC28J60_SPI_WAITFORTxREADY();
      ENC28J60_SPI_TXBUF = write_buf[i];
      ENC28J60_SPI_WAITFOREORx();
      ENC28J60_SPI_RXBUF;
    }
  } else if(write_buf == NULL) {
    for(i = 0; i < len; i++) {
      ENC28J60_SPI_WAITFORTxREADY();
      ENC28J60_SPI_TXBUF = 0;
      ENC28J60_SPI_WAITFOREORx();
      read_buf[i] = ENC28J60_SPI_RXBUF;
    }
  } else {
    for(i = 0; i < len; i++) {
      ENC28J60_SPI_WAITFORTxREADY();
      ENC28J60_SPI_TXBUF = write_buf[i];
      ENC28J60_SPI_WAITFOREORx();
      read_buf[i] = ENC28J60_SPI_RXBUF;
    }
  }

  return 0;
}
/*---------------------------------------------------------------------------*/
void
enc28j60_arch_spi_select(void)
{
  GPIO_CLR_PIN(GPIO_PORT_TO_BASE(ENC28J60_CS_N_PORT_NUM),
               GPIO_PIN_MASK(ENC28J60_CS_N_PIN_NUM));
}
/*---------------------------------------------------------------------------*/
void
enc28j60_arch_spi_deselect(void)
{
  GPIO_SET_PIN(GPIO_PORT_TO_BASE(ENC28J60_CS_N_PORT_NUM),
               GPIO_PIN_MASK(ENC28J60_CS_N_PIN_NUM));
}
/*---------------------------------------------------------------------------*/
