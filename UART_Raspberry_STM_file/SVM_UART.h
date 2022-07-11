#ifndef _SVM_H_
#define _SVM_H_

#include <iostream>
#include <string.h>
#include <errno.h>
// #include <wiringPi.h>
// #include <wiringSerial.h>
#include "cssl.h"

using namespace std;

cssl_t *serial;

struct
{
    uint8_t MotorInfo[6];
}Tx_Buffer;

struct
{
    uint8_t RFID[16];
    uint8_t Message;
}Rx_Buffer;

typedef enum
{
    No_Problem = 0x01,
    Warning    = 0x02,
    Error      = 0x03,
}MESSAGE_TYPEDEF;

void UART_Open_SerialPort(int baudrate);
void UART_Send_MotorInfo(uint8_t row, uint8_t column, uint8_t number_of_product);
void Print_RFID(void);
void Print_Message(void);
void Receive_Data_Processing(uint8_t *buf, int length);

//Build command
// g++ -Wall -O -o my_program my_program.c -lcssl

#endif
