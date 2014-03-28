/*
This is a test of an Epson EAX series display.
To use it, connect a display to your Arduino, according to the pin assignments shown below.
When you upload and run the sketch, you should see "Hello World" appear on the display.


Pin connections to the Arduino are as follows
(note Arduino pins are 0 based, and LCD pins are 1 based):

Arduino  LCD  Function
2        2    Data 0
3        3    Data 1
4        4    Data 2
5        5    Data 3
6        6    Data 4
7        7    Data 5
8        8    Data 6
9        9    Data 7
10       10   Data select. 1 = character input, 0 = instruction input
11       13   Clock
12       11   Data write enable (active low)
13       12   Busy flag read enable (active low)
14 [A0]  15   Reset (active low)

GND      14   Chip select

*/

#include <LiquidCrystalEAX.h>


LiquidCrystalEAX lcd(
   2, 3, 4, 5, 6, 7, 8, 9, // data 0-7
   10, // data select
   11, // clock input
   12, // write enable
   13, // read enable
   14  // reset
);

void setup()
{    
    lcd.init();
    lcd.enableClock();
    lcd.hardReset();
    lcd.clearDisplay();
    lcd.displayOn();
 
    lcd.write("Hello World");
}

void loop()
{
}
