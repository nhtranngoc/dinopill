#ifndef ADC_H
#define ADC_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/adc.h>

void adc_setup(void);
uint16_t read_adc(uint8_t channel);

#endif // ADC_H_