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

#if 1
#include "cc1200-arch.h"


#include "spi-arch.h"
#include "dev/gpio.h"
#include "dev/ioc.h"
#include "dev/nvic.h"

#include <string.h>

/*---------------------------------------------------------------------------*/
/* Determine the vector to be used for the GPIO0 callback function */
#if (CC1200_GPIO0_PORT_NUM == GPIO_A_NUM)
#define CC1200_GPIO0_PORT_VECTOR    NVIC_INT_GPIO_PORT_A
#elif (CC1200_GPIO0_PORT_NUM == GPIO_B_NUM)
#define CC1200_GPIO0_PORT_VECTOR    NVIC_INT_GPIO_PORT_B
#elif (CC1200_GPIO0_PORT_NUM == GPIO_C_NUM)
#define CC1200_GPIO0_PORT_VECTOR    NVIC_INT_GPIO_PORT_C
#elif (CC1200_GPIO0_PORT_NUM == GPIO_D_NUM)
#define CC1200_GPIO0_PORT_VECTOR    NVIC_INT_GPIO_PORT_D
#endif
/* Determine the vector to be used for the GPIO0 callback function */
#if (CC1200_GPIO2_PORT_NUM == GPIO_A_NUM)
#define CC1200_GPIO2_PORT_VECTOR    NVIC_INT_GPIO_PORT_A
#elif (CC1200_GPIO2_PORT_NUM == GPIO_B_NUM)
#define CC1200_GPIO2_PORT_VECTOR    NVIC_INT_GPIO_PORT_B
#elif (CC1200_GPIO2_PORT_NUM == GPIO_C_NUM)
#define CC1200_GPIO2_PORT_VECTOR    NVIC_INT_GPIO_PORT_C
#elif (CC1200_GPIO2_PORT_NUM == GPIO_D_NUM)
#define CC1200_GPIO2_PORT_VECTOR    NVIC_INT_GPIO_PORT_D
#endif
/*---------------------------------------------------------------------------*/
static void 
gpiox_callback(uint8_t port, uint8_t pin)
{
#if 0
  /*
   * Call-stack up to here
   * - gpio_port_b_isr() (gpio.c)
   * - notify() (gpio.c)
   *
   * ENERGEST_ON / OFF + lpm_exit() + interrupt trigger reset is performed
   * within gpio_port_x_isr()
   */

  leds_on(LEDS_RED);
  cc1200_rx_interrupt();
  leds_off(LEDS_RED);
  #endif
}
/*---------------------------------------------------------------------------*/
static void 
gpio_set_input(uint8_t port_num, uint8_t pin_num)
{
	#if 0
  GPIO_SOFTWARE_CONTROL(GPIO_PORT_TO_BASE(port_num),
                        GPIO_PIN_MASK(pin_num));
  ioc_set_over(port_num, pin_num, IOC_OVERRIDE_DIS);
  GPIO_SET_INPUT(GPIO_PORT_TO_BASE(port_num),
                 GPIO_PIN_MASK(pin_num));
                   #endif
}
/*---------------------------------------------------------------------------*/
static void 
gpio_set_output(uint8_t port_num, uint8_t pin_num)
{
	#if 0
  GPIO_SOFTWARE_CONTROL(GPIO_PORT_TO_BASE(port_num),
                        GPIO_PIN_MASK(pin_num));
  ioc_set_over(port_num, pin_num, IOC_OVERRIDE_DIS);
  GPIO_SET_OUTPUT(GPIO_PORT_TO_BASE(port_num),
                  GPIO_PIN_MASK(pin_num));
                    #endif
}
/*---------------------------------------------------------------------------*/
void 
cc1200_arch_init(void)
{
	#if 0

  /* CS_N output high */
  gpio_set_output(CC1200_CS_N_PORT_NUM,
                  CC1200_CS_N_PIN_NUM);

  cc1200_arch_spi_deselect();

  /* GPIOx input */
  gpio_set_input(CC1200_GPIO0_PORT_NUM,
                 CC1200_GPIO0_PIN_NUM);

  gpio_set_input(CC1200_GPIO2_PORT_NUM,
                 CC1200_GPIO2_PIN_NUM);

  gpio_set_input(CC1200_GPIO3_PORT_NUM,
                 CC1200_GPIO3_PIN_NUM);

  /* MISO input for reset sequence */
  gpio_set_input(CC1200_MISO_PORT_NUM,
                 CC1200_MISO_PIN_NUM);

  /* RESET_N output high */
  gpio_set_output(CC1200_RESET_N_PORT_NUM,
                  CC1200_RESET_N_PIN_NUM);

#if 1

  GPIO_SET_PIN(GPIO_PORT_TO_BASE(CC1200_RESET_N_PORT_NUM),
               GPIO_PIN_MASK(CC1200_RESET_N_PIN_NUM));

#else

  /* Perform reset sequence using RESET_N */
  GPIO_CLR_PIN(GPIO_PORT_TO_BASE(CC1200_RESET_N_PORT_NUM),
               GPIO_PIN_MASK(CC1200_RESET_N_PIN_NUM));

  /* 500 µs delay */
  /* TODO: Timeout to be verified */
  clock_delay_usec(500);

  /*  Release RESET_N */
  GPIO_SET_PIN(GPIO_PORT_TO_BASE(CC1200_RESET_N_PORT_NUM),
               GPIO_PIN_MASK(CC1200_RESET_N_PIN_NUM));

  /* Wait for SO to go low... */
  cc1200_arch_spi_select();
  while(GPIO_READ_PIN(GPIO_PORT_TO_BASE(CC1200_MISO_PORT_NUM),
                      GPIO_PIN_MASK(CC1200_MISO_PIN_NUM))) ;
  cc1200_arch_spi_deselect();

#endif

  /* SPI is initialized after reset sequence... */

#ifndef CC1200_SPI_INSTANCE
  /* Old SPI API */
  spi_init();
  spi_set_mode(SSI_CR0_FRF_MOTOROLA, 0, 0, 8);
#else
  spix_init(CC1200_SPI_INSTANCE);
  /* 
   * Adjust SPI mode
   * - SSICLK low in steady state
   * - Capture on first (rising) edge 
   */
  spix_set_mode(CC1200_SPI_INSTANCE, SSI_CR0_FRF_MOTOROLA, 0, 0, 8);
#endif
#endif
}
/*---------------------------------------------------------------------------*/
void 
cc1200_arch_spi_select(void)
{
	
	#if 0
  GPIO_CLR_PIN(GPIO_PORT_TO_BASE(CC1200_CS_N_PORT_NUM),
               GPIO_PIN_MASK(CC1200_CS_N_PIN_NUM));
               #endif
}
/*---------------------------------------------------------------------------*/
void 
cc1200_arch_spi_deselect(void)
{
	#if 0
  GPIO_SET_PIN(GPIO_PORT_TO_BASE(CC1200_CS_N_PORT_NUM),
               GPIO_PIN_MASK(CC1200_CS_N_PIN_NUM));
               #endif
}
/*---------------------------------------------------------------------------*/
void 
cc1200_arch_gpio0_setup_irq(int rising)
{
	#if 0
  /* Enable edge detection */
  GPIO_DETECT_EDGE(GPIO_PORT_TO_BASE(CC1200_GPIO0_PORT_NUM),
                   GPIO_PIN_MASK(CC1200_GPIO0_PIN_NUM));
  /* Single edge */
  GPIO_TRIGGER_SINGLE_EDGE(GPIO_PORT_TO_BASE(CC1200_GPIO0_PORT_NUM),
                           GPIO_PIN_MASK(CC1200_GPIO0_PIN_NUM));
  if (rising) {
    /* Trigger interrupt on rising edge */
    GPIO_DETECT_RISING(GPIO_PORT_TO_BASE(CC1200_GPIO0_PORT_NUM),
                       GPIO_PIN_MASK(CC1200_GPIO0_PIN_NUM));
  } else {
    /* Trigger interrupt on falling edge */
    GPIO_DETECT_FALLING(GPIO_PORT_TO_BASE(CC1200_GPIO0_PORT_NUM),
                        GPIO_PIN_MASK(CC1200_GPIO0_PIN_NUM));
  }
  /* Enable corresponding interrupt */
  nvic_interrupt_enable(CC1200_GPIO0_PORT_VECTOR);
  /* Register callback */
  gpio_register_callback(gpiox_callback,
                         CC1200_GPIO0_PORT_NUM,
                         CC1200_GPIO0_PIN_NUM);
                         #endif
}
/*---------------------------------------------------------------------------*/
void 
cc1200_arch_gpio2_setup_irq(int rising)
{
	#if 0
  /* Enable edge detection */
  GPIO_DETECT_EDGE(GPIO_PORT_TO_BASE(CC1200_GPIO2_PORT_NUM),
                   GPIO_PIN_MASK(CC1200_GPIO2_PIN_NUM));
  /* Single edge */
  GPIO_TRIGGER_SINGLE_EDGE(GPIO_PORT_TO_BASE(CC1200_GPIO2_PORT_NUM),
                           GPIO_PIN_MASK(CC1200_GPIO2_PIN_NUM));
  if (rising) {
    /* Trigger interrupt on rising edge */
    GPIO_DETECT_RISING(GPIO_PORT_TO_BASE(CC1200_GPIO2_PORT_NUM),
                       GPIO_PIN_MASK(CC1200_GPIO2_PIN_NUM));
  } else {
    /* Trigger interrupt on falling edge */
    GPIO_DETECT_FALLING(GPIO_PORT_TO_BASE(CC1200_GPIO2_PORT_NUM),
                        GPIO_PIN_MASK(CC1200_GPIO2_PIN_NUM));
  }
  /* Enable corresponding interrupt */
  nvic_interrupt_enable(CC1200_GPIO2_PORT_VECTOR);
  /* Register callback */
  gpio_register_callback(gpiox_callback,
                         CC1200_GPIO2_PORT_NUM,
                         CC1200_GPIO2_PIN_NUM);
                         #endif
}
/*---------------------------------------------------------------------------*/
void 
cc1200_arch_gpio0_enable_irq(void)
{
	#if 0
  /* Reset interrupt trigger */
  GPIO_CLEAR_INTERRUPT(GPIO_PORT_TO_BASE(CC1200_GPIO0_PORT_NUM),
                       GPIO_PIN_MASK(CC1200_GPIO0_PIN_NUM));
  /* Enable interrupt on the GPIO0 pin */
  GPIO_ENABLE_INTERRUPT(GPIO_PORT_TO_BASE(CC1200_GPIO0_PORT_NUM),
                        GPIO_PIN_MASK(CC1200_GPIO0_PIN_NUM));
                        #endif
}
/*---------------------------------------------------------------------------*/
void 
cc1200_arch_gpio0_disable_irq(void)
{
	#if 0
  /* Disable interrupt on the GPIO0 pin */
  GPIO_DISABLE_INTERRUPT(GPIO_PORT_TO_BASE(CC1200_GPIO0_PORT_NUM),
                         GPIO_PIN_MASK(CC1200_GPIO0_PIN_NUM));
                         #endif
}
/*---------------------------------------------------------------------------*/
void 
cc1200_arch_gpio2_enable_irq(void)
{
	#if 0
  /* Reset interrupt trigger */
  GPIO_CLEAR_INTERRUPT(GPIO_PORT_TO_BASE(CC1200_GPIO2_PORT_NUM),
                       GPIO_PIN_MASK(CC1200_GPIO2_PIN_NUM));
  /* Enable interrupt on the GPIO2 pin */
  GPIO_ENABLE_INTERRUPT(GPIO_PORT_TO_BASE(CC1200_GPIO2_PORT_NUM),
                        GPIO_PIN_MASK(CC1200_GPIO2_PIN_NUM));
                        #endif
}
/*---------------------------------------------------------------------------*/
void 
cc1200_arch_gpio2_disable_irq(void)
{
	#if 0
  /* Disable interrupt on the GPIO2 pin */
  GPIO_DISABLE_INTERRUPT(GPIO_PORT_TO_BASE(CC1200_GPIO2_PORT_NUM),
                         GPIO_PIN_MASK(CC1200_GPIO2_PIN_NUM));
                         #endif
}
/*---------------------------------------------------------------------------*/
int 
cc1200_arch_gpio0_read_pin(void)
{
	#if 0
  if(GPIO_READ_PIN(GPIO_PORT_TO_BASE(CC1200_GPIO0_PORT_NUM),
                   GPIO_PIN_MASK(CC1200_GPIO0_PIN_NUM))) {
    return 1;
  }
    #endif
  return 0;

}
/*---------------------------------------------------------------------------*/
int cc1200_arch_gpio2_read_pin(void)
{
	#if 0
  if(GPIO_READ_PIN(GPIO_PORT_TO_BASE(CC1200_GPIO2_PORT_NUM),
                   GPIO_PIN_MASK(CC1200_GPIO2_PIN_NUM))) {
    return 1;
  }
    #endif
  return 0;
}
/*---------------------------------------------------------------------------*/
int 
cc1200_arch_gpio3_read_pin(void)
{
	#if 0
  if(GPIO_READ_PIN(GPIO_PORT_TO_BASE(CC1200_GPIO3_PORT_NUM),
                   GPIO_PIN_MASK(CC1200_GPIO3_PIN_NUM))) {
    return 1;
  }
    #endif
  return 0;
}
/*---------------------------------------------------------------------------*/
int 
cc1200_arch_spi_rw_byte(uint8_t c)
{
	#if 0
  CC1200_SPI_WAITFORTxREADY();
  CC1200_SPI_TXBUF = c;
  CC1200_SPI_WAITFOREORx();

  return CC1200_SPI_RXBUF;
      #endif
}
/*---------------------------------------------------------------------------*/
int 
cc1200_arch_spi_rw(uint8_t *read_buf,
                   const uint8_t *write_buf,
                   uint16_t len)
{
  uint16_t i;

#if 0
  if(read_buf == NULL && write_buf == NULL) {
    return 1;
  } else if(read_buf == NULL) {
    for(i = 0; i < len; i++) {
      CC1200_SPI_WAITFORTxREADY();
      CC1200_SPI_TXBUF = write_buf[i];
      CC1200_SPI_WAITFOREORx();
      CC1200_SPI_RXBUF;
    }
  } else if(write_buf == NULL) {
    for(i = 0; i < len; i++) {
      CC1200_SPI_WAITFORTxREADY();
      CC1200_SPI_TXBUF = 0;
      CC1200_SPI_WAITFOREORx();
      read_buf[i] = CC1200_SPI_RXBUF;
    }
  } else {
    for(i = 0; i < len; i++) {
      CC1200_SPI_WAITFORTxREADY();
      CC1200_SPI_TXBUF = write_buf[i];
      CC1200_SPI_WAITFOREORx();
      read_buf[i] = CC1200_SPI_RXBUF;
    }
  }
    #endif
  return 0;
}
#endif
/*---------------------------------------------------------------------------*/
