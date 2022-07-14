#ifndef _SVM_H_
#define _SVM_H_

#include <iostream>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include "cssl.h"

using namespace std;

cssl_t *serial;

struct
{
    uint8_t MotorInfo[6];
}Tx_Buffer;

struct
{
    uint8_t Data[20];
    uint8_t RFID[16];
    uint8_t Message;
}Rx_Buffer;

struct Status_Flag
{
    bool RFID_enable;
    bool RFID_available;
    bool Msg_enable;
    bool Msg_available;
};
extern struct Status_Flag Flag;

typedef enum
{
    No_Problem = 0x01,
    Warning    = 0x02,
    Error      = 0x03,
}MESSAGE_TYPEDEF;
extern MESSAGE_TYPEDEF Message;

int count_data = 0;

void UART_Open_SerialPort(int baudrate);
void UART_Send_MotorInfo(uint8_t row, uint8_t column, uint8_t number_of_product);
void Print_RFID(void);
void Print_Message(void);
void Receive_Data_Processing(uint8_t *buf, int length);
void Get_RFID(uint8_t *array_16bytes);
uint8_t Get_Message(void);

//Build command
// g++ -Wall -O -o my_program my_program.c -lcssl

#endif
