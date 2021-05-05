#include <avr/io.h>

#include "src/delay/user_delay.h"
#include "src/debug/debug.h"
#include "src/elasticNodeMonitoring/elasticNodeMonitoring.h"

int main(void) {

    debugInit(NULL);

    MON_LED_INIT();
    debugWriteLine("LED's init.");

   while (1) {
       MON1_LED_ON();
       MON2_LED_ON();
       debugWriteLine("LED's on.");
       user_delay_ms(1000);
       MON1_LED_OFF();
       MON2_LED_OFF();
       debugWriteLine("LED's off.");
       user_delay_ms(1000);
   }

}
