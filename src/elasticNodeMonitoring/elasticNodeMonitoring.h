#ifndef ELASTICNODEMONITORING_H
#define ELASTICNODEMONITORING_H

#define RECHECK 1

#define OWN_STATE 0

#define START_AND_STOP_ACTIVE 0
#if START_AND_STOP_ACTIVE
#define START_OF_MEASUREMENT 1
#define END_OF_MEASUREMENT 255
#endif


#define STATE_MASK 0b00001111
#define SAMPLE_MASK 0b11110000

#define MON_LED_INIT() (DDRD |= ((1 << 2) | (1 << 3)))
#define MON1_LED_ON()      (PORTD |= (1<<3))
#define MON1_LED_OFF()      (PORTD &= ~(1<<3))
#define MON2_LED_ON()        (PORTD |= (1<<2))
#define MON2_LED_OFF()       (PORTD &= ~(1<<2))

#include <stdint.h>


uint8_t iic_read_from_device(uint8_t device_address);



#if OWN_STATE
void update_running_state(uint8_t state_variable);
#else
void update_running_state();

void update_sample_rate();

void update_state_main_mcu();
#endif

//void adapt_sample_rate(uint8_t *sample_rate);

uint8_t get_running_state();

uint8_t get_sample_rate();

#endif //ELASTICNODEMONITORING_H
