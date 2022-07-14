#include "SVM_UART.h"

struct Status_Flag Flag;
MESSAGE_TYPEDEF Message;

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

void Get_RFID(uint8_t *array_16bytes)
{
    for(int i=0; i<16; i++)
        array_16bytes[i] = Rx_Buffer.RFID[i];

    for(int i=0; i<16; i++)
        Rx_Buffer.RFID[i] = 0;
}

uint8_t Get_Message(void)
{
    uint8_t temp = Rx_Buffer.Message;
    Rx_Buffer.Message = 0x00;
    return temp;
}

void Receive_Data_Processing(uint8_t *buf, int length)
{
    count_data+=length;
    for(int i=0;i<length;i++) 
    {
        Rx_Buffer.Data[count_data-1] = buf[i];
    }
    if(count_data == 3)
    {
        if(Rx_Buffer.Data[0] == 'R' && Rx_Buffer.Data[1] == 'I' && Rx_Buffer.Data[2] == 'D')
        {
            Flag.RFID_enable = true;
        }
        else if(Rx_Buffer.Data[0] == 'M' && Rx_Buffer.Data[1] == 'S' && Rx_Buffer.Data[2] == 'G')
        {
            Flag.Msg_enable = true;
        }
    }
    if(Flag.RFID_enable)
    {
        for(int i=0;i<length;i++) 
        {
            Rx_Buffer.Data[count_data-1] = buf[i];
        }
        if(count_data == 20)
        {
            if(Rx_Buffer.Data[19] == Checksum(Rx_Buffer.Data,20))
            {
                for(int i=0; i<16; i++)
                    Rx_Buffer.RFID[i] = Rx_Buffer.Data[i+3];
                Flag.RFID_available = true;
                cout << "Receive RFID sucessfully" << endl;
            }
            Flag.RFID_enable = false;
            count_data = 0;
        }
    }
    else if(Flag.Msg_enable)
    {
        for(int i=0;i<length;i++) 
        {
            Rx_Buffer.Data[count_data-1] = buf[i];
        }
        if(count_data == 5)
        {
            if(Rx_Buffer.Data[4] == Checksum(Rx_Buffer.Data,5))
            {
                Rx_Buffer.Message = Rx_Buffer.Data[3];
                Flag.Msg_available = true;
                switch(Rx_Buffer.Message)
                {
                    case No_Problem:
                        cout << "No problem" << endl;
                    break;
                    case Warning:
                        cout << "Warning" << endl;
                    break;
                    case Error:
                        cout << "Error" << endl;
                    break;
                }
            }
            Flag.Msg_enable = false;
            count_data = 0;
        }
    }
}

int main(){

    UART_Open_SerialPort(9600);

    while(1)
    {
        UART_Send_MotorInfo(2,5,3);
        delay(1000);
    }

    return 0;
}
