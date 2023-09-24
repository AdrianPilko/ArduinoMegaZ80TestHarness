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

This code doesn't work yet, currently get (incorrect) output:
The address buss should be set to zero after the reset, but it's not!

holding reset low 
holding reset high
status control=MREQ RD 
Address bus = CE14     Data bus = 0
CLOCK HIGH
status control=MREQ RD 
Address bus = CE14     Data bus = 0
CLOCK LOW
Address bus = CE14     Data bus = 0
CLOCK HIGH
Address bus = CE14     Data bus = 0
CLOCK LOW
Address bus = 4614     Data bus = 0
CLOCK HIGH
Address bus = 4614     Data bus = 0
CLOCK LOW
Address bus = 4614     Data bus = 0
CLOCK HIGH
Address bus = 4614     Data bus = 0
CLOCK LOW
status control=IOREQ MREQ BUSAK RD WR 
Address bus = 4614     Data bus = 0
CLOCK HIGH
Address bus = 4614     Data bus = 0
CLOCK LOW
status control=IOREQ MREQ BUSAK RD WR 
Address bus = 4614     Data bus = 0
CLOCK HIGH
Address bus = 4614     Data bus = 0
CLOCK LOW
status control=MREQ RD 
Address bus = 4614     Data bus = 0
CLOCK HIGH
Address bus = 4614     Data bus = 0
CLOCK LOW
Address bus = 4614     Data bus = 0
CLOCK HIGH
Address bus = 4614     Data bus = 0
CLOCK LOW
Address bus = 4614     Data bus = 0
CLOCK HIGH
Address bus = 4614     Data bus = 0
CLOCK LOW
Address bus = 4614     Data bus = 0
CLOCK HIGH
Address bus = 4614     Data bus = 0
CLOCK LOW
status control=IOREQ MREQ BUSAK RD WR 
Address bus = 4614     Data bus = 0
CLOCK HIGH


*/


#define NUMBER_OF_ADDRESS_PINS 16
#define NUMBER_OF_DATA_PINS 8

int addressPins[NUMBER_OF_ADDRESS_PINS] = {22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52};
int dataPins[NUMBER_OF_DATA_PINS] = {14,15,16,17,18,19,20,21};
int CLK = 1; 

int HALT = 4; // pin 18 on Z80 // output from z80
int MREQ = 3; // pin 19 on Z80 // output from z80
int IORQ = 2; // pin 20 on z80 // output from z80

int REFRESH = 6;  // PIN 28 on Z80 OUTPUT from Z80
int M1 = 5; // PIN 27 on Z80 // OUTPUT from Z80
int RESET = 0; // PIN 26 on Z80  // INPUT to Z80
int BUSRQ = 7; // PIN 25 on Z80  // INPUT to Z80
int WAIT = 8; // PIN 24 on Z80 // INPUT to  Z80
int BUSAK = 9; // PIN 23 on Z80 // OUTPUT from Z80
int INT = 11;     // pin 16 on Z80 // input to z80
int NMI = 10;    // pin 17 on Z80 // input to z80


int readEnable = 53; // active low 
int writeEnable = 51; // active low 


bool currentClockState = LOW;

void toggleClockFaster()
{
  // clock  
  digitalWrite(CLK, currentClockState);
  if (currentClockState == HIGH) 
  {
    currentClockState = LOW;
  }
  else
  {    
    currentClockState = HIGH;
  }
  delay(10);
}

void toggleClock()
{
  // clock  
  // clock  
  digitalWrite(CLK, currentClockState);
  Serial.print("CLOCK");
  if (currentClockState == HIGH) 
  {
    Serial.println(" HIGH");
    currentClockState = LOW;
  } 
  else 
  {
    Serial.println(" LOW");
    currentClockState = HIGH;
  }
  delay(500);
}


void setAddressPinsToInput()
{
  for (int i = 0; i < NUMBER_OF_ADDRESS_PINS; i++)
  {
    pinMode(addressPins[i+addressPins[0]], INPUT);      
  }
}
  
void  setDataToOutput()
{
    // set pin mode to INPUT
    for (int i = 0; i < NUMBER_OF_DATA_PINS; i++)
    {
        pinMode(dataPins[i+dataPins[0]], OUTPUT);
    }
}
void  setDataToInput()
{
    // set pin mode to INPUT
    for (int i = 0; i < NUMBER_OF_DATA_PINS; i++)
    {
        pinMode(dataPins[i+dataPins[0]], INPUT);
    }
}

void resetCPU()
{
  toggleClockFaster();    
  digitalWrite(RESET, LOW);    
  Serial.println("holding reset low ");
  for (int i = 0; i < 100; i++)
  {
    toggleClockFaster();    
  }
  Serial.println("holding reset high");
  digitalWrite(RESET, HIGH); 
  delay(1000);
}

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  setAddressPinsToInput();
  setDataToOutput();

  pinMode(RESET, OUTPUT);   // remember that the INPUT / OUTPUT is from the point of view of the Arduino  
  pinMode(CLK, OUTPUT);

  
  pinMode(IORQ, INPUT);
  pinMode(MREQ, INPUT);
  pinMode(HALT, INPUT);
  pinMode(M1,INPUT);
  pinMode(REFRESH,INPUT);
  pinMode(BUSRQ,OUTPUT);
  pinMode(WAIT,OUTPUT);
  pinMode(BUSAK,INPUT);
  pinMode(INT,OUTPUT);
  pinMode(NMI,OUTPUT);
  
  pinMode(readEnable, INPUT);   
  pinMode(writeEnable, INPUT);
  
  digitalWrite(BUSRQ,HIGH);
  digitalWrite(WAIT,HIGH);
  digitalWrite(NMI,HIGH);
  digitalWrite(INT,HIGH);

  outputToDataPins(0);      
  // to reset z80 the reset must be held active (low) for a minimum of 3 clock cycles
  // then set to high after
  resetCPU();

}

void outputToDataPins(uint8_t val)
{
    setDataToOutput();
    for (int i = 0; i < NUMBER_OF_DATA_PINS; i++)
    {
        digitalWrite(dataPins[i+dataPins[0]], val << i);       
    }   
}

uint8_t readFromDataPins()
{
    uint8_t rv = 0;
    setDataToInput();
    for (int i = 0; i < NUMBER_OF_DATA_PINS; i++)
    {
        rv |= digitalRead(dataPins[i+dataPins[0]]) << i;       
    }   
    return rv;
}

void runCPUCycle()
{  
  uint16_t addressBus = 0;
  uint8_t dataBus = 0;
  bool writeEn = !digitalRead(writeEnable);    /// active low so not added before
  bool readEn = !digitalRead(readEnable);  /// active low so not added before
  bool ioRequest = !digitalRead(IORQ);
  bool memRequest = !digitalRead(MREQ);
  bool haltSet = !digitalRead(HALT);
  bool M1 = !digitalRead(M1);
  bool refreshSet = !digitalRead(REFRESH);
  bool busAckSet = !digitalRead(BUSAK);

 //pinMode(BUSRQ,OUTPUT);
  //pinMode(WAIT,OUTPUT);
  
  if (haltSet) Serial.print("HALT ");
    
  if (ioRequest || memRequest || haltSet || M1 || refreshSet || busAckSet)
  { 
    Serial.print("status control=");
    if (ioRequest) Serial.print("IOREQ ");
    if (memRequest) Serial.print("MREQ ");
    if (haltSet) Serial.print("HALT ");
    if (M1) Serial.print("M1 ");
    if (refreshSet) Serial.print("REFRESH ");
    if (busAckSet) Serial.print("BUSAK ");
    Serial.println();
  }
  
  if (readEn)
  { 
    Serial.print("RD ");
    // this means the Z80 is trying to read from the address on the address bus
    for (int i = 0; i < NUMBER_OF_ADDRESS_PINS; i++)
    {
        addressBus |= digitalRead(addressPins[i+22]) << i;       
    }
    dataBus = 0x00; // 0x00 is nop instruction
    //dataBus = 0x76; // 0x76 is halt

    outputToDataPins(dataBus);    
    Serial.print("Address bus = ");
    Serial.print(addressBus,BIN);
    Serial.print("\t\t Writing value to data = ");
    Serial.print(dataBus,BIN);    
    Serial.println();
  }
  
  if (writeEn)  
  {  // this means Z80 trying to send some data to data pins, so get arduino to read it
    Serial.print("WRite to address ");
    for (int i = 0; i < NUMBER_OF_ADDRESS_PINS; i++)
    {
        addressBus |= digitalRead(addressPins[i+22]) << i;       
    }    
    Serial.print(addressBus,BIN);        
    dataBus = readFromDataPins();
    Serial.println(dataBus, BIN);    
  }    
}

void runBareCPUCycle()
{  
  uint16_t addressBus = 0;
  uint8_t dataBus = 0;
  bool writeEn = !digitalRead(writeEnable);    /// active low so not added before
  bool readEn = !digitalRead(readEnable);  /// active low so not added before
  bool ioRequest = !digitalRead(IORQ);
  bool memRequest = !digitalRead(MREQ);
  bool haltSet = !digitalRead(HALT);
  bool M1 = !digitalRead(M1);
  bool refreshSet = !digitalRead(REFRESH);
  bool busAckSet = !digitalRead(BUSAK);
    
  if (ioRequest || memRequest || haltSet || M1 || refreshSet || busAckSet || writeEn || readEn)
  { 
    Serial.print("status control=");
    if (ioRequest == true) Serial.print("IOREQ ");
    if (memRequest== true) Serial.print("MREQ ");
    if (haltSet== true) Serial.print("HALT ");
    if (M1== true) Serial.print("M1 ");
    if (refreshSet== true) Serial.print("REFRESH ");
    if (busAckSet== true) Serial.print("BUSAK ");
    if (readEn== true) Serial.print("RD ");    
    if (writeEn== true) Serial.print("WR ");  
    Serial.println();
  }   
  setDataToOutput();
  setAddressPinsToInput();
  
  // this means the Z80 is trying to read from the address on the address bus
  for (int i = 0; i < NUMBER_OF_ADDRESS_PINS; i++)
  {
      addressBus |= digitalRead(addressPins[i+addressPins[0]]) << i;       
  }
  for (int i = 0; i < NUMBER_OF_DATA_PINS; i++)
  {
      dataBus |= digitalRead(dataPins[i+dataPins[0]]) << i;       
  }   
  Serial.print("Address bus = ");
  Serial.print(addressBus,HEX);
  Serial.print("\t\t Data bus = ");
  Serial.print(dataBus,HEX);    
  Serial.println();
}

void loop() {
  //runCPUCycle();
  runBareCPUCycle();
  toggleClock();
}
