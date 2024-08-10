//
// serial.c / serial.cpp
// A simple serial port writing example
// Written by Ted Burke - last updated 13-2-2013
//
// To compile with MinGW:
//
//      gcc -o serial.exe serial.c
//
// To compile with cl, the Microsoft compiler:
//
//      cl serial.cpp
//
// To run:
//
//      serial.exe
//
// https://batchloaf.wordpress.com/2013/02/13/writing-bytes-to-a-serial-port-in-c/
 
#include <windows.h>
#include <stdio.h>
#include <time.h>

void delay(int a_intNumOfMs)
{
    // Storing start time
    clock_t start_time = clock();
 
    // looping till required time is not achieved
    while (clock() < start_time + a_intNumOfMs);
}


int main()
{
    // Define the eight bytes to send command to 
	// write 25 ticks/100ms to right and left wheels
    char bytes_to_send[8];
	bytes_to_send[0] = 126;
    bytes_to_send[1] = 59;
    bytes_to_send[2] = 42;
    bytes_to_send[3] = 0;
    bytes_to_send[4] = 25;
	bytes_to_send[5] = 0;
	bytes_to_send[6] = 25;
	bytes_to_send[7] = 104;
 
    // Declare variables and structures
    HANDLE hSerial;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};
         
    // Open the highest available serial port number
    fprintf(stderr, "Opening serial port...");
    hSerial = CreateFile(
                "\\\\.\\COM18", GENERIC_READ|GENERIC_WRITE, 0, NULL,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    if (hSerial == INVALID_HANDLE_VALUE)
    {
            fprintf(stderr, "Error\n");
            return 1;
    }
    else fprintf(stderr, "OK\n");
     
    // Set device parameters (38400 baud, 1 start bit,
    // 1 stop bit, no parity)
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (GetCommState(hSerial, &dcbSerialParams) == 0)
    {
        fprintf(stderr, "Error getting device state\n");
        CloseHandle(hSerial);
        return 1;
    }
     
    dcbSerialParams.BaudRate = CBR_38400;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if(SetCommState(hSerial, &dcbSerialParams) == 0)
    {
        fprintf(stderr, "Error setting device parameters\n");
        CloseHandle(hSerial);
        return 1;
    }
 
    // Set COM port timeout settings
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if(SetCommTimeouts(hSerial, &timeouts) == 0)
    {
        fprintf(stderr, "Error setting timeouts\n");
        CloseHandle(hSerial);
        return 1;
    }
 
    // Send specified text (remaining command line arguments)
    DWORD bytes_written, total_bytes_written = 0;
    fprintf(stderr, "Sending bytes...");
	while(true)
	{
		if(!WriteFile(hSerial, bytes_to_send, 8, &bytes_written, NULL))
		{
			fprintf(stderr, "Error\n");
			CloseHandle(hSerial);
			return 1;
		}   
		fprintf(stderr, "%d bytes written\n", bytes_written);
		delay(50);
	}
     
    // Close serial port
    fprintf(stderr, "Closing serial port...");
    if (CloseHandle(hSerial) == 0)
    {
        fprintf(stderr, "Error\n");
        return 1;
    }
    fprintf(stderr, "OK\n");
 
    // exit normally
    return 0;
}