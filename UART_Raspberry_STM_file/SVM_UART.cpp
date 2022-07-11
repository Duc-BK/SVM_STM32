#include "SVM_UART.h"

static uint8_t Checksum(uint8_t *arr, int length)
{
    int sum = 0;
    for(int i=0; i<length-1; i++)
    {
        sum += arr[i];
    }
    return (~sum + 2);
}

static void callback(int id, uint8_t *buf, int length)
{
    Receive_Data_Processing(buf, length);
}

void UART_Open_SerialPort(int baudrate)
{
    cssl_start();
    
    serial=cssl_open("/dev/ttyS0",callback,0,
		     baudrate,8,0,1);

    if (!serial) {
		cout << cssl_geterrormsg() << endl;
    }
}

void UART_Send_MotorInfo(uint8_t row, uint8_t column, uint8_t number_of_product)
{
    Tx_Buffer.MotorInfo[0] = 'M';
    Tx_Buffer.MotorInfo[1] = 'T';
    Tx_Buffer.MotorInfo[2] = 'R';
    Tx_Buffer.MotorInfo[3] = row*10 + column;
    Tx_Buffer.MotorInfo[4] = number_of_product;
    Tx_Buffer.MotorInfo[5] = Checksum(Tx_Buffer.MotorInfo,sizeof(Tx_Buffer.MotorInfo));

    cssl_putstring(serial,(char *)Tx_Buffer.MotorInfo);
}

void Print_RFID(void)
{
    cout << "RFID: ";
    for(int i=0; i<16; i++)
        cout << Rx_Buffer.RFID[i] << " ";
    cout << endl;
}

void Print_Message(void)
{
    switch(Rx_Buffer.Message)
    {
        case No_Problem:
            cout << "Transaction completed" << endl;
        break;
        case Warning:
            cout << "Transaction completed but have to rotate a little bit" << endl;
        break;
        case Error:
            cout << "Transaction can not completed" << endl;
        break;
        default:
        break;
    }
}

void Receive_Data_Processing(uint8_t *buf, int length)
{
    // RFID data
    if(buf[0] == 'R' && buf[1] == 'I' && buf[2] == 'D' && buf[length-1] == Checksum(buf,length))
    {
        for(int i=0; i<16; i++)
            Rx_Buffer.RFID[i] = buf[i+3];
        Print_RFID();
    }

    // Message
    if(buf[0] == 'M' && buf[1] == 'S' && buf[2] == 'G' && buf[length-1] == Checksum(buf,length))
    {
        Rx_Buffer.Message = buf[3];
        Print_Message();
    }
}

int main(){

    UART_Open_SerialPort(9600);

    while(1)
    {
        UART_Send_MotorInfo(1,2,3);
        //delay(1000);
    }

    return 0;
}
