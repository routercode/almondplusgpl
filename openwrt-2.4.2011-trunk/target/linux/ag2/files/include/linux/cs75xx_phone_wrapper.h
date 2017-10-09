#ifndef CS75XX_PHONE_WRAPPER_H
#define CS75XX_PHONE_WRAPPER_H

typedef enum {
	VOIP_LED_OFF = 0,
	VOIP_LED_ON = 1,
	VOIP_LED_BLINK = 2
} voip_led_state_t;

/**
 * cs_slic_pcm_select - get the select SSP index used for PCM
 */
int cs_slic_pcm_select(void);

/**
 * cs_slic_hw_reset - send a low -> high signal to reset SLIC
 *
 * PCLK must be provided before calling this
 */
int cs_slic_hw_reset(void);

/**
 * cs_slic_read_byte - synchronous write byte from SLIC registers
 * @value: data
 * Context: can sleep
 *
 * This reads the data and returns zero or a negative error code.
 * Callable only from contexts that can sleep.
 */
int cs_slic_read_byte(unsigned char *value);

/**
 * cs_slic_write_byte - synchronous write byte to SLIC registers
 * @value: data
 * Context: can sleep
 *
 * This writes the data and returns zero or a negative error code.
 * Callable only from contexts that can sleep.
 */
int cs_slic_write_byte(unsigned char value);

/**
 * cs_voip_led_trigger - synchronous write byte to SLIC registers
 * @on: 0: LED off, others LED on
 */
int cs_voip_led_trigger(voip_led_state_t state);

/**
 * cs_register_hw_poll_timer - register a dedicated periodical hardware timer
 * @scale: period with unit of ms
 * @callback: a callback funtion when time is up
 * Context: can't sleep
 *
 * This only supports one dedicated periodical timer and the calll function runs
 * in a interrupt context.
 */
int cs_register_hw_poll_timer(unsigned int scale, int (*callback)(unsigned long));

/**
 * cs_unregister_hw_poll_timer - deregister the dedicated periodical hardware timer
 */
int cs_unregister_hw_poll_timer(void);

/**
 * cs_get_hw_timestamp - Get hardware timer clock cycle
 *
 * This API return current hardware clock count.
 */
unsigned int cs_get_hw_timestamp(void);

/**
 * cs_get_hw_timestamp_delta - calculate hardware timer clock cycle duration
 * @t1: 1st clock timer count
 * @t2: 2nd clock timer count
 *
 * This API calculate time duration between t1 and t2 in microsecond.
 */
unsigned int cs_get_hw_timestamp_delta(unsigned int t1, unsigned int t2);

#endif		/* CS75XX_PHONE_WRAPPER_H */

