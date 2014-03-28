/*
This is a test of 4 Epson EAX16027AR displays (16x2 characters each), hooked up in parallel.
They will be treated as one large display, showing any text which arrives via serial.

To use it, hook up 4 displays in parallel, according to the pin configurations below.
Upload the sketch, and while the Arduino is still connected to your computer, open the serial
monitor in the Arduino IDE and type in some text. When you press Enter, it should appear on
the displays, scrolling up as more text arrives.


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

15 [A1]  14   Chip select on display 1
16 [A2]  14   Chip select on display 2
17 [A3]  14   Chip select on display 3
18 [A4]  14   Chip select on display 4

*/

#include <LiquidCrystalEAX.h>

LiquidCrystalEAX lcd(
  2, 3, 4, 5, 6, 7, 8, 9, // data 0-7
  10, // data select
  11, // clock
  12, // write enable
  13, // read enable
  14  // reset
);

// Note that the first display is the bottom one.
const int g_chipSelectPins[] = {18, 17, 16, 15};

// 8 lines of 16 characters each.
// The order of lines is bottom to top.
char g_buffer[8][16] = {' '}; // initialise to empty spaces

// Which line number in g_buffer is currently the bottom line of the display.
// New incoming data is written to the bottom line.
// A linefeed will cause this number to decrement and wrap around.
int g_bottomLineNum = 0;


// The number of characters currently in the bottom line.
// This lets us know where to start writing new data which comes in, and when
//  to wrap onto the next line.
int g_bottomLineLen = 0;

void selectDisplay(int num)
{
  for (int i = 0; i < 4; ++i) {
    digitalWrite(g_chipSelectPins[i], (i == num) ? LOW : HIGH);
  }
}

// Update the display.
void updateDisplay(int numLines = 8)
{
  if (numLines > 8) numLines = 8;
  
  // Go through each line, from bottom to top.
  // In this context, displayLine 0 is always the bottom line.
  for (int displayLine = 0; displayLine < numLines; ++displayLine) {
    
    // Determine which line index this actually refers to.
    const int bufferIndex = (displayLine + g_bottomLineNum) % 8;
    
    // Is this the first line we're writing to a particular display?
    if (displayLine % 2 == 0) {
      // Yes - enable output on that display
      selectDisplay(displayLine / 2);
    }
    
    // Output this line. Note that displayLine starts from the bottom and goes up,
    //  but localLine starts at the top of each display and goes down.
    lcd.setCursorPos(0, (byte)(1 - (displayLine % 2)));
    lcd.write(g_buffer[bufferIndex], 16);
  }
}

// Start a new line.
// This effectively shifts the buffer contents up a line, and fills the new
//  bottom line with spaces.
// This doesn't update the displays.
void linefeed()
{
  // Rotate the buffer.
  if (g_bottomLineNum == 0) g_bottomLineNum = 7;
  else --g_bottomLineNum;
  
  // Fill the new bottom line with spaces and reset its length.
  memset(g_buffer[g_bottomLineNum], ' ', 16);
  g_bottomLineLen = 0;
}

// Write a new character to the buffer.
// This doesn't update the displays.
// Returns true if this character caused the display to wrap onto a new line.
// Returns false if not.
boolean append(char c)
{
  // Handle linefeed and carriage return characters.
  if (c == '\n' || c == '\r') {
    linefeed();
    return true;
  }
  // Handle tabs as four spaces.
  if (c == '\t') {
    return append(' ') || append(' ') || append(' ') || append(' ');
  }

  // Don't store null characters, as they will mess up the buffer.
  if (c == 0) c = ' ';
  
  // All other special/control character will end up being represented as spaces.
  
  // If the bottom line is already full then start a new line.
  boolean wrapped = false;
  if (g_bottomLineLen >= 16) {
    linefeed();
    wrapped = true;
  }
  
  // Append the character to the bottom line.
  g_buffer[g_bottomLineNum][g_bottomLineLen++] = c;
  return wrapped;
}


void setup()
{
  Serial.begin(9600);
  
  // Setup the chip select pins
  for (int i = 0; i < 4; ++i) {
    pinMode(g_chipSelectPins[i], OUTPUT);
    digitalWrite(g_chipSelectPins[i], LOW);
  }
  
  // Global setup
  lcd.init();
  lcd.enableClock();
  lcd.hardReset();
  
  // Display-specific setup
  selectDisplay(0);
  lcd.clearDisplay();
  lcd.displayOn();
  
  selectDisplay(1);
  lcd.clearDisplay();
  lcd.displayOn();
  
  selectDisplay(2);
  lcd.clearDisplay();
  lcd.displayOn();
  
  selectDisplay(3);
  lcd.clearDisplay();
  lcd.displayOn();
  
  
  updateDisplay();
}

void loop()
{
  if (Serial.available() > 0) {
    if (append((char)Serial.read())) {
      // The data trigger a line feed, so we need to update the whole display.
      updateDisplay();
    } else {
      // Only the bottom line was modified, so we don't need to update the rest of the display.
      updateDisplay(1);
    }
  }
}
