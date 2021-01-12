#include "elasticNodeMonitoring.h"
#include <inttypes.h>
#include <stdint.h>
#include "src/i2cmaster/i2cmaster.h"
#include "src/debug/debug.h"
#include "src/PAC1720_driver/PAC1720_definitions.h"
//#include "src/adapter_PAC1720/adapter_PAC1720.h"



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
    }
    else
    {
        result = i2c_readNak();
    }

    i2c_stop();
    return result;

}


#if OWN_STATE
void update_running_state(uint8_t state_variable){
        running_state = state_variable;
    }

#else
void update_state_main_mcu()
{
    state_of_main_mcu = iic_read_from_device(64);
}

void update_sample_rate(){
    sample_rate = (state_of_main_mcu & SAMPLE_MASK) >> 4;
}

void update_running_state(){
    running_state = state_of_main_mcu & STATE_MASK;
}
#endif


// void adapt_sample_rate(uint8_t *sample_rate){

//         // int8_t res;
//         // switch (*sample_rate)
//         // {
//         // case 0:
//         //     /* use CURRENT_SAMPLE_TIME_2ms5 */
//         //     dev_USB_WIREL.DEV_CH1_conf.CH_current_sense_sampling_time_reg  = CURRENT_SAMPLE_TIME_2ms5;
//         //     dev_USB_WIREL.DEV_CH2_conf.CH_current_sense_sampling_time_reg  = CURRENT_SAMPLE_TIME_2ms5;
//         //     dev_FPGA_VCC.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_2ms5;
//         //     dev_FPGA_VCC.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_2ms5;
//         //     dev_FPGA_SRAM.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_2ms5;
//         //     dev_FPGA_SRAM.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_2ms5;
//         //     dev_DAUGHTER_MCU.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_2ms5;
//         //     dev_DAUGHTER_MCU.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_2ms5;

//         //     res = adapter_init_PAC1720_user_defined(&dev_USB_WIREL);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_FPGA_VCC);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_FPGA_SRAM);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_DAUGHTER_MCU);
//         //     if(res != PAC1720_OK) print_error(res);
//         // break;
//         // case 1:
//         //     /* use CURRENT_SAMPLE_TIME_5ms */
//         //      dev_USB_WIREL.DEV_CH1_conf.CH_current_sense_sampling_time_reg  = CURRENT_SAMPLE_TIME_5ms;
//         //     dev_USB_WIREL.DEV_CH2_conf.CH_current_sense_sampling_time_reg  = CURRENT_SAMPLE_TIME_5ms;
//         //     dev_FPGA_VCC.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_5ms;
//         //     dev_FPGA_VCC.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_5ms;
//         //     dev_FPGA_SRAM.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_5ms;
//         //     dev_FPGA_SRAM.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_5ms;
//         //     dev_DAUGHTER_MCU.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_5ms;
//         //     dev_DAUGHTER_MCU.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_5ms;

//         //     res = adapter_init_PAC1720_user_defined(&dev_USB_WIREL);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_FPGA_VCC);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_FPGA_SRAM);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_DAUGHTER_MCU);
//         //     if(res != PAC1720_OK) print_error(res);
//         // break;
//         // case 2:
//         //     /* use CURRENT_SAMPLE_TIME_10ms */
//         //     dev_USB_WIREL.DEV_CH1_conf.CH_current_sense_sampling_time_reg  = CURRENT_SAMPLE_TIME_10ms;
//         //     dev_USB_WIREL.DEV_CH2_conf.CH_current_sense_sampling_time_reg  = CURRENT_SAMPLE_TIME_10ms;
//         //     dev_FPGA_VCC.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_10ms;
//         //     dev_FPGA_VCC.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_10ms;
//         //     dev_FPGA_SRAM.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_10ms;
//         //     dev_FPGA_SRAM.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_10ms;
//         //     dev_DAUGHTER_MCU.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_10ms;
//         //     dev_DAUGHTER_MCU.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_10ms;

//         //     res = adapter_init_PAC1720_user_defined(&dev_USB_WIREL);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_FPGA_VCC);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_FPGA_SRAM);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_DAUGHTER_MCU);
//         //     if(res != PAC1720_OK) print_error(res);
//         // break;
//         // case 3:
//         //     /* use CURRENT_SAMPLE_TIME_20ms */
//         //     dev_USB_WIREL.DEV_CH1_conf.CH_current_sense_sampling_time_reg  = CURRENT_SAMPLE_TIME_20ms;
//         //     dev_USB_WIREL.DEV_CH2_conf.CH_current_sense_sampling_time_reg  = CURRENT_SAMPLE_TIME_20ms;
//         //     dev_FPGA_VCC.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_20ms;
//         //     dev_FPGA_VCC.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_20ms;
//         //     dev_FPGA_SRAM.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_20ms;
//         //     dev_FPGA_SRAM.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_20ms;
//         //     dev_DAUGHTER_MCU.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_20ms;
//         //     dev_DAUGHTER_MCU.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_20ms;

//         //     int8_t res = adapter_init_PAC1720_user_defined(&dev_USB_WIREL);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_FPGA_VCC);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_FPGA_SRAM);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_DAUGHTER_MCU);
//         //     if(res != PAC1720_OK) print_error(res);
//         // break;
//         // case 4:
//         //     /* use CURRENT_SAMPLE_TIME_40ms */
//         //     dev_USB_WIREL.DEV_CH1_conf.CH_current_sense_sampling_time_reg  = CURRENT_SAMPLE_TIME_40ms;
//         //     dev_USB_WIREL.DEV_CH2_conf.CH_current_sense_sampling_time_reg  = CURRENT_SAMPLE_TIME_40ms;
//         //     dev_FPGA_VCC.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_40ms;
//         //     dev_FPGA_VCC.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_40ms;
//         //     dev_FPGA_SRAM.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_40ms;
//         //     dev_FPGA_SRAM.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_40ms;
//         //     dev_DAUGHTER_MCU.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_40ms;
//         //     dev_DAUGHTER_MCU.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_40ms;

//         //     res = adapter_init_PAC1720_user_defined(&dev_USB_WIREL);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_FPGA_VCC);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_FPGA_SRAM);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_DAUGHTER_MCU);
//         //     if(res != PAC1720_OK) print_error(res);
//         // break;
//         // case 5:
//         //     /* use CURRENT_SAMPLE_TIME_80ms */
//         //     dev_USB_WIREL.DEV_CH1_conf.CH_current_sense_sampling_time_reg  = CURRENT_SAMPLE_TIME_80ms;
//         //     dev_USB_WIREL.DEV_CH2_conf.CH_current_sense_sampling_time_reg  = CURRENT_SAMPLE_TIME_80ms;
//         //     dev_FPGA_VCC.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_80ms;
//         //     dev_FPGA_VCC.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_80ms;
//         //     dev_FPGA_SRAM.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_80ms;
//         //     dev_FPGA_SRAM.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_80ms;
//         //     dev_DAUGHTER_MCU.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_80ms;
//         //     dev_DAUGHTER_MCU.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_80ms;

//         //     res = adapter_init_PAC1720_user_defined(&dev_USB_WIREL);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_FPGA_VCC);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_FPGA_SRAM);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_DAUGHTER_MCU);
//         //     if(res != PAC1720_OK) print_error(res);
//         // break;
//         // case 6:
//         //     /* use CURRENT_SAMPLE_TIME_160ms */
//         //     dev_USB_WIREL.DEV_CH1_conf.CH_current_sense_sampling_time_reg  = CURRENT_SAMPLE_TIME_160ms;
//         //     dev_USB_WIREL.DEV_CH2_conf.CH_current_sense_sampling_time_reg  = CURRENT_SAMPLE_TIME_160ms;
//         //     dev_FPGA_VCC.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_160ms;
//         //     dev_FPGA_VCC.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_160ms;
//         //     dev_FPGA_SRAM.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_160ms;
//         //     dev_FPGA_SRAM.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_160ms;
//         //     dev_DAUGHTER_MCU.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_160ms;
//         //     dev_DAUGHTER_MCU.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_160ms;

//         //     res = adapter_init_PAC1720_user_defined(&dev_USB_WIREL);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_FPGA_VCC);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_FPGA_SRAM);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_DAUGHTER_MCU);
//         //     if(res != PAC1720_OK) print_error(res);
//         // break;
//         // case 7:
//         //     /* use CURRENT_SAMPLE_TIME_320ms */
//         //     dev_USB_WIREL.DEV_CH1_conf.CH_current_sense_sampling_time_reg  = CURRENT_SAMPLE_TIME_320ms;
//         //     dev_USB_WIREL.DEV_CH2_conf.CH_current_sense_sampling_time_reg  = CURRENT_SAMPLE_TIME_320ms;
//         //     dev_FPGA_VCC.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_320ms;
//         //     dev_FPGA_VCC.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_320ms;
//         //     dev_FPGA_SRAM.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_320ms;
//         //     dev_FPGA_SRAM.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_320ms;
//         //     dev_DAUGHTER_MCU.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_320ms;
//         //     dev_DAUGHTER_MCU.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_320ms;

//         //     res = adapter_init_PAC1720_user_defined(&dev_USB_WIREL);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_FPGA_VCC);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_FPGA_SRAM);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_DAUGHTER_MCU);
//         //     if(res != PAC1720_OK) print_error(res);
//         // break;
//         // default:
//         //     dev_USB_WIREL.DEV_CH1_conf.CH_current_sense_sampling_time_reg  = CURRENT_SAMPLE_TIME_DEFAULT;
//         //     dev_USB_WIREL.DEV_CH2_conf.CH_current_sense_sampling_time_reg  = CURRENT_SAMPLE_TIME_DEFAULT;
//         //     dev_FPGA_VCC.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_DEFAULT;
//         //     dev_FPGA_VCC.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_DEFAULT;
//         //     dev_FPGA_SRAM.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_DEFAULT;
//         //     dev_FPGA_SRAM.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_DEFAULT;
//         //     dev_DAUGHTER_MCU.DEV_CH1_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_DEFAULT;
//         //     dev_DAUGHTER_MCU.DEV_CH2_conf.CH_current_sense_sampling_time_reg = CURRENT_SAMPLE_TIME_DEFAULT;

//         //     res = adapter_init_PAC1720_user_defined(&dev_USB_WIREL);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_FPGA_VCC);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_FPGA_SRAM);
//         //     if(res != PAC1720_OK) print_error(res);
//         //     res = adapter_init_PAC1720_user_defined(&dev_DAUGHTER_MCU);
//         //     if(res != PAC1720_OK) print_error(res);
//         // break;
//         // }

// }



uint8_t get_running_state(){

    return (iic_read_from_device(64) & STATE_MASK);
}

uint8_t get_sample_rate(){
    return iic_read_from_device(64) >> 4;
}
