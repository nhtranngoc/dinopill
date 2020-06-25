#include "adc.h"

void adc_setup(void) {
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO0);

	rcc_periph_clock_enable(RCC_ADC1);

	/* Make sure the ADC doesn't run during config. */
	adc_power_off(ADC1);

	/* We configure everything for one single conversion. */
	adc_disable_scan_mode(ADC1);
	adc_set_single_conversion_mode(ADC1);
	adc_disable_external_trigger_regular(ADC1);
	adc_set_right_aligned(ADC1);
	/* We want to read the temperature sensor, so we have to enable it. */
	adc_enable_temperature_sensor();
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_28DOT5CYC);

	adc_power_on(ADC1);

	adc_reset_calibration(ADC1);
	adc_calibrate(ADC1);
}

uint16_t read_adc(uint8_t channel) {
    // adc_set_sample_time(ADC1, channel, ADC_SMPR_SMP_239DOT5CYC);
    adc_set_regular_sequence(ADC1, 1, &channel);
    adc_start_conversion_direct(ADC1);

    // while(!(ADC_SR(ADC1) & ADC_SR_EOC));
    return ADC_DR(ADC1);
}