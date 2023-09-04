/*
  Lepton.h - Library for accessing the Lepton thermal imaging camera (http://lepton.flir.com/)
  Created by Matt Williams, 17-19 June 2016
  BSD license - check license.txt for more information
 */

#include "Arduino.h"
#include "Lepton.h"
#include "Wire.h"
#include "SPI.h"

Lepton::Lepton(int sdaPin, int sclPin, int ssPin, int width, int height) : _sdaPin(sdaPin), _sclPin(sclPin), _ssPin(ssPin), _width(width), _height(height) {
}

void Lepton::begin() {
  Wire.begin(_sdaPin, _sclPin);

  pinMode(_ssPin, OUTPUT);
  digitalWrite(_ssPin, HIGH);

  SPI.begin();
}

uint16_t Lepton::readRegister(uint16_t reg) {
  setRegister(reg);
  Wire.requestFrom(DEVICE_ID, (uint8_t)2);
  return readWord();
}

void Lepton::writeRegister(uint16_t reg, uint16_t value) {
  startTransmission(reg);
  transmitWord(value);
  endTransmission();
}

uint16_t Lepton::doGetCommand(uint16_t commandIdBase, uint16_t* data) {
  writeRegister(REG_COMMAND_ID, commandIdBase | TYPE_GET);
  waitIdle();
  return readData(data);
}

void Lepton::doSetCommand(uint16_t commandIdBase, uint16_t* data, uint16_t dataLen) {
  writeData(data, dataLen);
  writeRegister(REG_COMMAND_ID, commandIdBase | TYPE_SET);
  waitIdle();
}

uint16_t Lepton::doRunCommand(uint16_t commandIdBase, uint16_t* data, uint16_t dataLen) {
  writeData(data, dataLen);
  writeRegister(REG_COMMAND_ID, commandIdBase | TYPE_RUN);
  waitIdle();
  return readData(data);
}

void Lepton::syncFrame() {
   SPI.beginTransaction(SPISettings(15000000, MSBFIRST, SPI_MODE3));
   digitalWrite(_ssPin, LOW);
   delay(0.02);
   
    static int count;
    if(count < 1)
     delay(1000);
    count++;
    if(count>=5)
    count = 5;
}

int Lepton::readFrame(uint16_t* data) {
  uint16_t row = 0;
  uint16_t id = waitNextFrame();
  while ((id & 0xfff) == row) {
    uint16_t crc = readFrameWord();
    for (int col = 0; col < 80; col++) {
      data[row * 80 + col] = readFrameWord();
    }
    row++;
    if (row < 45) {
      id = readFrameWord();
    } else {
      return 1;
    }
  }
  Serial.printf("readFrame ended with row %4x != id %4x\n", row, id, (id >> 12) & 0x7, id & 0xfff);
  return 0;
}

void Lepton::end() {
  digitalWrite(_ssPin, HIGH);
  SPI.endTransaction();
}

void Lepton::readFrameRaw(uint16_t* data) {
  data[0] = waitNextFrame();
  for (int i = 1; i < 82 * 60; i++) {
    data[i] = readFrameWord();
  }
}

void Lepton::startTransmission(uint16_t reg) {
  Wire.beginTransmission(DEVICE_ID);
  transmitWord(reg);
}

void Lepton::transmitWord(uint16_t value) {
  Wire.write(value >> 8 & 0xff);
  Wire.write(value & 0xff);    
}

void Lepton::endTransmission() {
  uint8_t error = Wire.endTransmission();
  if (error != 0) {
    Serial.print("error=");
    Serial.println(error);
  }    
}

uint16_t Lepton::readWord() {
  uint16_t value = Wire.read() << 8;
  value |= Wire.read();
  return value;
}

void Lepton::setRegister(uint16_t reg) {
  startTransmission(reg);
  endTransmission();
}

void Lepton::waitIdle() {
  while (readRegister(REG_STATUS) & STATUS_BIT_BUSY) {
  }
}

uint16_t Lepton::readData(uint16_t* data) {
  uint16_t dataLen = readRegister(REG_DATA_LEN) / 2; // The data sheet says the data length register is in 16-bit words, but it actually seems to be in bytes
  setRegister(REG_DATA_BASE);
  Wire.requestFrom(DEVICE_ID, (uint8_t)(dataLen * 2));
  for (int i = 0; i < dataLen; i++) {
    data[i] = readWord();
  }
  // TODO Check CRC
  return dataLen;
}

void Lepton::writeData(uint16_t* data, uint16_t dataLen) {
  startTransmission(REG_DATA_LEN);
  transmitWord(dataLen);
  for (int i = 0; i < dataLen; i++) {
    transmitWord(data[i]);
  }
  endTransmission();
}

uint16_t Lepton::readFrameWord() {
  // digitalWrite(_ssPin, LOW);
  uint16_t data = SPI.transfer(0x00) << 8;
  data |= SPI.transfer(0x00);
  // digitalWrite(_ssPin, HIGH);
  return data;
}

uint16_t Lepton::waitNextFrame() {
  uint16_t id = readFrameWord();
  while ((id & 0x0f00) == 0x0f00) {
    for (int i = 0; i < (_height + 1); i++) {
      readFrameWord();
    }
    id = readFrameWord();
    // Serial.printf("wait %4x %4x %4x \n", id, id & 0xf000, id & 0xfff);
  }
  return id;    
}

void Lepton::dumpHex(uint16_t *data, int dataLen) {
  for (int i = 0; i < dataLen; i++) {
    Serial.printf("%4x ", data[i]);
  }
  Serial.println();
}

