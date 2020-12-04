#include <util/delay.h>
#include "user_delay.h"

/** Function Definition **/
/*!
 *@brief This internal API provides the platform dependend delay functionality.
 */
void user_delay_ms(uint32_t period)
{
  while (period > 0){
    _delay_ms(1); /* _delay_ms must be called with const integer argument */
    period--;
  }
}
