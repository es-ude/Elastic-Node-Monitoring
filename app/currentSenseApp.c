//#include "include/currentSenseApp.h"

/************************************** Dependencies to be injected ************************************/
#include "app/adapter_PAC1720/adapter_PAC1720.h"
#include "src/i2cmaster/i2cmaster.h"
#include "src/delay/user_delay.h"
#include "lib/debug/debug.h"
#include <stdio.h>
#include <avr/io.h>

#include "src/measurement/sub_system.h"

#define MON1_LED_ON()      (PORTD |= (1<<3))
#define MON1_LED_OFF()      (PORTD &= ~(1<<3))
#define MON2_LED_ON()        (PORTD |= (1<<2))
#define MON2_LED_OFF()       (PORTD &= ~(1<<2))

void powerMeasurement(void);
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
//state indicator (state variable)
extern uint8_t state_of_main_mcu;
//the "true" running state
extern uint8_t running_state;
//the resulting sample rate
extern uint8_t sample_rate;

void update_state_LUTNet(uint8_t* p_state)
{
    uint8_t ret=0x00;
    uint8_t pin_state;
    // Pin map, all these pin on monitor should works as input mode.
    // Monitor    =======   LUTNet_HW
    // GND                  GND
    // MISO(PB3)  <======   Ready
    // SCK(PB1)   <======   copy
    // MOSI(PB2)  <======   enable

//    DDRB |=0x08;             // &= ~(0x02|0x04|0x08); // input mode
    PORTB = 0xff;


    pin_state = PINB;
    if ((pin_state&0x04)==0x04){
        ret |= 0x04;        // enable -> 0x04
    }

    if ((pin_state&0x02)==0x02){
        ret |= 0x02;        // copy -> 0x02
        MON2_LED_ON();
    }else{
        MON2_LED_OFF();
    }

    if ((pin_state&0x08)==0x08){
        ret |= 0x01;        // copy -> 0x02
        MON1_LED_ON();
    }else{
        MON1_LED_OFF();
    }

    *p_state = ret;
}


int main(void)
{
    DDRB =0x00;             // &= ~(0x02|0x04|0x08); // input mode
    MON1_LED_ON();
    /* Init the hardware, print error if fail */
    int8_t res = init_platform();
    debugWriteString("init");
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
    MON1_LED_ON();
    MON2_LED_ON();
    user_delay_ms(3000); // to make sure we still can program
    MON1_LED_OFF();
    MON2_LED_OFF();

//    while(1) {
//        DDRB |=0x08;             // &= ~(0x02|0x04|0x08); // input mode
//        PORTB = 0xff;
//        user_delay_ms(1000);
//        PORTB = 0x00;
//        user_delay_ms(1000);
//    }
    while(1){
        // reset timer
//        update_state_main_mcu(&state_of_main_mcu);
        update_state_LUTNet(&state_of_main_mcu);
        running_state = state_of_main_mcu & STATE_MASK;
        // CHAO: seems I need to change two lines above to adapt to lutnet project

        if(running_state){
            new_powerMeasurement(&state_of_main_mcu);
        }

        debugWaitUntilDone();
    }
    /* End of program */
    debugWriteLine("End measurement\r\n");
    tear_down_platform();
    return 0;
}

void powerMeasurement(void)
{
    ptr = (float*)(outputBuffer + 3);

    meas_fail = adapter_get_measurements_PAC1720(&dev_12V_5V);
    if (meas_fail)
        return;
    meas_fail = adapter_get_measurements_PAC1720(&dev_MCU_AUX_VCC);
    if (meas_fail)
        return;
    meas_fail = adapter_get_measurements_PAC1720(&dev_FPGA_IO);
    if (meas_fail)
        return;
    meas_fail = adapter_get_measurements_PAC1720(&dev_INT_BRAM_VCC);
    if (meas_fail)
        return;
#if DAUGHTER_POWERED
    meas_fail = adapter_get_measurements_PAC1720(&dev_BATT);
    if (meas_fail)
        return;
#endif
    *ptr = running_state;//iic_read_from_device(64) & STATE_MASK;//get_running_state();                       //alternative: running_state   //iic_read_from_device(64) & state_mask; //state of the Main MCU
    ptr++;
    *ptr = dev_12V_5V.DEV_CH1_measurements.POWER; // wireless
    ptr++;
    *ptr = dev_12V_5V.DEV_CH2_measurements.POWER; // usb
    ptr++;
    *ptr = dev_MCU_AUX_VCC.DEV_CH1_measurements.POWER; // fpga aux
    ptr++;
    *ptr = dev_MCU_AUX_VCC.DEV_CH2_measurements.POWER; // fpga int
    ptr++;
    *ptr = dev_INT_BRAM_VCC.DEV_CH1_measurements.POWER; // daughter
    ptr++;
    *ptr = dev_INT_BRAM_VCC.DEV_CH2_measurements.POWER; // mcu
    ptr++;
    *ptr = dev_FPGA_IO.DEV_CH1_measurements.POWER; // sram
    ptr++;
    *ptr = dev_FPGA_IO.DEV_CH2_measurements.POWER; // fpga io
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

//    switch(sample_rate)
//    {
//        case 0:                             //standard
//           sample_time = SAMPLE_RATE_STANDARD;
//           break;
//        case 1:                             //for reconfig
//           sample_time = SAMPLE_RATE_RECONFIG;
//           break;
//        case 2:                             //for sending input
//           sample_time = SAMPLE_RATE_SEND_INPUT;
//           break;
//        case 3:                             //for execution
//           sample_time = SAMPLE_RATE_EXEC;
//           break;
//        case 4:                             //for receiving result
//           sample_time = SAMPLE_RATE_RECEIVE_RESULT;
//           break;
//        default:
//           sample_time = 100;
//           break;
//    }
//    for (int i = 0; i < sample_time; i++)
//    {
//        user_delay_ms(1);
//    }

//    powerMeasurement();
//    debugWaitUntilDone();
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
    res = adapter_init_PAC1720_user_defined(&dev_FPGA_IO);
    if(res != PAC1720_OK) return res;
    res = adapter_init_PAC1720_user_defined(&dev_MCU_AUX_VCC);
    if(res != PAC1720_OK) return res;
    res = adapter_init_PAC1720_user_defined(&dev_INT_BRAM_VCC);
    if(res != PAC1720_OK) return res;
    res = adapter_init_PAC1720_user_defined(&dev_12V_5V);
#if DAUGHTER_POWERED
    if(res != PAC1720_OK) return res;
    res = adapter_init_PAC1720_user_defined(&dev_BATT);
#endif
    return res;
}

/* Clean up */
void tear_down_platform(void)
{
    adapter_destroy_PAC1720(&dev_FPGA_IO);
    adapter_destroy_PAC1720(&dev_MCU_AUX_VCC);
    adapter_destroy_PAC1720(&dev_INT_BRAM_VCC);
    adapter_destroy_PAC1720(&dev_12V_5V);
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
        sprintf(msg, "Failure while initializing: %d\r\n", res);
        debugWriteLine(msg);
        external_delay_function(1000);
    }
}

