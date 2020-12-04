/*! @file  adapter_PAC1720.c
 *  @brief Adapter for PAC1720 sensor driver 
 */
/*!
 * @defgroup adapter
 * @{
 */

/** Header includes **/
#include <stdio.h>
#include "adapter_PAC1720.h"


/******************************** Instances ******************************************/

/*!
 * @brief  Application dependent instances. 
 */
/* The struct holding communication function pointers used in this file */
struct FIELD_BUS_INTERFACE *internal_fieldbus_interface;

/** The platform dependend delay function used in this file */
delay_function_ptr internal_delay;

/*!
 * @brief  Device structures holding the sensor configuration and results. 
 */
/*! USB MONITOR device struct */
struct PAC1720_device dev_USB_WIREL =
{
    /* Optional name for the device */
	.DEV_name_opt                                           = "USB_WIRELESS_SENSOR",
    /* Bus address */
	.DEV_sensor_address                                     = 0x4A,
    /* Global sensor configuration */
    .DEV_configuration_reg                                  = CONFIG_ALL_CH_ENABLED,
    /* Conversion rate of sensor */
	.DEV_conversion_rate_reg                                = CONVERSION_CONTINUOUS,
    /* One shot register of sensor */
	.DEV_one_shot_reg                                       = ONE_SHOT_DEFAULT,
    /* Channel mask register of sensor*/
    .DEV_mask_reg                                           = MASK_ALL_CH_ALERT_ASSERT,
    /* Optional name for channel one */
    .DEV_CH1_conf.CH_name_opt                               = "WIRELESS_VCC",
    /* Resistance of sense resistor at channel one */
    .DEV_CH1_conf.CH_current_sense_resistor_value           = 0.82f,
    /* Sampling time of current sense for channel one */
    .DEV_CH1_conf.CH_current_sense_sampling_time_reg        = DEFAULT_CURRENT_SAMPLE_TIME,
    /* Current sense sample averaging of channel one */
    .DEV_CH1_conf.CH_current_sense_sampling_average_reg     = SAMPLE_AVERAGING_DISABLED,
    /* Full scale voltage range for current sensing */
    .DEV_CH1_conf.CH_current_sense_FSR_reg                  = DEFAULT_CURRENT_RANGE,
    /* High limit of current for channel one */
    .DEV_CH1_conf.CH_current_sense_high_limit_reg           = CURRENT_SENSE_HIGH_LIMIT_DEFAULT,
    /* Low limit of current for channel one */
	.DEV_CH1_conf.CH_current_sense_low_limit_reg            = CURRENT_SENSE_LOW_LIMIT_DEFAULT,
    /* Sampling time of source voltage sense */
    .DEV_CH1_conf.CH_source_voltage_sampling_time_reg       = DEFAULT_VSRC_SAMPLE_TIME,
    /* Source voltage sample averaging of channel one */
    .DEV_CH1_conf.CH_source_voltage_sampling_average_reg    = SAMPLE_AVERAGING_DISABLED,
    /* Source voltage high limit for channel one */
    .DEV_CH1_conf.CH_source_voltage_high_limit_reg          = SOURCE_VOLTAGE_HIGH_LIMIT_DEFAULT,
    /* Source voltage low limit for channel one */
	.DEV_CH1_conf.CH_source_voltage_low_limit_reg           = SOURCE_VOLTAGE_LOW_LIMIT_DEFAULT,
    /* Optional name for channel two */
    .DEV_CH2_conf.CH_name_opt                               = "USB_VCC",
    /* Resistance of sense resistor at channel two */
    .DEV_CH2_conf.CH_current_sense_resistor_value           = 0.152f,
    /* Sampling time of current sense for channel two*/
    .DEV_CH2_conf.CH_current_sense_sampling_time_reg        = DEFAULT_CURRENT_SAMPLE_TIME,
    /* Current sense sample averaging of channel two */
    .DEV_CH2_conf.CH_current_sense_sampling_average_reg     = SAMPLE_AVERAGING_DISABLED,
    /* Full scale voltage range for current sensing */
    .DEV_CH2_conf.CH_current_sense_FSR_reg                  = DEFAULT_CURRENT_RANGE,
    /* High limit of current for channel two */
    .DEV_CH2_conf.CH_current_sense_high_limit_reg           = CURRENT_SENSE_HIGH_LIMIT_DEFAULT,
    /* Low limit of current for channel two */
	.DEV_CH2_conf.CH_current_sense_low_limit_reg            = CURRENT_SENSE_LOW_LIMIT_DEFAULT,
    /* Sampling time of source voltage sense */
    .DEV_CH2_conf.CH_source_voltage_sampling_time_reg       = DEFAULT_VSRC_SAMPLE_TIME,
    /* Source voltage sample averaging of channel two */
    .DEV_CH2_conf.CH_source_voltage_sampling_average_reg    = SAMPLE_AVERAGING_DISABLED,
    /* Source voltage high limit for channel two */
    .DEV_CH2_conf.CH_source_voltage_high_limit_reg          = SOURCE_VOLTAGE_HIGH_LIMIT_DEFAULT,
    /* Source voltage low limit for channel two */
	.DEV_CH2_conf.CH_source_voltage_low_limit_reg           = SOURCE_VOLTAGE_LOW_LIMIT_DEFAULT,
};

/*! FPGA MONITOR device struct */
struct PAC1720_device dev_FPGA_VCC = {
        .DEV_name_opt                                           = "FPGA_VCC_SENSOR",
        .DEV_sensor_address                                     = 0x4F,
        .DEV_configuration_reg                                  = CONFIG_ALL_CH_ENABLED,
        .DEV_conversion_rate_reg                                = CONVERSION_CONTINUOUS,
        .DEV_one_shot_reg                                       = ONE_SHOT_DEFAULT,
        .DEV_mask_reg                                           = MASK_ALL_CH_ALERT_ASSERT,

        .DEV_CH1_conf.CH_name_opt                               = "FPGA_VCCAUX_MON",
        .DEV_CH1_conf.CH_current_sense_resistor_value           = 0.82f,
        .DEV_CH1_conf.CH_current_sense_sampling_time_reg        = DEFAULT_CURRENT_SAMPLE_TIME,
        .DEV_CH1_conf.CH_current_sense_sampling_average_reg     = SAMPLE_AVERAGING_DISABLED,
        .DEV_CH1_conf.CH_current_sense_FSR_reg                  = DEFAULT_CURRENT_RANGE,
        .DEV_CH1_conf.CH_current_sense_high_limit_reg           = CURRENT_SENSE_HIGH_LIMIT_DEFAULT,
        .DEV_CH1_conf.CH_current_sense_low_limit_reg            = CURRENT_SENSE_LOW_LIMIT_DEFAULT,
        .DEV_CH1_conf.CH_source_voltage_sampling_time_reg       = DEFAULT_VSRC_SAMPLE_TIME,
        .DEV_CH1_conf.CH_source_voltage_sampling_average_reg    = SAMPLE_AVERAGING_DISABLED,
        .DEV_CH1_conf.CH_source_voltage_high_limit_reg          = SOURCE_VOLTAGE_HIGH_LIMIT_DEFAULT,
        .DEV_CH1_conf.CH_source_voltage_low_limit_reg           = SOURCE_VOLTAGE_LOW_LIMIT_DEFAULT,

        .DEV_CH2_conf.CH_name_opt                               = "FPGA_VCCINT_MON",
        .DEV_CH2_conf.CH_current_sense_resistor_value           = 0.82f,
        .DEV_CH2_conf.CH_current_sense_sampling_time_reg        = DEFAULT_CURRENT_SAMPLE_TIME,
        .DEV_CH2_conf.CH_current_sense_sampling_average_reg     = SAMPLE_AVERAGING_DISABLED,
        .DEV_CH2_conf.CH_current_sense_FSR_reg                  = DEFAULT_CURRENT_RANGE,
        .DEV_CH2_conf.CH_current_sense_high_limit_reg           = CURRENT_SENSE_HIGH_LIMIT_DEFAULT,
        .DEV_CH2_conf.CH_current_sense_low_limit_reg            = CURRENT_SENSE_LOW_LIMIT_DEFAULT,
        .DEV_CH2_conf.CH_source_voltage_sampling_time_reg       = DEFAULT_VSRC_SAMPLE_TIME,
        .DEV_CH2_conf.CH_source_voltage_sampling_average_reg    = SAMPLE_AVERAGING_DISABLED,
        .DEV_CH2_conf.CH_source_voltage_high_limit_reg          = SOURCE_VOLTAGE_HIGH_LIMIT_DEFAULT,
        .DEV_CH2_conf.CH_source_voltage_low_limit_reg           = SOURCE_VOLTAGE_LOW_LIMIT_DEFAULT,
};

/*! SRAM and IO device struct */
struct PAC1720_device dev_FPGA_SRAM = {
        .DEV_name_opt                                           = "FPGA_SRAM_VCC",
        .DEV_sensor_address                                     = 0x4D,
        .DEV_configuration_reg                                  = CONFIG_ALL_CH_ENABLED,
        .DEV_conversion_rate_reg                                = CONVERSION_CONTINUOUS,
        .DEV_one_shot_reg                                       = ONE_SHOT_DEFAULT,
        .DEV_mask_reg                                           = MASK_ALL_CH_ALERT_ASSERT,

        .DEV_CH1_conf.CH_name_opt                               = "SRAM_VDD",
        .DEV_CH1_conf.CH_current_sense_resistor_value           = 0.82f,
        .DEV_CH1_conf.CH_current_sense_sampling_time_reg        = DEFAULT_CURRENT_SAMPLE_TIME,
        .DEV_CH1_conf.CH_current_sense_sampling_average_reg     = SAMPLE_AVERAGING_DISABLED,
        .DEV_CH1_conf.CH_current_sense_FSR_reg                  = DEFAULT_CURRENT_RANGE,
        .DEV_CH1_conf.CH_current_sense_high_limit_reg           = CURRENT_SENSE_HIGH_LIMIT_DEFAULT,
        .DEV_CH1_conf.CH_current_sense_low_limit_reg            = CURRENT_SENSE_LOW_LIMIT_DEFAULT,
        .DEV_CH1_conf.CH_source_voltage_sampling_time_reg       = DEFAULT_VSRC_SAMPLE_TIME,
        .DEV_CH1_conf.CH_source_voltage_sampling_average_reg    = SAMPLE_AVERAGING_DISABLED,
        .DEV_CH1_conf.CH_source_voltage_high_limit_reg          = SOURCE_VOLTAGE_HIGH_LIMIT_DEFAULT,
        .DEV_CH1_conf.CH_source_voltage_low_limit_reg           = SOURCE_VOLTAGE_LOW_LIMIT_DEFAULT,

        .DEV_CH2_conf.CH_name_opt                               = "FPGA_VCCIO_MON",
        .DEV_CH2_conf.CH_current_sense_resistor_value           = 0.82f,
        .DEV_CH2_conf.CH_current_sense_sampling_time_reg        = DEFAULT_CURRENT_SAMPLE_TIME,
        .DEV_CH2_conf.CH_current_sense_sampling_average_reg     = SAMPLE_AVERAGING_DISABLED,
        .DEV_CH2_conf.CH_current_sense_FSR_reg                  = DEFAULT_CURRENT_RANGE,
        .DEV_CH2_conf.CH_current_sense_high_limit_reg           = CURRENT_SENSE_HIGH_LIMIT_DEFAULT,
        .DEV_CH2_conf.CH_current_sense_low_limit_reg            = CURRENT_SENSE_LOW_LIMIT_DEFAULT,
        .DEV_CH2_conf.CH_source_voltage_sampling_time_reg       = DEFAULT_VSRC_SAMPLE_TIME,
        .DEV_CH2_conf.CH_source_voltage_sampling_average_reg    = SAMPLE_AVERAGING_DISABLED,
        .DEV_CH2_conf.CH_source_voltage_high_limit_reg          = SOURCE_VOLTAGE_HIGH_LIMIT_DEFAULT,
        .DEV_CH2_conf.CH_source_voltage_low_limit_reg           = SOURCE_VOLTAGE_LOW_LIMIT_DEFAULT,
};

/*! WIRELESS MONITOR device struct */
struct PAC1720_device dev_DAUGHTER_MCU  = {
        .DEV_name_opt                                           = "DAUGHTER_MCU_SENSOR",
        .DEV_sensor_address                                     = 0x29,
        .DEV_configuration_reg                                  = CONFIG_ALL_CH_ENABLED,
        .DEV_conversion_rate_reg                                = CONVERSION_CONTINUOUS,
        .DEV_one_shot_reg                                       = ONE_SHOT_DEFAULT,
        .DEV_mask_reg                                           = MASK_ALL_CH_ALERT_ASSERT,

        .DEV_CH1_conf.CH_name_opt                               = "DAUGHTER_VCC",
        .DEV_CH1_conf.CH_current_sense_resistor_value           = 0.82f,
        .DEV_CH1_conf.CH_current_sense_sampling_time_reg        = DEFAULT_CURRENT_SAMPLE_TIME,
        .DEV_CH1_conf.CH_current_sense_sampling_average_reg     = SAMPLE_AVERAGING_DISABLED,
        .DEV_CH1_conf.CH_current_sense_FSR_reg                  = DEFAULT_CURRENT_RANGE,
        .DEV_CH1_conf.CH_current_sense_high_limit_reg           = CURRENT_SENSE_HIGH_LIMIT_DEFAULT,
        .DEV_CH1_conf.CH_current_sense_low_limit_reg            = CURRENT_SENSE_LOW_LIMIT_DEFAULT,
        .DEV_CH1_conf.CH_source_voltage_sampling_time_reg       = DEFAULT_VSRC_SAMPLE_TIME,
        .DEV_CH1_conf.CH_source_voltage_sampling_average_reg    = SAMPLE_AVERAGING_DISABLED,
        .DEV_CH1_conf.CH_source_voltage_high_limit_reg          = SOURCE_VOLTAGE_HIGH_LIMIT_DEFAULT,
        .DEV_CH1_conf.CH_source_voltage_low_limit_reg           = SOURCE_VOLTAGE_LOW_LIMIT_DEFAULT,

        .DEV_CH2_conf.CH_name_opt                               = "MCU_VCC",
        .DEV_CH2_conf.CH_current_sense_resistor_value           = 0.82f,
        .DEV_CH2_conf.CH_current_sense_sampling_time_reg        = DEFAULT_CURRENT_SAMPLE_TIME,
        .DEV_CH2_conf.CH_current_sense_sampling_average_reg     = SAMPLE_AVERAGING_DISABLED,
        .DEV_CH2_conf.CH_current_sense_FSR_reg                  = DEFAULT_CURRENT_RANGE,
        .DEV_CH2_conf.CH_current_sense_high_limit_reg           = CURRENT_SENSE_HIGH_LIMIT_DEFAULT,
        .DEV_CH2_conf.CH_current_sense_low_limit_reg            = CURRENT_SENSE_LOW_LIMIT_DEFAULT,
        .DEV_CH2_conf.CH_source_voltage_sampling_time_reg       = DEFAULT_VSRC_SAMPLE_TIME,
        .DEV_CH2_conf.CH_source_voltage_sampling_average_reg    = SAMPLE_AVERAGING_DISABLED,
        .DEV_CH2_conf.CH_source_voltage_high_limit_reg          = SOURCE_VOLTAGE_HIGH_LIMIT_DEFAULT,
        .DEV_CH2_conf.CH_source_voltage_low_limit_reg           = SOURCE_VOLTAGE_LOW_LIMIT_DEFAULT,
};



#if DAUGHTER_POWERED
/*! BATTERY device struct */
struct PAC1720_device dev_BATT= {
        .DEV_name_opt                                           = "BATTERY_SENSOR",
        .DEV_sensor_address                                     = 0x4C,
        .DEV_configuration_reg                                  = CONFIG_ALL_CH_ENABLED,
        .DEV_conversion_rate_reg                                = CONVERSION_CONTINUOUS,
        .DEV_one_shot_reg                                       = ONE_SHOT_DEFAULT,
        .DEV_mask_reg                                           = MASK_ALL_CH_ALERT_ASSERT,

        .DEV_CH1_conf.CH_name_opt                               = "CHARGE",
        .DEV_CH1_conf.CH_current_sense_resistor_value           = 0.02f,
        .DEV_CH1_conf.CH_current_sense_sampling_time_reg        = DEFAULT_CURRENT_SAMPLE_TIME,
        .DEV_CH1_conf.CH_current_sense_sampling_average_reg     = SAMPLE_AVERAGING_DISABLED,
        .DEV_CH1_conf.CH_current_sense_FSR_reg                  = DEFAULT_CURRENT_RANGE,
        .DEV_CH1_conf.CH_current_sense_high_limit_reg           = CURRENT_SENSE_HIGH_LIMIT_DEFAULT,
        .DEV_CH1_conf.CH_current_sense_low_limit_reg            = CURRENT_SENSE_LOW_LIMIT_DEFAULT,
        .DEV_CH1_conf.CH_source_voltage_sampling_time_reg       = DEFAULT_VSRC_SAMPLE_TIME,
        .DEV_CH1_conf.CH_source_voltage_sampling_average_reg    = SAMPLE_AVERAGING_DISABLED,
        .DEV_CH1_conf.CH_source_voltage_high_limit_reg          = SOURCE_VOLTAGE_HIGH_LIMIT_DEFAULT,
        .DEV_CH1_conf.CH_source_voltage_low_limit_reg           = SOURCE_VOLTAGE_LOW_LIMIT_DEFAULT,

        .DEV_CH2_conf.CH_name_opt                               = "BATTERY",
        .DEV_CH2_conf.CH_current_sense_resistor_value           = 0.02f,
        .DEV_CH2_conf.CH_current_sense_sampling_time_reg        = DEFAULT_CURRENT_SAMPLE_TIME,
        .DEV_CH2_conf.CH_current_sense_sampling_average_reg     = SAMPLE_AVERAGING_DISABLED,
        .DEV_CH2_conf.CH_current_sense_FSR_reg                  = DEFAULT_CURRENT_RANGE,
        .DEV_CH2_conf.CH_current_sense_high_limit_reg           = CURRENT_SENSE_HIGH_LIMIT_DEFAULT,
        .DEV_CH2_conf.CH_current_sense_low_limit_reg            = CURRENT_SENSE_LOW_LIMIT_DEFAULT,
        .DEV_CH2_conf.CH_source_voltage_sampling_time_reg       = DEFAULT_VSRC_SAMPLE_TIME,
        .DEV_CH2_conf.CH_source_voltage_sampling_average_reg    = SAMPLE_AVERAGING_DISABLED,
        .DEV_CH2_conf.CH_source_voltage_high_limit_reg          = SOURCE_VOLTAGE_HIGH_LIMIT_DEFAULT,
        .DEV_CH2_conf.CH_source_voltage_low_limit_reg           = SOURCE_VOLTAGE_LOW_LIMIT_DEFAULT,
};
#endif

/********************************************* Function prototypes *************************************************/

/*!
 * @fn adapter_fbus_write
 * @brief This function provides the interface to the bus write function that is required by the driver.
 *
 * @note A pointer to this function is stored in every device struct instance. The function utilizes the provided 
 * bus communication struct.
 *
 * @param[in] sensor_address : Bus address of the sensor.
 * @param[in] reg_address : Register address of the sensor to be written to.
 * @param[in] data_ptr : Pointer to data that are to be written to the sensor.
 * @param[in] len : Length of the data to be written in bytes.
 * 
 * @return Result of API execution status
 * @retval 0 -> Success / != 0 value -> Error 
 */
int8_t adapter_fbus_write(const uint8_t sensor_address, const uint8_t reg_address, uint8_t *data_ptr, const uint16_t len);

/*!
 * @fn adapter_fbus_read
 * @brief This function provides the interface to the bus read function that is required by the driver.
 *
 * @note A pointer to this function is stored in every device struct instance. The function utilizes the provided 
 * bus communication struct.
 *
 * @param[in] sensor_address : Bus address of the sensor.
 * @param[in] reg_address : Register address of the sensor to be read from.
 * @param[in] data_ptr : Pointer to data that are to be read in from the sensor.
 * @param[in] len : Length of the data to be read in bytes.
 * 
 * @return Result of API execution status
 * @retval 0 -> Success / != 0 value -> Error 
 */
int8_t adapter_fbus_read(const uint8_t sensor_address, const uint8_t reg_address, uint8_t *data_ptr, const uint16_t len);

/*!
 * @fn adapter_delay
 * @brief This function provides the interface to the delay function that is required by the driver.
 *
 * @note A pointer to this function is stored in every device struct instance. Internally this function calls
 * the user provided delay function.
 *
 * @param[in] period : The period of time in ms that should be delayed.
 *  
 */
void adapter_delay(uint32_t period);

/*!
 * @fn fbus_write_loop
 * @brief Helper function for adapter_fbus_write.
 *
 * @param[in] data_ptr : Pointer to data that are to be written to the sensor.
 * @param[in] len : Length of the data to be written in bytes.
 * 
 * @return Result of API execution status
 * @retval 0 -> Success / != 0 value -> Error 
 */
int8_t fbus_write_loop(uint8_t *data_ptr, const uint16_t len);

/*!
 * @fn fbus_read_loop
 * @brief Helper function for adapter_fbus_read.
 *
 * @param[in] data_ptr : Pointer to data that are to be read in from the sensor.
 * @param[in] len : Length of the data to be read in bytes.
 * 
 * @return Result of API execution status
 * @retval 0 -> Success / != 0 value -> Error 
 */
int8_t fbus_read_loop(uint8_t *data_ptr, const uint16_t len);

/*!
 * @fn check_mandatory_dev_settings
 * @brief Helper function to check if mandatory values are set in the device struct.
 *
 * @param[in] dev_ptr : Pointer to a device struct instance.
 * 
 * @return Result 
 * @retval true -> Success / false -> Error 
 */
bool check_mandatory_dev_settings(struct PAC1720_device *dev_ptr);

/*!
 * @fn poll_fbus
 * @brief Helper function to poll sensors in the find_sensors function.
 *
 * @param[in/out] addresses : Pointer to an array with size of 32*sizeof(unit8_t).
 * @param[in] fieldbus_interface : Pointer to instance of struct FIELD_BUS_INTERFACE.
 * @param[in] loop_var : The actual loop count.
 * 
 * @return Result of API execution status
 * @retval 0 -> Success / != 0 value -> Error 
 */
uint8_t poll_fbus(uint8_t *addresses, struct FIELD_BUS_INTERFACE *fieldbus_interface, uint8_t loop_var);

/*!
 * @fn check_peripherals_initialized
 * 
 * @brief Helper function to check if the internal communication interface and delay function is initialized.
 * 
 * @return Result 
 * @retval true -> Success / false -> Error 
 */
bool check_peripherals_initialized(void);


/*!
 * @fn sensor_address_out_of_range
 * 
 * @brief Helper function to check if the sensor addresses of a device struct instance are valid.
 * 
 * @param[in] address : A sensor address.
 * 
 * @return Result 
 * @retval true -> Success / false -> Error 
 */
bool sensor_address_out_of_range(const uint8_t address);

/*!
 * @fn set_fieldbus_ptr
 * 
 * @brief Helper function to initialize the internal bus communication instance.
 * 
 * @param[in] external_fieldbus_interface : Pointer to an instance of struct FIELD_BUS_INTERFACE.
 * 
 */
void set_fieldbus_ptr(struct FIELD_BUS_INTERFACE *external_fieldbus_interface);

/*!
 * @fn set_delay_ptr
 * 
 * @brief Helper function to initialize the internal delay function.
 * 
 * @param[in] external_delay : Pointer to an implementation of a delay function.
 * 
 */
void set_delay_ptr(delay_function_ptr external_delay);


/********************************************* Function definitions **************************************************/

/*!
 * @brief Initializes the internal bus communication interface and delay function.
 */
void adapter_init_peripherals(struct FIELD_BUS_INTERFACE *fieldbus_interface, delay_function_ptr delay_fptr)
{
    if (fieldbus_interface != NULL && delay_fptr != NULL)
    {
        set_fieldbus_ptr(fieldbus_interface);
        set_delay_ptr(delay_fptr);
    } 
}

/*!
 * @brief Initializes the sensor device struct by configuration on the sensor.
 */
int8_t adapter_init_PAC1720_from_field(struct PAC1720_device *dev_ptr)
{
    if (dev_ptr != NULL && check_mandatory_dev_settings(dev_ptr) && check_peripherals_initialized())
    {
        return init_device_PAC1720_from_field(dev_ptr, &adapter_fbus_write, &adapter_fbus_read, &adapter_delay);
    }
    else
    {
        return PAC1720_INIT_ERROR;
    }
}

/*!
 * @brief Initializes the sensor with the values of the device struct instance.
 */
int8_t adapter_init_PAC1720_user_defined(struct PAC1720_device *dev_ptr)
{
    if (dev_ptr != NULL && check_mandatory_dev_settings(dev_ptr) && check_peripherals_initialized())
    {
        return init_device_PAC1720_user_defined(dev_ptr, &adapter_fbus_write, &adapter_fbus_read, &adapter_delay);
    }
    else
    {
        return PAC1720_INIT_ERROR;
    }
}

/*!
 * @brief Destroys all internal pointers of device struct.
 */
void adapter_destroy_PAC1720(struct PAC1720_device *device_ptr)
{
    destroy_device_PAC1720(device_ptr);
}

/*!
 * @brief Gets all measurement results of the sensor and calculates the actual values.
 */
int8_t adapter_get_measurements_PAC1720(struct PAC1720_device *device_ptr)
{
    return get_all_measurements_PAC1720(device_ptr);
}

/*!
 * @brief Gets all measurement results of the sensor without calculating actual values.
 */
int8_t adapter_get_raw_measurements_PAC1720(struct PAC1720_device *device_ptr)
{
    return get_raw_measurements_PAC1720(device_ptr);
}

/*!
 * @brief Writes to the one shot register of the sensor.
 */
int8_t adapter_write_one_shot_PAC1720(struct PAC1720_device *device_ptr)
{
    return write_out_one_shot_register(device_ptr);
}

/*!
 * @brief Sets the counter of measurements to zero.
 */
void adapterResetMeasCounts(struct PAC1720_device *device_ptr)
{
    device_ptr->DEV_CH1_measurements.meas_cnt = 0;
    device_ptr->DEV_CH2_measurements.meas_cnt = 0;
}

/*!
 * @brief Searches for sensors (addresses) on the platform.
 */
uint8_t adapter_find_sensors(uint8_t *addresses, struct FIELD_BUS_INTERFACE *fieldbus_interface, delay_function_ptr delay_fptr)
{
    uint8_t no_match = 1;
    uint8_t count = 0;
    uint8_t res = 0;
    /* Loop over all possible sensor addresses */
    for (uint8_t loop_var = 0; loop_var < SENSOR_ADDRESS_NUMBER; loop_var++)
    {
        /* Loop for max_search_attempts times and poll sensor address */
        while (no_match && count <= max_search_attempts)
        {
            /* Call address by bus communication and set address in array when match */
            no_match = poll_fbus(addresses, fieldbus_interface, loop_var);
            /* Increment  brak condition and sleep 10ms */
            if (no_match)
            {
                count++;
                delay_fptr(10);
            } 
        }
        /* If sensor has responsed count match results up */
        if(!no_match)
        {
            res++;
        }
        /* reset break conditions */
        no_match = 1;
        count = 0;
    }
    return res;
}

/*!
 * @brief Internal bus write function required by driver.
 */
int8_t adapter_fbus_write(const uint8_t sensor_address, const uint8_t reg_address, uint8_t *data_ptr, const uint16_t len)
{  
    uint8_t result = PAC1720_OK;
    /* Send start communication (blocking) */
    internal_fieldbus_interface->startWait((sensor_address << BUS_ADDRESS_SHIFT) + I2C_WRITE); 
    /* Write register address */
    result = internal_fieldbus_interface->write(reg_address);
    if(result != PAC1720_OK) 
    {
        return result;
    }
    /* Loop over data and send */
    result = fbus_write_loop(data_ptr, len);

    /** Stop communication */
    internal_fieldbus_interface->stop();
    return result;
}

/*!
 * @brief Internal bus read function required by driver.
 */
int8_t adapter_fbus_read(const uint8_t sensor_address, const uint8_t reg_address, uint8_t *data_ptr, const uint16_t len)
{   
    uint8_t result = PAC1720_OK;
    /* Send start communication (blocking) */
    internal_fieldbus_interface->startWait((sensor_address << BUS_ADDRESS_SHIFT) + I2C_WRITE);
    /* Write register address */
    result = internal_fieldbus_interface->write(reg_address); 
    if(result != PAC1720_OK)
    {
        return result;
    }
    /* Repeat start in read mode (blocking) */
    result = internal_fieldbus_interface->repStart((sensor_address << BUS_ADDRESS_SHIFT) + I2C_READ);
    if(result != PAC1720_OK) 
    {
        return result;
    }
    /* Loop over incoming data and store them in data structure */
    result = fbus_read_loop(data_ptr, len);

    /** Stop communication */
    internal_fieldbus_interface->stop();
    return result;
}

/* User defined delay function */
void adapter_delay(uint32_t period)
{
    /* Provided delay function */
    internal_delay(period);
}

/*!
 * @brief Write loop function.
 */
int8_t fbus_write_loop(uint8_t *data_ptr, const uint16_t len)
{
    uint8_t result = PAC1720_OK;
    /** Write to #len registers, increase the address #len times, return error if fails */
    for(uint16_t i = 0; i < len; i++)
    {
        result = internal_fieldbus_interface->write(*data_ptr);
        if(result != PAC1720_OK) return result;
        data_ptr++;
    }
    return result;
}

/*!
 * @brief Read loop function.
 */
int8_t fbus_read_loop(uint8_t *data_ptr, const uint16_t len)
{
    uint8_t result = PAC1720_OK;
    /* Read from #len registers, increase the address #len times */
    for(uint16_t i = 0; i < len; i++)
    {   /** Read register and send ACK */
        if(i < len -1)  *data_ptr = internal_fieldbus_interface->readAck();
        /* After the last read send NACK */
        else            *data_ptr = internal_fieldbus_interface->readNak(); 
        data_ptr++;
    }
    return result;
}

/*!
 * @brief Polls sensor addresses within find_sensors function.
 */
uint8_t poll_fbus(uint8_t *addresses, struct FIELD_BUS_INTERFACE *fieldbus_interface, uint8_t loop_var)
{
    /* Get the current sensor address to poll out of lookup-table, indexed by loop variable */
    uint8_t sensor_addr = PAC1720_addresses[loop_var];
    /* Start communication (non-blocking) and stop afterwards*/
    uint8_t no_match = fieldbus_interface->start((sensor_addr << BUS_ADDRESS_SHIFT) + I2C_WRITE);
    fieldbus_interface->stop();
    /* If sensor has ACKed store address at its index in the array */
    if(!no_match)
    {
        uint8_t *offset = addresses + loop_var;
        *offset = sensor_addr;
    }
    /* Return match or fail */
    return no_match;
}

/*!
 * @brief Helper function checks if communication interface and delay function is initialized.
 */
bool check_peripherals_initialized(void)
{
    return (internal_fieldbus_interface != NULL && internal_delay != NULL);
}

/*!
 * @brief Helper function checks if mandatory settings are provided.
 */
bool check_mandatory_dev_settings(struct PAC1720_device *dev_ptr) 
{
    /* Check if sensor address is valid and resistances are set */
    if ( sensor_address_out_of_range(dev_ptr->DEV_sensor_address)               
         || dev_ptr->DEV_CH1_conf.CH_current_sense_resistor_value == 0  
         || dev_ptr->DEV_CH2_conf.CH_current_sense_resistor_value == 0 )
    {
        return false;
    }
    return true;
}

/*!
 * @brief Helper function checks if sensor address is within existing addresses.
 */
bool sensor_address_out_of_range(const uint8_t address)
{
    if(address == 0x18 || (address > 0x27 && address < 0x2F) || (address > 0x47 && address < 0x50))
    {
        return false;
    }
    return true;
}

/*!
 * @brief Helper function that initializes the internal bus communication.
 */
void set_fieldbus_ptr(struct FIELD_BUS_INTERFACE *external_fieldbus_interface)
{
    internal_fieldbus_interface = external_fieldbus_interface;
}

/*!
 * @brief Helper function that csets the internal delay function.
 */
void set_delay_ptr(delay_function_ptr external_delay)
{
    internal_delay = external_delay;
}

/*!
 * @brief Prints the measurements by the debug function.
 */
void print_measurements_PAC1720(struct PAC1720_device * dev, debugWriteLine_fptr debug_fptr)
{
    char msg[256];
    /* Print measurement count */
    sprintf(msg, "%u ", dev->DEV_CH1_measurements.meas_cnt);
    debug_fptr(msg);
    /* Print channel one measurements */
    sprintf(msg, "%s %f %f %f ", dev->DEV_CH1_conf.CH_name_opt, dev->DEV_CH1_measurements.CURRENT, dev->DEV_CH1_measurements.SOURCE_VOLTAGE, dev->DEV_CH1_measurements.POWER);
    debug_fptr(msg);
    /* Print channel two measurements */
    sprintf(msg, "%s %f %f %f\r\n", dev->DEV_CH2_conf.CH_name_opt, dev->DEV_CH2_measurements.CURRENT, dev->DEV_CH2_measurements.SOURCE_VOLTAGE, dev->DEV_CH2_measurements.POWER);
    debug_fptr(msg);
}


/*!
 * @brief Prints the measurements as raw floats by the debug function.
 */
void print_raw_power_measurements_PAC1720(struct PAC1720_device * dev, debugWriteStringLength_fptr debug_fptr)
{
    /* Print channel one measurements */
    debug_fptr((char*) &dev->DEV_CH1_measurements.POWER, sizeof(float));
    /* Print channel two measurements */
    debug_fptr((char*) &dev->DEV_CH2_measurements.POWER, sizeof(float));
}

/*!
 * @brief Prints the raw measurements results by the provided debug function.
 */
void print_raw_measurements_PAC1720(struct PAC1720_device * dev, debugWriteLine_fptr debug_fptr)
{
    char msg[256];
    /* Print measurement count */
    sprintf(msg, "%u", dev->DEV_CH1_measurements.meas_cnt);
    debug_fptr(msg);
    /* Print channel one measurements */
    sprintf(msg, "%s %x %x %x", dev->DEV_CH1_conf.CH_name_opt, get_channel_sense_voltage_read(&dev->DEV_CH1_measurements), get_channel_src_voltage_read(&dev->DEV_CH1_measurements), get_channel_pwr_ratio_read(&dev->DEV_CH1_measurements));
    debug_fptr(msg);
    /* Print channel two measurements */
    sprintf(msg, "%s %x %x %x", dev->DEV_CH2_conf.CH_name_opt, get_channel_sense_voltage_read(&dev->DEV_CH2_measurements), get_channel_src_voltage_read(&dev->DEV_CH2_measurements), get_channel_pwr_ratio_read(&dev->DEV_CH2_measurements));
    debug_fptr(msg);
}
 
///*!
// * @brief Prints the configuration of a sensor struct instance by the provided debug function.
// */
//void debug_PAC1720(struct PAC1720_device * dev, debugWriteLine_fptr debug_fptr){
//    char msg[512];
//    /* Print global configuration */
//    sprintf(msg, "Name: %s,\r\n\r\naddr: %x, conf_reg: %x, conv_rate_reg: %x, oneshot_reg: %x, mask_reg: %x\r\n\r\n", dev->DEV_name_opt, dev->DEV_sensor_address, dev->DEV_configuration_reg, dev->DEV_conversion_rate_reg, dev->DEV_one_shot_reg, dev->DEV_mask_reg);
//    debug_fptr(msg);
//    /* Print status regs */
//    uint8_t high_lim_status_reg = 0;
//    high_lim_status_reg |= dev->DEV_CH1_measurements.conversion_done << 7;
//    high_lim_status_reg |= dev->DEV_CH2_measurements.sense_voltage_high_limit << 3;
//    high_lim_status_reg |= dev->DEV_CH2_measurements.source_voltage_high_limit << 2;
//    high_lim_status_reg |= dev->DEV_CH1_measurements.sense_voltage_high_limit << 1;
//    high_lim_status_reg |= dev->DEV_CH1_measurements.source_voltage_high_limit;
//    uint8_t low_lim_status_reg = 0;
//    low_lim_status_reg |= dev->DEV_CH2_measurements.sense_voltage_low_limit << 3;
//    low_lim_status_reg |= dev->DEV_CH2_measurements.source_voltage_low_limit << 2;
//    low_lim_status_reg |= dev->DEV_CH1_measurements.sense_voltage_low_limit << 1;
//    low_lim_status_reg |= dev->DEV_CH1_measurements.source_voltage_low_limit;
//    sprintf(msg, "high_lim_status_reg: %x, low_lim_status_reg: %x\r\n\r\n", high_lim_status_reg, low_lim_status_reg);
//    debug_fptr(msg);
//    /* Print sampling time, averaging configuration and FSR values */
//    uint8_t Vsrc_sample_reg = 0;
//    Vsrc_sample_reg |= dev->DEV_CH2_conf.CH_source_voltage_sampling_time_reg << 6;
//    Vsrc_sample_reg |= dev->DEV_CH2_conf.CH_source_voltage_sampling_average_reg << 4;
//    Vsrc_sample_reg |= dev->DEV_CH1_conf.CH_source_voltage_sampling_time_reg << 2;
//    Vsrc_sample_reg |= dev->DEV_CH1_conf.CH_source_voltage_sampling_average_reg;
//    uint8_t Vsense_ch1_sample_reg = 0;
//    Vsense_ch1_sample_reg |= dev->DEV_CH1_conf.CH_current_sense_sampling_time_reg << 4;
//    Vsense_ch1_sample_reg |= dev->DEV_CH1_conf.CH_current_sense_sampling_average_reg << 2;
//    Vsense_ch1_sample_reg |= dev->DEV_CH1_conf.CH_current_sense_FSR_reg;
//    uint8_t Vsense_ch2_sample_reg = 0;
//    Vsense_ch2_sample_reg |= dev->DEV_CH2_conf.CH_current_sense_sampling_time_reg << 4;
//    Vsense_ch2_sample_reg |= dev->DEV_CH2_conf.CH_current_sense_sampling_average_reg << 2;
//    Vsense_ch2_sample_reg |= dev->DEV_CH2_conf.CH_current_sense_FSR_reg;
//    sprintf(msg, "Vsrc_sample_reg: %x, Vsense_ch1_sample_reg: %x, Vsense_ch2_sample_reg: %x\r\n\r\n", Vsrc_sample_reg, Vsense_ch1_sample_reg, Vsense_ch2_sample_reg);
//    debug_fptr(msg);
//    /* Print Full Scale values for channel one */
//    sprintf(msg, "Name: %s,\r\nRes: %f\r\n", dev->DEV_CH1_conf.CH_name_opt, dev->DEV_CH1_conf.CH_current_sense_resistor_value);
//    debug_fptr(msg);
//    sprintf(msg, "FSC: %f, FSV: %f, FSP: %f\r\n", get_channel_FSC(&dev->DEV_CH1_conf), get_channel_FSV(&dev->DEV_CH1_conf), get_channel_FSP(&dev->DEV_CH1_conf));
//    debug_fptr(msg);
//    /* Print measurement results, raw and calculated for channel one */
//    sprintf(msg, "CH1 COUNT: %d,\r\n", dev->DEV_CH1_measurements.meas_cnt);
//    debug_fptr(msg);
//    sprintf(msg, "v_sense_voltage_reg: %x,  v_source_voltage_reg: %x, power_ratio_reg: %x\r\n", get_channel_sense_voltage_read(&dev->DEV_CH1_measurements), get_channel_src_voltage_read(&dev->DEV_CH1_measurements), get_channel_pwr_ratio_read(&dev->DEV_CH1_measurements));
//    debug_fptr(msg);
//    sprintf(msg, "CH1 SENSE_VOLTAGE: %fmV, CURRENT: %fA, SOURCE_VOLTAGE: %fV, POWER: %fW\r\n\r\n", dev->DEV_CH1_measurements.SENSE_VOLTAGE, dev->DEV_CH1_measurements.CURRENT, dev->DEV_CH1_measurements.SOURCE_VOLTAGE, dev->DEV_CH1_measurements.POWER);
//    debug_fptr(msg);
//    /* Print Full Scale values for channel two */
//    sprintf(msg, "Name: %s,\r\nRes: %f\r\n", dev->DEV_CH2_conf.CH_name_opt, dev->DEV_CH2_conf.CH_current_sense_resistor_value);
//    debug_fptr(msg);
//    sprintf(msg, "FSC: %f, FSV: %f, FSP: %f\r\n\r\n", get_channel_FSC(&dev->DEV_CH2_conf), get_channel_FSV(&dev->DEV_CH2_conf), get_channel_FSP(&dev->DEV_CH2_conf));
//    debug_fptr(msg);
//    /* Print measurement results, raw and calculated for channel two */
//    sprintf(msg, "CH2 COUNT: %d,\r\n", dev->DEV_CH2_measurements.meas_cnt);
//    debug_fptr(msg);
//    sprintf(msg, "v_sense_voltage_reg: %x,  v_source_voltage_reg: %x, power_ratio_reg: %x\r\n", get_channel_sense_voltage_read(&dev->DEV_CH2_measurements), get_channel_src_voltage_read(&dev->DEV_CH2_measurements), get_channel_pwr_ratio_read(&dev->DEV_CH2_measurements));
//    debug_fptr(msg);
//    sprintf(msg, "CH2 SENSE_VOLTAGE: %fmV, CURRENT: %fA, SOURCE_VOLTAGE: %fV, POWER: %fW\r\n\r\n", dev->DEV_CH2_measurements.SENSE_VOLTAGE, dev->DEV_CH2_measurements.CURRENT, dev->DEV_CH2_measurements.SOURCE_VOLTAGE, dev->DEV_CH2_measurements.POWER);
//    debug_fptr(msg);
//    /* Print limit settings */
//    sprintf(msg, "CH1_current_sense_high_limit_reg: %x, CH2_current_sense_high_limit_reg: %x\r\n", dev->DEV_CH1_conf.CH_current_sense_high_limit_reg, dev->DEV_CH2_conf.CH_current_sense_high_limit_reg);
//    debug_fptr(msg);
//    sprintf(msg, "CH1_current_sense_low_limit_reg: %x, CH2_current_sense_low_limit_reg: %x\r\n", dev->DEV_CH1_conf.CH_current_sense_low_limit_reg, dev->DEV_CH2_conf.CH_current_sense_low_limit_reg);
//    debug_fptr(msg);
//    sprintf(msg, "CH1_source_voltage_high_limit_reg: %x, CH2_source_voltage_high_limit_reg: %x\r\n", dev->DEV_CH1_conf.CH_source_voltage_high_limit_reg, dev->DEV_CH2_conf.CH_source_voltage_high_limit_reg);
//    debug_fptr(msg);
//    sprintf(msg, "CH1_source_voltage_low_limit_reg: %x, CH2_source_voltage_low_limit_reg: %x\r\n\r\n", dev->DEV_CH1_conf.CH_source_voltage_low_limit_reg, dev->DEV_CH2_conf.CH_source_voltage_low_limit_reg);
//    debug_fptr(msg);
//    /* Print sensor information */
//    sprintf(msg, "id: %x, man: %x, rev: %x\r\n\r\n\r\n\r\n", get_sensor_product_id(dev), get_sensor_manufact_id(dev), get_sensor_revision_id(dev));
//    debug_fptr(msg);
//}

/*!
 * @brief Returns pointers to implementation for testing purposes.
 */
const void* get_ADAPTER_TEST_FPTR_FIELD(void)
{
    static const void* test_fptr_field[] =  {
                                                 (void*) &adapter_fbus_write,
                                                 (void*) &adapter_fbus_read,
                                                 (void*) &adapter_delay,
                                                 (void*) &sensor_address_out_of_range,
                                                 (void*) NULL,
                                                 (void*) &poll_fbus,
                                                 (void*) &set_fieldbus_ptr,
                                                 (void*) &set_delay_ptr,
                                                 (void*) &check_mandatory_dev_settings,
                                                 (void*) &check_peripherals_initialized,
                                                 (void*) &fbus_write_loop,
                                                 (void*) &fbus_read_loop
                                            };

    return &test_fptr_field;
}

/*
* @}
*/
