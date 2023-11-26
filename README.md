# FlatMaster
This is the Arduino code for a LED panel controller.
The construction details are on the website www.digitalastrophotography.co.uk and the ASCOM driver (FlatMaestroASCOM) is published on GITHUB too.
It is perhaps the simplest circuit, with just the Arduino, power MOSFET, resistor and two capacitors.
When the unit turns on, it defaults to zero power. EEPROM code is included in the Arduino driver but but disabled to recover last brightness value on turn on.
