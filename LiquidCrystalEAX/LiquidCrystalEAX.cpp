/*
 * LiquidCrystalEAX.h
 * Arduino library for controlling the Epson EA-X series of alphanumeric LCD displays.
 *
 * Author:  Peter R. Bloomfield
 * Website: www.avid-insight.co.uk
 */

#include "LiquidCrystalEAX.h"

//--------------------------------------------------------------------------------
// CONSTRUCTION / DESTRUCTION.

LiquidCrystalEAX::LiquidCrystalEAX(
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
  ) :
  m_pinDataSelect(pinDataSelect),
  m_pinClock(pinClock),
  m_pinWriteEnable(pinWriteEnable),
  m_pinReadEnable(pinReadEnable),
  m_pinReset(pinReset),
  m_clockEnabled(false)
{
  m_pinData[0] = pinData0;
  m_pinData[1] = pinData1;
  m_pinData[2] = pinData2;
  m_pinData[3] = pinData3;
  m_pinData[4] = pinData4;
  m_pinData[5] = pinData5;
  m_pinData[6] = pinData6;
  m_pinData[7] = pinData7;
}

LiquidCrystalEAX::~LiquidCrystalEAX()
{
  // Make sure we turn off the clock if it was enabled on this instance.
  // It's important not to do this if the clock wasn't enabled, as it could
  //  interfere with other stuff happening in the sketch.
  if (m_clockEnabled) disableClock();
}


//--------------------------------------------------------------------------------
// GENERAL OPERATIONS.

void LiquidCrystalEAX::init()
{
  // Initialise all pins as outputs.
  pinMode(m_pinDataSelect, OUTPUT);
  pinMode(m_pinClock, OUTPUT);
  pinMode(m_pinWriteEnable, OUTPUT);
  pinMode(m_pinReadEnable, OUTPUT);
  pinMode(m_pinReset, OUTPUT);
  for (int i = 0; i < 8; ++i) {
    pinMode(m_pinData[i], OUTPUT);
  }
  
  // Make sure everything is initialised to necessary defaults.
  digitalWrite(m_pinDataSelect, LOW);
  digitalWrite(m_pinClock, LOW);
  digitalWrite(m_pinWriteEnable, HIGH); // active low
  digitalWrite(m_pinReadEnable, HIGH); // active low
  digitalWrite(m_pinReset, HIGH); // active low
  
  for (int i = 0; i < 8; ++i) {
    digitalWrite(m_pinData[i], LOW);
  }
  
  // Make sure the display has time to boot up
  delay(50);
  
  displayOn();
}

void LiquidCrystalEAX::enableClock()
{
  tone(m_pinClock, 2000000);
  m_clockEnabled = true;
}

void LiquidCrystalEAX::disableClock()
{
  noTone(m_pinClock);
  digitalWrite(m_pinClock, LOW);
  m_clockEnabled = false;
}

void LiquidCrystalEAX::hardReset()
{
  digitalWrite(m_pinReset, LOW);
  delay(6);
  digitalWrite(m_pinReset, HIGH);
  delay(6);
}

char LiquidCrystalEAX::validateCharacter(char c)
{
  if (c < 0x20 || c > 0xDF) return 0x20;
  return c;
}


//--------------------------------------------------------------------------------
// CONTROL.

void LiquidCrystalEAX::setCursorPos(byte ch, byte line)
{
  // The character position can use up to 6 bits.
  // The line position is only 1 bit (0 or 1).
  
  enableInstructionData();
  setData((ch & 0x3F) | ((line & 1) << 6) | (1 << 7));
  doWrite();
}

void LiquidCrystalEAX::softReset()
{
  sendInstruction(CMD_SYS_RESET);
}

void LiquidCrystalEAX::clearDisplay()
{
  sendInstruction(CMD_CLEAR_DISPLAY);
}

void LiquidCrystalEAX::moveCursorHome()
{
  sendInstruction(CMD_CURSOR_HOME);
}

void LiquidCrystalEAX::returnCursor()
{
  sendInstruction(CMD_CURSOR_RETURN);
}

void LiquidCrystalEAX::setCursorDir(boolean increment)
{
  sendInstruction(increment ? CMD_CURSOR_DIR_INC : CMD_CURSOR_DIR_DEC);
}

void LiquidCrystalEAX::moveCursor(int dist)
{
  const Command cmd = (dist > 0) ? CMD_CURSOR_INC : CMD_CURSOR_DEC;
  if (dist < 0) dist *= -1;
  
  for (int i = 0; i < dist; ++i) {
    sendInstruction(cmd);
  }
}

void LiquidCrystalEAX::useBlockCursor()
{
  sendInstruction(CMD_CURSOR_FONT_BLOCK);
}

void LiquidCrystalEAX::useUnderlineCursor(boolean blinking)
{
  sendInstruction(CMD_CURSOR_FONT_UNDERLINE);
  sendInstruction(blinking ? CMD_CURSOR_UNDERLINE_BLINK_ON : CMD_CURSOR_UNDERLINE_BLINK_OFF);
}

void LiquidCrystalEAX::showCursor()
{
  sendInstruction(CMD_CURSOR_ON);
}

void LiquidCrystalEAX::hideCursor()
{
  sendInstruction(CMD_CURSOR_OFF);
}

void LiquidCrystalEAX::displayOn()
{
  sendInstruction(CMD_DISPLAY_ON);
}

void LiquidCrystalEAX::displayOff()
{
  sendInstruction(CMD_DISPLAY_OFF);
}

void LiquidCrystalEAX::suppressDisplay(boolean suppress)
{
  sendInstruction(suppress ? CMD_SUPPRESS_ON : CMD_SUPPRESS_OFF);
}


//--------------------------------------------------------------------------------
// OUTPUT.

void LiquidCrystalEAX::write(char c)
{
  enableCharacterData();
  setData((byte)validateCharacter(c));
  doWrite();
  delayMicroseconds(10);
}

void LiquidCrystalEAX::write(const char *str, byte length)
{
  // Only output what we've got room for
  for (byte i = 0; i < length; ++i) {
    // Stop if we hit a null character
    if (str[i] == 0) break;
    write(str[i]);
  }
}

void LiquidCrystalEAX::write(const String &str)
{
  const int len = str.length();
  for (int i = 0; i < len; ++i) {
    write(str[i]);
  }
}


//--------------------------------------------------------------------------------
// LOW LEVEL OPERATIONS.

void LiquidCrystalEAX::sendInstruction(Command cmd)
{

  enableInstructionData();
  setData((byte)cmd);
  doWrite();
}

void LiquidCrystalEAX::sendRawInstruction(byte cmd)
{

  enableInstructionData();
  setData(cmd);
  doWrite();
}

void LiquidCrystalEAX::doWrite()
{
  digitalWrite(m_pinWriteEnable, LOW);
  delayMicroseconds(10);
  digitalWrite(m_pinWriteEnable, HIGH);
  delayMicroseconds(10);
  
  waitUntilNotBusy();
}

void LiquidCrystalEAX::enableBusyRead()
{
  digitalWrite(m_pinReadEnable, LOW); // active low
  delayMicroseconds(10);
}

void LiquidCrystalEAX::disableBusyRead()
{
  digitalWrite(m_pinReadEnable, HIGH); // active low
  delayMicroseconds(10);
}

void LiquidCrystalEAX::enableInstructionData()
{
  digitalWrite(m_pinDataSelect, LOW);
  delayMicroseconds(10);
}

void LiquidCrystalEAX::enableCharacterData()
{
  digitalWrite(m_pinDataSelect, HIGH);
  delayMicroseconds(10);
}

void LiquidCrystalEAX::setData(byte data)
{
  for (int i = 0; i < 8; ++i) {
    digitalWrite(m_pinData[i], (((data >> i) & 1) == 0) ? LOW : HIGH);
  }
  delayMicroseconds(10);
}

void LiquidCrystalEAX::clearData()
{
  for (int i = 0; i < 8; ++i) {
    digitalWrite(m_pinData[i], LOW);
  }
  delayMicroseconds(10);
}

boolean LiquidCrystalEAX::waitUntilNotBusy(unsigned long timeout)
{
  // Log the start time so we know when the timeout expires
  const unsigned long start = millis();
  
  // The last data pin on the display also acts as a busy flag output
  pinMode(m_pinData[7], INPUT_PULLUP);
  enableInstructionData();
  
  // Keep going until the busy flag is clear or the timeout expires
  int busyState = HIGH;
  while (busyState == HIGH && (millis() < start) < timeout) {
    enableBusyRead();
    busyState = digitalRead(m_pinData[7]);
    disableBusyRead();
  }
  
  // Revert to normal behaviour
  pinMode(m_pinData[7], OUTPUT);
  digitalWrite(m_pinData[7], LOW);
  
  return busyState == LOW;
}


