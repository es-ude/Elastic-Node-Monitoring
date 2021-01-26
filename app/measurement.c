//#include "include/currentSenseApp.h"

/************************************** Dependencies to be injected ************************************/
#include "app/adapter_PAC1720/adapter_PAC1720.h"
#include "src/i2cmaster/i2cmaster.h"
#include "src/delay/user_delay.h"
#include "src/debug/debug.h"
#include <stdio.h>
#include <avr/io.h>

#include "src/measurement/sub_system.h"


void adapt_sample_rate();
void powerMeasurement();
#define PRINT_VARIABLES 1
#if DAUGHTER_POWERED
#define NUM_SENSORS 5
#else
#define NUM_SENSORS 4
#endif
#if PRINT_MCUSTATE
#define PRINT_STATE 1
#else
#define PRINT_STATE 0
#endif

/* Instantiate a bus interface */
struct FIELD_BUS_INTERFACE external_fieldbus_interface = {
        /** Assign i2cmaster library function pointer to members */
        .init       = &i2c_init,
        .stop       = &i2c_stop,
        .start      = &i2c_start,
        .repStart   = &i2c_rep_start,
        .startWait  = &i2c_start_wait,
        .write      = &i2c_write,
        .readAck    = &i2c_readAck,
        .readNak    = &i2c_readNak
};

/** Assign user provided delay function to pointer */
delay_function_ptr external_delay_function = &user_delay_ms;

/***************************************** Function prototypes *******************************************/
int8_t  init_platform(void);
void    tear_down_platform(void);
void    check_user_input(uint8_t *state);
void    set_state(uint8_t data, uint8_t *state);
void    print_error(int8_t res);

char outputBuffer[(NUM_SENSORS*2*PRINT_VARIABLES + PRINT_STATE)*sizeof(float)+8];
float *ptr;
uint8_t meas_fail = 0;


/*********************************************** Main ****************************************************/
#if OWN_STATE == 0
//state indicator (state variable)
extern uint8_t state_of_main_mcu;
//the "true" running state
extern uint8_t running_state;
//the resulting sample rate
extern uint8_t sample_rate;
#endif





int main(void)
{
    /* Init the hardware, print error if fail */
    DDRD |= ((1<<2)|(1<<3));
    MON1_LED_ON();
    MON2_LED_ON();

    int8_t res = init_platform();
    if(res != PAC1720_OK)
    {
        print_error(res);
    }

    MON1_LED_OFF();
    MON2_LED_OFF(); // so if the LEDs are on, then the sensor must be broken

    /* Debug string */
//    char msg[64];
    /** User controlled state */
    uint8_t state = 7;
    /* Result of get_measurements */

    running_state = 0;

    outputBuffer[0] = 0x01;
    outputBuffer[1] = 0x02;
    outputBuffer[2] = 0x03;
    outputBuffer[(NUM_SENSORS*2*PRINT_VARIABLES+PRINT_STATE)*sizeof(float)+8 -5] = 5;
    outputBuffer[(NUM_SENSORS*2*PRINT_VARIABLES+PRINT_STATE)*sizeof(float)+8 -4] = 4;
    outputBuffer[(NUM_SENSORS*2*PRINT_VARIABLES+PRINT_STATE)*sizeof(float)+8 -3] = 3;
    outputBuffer[(NUM_SENSORS*2*PRINT_VARIABLES+PRINT_STATE)*sizeof(float)+8 -2] = 2;
    outputBuffer[(NUM_SENSORS*2*PRINT_VARIABLES+PRINT_STATE)*sizeof(float)+8 -1] = 1;


    uint16_t timer_1_sec = 0;

//todo add a number of rounds

#if START_AND_STOP_ACTIVE
    while (running_state != START_OF_MEASUREMENT)
    {
        /* code wait*/
        update_state_main_mcu();
        update_running_state();
    }
#endif


#if PRINT_STATE
    while(1){
        #if START_AND_STOP_ACTIVE
            if(state_of_main_mcu == END_OF_MEASUREMENT){
                MON1_LED_ON();
                MON2_LED_OFF();
               break;
            }
        #endif


        powerMeasurement();
        switch (running_state)
        {
            case 0:
                MON1_LED_OFF();
                MON2_LED_OFF();
                /* code */
                break;
            default:
                MON1_LED_OFF();
                MON2_LED_ON();

                break;
        }

        debugWaitUntilDone();
    }
#else
    while (1)
    {
        powerMeasurement();
        debugWaitUntilDone();
    }
#endif

    /* End of program */
    MON1_LED_ON();
    MON2_LED_OFF();
    debugWriteLine("End measurement\r\n");
    tear_down_platform();
    return 0;
}

void powerMeasurement(void)
{
    //only when state comes from I2C if anywhere else then own state
#if PRINT_STATE

    # if OWN_STATE
            //use own variable
            update_running_state(state_variable);

        #else
            update_state_main_mcu();
            update_running_state();
            // update_sample_rate();
        #endif
#endif


    if (running_state!=0) {

        ptr = (float*)(outputBuffer + 3);

        *ptr = running_state;//
        ptr++;
        //TODO: Why fails after some time

        // Fails third

        meas_fail = adapter_get_measurements_PAC1720(&dev_USB_WIREL);
        if (meas_fail){
            //        debugWriteString(meas_fail);
            //        debugWriteString("Fehler 1");
            MON1_LED_ON();
            MON2_LED_ON();
            return;
        }
        //    external_delay_function(5);

        // Fails second

        meas_fail = adapter_get_measurements_PAC1720(&dev_FPGA_VCC);
        if (meas_fail){
            //        debugWriteString("Fehler 2");
            MON1_LED_ON();
            MON2_LED_ON();
            return;
        }
        //    external_delay_function(5);


        // Does not sem to fail
        meas_fail = adapter_get_measurements_PAC1720(&dev_DAUGHTER_MCU);
        if (meas_fail){
            //        debugWriteString("Fehler 3");
            MON1_LED_ON();
            MON2_LED_ON();
            return;
        }
        //    external_delay_function(5);

        // Fails first
        meas_fail = adapter_get_measurements_PAC1720(&dev_FPGA_SRAM);
        if (meas_fail){
            //        debugWriteString("Fehler 4");
            MON1_LED_ON();
            MON2_LED_ON();
            return;
        }
        //    external_delay_function(5);

#if DAUGHTER_POWERED
        meas_fail = adapter_get_measurements_PAC1720(&dev_BATT);
        if (meas_fail)
            return;
#endif

        *ptr = dev_USB_WIREL.DEV_CH1_measurements.POWER; // wireless
        ptr++;
        *ptr = dev_USB_WIREL.DEV_CH2_measurements.POWER; // usb
        ptr++;

        *ptr = dev_FPGA_VCC.DEV_CH1_measurements.POWER; // fpga aux
        ptr++;
        *ptr = dev_FPGA_VCC.DEV_CH2_measurements.POWER; // fpga int
        ptr++;

        *ptr = dev_DAUGHTER_MCU.DEV_CH1_measurements.POWER; // daughter
        ptr++;
        *ptr = dev_DAUGHTER_MCU.DEV_CH2_measurements.POWER; // mcu

        ptr++;
        *ptr = dev_FPGA_SRAM.DEV_CH1_measurements.POWER; // sram
        ptr++;
        *ptr = dev_FPGA_SRAM.DEV_CH2_measurements.POWER; // fpga io

#if DAUGHTER_POWERED
        ptr++;
        *ptr = dev_BATT.DEV_CH1_measurements.POWER; // charge
        ptr++;
        *ptr = dev_BATT.DEV_CH2_measurements.POWER; // battery
        ptr++;
#endif
#if PRINT_VARIABLES == 3
        *ptr = dev_USB_WIREL.DEV_CH1_measurements.SOURCE_VOLTAGE; // wireless
            ptr++;
            *ptr = dev_USB_WIREL.DEV_CH1_measurements.CURRENT; // wireless
            ptr++;
            *ptr = dev_USB_WIREL.DEV_CH2_measurements.SOURCE_VOLTAGE; // usb
            ptr++;
            *ptr = dev_USB_WIREL.DEV_CH2_measurements.CURRENT; // usb
            ptr++;
            *ptr = dev_FPGA_VCC.DEV_CH1_measurements.SOURCE_VOLTAGE; // fpga aux
            ptr++;
            *ptr = dev_FPGA_VCC.DEV_CH1_measurements.CURRENT; // fpga aux
            ptr++;
            *ptr = dev_FPGA_VCC.DEV_CH2_measurements.SOURCE_VOLTAGE; // fpga int
            ptr++;
            *ptr = dev_FPGA_VCC.DEV_CH2_measurements.CURRENT; // fpga int
            ptr++;
            *ptr = dev_DAUGHTER_MCU.DEV_CH1_measurements.SOURCE_VOLTAGE; // daughter
            ptr++;
            *ptr = dev_DAUGHTER_MCU.DEV_CH1_measurements.CURRENT; // daughter
            ptr++;
            *ptr = dev_DAUGHTER_MCU.DEV_CH2_measurements.SOURCE_VOLTAGE; // mcu
            ptr++;
            *ptr = dev_DAUGHTER_MCU.DEV_CH2_measurements.CURRENT; // mcu
            ptr++;
            *ptr = dev_FPGA_SRAM.DEV_CH1_measurements.SOURCE_VOLTAGE; // sram
            ptr++;
            *ptr = dev_FPGA_SRAM.DEV_CH1_measurements.CURRENT; // sram
            ptr++;
            *ptr = dev_FPGA_SRAM.DEV_CH2_measurements.SOURCE_VOLTAGE; // fpga io
            ptr++;
            *ptr = dev_FPGA_SRAM.DEV_CH2_measurements.CURRENT; // fpga io
        #if DAUGHTER_POWERED
            ptr++;
            *ptr = dev_BATT.DEV_CH1_measurements.SOURCE_VOLTAGE; // charge
            ptr++;
            *ptr = dev_BATT.DEV_CH1_measurements.CURRENT; // charge
            ptr++;
            *ptr = dev_BATT.DEV_CH2_measurements.SOURCE_VOLTAGE; // battery
            ptr++;
            *ptr = dev_BATT.DEV_CH2_measurements.CURRENT; // battery
        #endif
#endif


        debugWriteStringLength(outputBuffer, (NUM_SENSORS * 2 * PRINT_VARIABLES + PRINT_STATE) * sizeof(float) + 8);
    }
}


/* Initialize hardware */
int8_t init_platform(void)
{
    int8_t res = PAC1720_OK;
    /* Init debug */
    debugInit(NULL);
    external_fieldbus_interface.init();

    /* Inject bus communication and delay function pointer to adapter */
    adapter_init_peripherals(&external_fieldbus_interface, external_delay_function);
    /* Configure sensors, struct instances are located in adapter */
    res = adapter_init_PAC1720_user_defined(&dev_DAUGHTER_MCU);
    if(res != PAC1720_OK) return res;
    res = adapter_init_PAC1720_user_defined(&dev_FPGA_VCC);
    if(res != PAC1720_OK) return res;
    res = adapter_init_PAC1720_user_defined(&dev_FPGA_SRAM);
    if(res != PAC1720_OK) return res;
    res = adapter_init_PAC1720_user_defined(&dev_USB_WIREL);
#if DAUGHTER_POWERED
    if(res != PAC1720_OK) return res;
    res = adapter_init_PAC1720_user_defined(&dev_BATT);
#endif
    return res;
}

/* Clean up */
void tear_down_platform(void)
{
    adapter_destroy_PAC1720(&dev_DAUGHTER_MCU);
    adapter_destroy_PAC1720(&dev_FPGA_VCC);
    adapter_destroy_PAC1720(&dev_FPGA_SRAM);
    adapter_destroy_PAC1720(&dev_USB_WIREL);
#if DAUGHTER_POWERED
    adapter_destroy_PAC1720(&dev_BATT);
#endif
}

/* Check if user sets another application state */
void check_user_input(uint8_t *state)
{
    debugGetChar();
    if (debugReadCharAvailable())
    {
        uint8_t data = debugGetChar();
        set_state(data, state);
    }
}

/* Switch state according to user input */
void set_state(uint8_t data, uint8_t *state)
{
    switch (data)
    {
        case 'U': // USB monitoring
            *state = 3;
            break;
        case 'A': // monitor ALL
            *state = 6;
            break;
        case 'f': // print raw floats
            *state = 7;
            break;
        case 'F': // FPGA monitoring
            *state = 4;
            break;
        case 'W': // Wireless monitoring
            *state = 5;
            break;
        case 'q': // Leave program
            *state = 0;
            break;
        default: // Reset measurement counters and loop without monitoring
            *state = 1;
    }
}

/* Print error in loop */
void print_error(int8_t res){
    for (;;)
    {
        char msg[64];
        sprintf(msg, "!!!Failure while initializing: %d\r\n", res);
        debugWriteLine(msg);
        external_delay_function(1000);
    }
}


void adapt_sample_rate(){

    int8_t res;

    dev_USB_WIREL.DEV_CH1_conf.CH_current_sense_sampling_time_reg  = sample_rate;
    dev_USB_WIREL.DEV_CH2_conf.CH_current_sense_sampling_time_reg  = sample_rate;
    dev_FPGA_VCC.DEV_CH1_conf.CH_current_sense_sampling_time_reg = sample_rate;
    dev_FPGA_VCC.DEV_CH2_conf.CH_current_sense_sampling_time_reg = sample_rate;
    dev_FPGA_SRAM.DEV_CH1_conf.CH_current_sense_sampling_time_reg = sample_rate;
    dev_FPGA_SRAM.DEV_CH2_conf.CH_current_sense_sampling_time_reg = sample_rate;
    dev_DAUGHTER_MCU.DEV_CH1_conf.CH_current_sense_sampling_time_reg = sample_rate;
    dev_DAUGHTER_MCU.DEV_CH2_conf.CH_current_sense_sampling_time_reg = sample_rate;

    res = adapter_init_PAC1720_user_defined(&dev_USB_WIREL);
    if(res != PAC1720_OK) print_error(res);
    res = adapter_init_PAC1720_user_defined(&dev_FPGA_VCC);
    if(res != PAC1720_OK) print_error(res);
    res = adapter_init_PAC1720_user_defined(&dev_FPGA_SRAM);
    if(res != PAC1720_OK) print_error(res);
    res = adapter_init_PAC1720_user_defined(&dev_DAUGHTER_MCU);
    if(res != PAC1720_OK) print_error(res);

}
