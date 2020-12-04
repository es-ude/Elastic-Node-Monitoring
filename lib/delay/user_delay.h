/*
 * user_delay.h
 */

/**
* @file	user_delay.h
*
* @brief Provides a user defined delay functionality for the specific platform.
*/

#pragma once

/*! CPP guard */
#ifdef __cplusplus
extern "C"
{
#endif

#include <inttypes.h>

/* function prototype declarations */
/*!
 * @fn user_delay_ms
 * 
 * @brief The platform dependend delay function is called internally for the specified period.
 */
void user_delay_ms(uint32_t period);


#ifdef __cplusplus
}
#endif /* End of CPP guard */
