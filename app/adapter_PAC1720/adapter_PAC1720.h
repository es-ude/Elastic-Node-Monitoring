/*! @file  adapter_PAC1720.h
 *  @brief Adapter for PAC1720 sensor driver 
 */
/*!
 * @defgroup adapter
 * @{*/

#pragma once

/*! CPP guard */
#ifdef __cplusplus
extern "C"
{
#endif



/* Header includes */
#include "src/PAC1720_driver/PAC1720_driver.h"

/** default values for the configuration settings **/
#define DEFAULT_VSRC_SAMPLE_TIME VSRC_SAMPLE_TIME_20ms
#define DEFAULT_CURRENT_SAMPLE_TIME CURRENT_SAMPLE_TIME_20ms
//#define DEFAULT_VSENSE_RANGE VSRC_SENSE_RANGE_10mV
#define DEFAULT_CURRENT_RANGE CURRENT_SENSE_RANGE_40mV
#define DAUGHTER_POWERED 0
#define PRINT_MCUSTATE 1

/********************************************* Defines ************************************/
/** Defines the data direction (reading from I2C device) in i2c_start(),i2c_rep_start() */
#ifndef I2C_READ
#define I2C_READ    1
#endif
/** Defines the data direction (writing to I2C device) in i2c_start(),i2c_rep_start() */
#ifndef I2C_WRITE
#define I2C_WRITE   0
#endif

/** Defines the number of bits for an address shift in bus communication (writing or reading from I2C device)*/
static const uint8_t BUS_ADDRESS_SHIFT   = 1;

/** Define max attempts to search for active sensors */
static const uint8_t max_search_attempts = 10;


/******************************** Application specific implementation **********************/
/* Structure definition */
/*!
 * @brief  Struct that is used to define bus communication interface.
 *
 * @param init : A pointer to the init function of an application specific bus interface.
 * @param stop : A pointer to the stop communication function of an application specific bus interface.
 * @param start : A pointer to the start communication function of an application specific bus interface.
 * @param repStart : A pointer to the repeated-start communication function of an application specific bus interface.
 * @param startWait : A pointer to the start-and-wait communication function of an application specific bus interface.
 * @param write : A pointer to the write function of an application specific bus interface.
 * @param readAck : A pointer to the read-and-send-ACK function of an application specific bus interface.
 * @param readNak : A pointer to the read-and-send-NACK function of an application specific bus interface.
 *
 * @note Instantiate this struct in the application and assign the members to implementations of bus communication.
 */
struct FIELD_BUS_INTERFACE
{
    void            (*init)     (void);
    void            (*stop)     (void);
    unsigned char   (*start)    (unsigned char address);
    unsigned char   (*repStart) (unsigned char address);
    void            (*startWait)(unsigned char address);
    unsigned char   (*write)    (unsigned char data);
    unsigned char   (*readAck)  (void);
    unsigned char   (*readNak)  (void);
};

/*!
 * This typedef provides the prototype for an application specific delay function.
 *
 * @note Provide a pointer to an implementation to the adapter_init_peripherals function.
 *
 * @see adapter_init_peripherals
 *
 * @param[in] period: Time period in milliseconds
 */
typedef void (*delay_function_ptr)  (uint32_t period);

/*!
 * This typedef provides the prototype for an application specific debug function.
 *
 * @note Provide a pointer to an implementation to the adapter_init_peripherals function.
 *
 * @see adapter_init_peripherals
 *
 * @param[in] s: String to be written to a host.
 */
typedef void (*debugWriteLine_fptr) (char *s);
typedef void (*debugWriteStringLength_fptr) (char *s, uint16_t length);


/******************************** Drvice struct instances ***********************************************/
/** Sensor struct instances that are used in the current application setup.
 *  Located in the source file to keep the main function clear.
 */
extern struct PAC1720_device        dev_USB_WIREL;
extern struct PAC1720_device        dev_FPGA_VCC;
extern struct PAC1720_device        dev_FPGA_SRAM;
extern struct PAC1720_device        dev_DAUGHTER_MCU;
extern struct PAC1720_device        dev_BATT;



/******************************** Function prototypes *************************************/

/*!
 * @fn adapter_init_peripherals
 *
 * @brief Initializes the adapter's interface of bus communication and delay function,
 * provide a pointer to an instance of FIELD_BUS_INTERFACE where members are replaced by
 * bus communication functions and a pointer to a delay function.
 *
 * @param[in] fieldbus_interface : Pointer to instance of struct FIELD_BUS_INTERFACE.
 * @param[in] delay_fptr : Pointer to a platform specific delay function.
 *
 */
void adapter_init_peripherals(struct FIELD_BUS_INTERFACE *fieldbus_interface, delay_function_ptr delay_fptr);

/*!
 *  @fn adapter_init_PAC1720_from_field
 *
 *  @brief Interface to the first init function of the driver. Used to init the struct from sensor configuration.
 *  The user provided device instance struct is overwritten by the actual configuration of the sensor.
 *
 *  @param[in/out] device_ptr : Pointer to structure instance of sensor.
 *
 *  @see PAC1720_definitions.h
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success / != 0 value -> Error
 */
int8_t adapter_init_PAC1720_from_field(struct PAC1720_device *dev_ptr);

/*!
 *  @fn adapter_init_PAC1720_user_defined
 *
 *  @brief Interface to the second init function of the driver. Used to init the sensor from the
 *  configuration provided by the user defined device struct instance.
 *
 *  @param[in/out] device_ptr : Pointer to structure instance of sensor.
 *
 *  @see PAC1720_definitions.h
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success / != 0 value -> Error
 */
int8_t adapter_init_PAC1720_user_defined(struct PAC1720_device *dev_ptr);

/*!
 * @fn adapter_destroy_PAC1720
 *
 * @brief Interface to the destroy function of the driver. Internal pointers are destroyed
 * and allocated memory is freed.
 *
 * @param[in/out] device_ptr : Pointer to structure instance of sensor.
 */
void adapter_destroy_PAC1720(struct PAC1720_device *device_ptr);

/*!
 * @fn adapter_get_measurements_PAC1720
 *
 * @brief Interface to the get_measurements function of the driver. This function is to be called
 *  in order to read the measurement results from the sensor and calculate the actual values.
 *
 * @note The actual results for a channel can be accessed by the struct PAC1720_CH_measurements that is embedded into
 *  the sensor device struct for every channel. The hidden raw measurement results of the sensor can be fetched by getter functions.
 *
 * @param[in/out] device_ptr : Pointer to structure instance of sensor.
 *
  * @return Result of API execution status
 *  @retval 0 -> Success / != 0 value -> Error
 */
int8_t adapter_get_measurements_PAC1720(struct PAC1720_device *device_ptr);

/*!
 * @fn adapter_get_raw_measurements_PAC1720
 *
 * @brief Interface to the get_raw_measurements function of the driver. This function is to be called
 *  in order to read the measurement results from the sensor without calculation of the actual values.
 *
 * @note The hidden raw measurement results of the sensor can be fetched by getter functions.
 *
 * @param[in/out] device_ptr : Pointer to structure instance of sensor.
 *
  * @return Result of API execution status
 *  @retval 0 -> Success / != 0 value -> Error
 */
int8_t adapter_get_raw_measurements_PAC1720(struct PAC1720_device *device_ptr);

/*!
 * @fn adapter_write_one_shot_PAC1720
 *
 * @brief Interface to the write_one_shot function of the driver. This function is to be called
 *  in order to write to the one shot register of the sensor. The according value in the device struct instance
 *  will be written.
 *
 * @note Used to initiate a measurement cycle when the sensor is in sleep mode.
 *
 * @param[in/out] device_ptr : Pointer to structure instance of sensor.
 *
  * @return Result of API execution status
 *  @retval 0 -> Success / != 0 value -> Error
 */
int8_t adapter_write_one_shot_PAC1720(struct PAC1720_device *device_ptr);

/*!
 * @fn adapterResetMeasCounts
 *
 * @brief Resets the counter within the measurement structs of the device struct instance.
 *
 * @param[in/out] device_ptr : Pointer to structure instance of sensor.
 *
 */
void adapterResetMeasCounts(struct PAC1720_device *device_ptr);

/*!
 * @fn adapter_find_sensors
 *
 * @brief This API provides a function to find sensors on a platform by calling possible addresses.
 *
 * @note Calls all possible sensor addresses and flags a match.
 *
 * @param[in/out] addresses : Pointer to an array with size of 32* sizeof(uint8_t), initialized by 0. Found addresses are written to the array.
 * @param[in] fieldbus_interface : Pointer to an instance of struct FIELD_BUS_INTERFACE to allow bus communication.
 * @param[in] delay_fptr : Pointer to a platform specific delay function.
 *
 * @return Result of API execution status
 * @retval 0 -> Success / != 0 value -> Error
 */
uint8_t adapter_find_sensors(uint8_t *addresses, struct FIELD_BUS_INTERFACE *fieldbus_interface, delay_function_ptr delay_fptr);

/*!
 * @fn debug_print_measurements_PAC1720PAC1720
 *
 * @brief This function sends the actual measurement results from the device struct instance by the debug function.
 *
 *
 * @param[in] dev : Pointer the device structure instance to debug.
 * @param[in] debug_fptr : Pointer to a aplication specific debug function.
 *
 */
void print_measurements_PAC1720(struct PAC1720_device * dev, debugWriteLine_fptr debug_fptr);

/*!
 * @fn NOT UPDATED debug_print_measurements_PAC1720PAC1720
 *
 * @brief This function sends the actual measurement results from the device struct instance by the debug function.
 *
 *
 * @param[in] dev : Pointer the device structure instance to debug.
 * @param[in] debug_fptr : Pointer to a aplication specific debug function.
 *
 */
void print_raw_power_measurements_PAC1720(struct PAC1720_device * dev, debugWriteStringLength_fptr debug_fptr);

/*!
 * @fn print_raw_measurements_PAC1720
 *
 * @brief This function sends not calculated measurement results from the device struct instance by the debug function.
 *
 *
 * @param[in] dev : Pointer the device structure instance to debug.
 * @param[in] debug_fptr : Pointer to a aplication specific debug function.
 *
 */
void print_raw_measurements_PAC1720(struct PAC1720_device * dev, debugWriteLine_fptr debug_fptr);

/*!
 * @fn debug_PAC1720
 *
 * @brief This function sends the whole configuration of a sensor device instance by the debug function.
 *
 * @param[in] dev : Pointer the device structure instance to debug.
 * @param[in] debug_fptr : Pointer to a platform specific debug function.
 *
 */
void debug_PAC1720(struct PAC1720_device * dev, debugWriteLine_fptr debug_fptr);

/*!
 * @fn get_ADAPTER_TEST_FPTR_FIELD
 *
 * @brief This function returns an array of function pointers to the implementation.
 *  For testing purpose only.
 *
 * @return Array of void pointers.
 */
const void* get_ADAPTER_TEST_FPTR_FIELD(void);

#ifdef __cplusplus
}
#endif /* End of CPP guard */

/*
* @}
*/
