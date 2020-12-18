//
// Created by David Lewakis / david.lewakis@gmail.com on 03/12/2020.
//

#include <avr/io.h>
#include "src/measurement/sub_system.h"
#include "src/delay/user_delay.h"
#include "lib/debug/debug.h"

int main(void) {

    debg

    DDRD |= ((1<<2)|(1<<3));

   while (1) {

       MON1_LED_ON();
       MON2_LED_ON();
       _delay_ms(100);
       MON1_LED_OFF();
       MON2_LED_OFF();

   }

}