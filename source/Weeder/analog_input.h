#ifndef ANALOG_INPUT_H
#define ANALOG_INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#define MAX_DATA_LENGTH 255
#define MAX_COMMAND_LENGTH 100

#define MODE_DEFAULT_SUCCESS_BIT (1 << 0)
#define DECIMAL_DEFAULT_SUCCESS_BIT (1 << 1)
#define INVALID_HANDLE_VALUE -1

/**
 * @brief Configure Serial port with regular setting: 9600 baud, no parity, 8 data bits and 1 stop bit
 * 
 * @return int handle to serial port acquired
 */
int ConfigureSerial();

/**
 * @brief set defaults settings: Mode: votlage level and Decimal setting
 * 
 * @param hSerial handle to serial port acquired
 * @param module_header_identifier 
 */
void ConfigureDefaults(int hSerial, char module_header_identifier);

/**
 * @brief Sends command in C-char array with termination '\r' and '\0'
 * 
 * @param hSerial 
 * @param command 
 */

void SendCommand(int hSerial, const char* command);

/**
 * @brief Reads the response from WTAIN module and validate the response against expected patter
 * 
 * @param hSerial 
 * @param buffer 
 */
void ReadResponse(int hSerial, char* buffer);

/**
 * @brief Combines SendCommand and ReadResponse() to simplify in most usage case
 * 
 * @param hSerial handles to serial port acquired
 * @param command command char array to send
 * @param buffer 
 */
void SendCommandReadResponse(int hSerial, const char* command, char* buffer);

/**
 * @brief Read a particular channel
 * 
 * @param hSerial handles to serial port acquired
 * @param header  module indicator
 * @param channel channel indicator
 * @param buffer 
 */
void ReadChannel(int hSerial, char header, char channel, char* buffer);

#endif // ANALOG_INPUT_Hs