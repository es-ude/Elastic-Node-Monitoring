/*****
 * Software Half-duplex UART Driver
 * Henry Chan (hc352 @cornell.edu)
 * Based on AVR304
 *
 * Sample Usage:
 * RECEIVING
 * sw_uart_receive_byte();
 * // Other instructions while receiving
 * if (sw_uart_state == IDLE) {
 *    x = sw_uart_rxdata;
 * }
 *
 * TRANSMITTING
 * sw_uart_send_byte(c);
 * // Other instructions while transmitting
 * if (sw_uart_state == IDLE) {
 *    //Stuff when transmission done
 * }
 ****/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>

#include "debug.h"
//#include "lib/LUFA-Setup/Helpers.h"
#include "lib/lufa/lufa.h"
#include <util/delay.h>

void debugWriteBin(uint32_t num, uint8_t length);

uint8_t debugEnabled = 0x01;
//volatile uint8_t uartData;
//volatile uint8_t uartFlag;

void debugDisable(void)
{
    // debugWriteLine("Debug disabled");
    debugEnabled = 0x0;
}
void debugEnable(void)
{
    debugEnabled = 0x1;
    // debugWriteLine("Debug enabled");
}

void debugTask(void)
{
    lufaTask();
}

void debugInit(void (*receiveHandler)(uint8_t))
{
    initLufa();
    while(!lufaOutputAvailable())
    {
        _delay_ms(100);
    }
//	setUpUsbSerial();
//	debugWriteString("\r\n\n\n\nStarting debug...");
//	debugNewLine();
}

uint16_t debugNumInputAvailable(void)
{
    return lufaNumInputAvailable();
}

void setDebugReceiveHandler(void (*receiveHandler)(uint8_t))
{
    // usb not using interrupts
    // setUpUsbSerial();
}


// ignore extra parameter used for flash readback
void debugWriteCharHelper(uint8_t c, uint8_t last)
{
    debugWriteChar(c);
}


void debugNewLine(void)
{
    debugWriteCharBlock('\r');
    debugWriteCharBlock('\n');
    debugWaitUntilDone();
}

void debugWriteLine(char *s)
{
    debugWriteString(s);
//	debugWriteChar('\n');
    debugNewLine();
}

void debugWriteString(char *s)
{
    lufaWriteString(s);
}

void debugWriteStringLength(char *s, uint16_t length)
{
    lufaWriteStringLength(s, length);
//    char *ptr = s;
//    for(uint16_t i = 0; i < length; i++)
//    {
//        writeByteAsync(*ptr++);
//    }
//	periodicUsbTask();
//    _delay_ms(10);
}

void debugWriteChar(uint8_t c)
{
    lufaWriteByte(c);
}


void debugWriteCharBlock(uint8_t c)
{
    lufaWriteByte(c);
    debugWaitUntilDone();
}

void debugReadChar(void)
{
#warning "not used"
//    int16_t incoming = lufaReadByte();
//	uartData = (uint8_t) incoming;
//	uartFlag = (incoming != -1);
}

uint8_t debugReadCharAvailable(void)
{
    return lufaReadAvailable();
}

void debugReadCharProcessed(void)
{
    // not needed for usb
}

uint8_t debugReadCharBlock(void)
{
    return lufaReadByteBlocking();
}

uint8_t debugGetChar(void)
{
    return lufaGetChar();
}


void debugWriteBool(uint8_t input)
{
    if(input)
        debugWriteString("true");
    else
        debugWriteString("false");
}

void debugWaitUntilDone(void)
{
//    flushUsb();
    lufaWaitUntilDone();
//#warning "lufa not waiting for finished"
}

uint8_t debugSending(void)
{
#warning "lufa not waiting for finished"
}


void debugAck(uint8_t c)
{
    debugWriteCharBlock(c);
    // uartWriteCharBlock(c);
}

void debugWriteHex32(uint32_t num)
{
    char *buf = (char *) malloc(10);
    sprintf(buf, "%08lX", num);
    debugWriteString(buf);
    free(buf);
}

void debugWriteBin32(uint32_t num)
{
    debugWriteBin(num, 32);
}

void debugWriteBin8(uint8_t num)
{
    debugWriteBin((uint32_t) num, 8);
}

void debugWriteBin4(uint8_t num)
{
    debugWriteBin((uint32_t) num, 4);
}

void debugWriteBin(uint32_t num, uint8_t length)
{
    debugWriteString("0b");
    uint32_t i = (uint32_t) 1 << (length - 1);
    uint32_t number = num;
    for(; i; i >>= 1)
    {
        if (number & i)
            debugWriteCharBlock('1');
        else
            debugWriteCharBlock('0');
        // number >>= 1;
    }
}

void debugWriteHex8(uint8_t num)
{
    char *buf = (char *) malloc(10);
    sprintf(buf, "%02X", num);
    debugWriteString(buf);
    free(buf);
}

void debugWriteHex8Helper(uint8_t num, uint8_t last)
{
    debugWriteHex8(num);
}

void debugWriteHex16(uint16_t num)
{
    char *buf = (char *) malloc(10);
    sprintf(buf, "%04X", num);
    debugWriteString(buf);
    free(buf);
}

void debugWriteDec8(uint8_t num)
{
    char *buf = (char *) malloc(10);
    sprintf(buf, "%2d", num);
    debugWriteString(buf);
    free(buf);
}

void debugWriteFloat(float num)
{
    char *buf = (char *) malloc(10);
    sprintf(buf, "%.2f", num);
    debugWriteString(buf);
    free(buf);
}

void debugWriteFloatFull(float num)
{
    char *buf = (char *) malloc(100);
    sprintf(buf, "%f", num);
    debugWriteString(buf);
    free(buf);
}

void debugWriteDec16(uint16_t num)
{
    char *buf = (char *) malloc(10);
    sprintf(buf, "%u", num);
    debugWriteString(buf);
    free(buf);
}

void debugWriteDec32(uint32_t num)
{
    char *buf = (char *) malloc(10);
    sprintf(buf, "%lu", num);
    debugWriteString(buf);
    free(buf);
}

void debugWriteDec32S(int32_t num)
{
    char *buf = (char *) malloc(10);
    sprintf(buf, "%ld", num);
    debugWriteString(buf);
    free(buf);
}

void debugDone(void)
{
    debugWriteLine("\n$$");
}

void debugReady(void)
{
    debugWriteLine("\n%%");
}

// void printUartHandler(void)
// {
// 	debugWriteString("Uart Handler: ");
// // 	debugWriteHex16((uint16_t) (sendingBuf.head));
// // 	debugWriteChar(' ');
// // 	debugWriteHex16((uint16_t) (sendingBuf.last));
// // 	debugWriteChar(' ');

// 	debugWriteHex32((uint16_t) (getUartReceiveHandler()));
// 	debugWriteChar(' ');
// 	debugWriteHex8(UCSR1B);
// 	debugWriteChar(' ');
// 	debugWriteDec8((SREG & _BV(7)) != 0);
// 	debugNewLine();


// // 	debugWriteHex32((uint32_t) (&uartReceiveHandler));
// // 	debugWriteChar(' ');

// // 	return 0;
// // 	// return (uint16_t) uartReceiveHandler;
// }