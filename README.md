# LiquidCrystalEAX library for Arduino

 * License: GNU GPL v3

 * Author:  Peter R. Bloomfield
  * My blog: http://avidinsight.uk
  * My online CV/portfolio: http://peter.avidinsight.uk


# Contents

 1. INTRODUCTORY NOTE
 2. SUMMARY
 3. SETUP NOTES (HARDWARE)
 4. EXAMPLE USAGE (SOFTWARE)
 5. TROUBLESHOOTING

# 1. Introductory Note

This library is still in early development. It is my intention to
overhaul it so that the methods and usage more closely resemble the standard
LiquidCrystal library (including inheriting from the Print class).

In other words, expect some breaking interface changes. (Eventually...)


# 2. Summary

This is an Arduino library for controlling Epson EA-X series alphanumeric LCD
displays. These displays are not compatible with the typical Hitachi HD44780
driver, so I created this library instead.

Based on the specification in the datasheet, it should support all of the
following models:

 *  EA-X16017AR (16x1 characters)
 *  EA-X16027AR (16x2 characters)
 *  EA-X20017AR (20x1 characters)
 *  EA-X20027AR (20x2 characters)
 *  EA-X24017AR (24x1 characters)
 *  EA-X40017AR (40x1 characters)
 *  EA-X40027AR (40x2 characters)

At the time of writing it's only been tested on the EA-X16027AR.


# 3. Setup Notes (Hardware)

You will need to connect at least 11 pins of the LCD to your Arduino. These are:

 * 8 data pins (D0-D7)
 * Data select (listed as A0 in the datasheet -- not related to A0 on the Arduino!)
 * Write enable (WR)
 * Read enable (RD)

Additional LCD pins which may be connected to the Arduino, or wired separately:

 * Reset
 * Clock input (ENABLE) 
 * Chip select
 
Pins which must be wired separately:

 * Vout (contrast)

 
## 3.1 Reset
Connecting the Reset pin to your Arduino will let you reset it from software.
If you don't intend to do that then connect it directly to +5v, as it is active
low.

## 3.2 Clock Input
The LCD display requires some kind of clock input (ENB) at around 2MHz. You may
want to supply an external clock source for this. As an easy alternative, you
can connect it to any of the Arduino's output pins, and let this library
generate a suitable signal. However, please note that this makes use of the
core tone() function. That means it can interfere with some PWM, and any other
use of tone().

See the Arduino documentation for more details on that:

 * http://arduino.cc/en/Reference/Tone

## 3.3 Chip Select
If you are only using one display, connect the chip select pin directly to
ground (it is active low).

If you are using multiple displays connected in parallel, you will need to
connect each display's chip select pin to a separate output. Bring a specific
chip select pin low to enable that display.

This library does not provide any support for chip select. You will need to set
it up yourself.
 
## 3.4 Vout
The EAX displays have a Vout pin for controlling contrast. You should connect
it to a potentiometer or fixed voltage divider, as described in the data sheet.
Do not leave it floating.


# 4. Example Usage (software)

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


# 5. Troubleshooting

I've found that it's quite easy to make several mistakes which can prevent the
display from working as expected.

Things to check if the display doesn't seem to be working:

 * Pin connections - make sure everything is connected as intended, and that
    the correct pin numbers are passed to the constructor.
    Also make sure none of the pins are left floating.
 
 * Clock input - make sure a suitable clock signal is being supplied (~2MHz).
    Any other use of the Arduino tone() function could interfere with it.
    
 * Vout (constrast) - try adjusting the contrast voltage divider, as described
    in the display's datasheet. If all else fails, try connecting it directly
    to +5v or ground.
    
 * Chip select - make sure the chip select pin is tied to ground, or being
    programmatically brought down to ground by your code (if applicable).
    
 * Activate display - currently, you need to call the library's displayOn()
    method before anything will appear, as the display is deactivated by
    default in the hardware.

