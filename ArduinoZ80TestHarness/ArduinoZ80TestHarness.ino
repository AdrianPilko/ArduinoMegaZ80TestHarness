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

Not all cycles are implemented, can write a program and put in the Z80_RAM[]
in function "initialiseProgram()"

currently only have nops followed by HALT, all the machine cycles will be implemented eventually
1) instruction op code fetch
2) memory read or write
3) Input or output cycles
4) Interrupt request / ack
5) bus request / ack

this is the output for that extremely simple program:

holding reset low 
holding reset high
RD MREQ M1 Address set to=0
Setting data bus to 0
RD MREQ M1 Address set to=0
Setting data bus to 0
MREQ REFRESH REFRESH RD MREQ M1 Address set to=1
Setting data bus to 0
RD MREQ M1 Address set to=1
Setting data bus to 0
MREQ REFRESH REFRESH RD MREQ M1 Address set to=2
Setting data bus to 0
RD MREQ M1 Address set to=2
Setting data bus to 0
MREQ REFRESH REFRESH RD MREQ M1 Address set to=3
Setting data bus to 0
RD MREQ M1 Address set to=3
Setting data bus to 0
MREQ REFRESH REFRESH RD MREQ M1 Address set to=4
Setting data bus to 0
RD MREQ M1 Address set to=4
Setting data bus to 0
MREQ REFRESH REFRESH RD MREQ M1 Address set to=5
Setting data bus to 0
RD MREQ M1 Address set to=5
Setting data bus to 0
MREQ REFRESH REFRESH RD MREQ M1 Address set to=6
Setting data bus to 76
RD MREQ M1 Address set to=6
Setting data bus to 76
MREQ REFRESH HALT 


This now is the basis for a z80 test harness where the all machine cycles can be 
implemented and emulate RAM (data and program code.


*/

// give it 1K of RAM, should be enough for program and some data
uint8_t Z80_RAM[1024];
uint16_t addressBus = 0;


#define NUMBER_OF_ADDRESS_PINS 16
#define NUMBER_OF_DATA_PINS 8
int addressPins[NUMBER_OF_ADDRESS_PINS] = {22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52};
int dataPins[NUMBER_OF_DATA_PINS] = {14,15,16,17,18,19,20,21};

int CLK = 49; 
int HALT = 4; // pin 18 on Z80 // output from z80
int MREQ = 3; // pin 19 on Z80 // output from z80
int IORQ = 2; // pin 20 on z80 // output from z80

int REFRESH = 6;  // PIN 28 on Z80 OUTPUT from Z80
int M1 = 5; // PIN 27 on Z80 // OUTPUT from Z80
int RESET = 47; // PIN 26 on Z80  // INPUT to Z80
int BUSRQ = 7; // PIN 25 on Z80  // INPUT to Z80
int WAIT = 8; // PIN 24 on Z80 // INPUT to  Z80
int BUSAK = 9; // PIN 23 on Z80 // OUTPUT from Z80
int INT = 11;     // pin 16 on Z80 // input to z80
int NMI = 10;    // pin 17 on Z80 // input to z80

int readEnable = 53; // active low 
int writeEnable = 51; // active low 


bool writeEn = false;
bool readEn = false;
bool ioRequest = false;
bool memRequest =false;
bool haltSet = false;
bool MachineOne = false;
bool refreshSet = false;
bool busAckSet = false;

int clockLowCount = 0;
int clockHighCount = 0;
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

void setClock(bool high)
{
  // true == high !
  if (high == true)
  {
    digitalWrite(CLK, HIGH);
  }
  else
  {
    digitalWrite(CLK, LOW);
  }
}

void toggleClock()
{
  // clock  
  digitalWrite(CLK, currentClockState);
  //Serial.print("CLOCK");
  if (currentClockState == HIGH) 
  {
    //Serial.println(" HIGH");
    currentClockState = LOW;
  } 
  else 
  {
    //Serial.println(" LOW");
    currentClockState = HIGH;
  }
  delay(250);
}

void initialiseProgram()
{
  // ok so write a simple machine code program
  Z80_RAM[0] = 0x0;
  Z80_RAM[1] = 0x0;
  Z80_RAM[2] = 0x0;
  Z80_RAM[3] = 0x0;
  Z80_RAM[4] = 0x0;
  Z80_RAM[5] = 0x0;
  Z80_RAM[6] = 0x76;
}


void setAddressPinsToInput()
{
  for (int i = 0; i < NUMBER_OF_ADDRESS_PINS; i++)
  {
    pinMode(addressPins[i], INPUT);      
  }
}
  
void  setDataToOutput()
{
    // set pin mode to INPUT
    for (int i = 0; i < NUMBER_OF_DATA_PINS; i++)
    {
        pinMode(dataPins[i], OUTPUT);
    }
}
void  setDataToInput()
{
    // set pin mode to INPUT
    for (int i = 0; i < NUMBER_OF_DATA_PINS; i++)
    {
        pinMode(dataPins[i], INPUT);
    }
}

void resetCPU()
{
  digitalWrite(RESET, LOW);    
  toggleClock();    
  Serial.println("holding reset low ");
  for (int i = 0; i < 24; i++)
  {
    toggleClock();    
    digitalWrite(RESET, LOW);
  }
  Serial.println("holding reset high");
  digitalWrite(RESET, HIGH); 
  toggleClock();   
}

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
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
  
  setAddressPinsToInput();
  setDataToInput();
  initialiseProgram();
  // to reset z80 the reset must be held active (low) for a minimum of 3 clock cycles
  // then set to high after
  resetCPU();
  

}

void outputToDataPins(uint8_t val)
{       
    setDataToOutput();
    Serial.print("Setting data bus to ");
    Serial.println(val,HEX);    
    
    for (int i = 0; i < NUMBER_OF_DATA_PINS; i++)
    {
      uint8_t thebit = val & 0x01;
      
      digitalWrite(dataPins[i], thebit);       

      val >>= 1;
    }   
}

uint8_t readFromDataPins()
{
    uint8_t rv = 0;
    setDataToInput();
    for (int i = 0; i < NUMBER_OF_DATA_PINS; i++)
    {
        rv |= digitalRead(dataPins[i]) << i;       
    }   
    return rv;
}

void readStatus()
{
  writeEn = !digitalRead(writeEnable);    /// active low so not added before
  readEn = !digitalRead(readEnable);  /// active low so not added before
  ioRequest = !digitalRead(IORQ);
  memRequest = !digitalRead(MREQ);
  haltSet = !digitalRead(HALT);
  MachineOne = !digitalRead(M1);
  refreshSet = !digitalRead(REFRESH);
  busAckSet = !digitalRead(BUSAK);
}

void printCurrentAddressBus()
{    
    for (int i = 0; i < NUMBER_OF_ADDRESS_PINS; i++)
    {
        addressBus <<= 1;
        addressBus |= digitalRead(addressPins[i]);       
    }
    Serial.print("Address set to=");
    Serial.print(addressBus,HEX); 
    Serial.println();
}



// we need to keep track of Timing cycle and Machine cycle 
int T_cycle = 1; // cycles from 1 to 4 on an instruction fetch, other cycles may be longer
int M_cycle = 1; // M state cycles through (as far as I can tell M1 opcode fetch, M2 - memory read,  M3 - memory write

void loop() 
{    
  digitalWrite(BUSRQ,HIGH);
  digitalWrite(WAIT,HIGH);
  digitalWrite(NMI,HIGH);
  digitalWrite(INT,HIGH); 

  setClock(HIGH);
  clockHighCount++;
  delay(10);
  setClock(LOW);
  delay(10);
  clockLowCount++;
  readStatus();
  if (readEn == true) Serial.print("RD ");
  if (writeEn == true) Serial.print("WR ");
  if (ioRequest == true) Serial.print("IOREQ ");
  if (memRequest== true) Serial.print("MREQ ");
  if (haltSet== true) 
  {
    Serial.print("HALT ");
    delay(10000);
    exit(0); // seems fair to exit the test harness at this point!
  }
  if (MachineOne== true) Serial.print("M1 ");
  if (refreshSet== true) Serial.print("REFRESH ");
  if (busAckSet== true) Serial.print("BUSAK ");   
  
  if (memRequest && MachineOne && readEn) 
  {
      printCurrentAddressBus();
      outputToDataPins(Z80_RAM[addressBus]);
  }     
}
