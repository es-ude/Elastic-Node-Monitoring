#include "sub_system.h"
#include <inttypes.h>
#include <stdint.h>
#include "lib/i2cmaster/i2cmaster.h"
#include "lib/debug/debug.h"




uint8_t state_of_main_mcu;
uint8_t running_state;
uint8_t sample_rate;


uint8_t iic_read_from_device(uint8_t device_address)
{
    uint8_t result;

    result = i2c_start(device_address | 1);
    if (result)
    {
        debugWriteString("\r\ncould't repeat start");
        //led to indicate start cond failed
    }
    else
    {   
        //led to indicate start cond succesfully
        result = i2c_readNak();
    }
    
    i2c_stop();
    return result;
    
}
#if OWN_STATE
    void update_running_state(uint8_t *running_state,uint8_t state_variable){
        *running_state = own_state;
    }
#endif


#if OWN_STATE == 0
void update_state_main_mcu(uint8_t *state_of_main_mcu)
{
    *state_of_main_mcu = iic_read_from_device(64);
}

void update_sample_rate(uint8_t *sample_rate, uint8_t *state_of_main_mcu){
    *sample_rate = (*state_of_main_mcu & SAMPLE_MASK) >> 4;
}

void update_running_state(uint8_t *running_state, uint8_t *state_of_main_mcu)
    {
    *running_state = *state_of_main_mcu & STATE_MASK;
    }
#endif





uint8_t get_running_state(){

    return (iic_read_from_device(64) & STATE_MASK);
}

uint8_t get_sample_rate(){
    return iic_read_from_device(64) >> 4;
}