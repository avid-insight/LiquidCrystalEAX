/*
 * LiquidCrystalEAX.h
 * Arduino library for controlling the Epson EA-X series of alphanumeric LCD displays.
 *
 * Author:  Peter R. Bloomfield
 * Website: www.avid-insight.co.uk
 */

#ifndef LIQUIDCRYSTALEAX_H
#define LIQUIDCRYSTALEAX_H

#include "Arduino.h"

class LiquidCrystalEAX
{
public:
  //--------------------------------------------------------------------------------
  // CONSTRUCTION / DESTRUCTION.

  // Constructor. This initialises the pin assignments.
  // You will need to call init() manually during setup.
  LiquidCrystalEAX(
    int pinData0,       // D0
    int pinData1,       // D1
    int pinData2,       // D2
    int pinData3,       // D3
    int pinData4,       // D4
    int pinData5,       // D5
    int pinData6,       // D6
    int pinData7,       // D7
    int pinDataSelect,  // A0
    int pinClock,       // ENB
    int pinWriteEnable, // WR
    int pinReadEnable,  // RD
    int pinReset        // RESET
  );
  
  // Destructor. Stops the clock signal if it was enabled.
  ~LiquidCrystalEAX();
  
  
  //--------------------------------------------------------------------------------
  // GENERAL OPERATIONS.
  
  // Do initial setup of the pins.
  // This sets the pins up as outputs, and puts everything in an idle state.
  // Note that it will stop the clock running if it was already enabled,
  //  so call enableClock() AFTER this.
  void init();
  
  // Start the clock running.
  void enableClock();
  
  // Stop the clock running.
  void disableClock();
  
  // Trigger a hardware reset of the LCD.
  // This drops the rest line low briefly and brings it back up.
  void hardReset();
  
  // Validate the given character for display.
  // If it is not supported or printable, it returns a space character.
  // Otherwise it returns the input character unmodified.
  static char validateCharacter(char c);
  
  
  //--------------------------------------------------------------------------------
  // CONTROL.
  
  // Set the cursor position by character and line.
  // Note that these are 0-based; e.g. the first character on the first line is 0,0.
  void setCursorPos(byte ch, byte line);
  
  // Do a software reset of the display.
  // This sends a reset command.
  void softReset();
  
  // Clear the contents of the display and return the cursor to the home position.
  // This leaves all other settings intact (i.e. it is not a full reset).
  void clearDisplay();
  
  // Move the cursor back to the home position (1st character on the 1st line).
  void moveCursorHome();
  
  // Move the cursor back to the start of the _current_ line.
  void returnCursor();
  
  // Set the cursor's direction to increment or decrement.
  // This determines where the cursor after each character is written.
  // true = increment, false = decrement
  void setCursorDir(boolean increment);
  
  // Move the cursor the specified number of places right or left
  //  (use a negative number for left)
  void moveCursor(int dist);
  
  // Set the cursor font to a blinking block.
  // Note: If the cursor has been hidden (using hideCursor()), this won't switch it back on.
  void useBlockCursor();
  
  // Set the cursor font to an underline.
  // It can blink constantly or remain steady.
  // Note: If the cursor has been hidden (using hideCursor()), this won't switch it back on.
  void useUnderlineCursor(boolean blinking = true);
  
  // Switch the visible cursor on.
  void showCursor();
  
  // Switch the visible cursor off.
  void hideCursor();
  
  // Switch the display on.
  // It is always off by default when powered-on or reset.
  void displayOn();
  
  // Switch the display off.
  // This doesn't clear memory.
  void displayOff();
  
  // Suppress the display from the cursor position onwards.
  // This only affects one-line displays.
  void suppressDisplay(boolean suppress);
  
  
  //--------------------------------------------------------------------------------
  // OUTPUT.
  // Note: The display will NOT automatically wrap text around onto the second line.
  
  // Write the given character to the display.
  // If character is not supported or printable,
  void write(char c);
  
  // Write the given character array string to the display.
  // This will write up to the specified number of characters, or up to the first
  //  null character (whichever comes first).
  void write(const char *str, byte length = 255);
  
  // Write the given string object to the display.
  void write(const String &str);
  
  
private:
  //--------------------------------------------------------------------------------
  // DEFINITIONS.
  
  // Enumeration of different commands which can be sent to the display.
  enum Command
  {
    CMD_SYS_RESET = 0x10,
    CMD_CLEAR_DISPLAY = 0x01,
    CMD_CURSOR_HOME = 0x02,
    CMD_CURSOR_RETURN = 0x03,
    CMD_CURSOR_DIR_INC = 0x04,
    CMD_CURSOR_DIR_DEC = 0x05,
    CMD_CURSOR_INC = 0x06,
    CMD_CURSOR_DEC = 0x07,
    CMD_CURSOR_FONT_UNDERLINE = 0x08,
    CMD_CURSOR_FONT_BLOCK = 0x09,
    CMD_CURSOR_UNDERLINE_BLINK_OFF = 0x0A,
    CMD_CURSOR_UNDERLINE_BLINK_ON = 0x0B,
    CMD_DISPLAY_OFF = 0x0C,
    CMD_DISPLAY_ON = 0x0D,
    CMD_CURSOR_OFF = 0x0E,
    CMD_CURSOR_ON = 0x0F,
    CMD_SUPPRESS_ON = 0x20,
    CMD_SUPPRESS_OFF = 0x60
  };
  
  
  //--------------------------------------------------------------------------------
  // LOW LEVEL OPERATIONS.

  // Send the given enumerated instruction to the LCD.
  void sendInstruction(Command cmd);
  
  // Send the given raw instruction data to the LCD.
  void sendRawInstruction(byte cmd);

  // Strobe the write enable line.
  // Call this to instruct the LCD to read the data lines.
  // It is active low, so this brings it low briefly, then makes it high again.
  void doWrite();
  
  // Enable busy flag reading.
  // This latches until it is disabled.
  void enableBusyRead();
  
  // Disable busy flag reading.
  // This latches until it is enabled.
  void disableBusyRead();
  
  // Enable instruction data.
  // This latches, and is only changed by a subsequent call to enableCharacterData().
  void enableInstructionData();
  
  // Enable instruction data.
  // This latches, and is only changed by a subsequent call to enableInstructionData().
  void enableCharacterData();
  
  // Set the data pins based on the given binary data.
  // The most significant bit goes to D7. Least significant goes to D0.
  void setData(byte data);
  
  // Clears the data pins back to 0.
  void clearData();
  
  // Wait until the display reports that it's not busy any more.
  // Returns true if the display successfully reports it's not busy.
  // Returns false if the specified timeout (in milliseconds) expires before then,
  //  meaning it gave up waiting.
  // NOTE: This switches to instruction mode rather than character mode.
  boolean waitUntilNotBusy(unsigned long timeout = 100);
  
  
  //--------------------------------------------------------------------------------
  // DATA.
  
  // General pin assignments
  int m_pinDataSelect;    // A0
  int m_pinClock;         // ENB
  int m_pinWriteEnable;   // WR
  int m_pinReadEnable;    // RD
  int m_pinReset;         // RESET
  
  // The pins used for sending/receiving data.
  // The indices correspond to pins D0-D7 on the display.
  int m_pinData[8];
  
  // Indicates if the clock has been enabled on this instance.
  boolean m_clockEnabled;
  
  
  //--------------------------------------------------------------------------------
  // DISALLOWED FUNCTIONS.

  // Default constructor is private/undefined to prevent construction without the
  //  pin assignment parameters.
  LiquidCrystalEAX();
  
  // Copy constructor is private/undefined to prevent copying, as that could have
  //  unexpected side effects.
  LiquidCrystalEAX(const LiquidCrystalEAX &);

};

#endif //LIQUIDCRYSTALEAX_H
