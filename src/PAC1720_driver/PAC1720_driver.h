/*! @file PAC17Driver.h
 *  @brief Sensor driver for PAC1720 sensor 
 * */
/*!
 * @defgroup driver
 * @{*/

#pragma once

/*! CPP guard */
#ifdef __cplusplus
extern "C"
{
#endif

/* Header includes */
#include "PAC1720_definitions.h"


/************************************************* Function prototypes ***************************************************/

/*!
 * @fn init_device_PAC1720_from_field
 * @brief This API initializes the provided sensor struct with the configuration that is actual on the sensor.
 *
 *  @param[in,out] device_ptr : Pointer to structure instance of sensor.
 *  @param[in] ext_write : Pointer to the user defined bus write function.
 *  @param[in] ext_read : Pointer to the user defined bus read function.
 *  @param[in] ext_delay : Pointer to the user defined delay function.
 * 
 *  @note In the initalization internal pointers are created and need to be freed after program execution.
 * 
 *  @see destroy_device_PAC1720.
 * 
 *  @return Result of API execution status
 *  @retval 0 -> Success / != 0 value -> Error 
 */
int8_t init_device_PAC1720_from_field(struct PAC1720_device *device_ptr, PAC1720_fptr ext_write, PAC1720_fptr ext_read, delay_fptr ext_delay);

/*!
 *  @fn init_device_PAC1720_user_defined
 * 
 *  @brief This API configures the sensor with the values in the provided sensor struct.
 *
 *  @param[in,out] device_ptr : Pointer to structure instance of sensor.
 *  @param[in] ext_write : Pointer to the user defined bus write function.
 *  @param[in] ext_read : Pointer to the user defined bus read function.
 *  @param[in] ext_delay : Pointer to the user defined delay function.
 * 
 *  @note In the initalization internal pointers are created and need to be freed after program execution.
 * 
 *  @see destroy_device_PAC1720.
 * 
 *  @return Result of API execution status
 *  @retval 0 -> Success / != 0 value -> Error 
 */
int8_t init_device_PAC1720_user_defined(struct PAC1720_device *device_ptr, PAC1720_fptr ext_write, PAC1720_fptr ext_read, delay_fptr ext_delay);

/*!
 * @fn destroy_device_PAC1720
 * @brief This API destroys all internal pointers that are contained in the sensor struct and frees all allocated memory.
 *
 *  @param[in,out] device_ptr : Pointer to structure instance of sensor.
 */
void destroy_device_PAC1720(struct PAC1720_device *device_ptr);

/*!
 *  @fn get_all_measurements_PAC1720
 *  @brief This API is the interface to meaurements. After calling this function, the results in the measurement struct are updated.
 *
 *  @param[in,out] device_ptr : Pointer to structure instance of sensor.
 * 
 *  @note The actual measurement results can be accessed by the sensor struct's internal measurement container.
 * 
 *  @see PAC1720_CH_measurements.
 * 
 *  @return Result of API execution status
 *  @retval 0 -> Success / != 0 value -> Error 
 */
int8_t get_all_measurements_PAC1720(struct PAC1720_device *device_ptr);

/*!
 *  @fn get_raw_measurements_PAC1720
 *  @brief This API fetches only the results from the sensor but executes no calculation to actual values.
 *
 *  @param[in,out] device_ptr : Pointer to structure instance of sensor.
 * 
 *  @note The internal values can be accessed by the provided getter functions.
 * 
 *  @return Result of API execution status
 *  @retval 0 -> Success / != 0 value -> Error 
 */
int8_t get_raw_measurements_PAC1720(struct PAC1720_device *device_ptr);

/*!
 *  @fn write_out_one_shot_register
 * 
 *  @brief This api writes to the one shot register of the sensor.
 * 
 *  @param[in] device_ptr : Pointer to structure instance of sensor.
 *
 *  @return Result of API execution status
 *  @retval 0 -> Success / != 0 value -> Error 
 */
int8_t write_out_one_shot_register(struct PAC1720_device *device_ptr);


/***************************************************** Private data getters ****************************************/

/*!
 *  @fn get_sensor_product_id
 * 
 *  @brief Returns the product id of the sensor device.
 *
 *  @param[in] device_ptr : Pointer to structure instance of sensor.
 * 
 *  @return Sensor id.
 */
uint8_t get_sensor_product_id(struct PAC1720_device *device_ptr);

/*!
 *  @fn get_sensor_manufact_id
 * 
 *  @brief Returns the manufacturer id of the sensor device.
 *
 *  @param[in] device_ptr : Pointer to structure instance of sensor.
 * 
 *  @return Manufacturer id.
 */
uint8_t get_sensor_manufact_id(struct PAC1720_device *device_ptr);

/*!
 *  @fn get_sensor_revision_id
 * 
 *  @brief Returns the sensor revision of the sensor device.
 *
 *  @param[in] device_ptr : Pointer to structure instance of sensor.
 * 
 *  @return Sensor revision.
 */
uint8_t get_sensor_revision_id(struct PAC1720_device *device_ptr);

/*!
 *  @fn get_channel_FSC
 * 
 *  @brief Returns the calculated FSC of the channel.
 *
 *  @param[in] config_ptr : Pointer to channel config struct stored in struct instance of sensor.
 * 
 *  @see PAC1720_CH_config.
 * 
 *  @return FSC.
 */
float get_channel_FSC(struct PAC1720_CH_config *config_ptr);

/*!
 *  @fn get_channel_FSV
 * 
 *  @brief Returns the calculated FSV of the channel.
 *
 *  @param[in] config_ptr : Pointer to channel config struct stored in struct instance of sensor.
 * 
 *  @see PAC1720_CH_config.
 * 
 *  @return FSV.
 */
float get_channel_FSV(struct PAC1720_CH_config *config_ptr);

/*!
 *  @fn get_channel_FSP
 * 
 *  @brief Returns the calculated FSP of the channel.
 *
 *  @param[in] config_ptr : Pointer to channel config struct stored in struct instance of sensor.
 * 
 *  @see PAC1720_CH_config.
 * 
 *  @return FSP.
 */
float get_channel_FSP(struct PAC1720_CH_config *config_ptr);

/*!
 *  @fn get_channel_src_voltage_read
 * 
 *  @brief Returns the unconverted source voltage result of the sensor device.
 *
 *  @param[in] config_ptr : Pointer to channel measurement struct stored in struct instance of sensor.
 * 
 *  @see PAC1720_CH_measurements.
 * 
 *  @return Source voltage ratio.
 */
uint16_t get_channel_src_voltage_read(struct PAC1720_CH_measurements *meas_ptr);

/*!
 *  @fn get_channel_sense_voltage_read
 * 
 *  @brief Returns the unconverted sense voltage result of the sensor device.
 *
 *  @param[in] config_ptr : Pointer to channel measurement struct stored in struct instance of sensor.
 * 
 *  @see PAC1720_CH_measurements.
 * 
 *  @return Sense voltage ratio.
 */
uint16_t get_channel_sense_voltage_read(struct PAC1720_CH_measurements *meas_ptr);

/*!
 *  @fn get_channel_pwr_ratio_read
 * 
 *  @brief Returns the unconverted power ratio result of the sensor device.
 *
 *  @param[in] config_ptr : Pointer to channel measurement struct stored in struct instance of sensor.
 * 
 *  @see PAC1720_CH_measurements.
 * 
 *  @return Power ratio.
 */
uint16_t get_channel_pwr_ratio_read(struct PAC1720_CH_measurements *meas_ptr);

/*!
 *  @fn get_TEST_DRIVER_FPTR_FIELD
 * 
 *  @brief Returns an array of pointer to implementations.
 * 
 *  @note For testing purposes only.
 * 
 *  @return Void pointer array.
 */
const void* get_TEST_DRIVER_FPTR_FIELD(void);

#ifdef __cplusplus
}
#endif /* End of CPP guard */
/** @}*/