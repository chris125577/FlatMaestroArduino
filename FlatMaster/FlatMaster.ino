// Receives byte as string with # terminator on serial
// Writes output to analog pin and broadcasts back value
/*
    Name:       FlatMaster.ino
    Created:	22/11/2023 10:46:25
    Author:     chris woodhouse
    V1.1        works - translates serial number string into pwm
*/

// Define User Types below here or use a .h file
//

#include <EEPROM.h>
#include <avr/wdt.h>   // library which has a watchdog function

#define version "V1.1"   // change with version number

// string stuff

String cmd;  // received command from serial port/ASCOM
// constants
const char start = '$'; // output starter char (and EEPROM) not used on rx serial command
const char terminator = '#'; // end character for string (rx and tx)
const unsigned long baud = 19200; // serial baud rate
const uint8_t PWMpin = 3;  // PWM pin

// global variables
int brightness = 0;   // 0-255 brightness level


// Define Function Prototypes that use User Types below here or use a .h file
//


// Define Functions below here or use other .ino or cpp files
//

// The setup() function runs once each time the micro-controller starts
void setup()
{
    pinMode(PWMpin, OUTPUT);  // for LED panel
    pinMode(LED_BUILTIN, OUTPUT); // for diagnostics
    Serial.begin(baud);  // 
    Serial.setTimeout(1000);
    analogWrite(PWMpin, brightness);
    //readEEPROM();
    //wdt_enable(WDTO_2S);  // set up watchdog to kick in if it is not reset in 2 seconds
}

// Add the main program code into the continuous loop() function
void loop()
{
    if (Serial.available() > 0)  // check to see if ASCOM driver is sending command
    {
        cmd = Serial.readStringUntil(terminator);  //  cmd has just string and not end character, should be up to three chars
        if (cmd != NULL) 
        {
            brightness = cmd.toInt();
            if (brightness > 255) brightness = 255;  // just to ensure compliance
            analogWrite(PWMpin, brightness); //  adjust panel
        }
        else  // pulse LED on for a second to highlight serial error condition
        {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(1000);
            digitalWrite(LED_BUILTIN, LOW);
        }
        //if(readfromEEPROM()) analogWrite(PWMpin, brightness);  // restore last status
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
    }
    else delay(2000); // if no command, only broadcast every 2secs
    USBserial(); //broadcast status
    //wdt_reset(); // reset watchdog timer on each cycle (cannot go beyond 2 seconds, which is a limitation)
}

// private utilities
void USBserial(void)   // broadcast status back (0 = off)
{
    Serial.print(start);  // start character
    Serial.print(brightness); // panel brightness
    Serial.print(terminator);  // terminal character
}

/* not sure whether to store brightness in EEPROM so that it comes on with power
// update EEPROM with calibration string cmd (not required at present)
void updateEEPROM()
{
    int i;
    EEPROM.write(0, start); // cmd does not have start character from PC $nnn#
    for (i = 1; (i < cmd.length() && cmd.length() < 5); i++)
        EEPROM.write(i , cmd[i]);
    EEPROM.write(cmd.length() + 1, terminator); // write framing terminator as terminator is stripped off rx string
}

// read string up to '#' and put in cmd string, if nothing in eeprom, return false
bool readfromEEPROM()
{
    if (EEPROM.read(0) != start)
    {
        return false;
    }
    char c;
    cmd = ""; // initialize command string
    int index = 1;
    c = EEPROM.read(index);
    while (c != end && index < 5 ) // or max chars reached
    {
        cmd = cmd + c;  // build command word
        index++;
        c = EEPROM.read(index); // read next character      
    }
    return true;
}
*/