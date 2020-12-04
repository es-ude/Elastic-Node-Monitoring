#ifndef CURRENT_MEASUREMENT_H
#define CURRENT_MEASUREMENT_H

#include <inttypes.h>
#include <avr/interrupt.h>
#include "lib/circular_buffer/circular_buffer.h"


#define NUM_MEASUREMENTS 8
typedef union currentMeasurementTmp {
	uint16_t measurements[NUM_MEASUREMENTS];
	struct {
		uint16_t usb;
//		uint16_t monitor;
		uint16_t wireless;
		uint16_t mcu;
		uint16_t daughter;
		uint16_t vccaux;
		uint16_t vccint;
		uint16_t vccio;
		uint16_t sram;
	} breakdown;
} currentMeasurement;

typedef union currentMeasurementFloatTmp {
	float measurements[NUM_MEASUREMENTS];
	struct {
		float usb;
//		float monitor;
		float wireless;
		float mcu;
        float daughter;
        float vccaux;
		float vccint;
		float vccio;
		float sram;
	} breakdown;
} currentMeasurementFloat;

uint8_t pushCurrentMeasurement(circularBuffer *buf, currentMeasurement *measurement);
uint8_t popCurrentMeasurement(circularBuffer *buf, currentMeasurementFloat *measurement);
void currentMeasurementConvert(currentMeasurement *measurement, currentMeasurementFloat *converted);


#endif