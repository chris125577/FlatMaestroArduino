// Receives number as string with # terminator on serial port
// Writes output to analog pin and broadcasts back value 0-255
/*
    Name:       FlatMaster.ino
    Created:	22/11/2023 10:46:25
    Author:     chris woodhouse  cwoodhou@icloud.com
    V1.1        works - translates serial number string into pwm
    V1.2        put in EEPROM save option - on line 45
    V1.3        changed PWM output to higher frequency to prevent banding on camera
    V1.4        PWM output is never flat zero - added analog out to do that
    V1.5        rescaled input to usable range 0-255 >> 10-250
    V1.6        not convinced it has fine resolution, changing to 0-100 %input
*/

// Define User Types below here or use a .h file


#include <EEPROM.h>
#include <AVR_PWM.h>   // library for high speed hardware PWM

#define version "V1.6"   // change with version number

// string stuff

String cmd;  // received command from serial port/ASCOM

// constants
const char start = '$'; // output starter char (and EEPROM) not used on rx serial command
const char terminator = '#'; // end character for string (rx and tx)
const unsigned long baud = 19200; // serial baud rate
const uint8_t PWMpin = 3;  // PWM pin
const unsigned long frequency = 62500;  // slowest frequency
const int maxlevel = 100;  //ascom level
float dutycycle = 0;  // 4-97

// global variables
int brightness = 0;   // 0-100 brightness level


// Define Function Prototypes that use User Types below here or use a .h file
//
AVR_PWM* PWM_Instance;

// Define Functions below here or use other .ino or cpp files
//

// The setup() function runs once each time the micro-controller starts
void setup()
{
    pinMode(PWMpin, OUTPUT);  // for LED panel
    pinMode(LED_BUILTIN, OUTPUT); // for diagnostics
    Serial.begin(baud);  // 
    Serial.setTimeout(1000);
    //readfromEEPROM();   // uncomment this line if you want LED to resume last brightness on power up.
    // following three lines are for high speed PWM
    if (brightness != 0) dutycycle = ((4 + brightness)/107 );  // 0-100 = 4-97%  (ignore documentation in GIT
    else dutycycle = 0;
    PWM_Instance = new AVR_PWM(PWMpin, frequency, dutycycle); //   clock/256, dutycycle= 4-97
    PWM_Instance->setPWM(); // initialize  but override if zero...
    if(brightness == 0) analogWrite(PWMpin, 0); // disable blips when zero  
}

// Add the main program code into the continuous loop() function
void loop()
{
    if (Serial.available() > 0)  // check to see if ASCOM driver is sending command
    {
        cmd = Serial.readStringUntil(terminator);  //  cmd has just string and not end character, should be up to three chars
        if (cmd != NULL) 
        {
            brightness = cmd.toInt();        // bring in ASCOM brightness level command     
            if (brightness > maxlevel) brightness = maxlevel;  // just to ensure compliance
            if (brightness < 0) brightness = 0; // just to ensure compliance
            if (brightness != 0)  // truncate below 5%
            {
                dutycycle = ((4 + brightness) / 107);  // 4-97  %
                PWM_Instance->setPWM(PWMpin, frequency, dutycycle);
                updateEEPROM();  // save brightness level to EEPROM
            }
            else
            {
                dutycycle = 0;
                analogWrite(PWMpin, 0); // disable blips when zero 
            }
        }
        else  // pulse LED on for a second to highlight serial error condition
        {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(1000);
            digitalWrite(LED_BUILTIN, LOW);
        }
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);  // short pulse to show received command
        digitalWrite(LED_BUILTIN, LOW);
    }
    else delay(2000); // if no command, only broadcast every 2secs
    USBserial(); //broadcast status
}

// private utilities
void USBserial(void)   // broadcast status back (0 = off)
{
    Serial.print(start);  // start character
    Serial.print(brightness); // panel brightness
    Serial.print(terminator);  // terminal character
}

// update EEPROM with brightness value
void updateEEPROM()
{
    byte level = int(brightness);
    EEPROM.write(0, start); // cmd does not have start character from PC $nnn
    EEPROM.write(1, level);
}

// check eeprom has been initiated and return brightness value
int readfromEEPROM()
{
    if (EEPROM.read(0) != start)
    {
        brightness = 0;
        return false;
    }
    else
    {
        brightness = EEPROM.read(1);
        return true;
    }
}
