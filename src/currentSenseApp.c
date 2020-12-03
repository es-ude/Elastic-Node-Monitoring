//#include "include/currentSenseApp.h"

/************************************** Dependencies to be injected ************************************/
#include "src/adapter_PAC1720/adapter_PAC1720.h"
#include "lib/i2cmaster/i2cmaster.h"
#include "lib/delay/user_delay.h"
#include "lib/debug/debug.h"
#include <stdio.h>
#include <avr/io.h>

#include "lib/measurement/sub_system.h"



void powerMeasurement();
void new_powerMeasurement(uint8_t *state_of_the_main_MCU);
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
    int8_t res = init_platform();
    if(res != PAC1720_OK) print_error(res);
    /* Debug string */
//    char msg[64];
    /** User controlled state */
    uint8_t state = 7;
    /* Result of get_measurements */



    outputBuffer[0] = 0x01;
    outputBuffer[1] = 0x02;
    outputBuffer[2] = 0x03;
    outputBuffer[(NUM_SENSORS*2*PRINT_VARIABLES+PRINT_STATE)*sizeof(float)+8 -5] = 5;
    outputBuffer[(NUM_SENSORS*2*PRINT_VARIABLES+PRINT_STATE)*sizeof(float)+8 -4] = 4;
    outputBuffer[(NUM_SENSORS*2*PRINT_VARIABLES+PRINT_STATE)*sizeof(float)+8 -3] = 3;
    outputBuffer[(NUM_SENSORS*2*PRINT_VARIABLES+PRINT_STATE)*sizeof(float)+8 -2] = 2;
    outputBuffer[(NUM_SENSORS*2*PRINT_VARIABLES+PRINT_STATE)*sizeof(float)+8 -1] = 1;


uint16_t timer_1_sec = 0;

DDRD |= ((1<<2)|(1<<3));
MON1_LED_OFF();
MON2_LED_OFF();


#if START_AND_STOP_ACTIVE
while (state_of_main_mcu != START_OF_MEASUREMENT)
{
    /* code wait*/
    update_state_main_mcu(&state_of_main_mcu);
}
#endif




    while(1){
        
        MON2_LED_ON();
        MON1_LED_OFF();

        #if OWN_STATE == 0
            update_state_main_mcu(&state_of_main_mcu);
            update_running_state(&running_state, &state_of_main_mcu);
        #endif
        
        # if OWN_STATE
            //use own variable
            update_running_state(&running_state, state_variable); 
        #endif
        
               

        switch (running_state)
        {
        case 0:
            /* code */
            break;
        default:
                      
            powerMeasurement();
            //new_powerMeasurement(&state_of_main_mcu); 
            break;
        }


        
        #if START_AND_STOP_ACTIVE
        if(state_of_main_mcu == END_OF_MEASUREMENT){
            MON1_LED_ON();
            MON2_LED_OFF();
           break; 
        }
        #endif
        
       
    //    if(state_of_the_main_MCU){
    //        sample_rate =  (state_of_the_main_MCU & sample_mask) >> 4;
    //        powerMeasurement();
    //    }

    //    switch (running_state)
    //    {
    //    case 15:
    //        /* the final result is being received after that the measurement should stop */
    //        //something like last_measurement()
    //        break;
    //    case 0:
    //         //no measurement neccecary
    //         break;

    //    default:
    //         new_powerMeasurement(*state_of_main_mcu);
    //        break;
    //    }
       
        debugWaitUntilDone();


    }
    /* End of program */
    MON1_LED_ON();
    MON2_LED_OFF();
    debugWriteLine("End measurement\r\n");
    tear_down_platform();
    return 0;
}

void powerMeasurement(void)
{
    ptr = (float*)(outputBuffer + 3);

    meas_fail = adapter_get_measurements_PAC1720(&dev_USB_WIREL);
    if (meas_fail){
        MON1_LED_ON();
        MON2_LED_ON();
        return;
    }        
    meas_fail = adapter_get_measurements_PAC1720(&dev_FPGA_VCC);
    if (meas_fail){
        MON1_LED_ON();
        MON2_LED_ON();
        return;
    } 
    meas_fail = adapter_get_measurements_PAC1720(&dev_DAUGHTER_MCU);
    if (meas_fail){
        MON1_LED_ON();
        MON2_LED_ON();
        return;
    } 
    meas_fail = adapter_get_measurements_PAC1720(&dev_FPGA_SRAM);
    if (meas_fail){
        MON1_LED_ON();
        MON2_LED_ON();
        return;
    } 
#if DAUGHTER_POWERED
    meas_fail = adapter_get_measurements_PAC1720(&dev_BATT);
    if (meas_fail)
        return;
#endif
    //only when state comes from I2C if anywhere else then own state
    *ptr = running_state;//
    ptr++;
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

//    debugWriteFloatFull(dev_FPGA_VCC.DEV_CH1_measurements.POWER);
//    debugNewLine();


    debugWriteStringLength(outputBuffer, (NUM_SENSORS*2*PRINT_VARIABLES + PRINT_STATE)*sizeof(float)+8);
}


//new powerMeasurement
void new_powerMeasurement(uint8_t *state_of_main_mcu){

    uint8_t sample_time = 2.5;
    // running_state = *state_of_the_main_MCU & STATE_MASK;
    // sample_rate = *state_of_the_main_MCU & SAMPLE_MASK;
    //running_state = get_running_state();
    //update_running_state(&running_state, running_state);
    sample_rate = get_sample_rate();

    

    powerMeasurement();
    debugWaitUntilDone();

    

    //     switch(sample_rate)
    //     {
    //     case 0:                             //standard
    //        sample_time = SAMPLE_RATE_STANDARD;
    //        break;
    //     case 1:                             //for reconfig
    //        sample_time = SAMPLE_RATE_RECONFIG;
    //        break;
    //     case 2:                             //for sending input
    //        sample_time = SAMPLE_RATE_SEND_INPUT;
    //        break;
    //     case 3:                             //for execution
    //        sample_time = SAMPLE_RATE_EXEC;
    //        break;
    //     case 4:                             //for receiving result
    //        sample_time = SAMPLE_RATE_RECEIVE_RESULT;
    //        break;
    //     default:
    //        sample_time = 100;
    //        break;
    // }
    // for (int i = 0; i < sample_time; i++)
    // {
    //     user_delay_ms(1);
    // }
    
    //powerMeasurement();
    //debugWaitUntilDone();

    

   



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
    debugReadChar();
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
        sprintf(msg, "Failure while initializing: %d\r\n", res);
        debugWriteLine(msg);
        external_delay_function(1000);
    }
}


