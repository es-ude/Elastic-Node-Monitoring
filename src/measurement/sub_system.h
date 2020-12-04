#ifndef SUB_SYSTEM_H
#define SUB_SYSTEM_H

#define SAMPLE_RATE_STANDARD 100
#define SAMPLE_RATE_SEND_INPUT 2.5
#define SAMPLE_RATE_RECONFIG 10
#define SAMPLE_RATE_EXEC 20
#define SAMPLE_RATE_RECEIVE_RESULT 50
//...

#define OWN_STATE 0

#define START_AND_STOP_ACTIVE 0
#if START_AND_STOP_ACTIVE
#define START_OF_MEASUREMENT 1
#define END_OF_MEASUREMENT 255
#endif


#define STATE_MASK 0b00001111
#define SAMPLE_MASK 0b11110000

#define MON1_LED_ON()      (PORTD |= (1<<3))
#define MON1_LED_OFF()      (PORTD &= ~(1<<3))
#define MON2_LED_ON()        (PORTD |= (1<<2))
#define MON2_LED_OFF()       (PORTD &= ~(1<<2))

#include <stdint.h>


uint8_t iic_read_from_device(uint8_t device_address);

#if OWN_STATE == 0
void update_state_main_mcu(uint8_t *state_of_main_mcu);
#endif

#if OWN_STATE == 0
void update_running_state(uint8_t *running_state, uint8_t *state_of_main_mcu);

void update_sample_rate(uint8_t *sample_rate, uint8_t *state_of_main_mcu);
#endif

#if OWN_STATE
void update_running_state(uint8_t *running_state, uint8_t state_variable);
#endif



uint8_t get_running_state();

uint8_t get_sample_rate();













#endif