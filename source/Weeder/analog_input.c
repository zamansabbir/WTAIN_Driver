/**
 * @file analog_input.c
 * @author Sabbir Zaman (sabbir.zaman@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-02
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "analog_input.h"

#define COM_PORT "/dev/ttyS0"  // Adjust as needed for your system
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)  // can be used to debug along with line macro __LINE__ in printf

static char command_sent[MAX_COMMAND_LENGTH];

static int ValidateResponse(const char* response);
static void SetWTAInModeToDefault(int hSerial, char module_header_identifier, char channel, unsigned char* status);
static void SetWTAInDecimalDefault(int hSerial, char module_header_identifier, char channel, unsigned char* status);

int ConfigureSerial() {
    int hSerial;
    struct termios options;

    // Open the serial port
    hSerial = open(COM_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
    if (hSerial == -1) {
        perror("Error opening serial port");
        return -1;
    }

    // Configure serial port settings
    tcgetattr(hSerial, &options);
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    options.c_cflag |= (CLOCAL | CREAD);  // Enable receiver and set local mode
    options.c_cflag &= ~PARENB;           // No parity
    options.c_cflag &= ~CSTOPB;           // 1 stop bit
    options.c_cflag &= ~CSIZE;            // Mask the character size bits
    options.c_cflag |= CS8;               // 8 data bits
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // No software flow control
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input
    options.c_oflag &= ~OPOST;            // Raw output

    // Set read timeout
    options.c_cc[VMIN] = 0;     // Minimum number of characters to read
    options.c_cc[VTIME] = 10;   // Timeout in deciseconds

    // Apply the settings to the serial port
    if (tcsetattr(hSerial, TCSANOW, &options) != 0) {
        perror("Error setting serial port attributes");
        close(hSerial);
        return -1;
    }

    return hSerial;
}


static void SetWTAInModeToDefault(int hSerial, char module_header_identifier, char channel, unsigned char* status) {
    char szBuff[MAX_DATA_LENGTH] = {0};
	//char *set_mode_cmd = "AMA1\r\0";
    char command[8];
    char mode = '1';
    sprintf(command, "%cM%c%c", module_header_identifier, channel, mode);
    SendCommandReadResponse(hSerial, command, szBuff); // should echo back the same command if connected properly

    char *set_mode_cmd = command;
    set_mode_cmd[4] = '\r';
    set_mode_cmd[5] = '\0';

	if (strncmp(szBuff, set_mode_cmd, strlen(set_mode_cmd)) != 0) {
        *status &= ~MODE_DEFAULT_SUCCESS_BIT;
	}
   else {
       *status |= MODE_DEFAULT_SUCCESS_BIT;;
   }
}

static void SetWTAInDecimalDefault(int hSerial, char module_header_identifier, char channel, unsigned char* status) {
    char szBuff[MAX_DATA_LENGTH] = {0};
    char command[8];
    char decimal_point = '0';
    sprintf(command, "%cD%c%c", module_header_identifier, channel, decimal_point);
	
    SendCommandReadResponse(hSerial, command, szBuff); // should echo back the same command if connected properly
    char *set_mode_cmd = command;
    set_mode_cmd[4] = '\r';
    set_mode_cmd[5] = '\0';

	if (strncmp(szBuff, set_mode_cmd, strlen(set_mode_cmd)) != 0) {
        *status &= ~DECIMAL_DEFAULT_SUCCESS_BIT;
	}
   else {
        *status |= DECIMAL_DEFAULT_SUCCESS_BIT;
   }
 }

void ConfigureDefaults(int hSerial, char module_header_identifier) {
    char channel[4] = {'A', 'B', 'C', 'D'};
    for (int i = 0; i < 4; i++) 
    {
        unsigned char status = 0;
        SetWTAInModeToDefault(hSerial, module_header_identifier, channel[i], &status);
        SetWTAInDecimalDefault(hSerial, module_header_identifier, channel[i],&status);
         if (status != (MODE_DEFAULT_SUCCESS_BIT | DECIMAL_DEFAULT_SUCCESS_BIT)) 
         {
            printf("Failed to set defaults for channel %c.\n", channel[i]);

            if (!(status & MODE_DEFAULT_SUCCESS_BIT)) 
            {
                printf("Failed to set mode to default at channel %c.\n", channel[i]);
            }
            
            if (!(status & DECIMAL_DEFAULT_SUCCESS_BIT)) 
            {
                printf("Failed to set decimal to default at channel %c.\n", channel[i]);
            }
        }
    }
}

void SendCommand(int hSerial, const char* command) {
    int len = strlen(command);
    strncpy(command_sent,command, len);
    command_sent[len] = '\r'; // carriage return
    command_sent[len + 1] = '\0'; // null terminating string

    printf("Sending: %s\n", command_sent);
    int n = write(hSerial, command_sent, strlen(command_sent));
    if (n < 0) {
        perror("Error writing to serial port");
    }
}

void ReadResponse(int hSerial, char* buffer) {
    int n = read(hSerial, buffer, MAX_DATA_LENGTH - 1);
    if (n < 0) {
        perror("Error reading from serial port");
    } else {
        buffer[n] = '\0';
        printf("Response: %s\n", buffer);
        if (!ValidateResponse(buffer)) {
            fprintf(stderr, "Invalid response: %s\n", buffer);
        }
    }
}

void SendCommandReadResponse(int hSerial, const char* command, char* buffer) {
    SendCommand(hSerial, command);
    ReadResponse(hSerial, buffer);
}

void ReadChannel(int hSerial, char header, char channel, char* buffer) {
    char command[8];
    sprintf(command, "%cR%c", header, channel);

    printf("read channel command: %s\n", command);
    SendCommandReadResponse(hSerial, command, buffer);
    
}

static int ValidateResponse(const char* response) {
    // If the response starts with the same header as the command, it is likely valid
    if (response[0] != command_sent[0]) {
        printf("Error: Unexpected header in response: expected %c, got %c\n", command_sent[0], response[0]);
        return 0;
    }
    // In case of error, the device returns the header followed by '?'
    if (response[1] == '?') {
        printf("Error in command: %s\n", command_sent);
        return 0;
    }


    // Successful execution of DECIMAL, ZERO, SPAN, FACTOR commands should echo the command back
    if (strstr(response, command_sent) != NULL) {
        printf("Command: %s executed successfully\n", command_sent);
        return 1;
    }
    // If none of the above, assume it's a READ command and the response is a decimal number
    // A more thorough check would involve parsing the response and checking if it's a valid number
    return 1;
}