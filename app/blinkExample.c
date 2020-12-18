#include <avr/io.h>
#include "src/measurement/sub_system.h"
#include "src/delay/user_delay.h"
#include "src/debug/debug.h"

int main(void) {

    DDRD |= ((1<<2)|(1<<3));

   while (1) {
       MON1_LED_ON();
       MON2_LED_ON();
       user_delay_ms(100);
       MON1_LED_OFF();
       MON2_LED_OFF();
       user_delay_ms(100);
   }

}
