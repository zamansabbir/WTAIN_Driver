#include <stdio.h>

#include "Weeder/analog_input.h"

int main() {
    int hSerial = ConfigureSerial();
    if (hSerial == -1)
    {
        return 1;
    }

    char szBuff[MAX_DATA_LENGTH] = {0};
    ConfigureDefaults(hSerial, 'A');
    ReadChannel(hSerial, 'A', 'A', szBuff);
    ReadChannel(hSerial, 'A', 'A', szBuff);
    //printf("Response: %s\n", szBuff);

    close(hSerial);
    return 0;
}