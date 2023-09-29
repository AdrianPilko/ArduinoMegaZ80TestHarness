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



This now is the basis for a z80 test harness where the all machine cycles can be 
implemented and emulate RAM (data and program code.


*/

// give it 1K of RAM, should be enough for program and some data
#define SIZE_OF_RAM 1024
uint8_t Z80_RAM[SIZE_OF_RAM];
uint16_t addressBus = 0;
uint8_t dataBus;


#define NUMBER_OF_ADDRESS_PINS 16
#define NUMBER_OF_DATA_PINS 8
int addressPins[NUMBER_OF_ADDRESS_PINS] = {52,50,48,46,44,42,40,38,36,34,32,30,28,26,24,22};
int dataPins[NUMBER_OF_DATA_PINS] = {15,14,17,20,21,19,18,16};

int CLK = 12; 
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

void setAddressPinsToInput()
{
  for (int i = 0; i < NUMBER_OF_ADDRESS_PINS; i++)
  {
    pinMode(addressPins[i], INPUT);      
  }
}
  
void  setDataToOutput()
{
    // set pin mode to OUTPUT
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
    // clock provided externally by 555 timer
  Serial.println();
  Serial.println("CPU is in reset, RESET held active");
  digitalWrite(RESET, LOW);      
  for (int i = 0; i < 6; i++)
  {
    waitExternalClock();
  }
  digitalWrite(RESET, HIGH); 
  Serial.println("CPU now reset");
}

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  pinMode(RESET, OUTPUT);   // remember that the INPUT / OUTPUT is from the point of view of the Arduino  
  pinMode(CLK, INPUT);   // to grab 555 timer clock output

  
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

void printStatus()
{
  if (readEn == true) Serial.print("RD ");
  if (writeEn == true) Serial.print("WR ");
  if (ioRequest == true) Serial.print("IOREQ ");
  if (memRequest == true) Serial.print("MREQ ");
  if (haltSet == true) 
  {
    Serial.print("HALT ");
    delay(10000);
    exit(0); // seems fair to exit the test harness at this point!
  }
  if (MachineOne== true) Serial.print("M1 ");
  if (refreshSet== true) Serial.print("REFRESH ");
  if (busAckSet== true) Serial.print("BUSAK ");   
}

void readAddressBus()
{    
    for (int i = 0; i < NUMBER_OF_ADDRESS_PINS; i++)
    {
        addressBus <<= 1;
        addressBus |= digitalRead(addressPins[i]);       
    }
}

void printAddressAndDataBus()
{
  Serial.print("0x");
  Serial.print(addressBus,HEX); 
  if(writeEn)  
    Serial.print(" W "); 
  else
    Serial.print(" R "); 
  Serial.print("0x");
  Serial.print(dataBus,HEX); 
  Serial.println();
}

// we need to keep track of Timing cycle and Machine cycle 
int T_cycle = 1; // cycles from 1 to 4 on an instruction fetch, other cycles may be longer
int M_cycle = 1; // M state cycles through (as far as I can tell M1 opcode fetch, M2 - memory read,  M3 - memory write


void initialiseProgram()
{
  // ok so write a simple machine code program
#if 0  
  Z80_RAM[0] = 0x06;
  Z80_RAM[1] = 0x50;
  Z80_RAM[2] = 0x3e;
  Z80_RAM[3] = 0x51;
  Z80_RAM[4] = 0x80;
  Z80_RAM[20] = 0x76;
  Z80_RAM[0x50] = 0x1d;
  Z80_RAM[0x50] = 0x1e;
#endif
  Z80_RAM[0] = 0x00;
  Z80_RAM[1] = 0x00;
  Z80_RAM[2] = 0x00;
  Z80_RAM[3] = 0x00;
  Z80_RAM[4] = 0x76;
}

bool lastClock = false;
int clockTransitionLowHigh = 0;

void waitExternalClock()
{
  bool clockTransitioned = false;
  while (!clockTransitioned)
  {
    bool currentClock = digitalRead(CLK);
    if ((currentClock == false) && (lastClock == true))
    {
      Serial.print(clockTransitionLowHigh++);
      Serial.print("\t\t");
     
      readStatus();
      printStatus(); 
      Serial.print("\t");
      readAddressBus();
      Serial.print("\t");
      printAddressAndDataBus();
      clockTransitioned = true;
    }
   // if ((currentClock == false) && (lastClock == true)) break;
    lastClock = currentClock;        
  }  
}

void loop() 
{  
  while(1)
  {  
    digitalWrite(BUSRQ,HIGH);
    digitalWrite(WAIT,HIGH);
    digitalWrite(NMI,HIGH);
    digitalWrite(INT,HIGH); 
    
    setDataToOutput();
    MachineOne = false;
  
  
    waitExternalClock();
    
    if (refreshSet == true)
    {
      // do thothing!!!
    }
    else
    {
      if ((MachineOne) && (memRequest) && (readEn))
      {   
        printStatus(); 
        setDataToOutput();  
        readAddressBus();
        dataBus = Z80_RAM[addressBus];
        if (addressBus > SIZE_OF_RAM)
        {
          Serial.print("got address outside RAM");
          delay(250);
          exit(0);
        }    
        outputToDataPins(dataBus);
        printAddressAndDataBus();
      }     
      else if (readEn)
      {
          printStatus();
          setDataToOutput();  
          readAddressBus();
           
          if (addressBus > SIZE_OF_RAM)
          {
            Serial.print("got address outside RAM");
            delay(250);
            exit(0);
          }
          dataBus = Z80_RAM[addressBus];        
          outputToDataPins(dataBus);
          printAddressAndDataBus();
      }
      else if (writeEn)
      {
        printStatus();
        setDataToInput();
        readAddressBus();
        if (addressBus < SIZE_OF_RAM)
        {
          dataBus = Z80_RAM[addressBus] =  readFromDataPins();
        }  
        printAddressAndDataBus();
      }
    }
  
    readEn = false;
    writeEn = false;
    ioRequest = false;
    memRequest = false;
    haltSet = false;
    MachineOne = false;
    refreshSet = false;
    busAckSet = false;
  }
}
