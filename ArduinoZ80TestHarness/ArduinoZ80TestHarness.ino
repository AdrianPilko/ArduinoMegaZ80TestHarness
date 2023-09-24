/* 
Copyright (c) 2023 Adrian Pilkington

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Ardiuno Mega test harness for Z80A CPU

currently get output fixed at:
holding reset low to 10 clock cycles
WR RD Address bus = 11111101101010 Data bus = 11111111
WR RD Address bus = 11111101101010 Data bus = 11111111
WR Address bus = 0 Data bus = 11111111
WR Address bus = 0 Data bus = 11111111
WR RD Address bus = 11111101101010 Data bus = 11111111
WR RD Address bus = 11111101101010 Data bus = 11111111
WR RD Address bus = 11111101101010 Data bus = 11111111
WR Address bus = 0 Data bus = 11111111
WR Address bus = 0 Data bus = 11111111
WR RD Address bus = 11111101101010 Data bus = 11111111
holding reset high
WR Address bus = 0 Data bus = 11111111
WR Address bus = 0 Data bus = 11111111
WR RD Address bus = 11111101101010 Data bus = 11111111
WR RD Address bus = 11111101101010 Data bus = 11111111
WR RD Address bus = 11111101101010 Data bus = 11111111
WR Address bus = 0 Data bus = 11111111
WR Address bus = 0 Data bus = 11111111
WR RD Address bus = 11111101101010 Data bus = 11111111
WR RD Address bus = 11111101101010 Data bus = 11111111


*/


#define NUMBER_OF_ADDRESS_PINS 16
#define NUMBER_OF_DATA_PINS 8

int addressPins[NUMBER_OF_ADDRESS_PINS] = {22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52};
int dataPins[NUMBER_OF_DATA_PINS] = {14,15,16,17,18,19,20,21};
int resetPin = 0; // active low 
int clockPin = 1; 
int readEnable = 53; // active low 
int writeEnable = 51; // active low 

void toggleClock()
{
  // clock
  digitalWrite(clockPin, HIGH);
  delay(100);
  digitalWrite(clockPin, LOW);  
}


void setAddressPinsToInput()
{
  for (int i = 0; i < NUMBER_OF_ADDRESS_PINS; i++)
  {
    pinMode(addressPins[i+22], INPUT);      
  }
}
  
void  setDataToOutput()
{
    // set pin mode to INPUT
    for (int i = 0; i < NUMBER_OF_DATA_PINS; i++)
    {
        pinMode(dataPins[i+14], OUTPUT);
    }
}
void  setDataToInput()
{
    // set pin mode to INPUT
    for (int i = 0; i < NUMBER_OF_DATA_PINS; i++)
    {
        pinMode(dataPins[i+14], INPUT);
    }
}

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  setAddressPinsToInput();
  setDataToOutput();

  pinMode(clockPin, OUTPUT);   // remember that the INPUT / OUTPUT is from the point of view of the Arduino
  pinMode(resetPin, OUTPUT);
  pinMode(readEnable, INPUT);   
  pinMode(writeEnable, INPUT);

  // to reset z80 the reset must be held active (low) for a minimum of 3 clock cycles
  // then set to high after
  digitalWrite(resetPin, LOW);    
  Serial.println("holding reset low to 10 clock cycles");
  for (int i = 0; i < 10; i++)
  {
    toggleClock();    
  }
  Serial.println("holding reset high");
  digitalWrite(resetPin, HIGH); 
}

void outputToDataPins(uint8_t val)
{
    setDataToOutput();
    for (int i = 0; i < NUMBER_OF_DATA_PINS; i++)
    {
        digitalWrite(dataPins[i+14], val << i);       
    }   
}

uint8_t readFromDataPins()
{
    uint8_t rv = 0;
    setDataToInput();
    for (int i = 0; i < NUMBER_OF_DATA_PINS; i++)
    {
        rv |= digitalRead(dataPins[i+14]) << i;       
    }   
    return rv;
}

void readDataAndAddress()
{
  uint16_t addressBus = 0;
  uint8_t dataBus = 0;
  bool writeEn = !digitalRead(writeEnable);    /// active low so not added before
  bool readEn = !digitalRead(readEnable);  /// active low so not added before

  if (readEn)
  { 
    Serial.print("RD ");
    // this means the Z80 is trying to read from the address on the address bus
    for (int i = 0; i < NUMBER_OF_ADDRESS_PINS; i++)
    {
        addressBus |= digitalRead(addressPins[i+22]) << i;       
    }
    // set the Data pins to OUTPUT and send 0xff
    dataBus = 0x0; // 0x00 is nop instruction
    outputToDataPins(dataBus);
  }
  
  if (writeEn)  
  {  // this means Z80 trying to send some data, so get arduino to read it
    Serial.print("WR ");
    outputToDataPins(0xff);
    for (int i = 0; i < NUMBER_OF_DATA_PINS; i++)
    {
        dataBus |= (digitalRead(dataPins[i+14]) << i);       
    }   
  }  
  Serial.print("Address bus = ");
  Serial.print(addressBus,BIN);
  Serial.print("\t\tData bus = ");
  Serial.print(dataBus,BIN);
  Serial.println();
}

void loop() {
  toggleClock();
  readDataAndAddress();
  toggleClock();  
}
