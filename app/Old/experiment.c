#include <avr/power.h>
#include <avr/wdt.h>
#include "lib/debug/debug.h"
#include "lib/current_sense/v4/current_sense.h"
#include "lib/current_sense/v4/current_measurement.h"
#include <util/delay.h>
#include <stddef.h>

void setup(void);
void printDemoConfiguration(void);
void startMeasurements(void);
//void stopMeasurements(void);
void transmitCurrentMeasurements(void);

int main(void) {
    debugInit(NULL);
    findCurrentSensors();
	for (int i = 0; i < 100; ++i) {
		debugWriteString("hello there\n");
	}
// 	_delay_ms(5000);
//	for (int i = 0; i < 100; ++i) {
//		debugWriteString("hello there\n");
//	}
//	_delay_ms(2000);

    setup();
    debugWriteString("finished setup\n\n");
//    printDemoConfiguration();
    debugWriteString("printed demo config\n");

//	while (1) {
//		debugWriteString("done\n");
//		_delay_ms(10000);
//	}

//	startMeasurements();
    debugWriteString("started measurements\n");
    int first = 1;
    uint8_t sleep;
    while (1) {
//        transmitCurrentMeasurements();

        switch(SENSE_BITS)
        {
            case 12:
                sleep = 80;
                break;
            case 11:
                sleep = 40;
                break;
            case 10:
                sleep = 20;
                break;
            case 9:
                sleep = 10;
                break;
            case 8:
                sleep = 5;
                break;
            case 7:
                sleep = 3;
                break;
            default:
                sleep = 255;
                break;
        }
//        debugWriteDec8(sleep);
//		debugNewLine();
        _delay_ms(sleep );
    }

}

void setup(void){
    debugWriteString("entered setup\n");
//	TODO: Add this back once i know wtf is going on.
//    debugInit(NULL);
    initCurrentSense();
    currentSenseConfig();
    MCUSR &= ~(1 << WDRF);
    wdt_disable();
    clock_prescale_set(clock_div_1);
    debugWriteString("Done setup\n");
}

//void printDemoConfiguration(void){
//    debugWriteString("Current Config: ");
//    debugWriteBin8(CURRENT_CONFIG);
//    debugWriteChar(' ');
//    debugWriteDec16(CURRENT_SENSE_OCR);
//    debugNewLine();
//
//    printCurrent(CURRENT_USB);
//    printCurrent(CURRENT_MONITOR);
//    printCurrent(CURRENT_WIRELESS);
//    printCurrent(CURRENT_MCU);
//    printCurrent(CURRENT_FPGA_VCCINT);
//    printCurrent(CURRENT_FPGA_VCCAUX);
//    debugNewLine();
//}

void startMeasurements(void){
    debugWriteString("Starting measurements");
    currentSenseClearMeasurements();
    currentSenseBegin();
}

//void stopMeasurements(void){
//	debugWriteString("Stopping measurements");
//	currentSenseEnd();
//}

void transmitCurrentMeasurements(void){
    cli();
    currentMeasurementFloat current = fetchCurrentMeasurementFloat();
    for (int i = 0; i < NUM_MEASUREMENTS; ++i) {
        debugWriteFloatFull(current.measurements[i]);
//		_delay_ms(1);
        debugWriteChar(',');
//		_delay_ms(1);
    }
//	debugWriteChar(0xff);
//	debugWriteStringLength((char *) current.measurements, sizeof(currentMeasurementFloat));
//	debugWriteChar(0xff);
//	debugWriteChar('\n');
    debugNewLine();
    sei();
}
