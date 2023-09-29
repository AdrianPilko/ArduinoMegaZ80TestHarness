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

CPU is in reset, RESET held active

CPU is in reset, RESET held active
CPU now reset
****OP FETCH  RD MREQ M1 Address bus = 0x0 R  DataBus = 0x6
****OP FETCH  RD MREQ M1 Address bus = 0x0 R  DataBus = 0x6
****READ  RD MREQ Address bus = 0x1 R  DataBus = 0x11
****READ  RD MREQ Address bus = 0x1 R  DataBus = 0x11
****OP FETCH  RD MREQ M1 Address bus = 0x2 R  DataBus = 0x3E
****OP FETCH  RD MREQ M1 Address bus = 0x2 R  DataBus = 0x3E
****READ  RD MREQ Address bus = 0x3 R  DataBus = 0x22
****READ  RD MREQ Address bus = 0x3 R  DataBus = 0x22
****OP FETCH  RD MREQ M1 Address bus = 0x4 R  DataBus = 0x80
****OP FETCH  RD MREQ M1 Address bus = 0x4 R  DataBus = 0x80
****OP FETCH  RD MREQ M1 Address bus = 0x5 R  DataBus = 0x32
****OP FETCH  RD MREQ M1 Address bus = 0x5 R  DataBus = 0x32
****READ  RD MREQ Address bus = 0x6 R  DataBus = 0xC
****READ  RD MREQ Address bus = 0x6 R  DataBus = 0xC
****READ  RD MREQ Address bus = 0x7 R  DataBus = 0x0
****READ  RD MREQ Address bus = 0x7 R  DataBus = 0x0
****WRITE WR MREQ Address bus = 0xC W  DataBus = 0x33
****OP FETCH  RD MREQ M1 Address bus = 0x8 R  DataBus = 0x76
****OP FETCH  RD MREQ M1 Address bus = 0x8 R  DataBus = 0x76
****OP FETCH  RD MREQ HALT 
Memory contents
address = 0x0 0x6
address = 0x1 0x11
address = 0x2 0x3E
address = 0x3 0x22
address = 0x4 0x80
address = 0x5 0x32
address = 0x6 0xC
address = 0x7 0x0
address = 0x8 0x76
address = 0x9 0x0
address = 0xA 0x0
address = 0xB 0x0
address = 0xC 0x33
address = 0xD 0x0
address = 0xE 0x0
address = 0xF 0x0
address = 0x10  0x0
address = 0x11  0x0
address = 0x12  0x0
address = 0x13  0x0
address = 0x14  0x0
address = 0x15  0x0
address = 0x16  0x0
address = 0x17  0x0
address = 0x18  0x0
address = 0x19  0x0
address = 0x1A  0x0
address = 0x1B  0x0
address = 0x1C  0x0
address = 0x1D  0x0
address = 0x1E  0x0
address = 0x1F  0x0


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
  for (int i = 0; i < 8; i++)
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
    printFirst32MemeoryLocations();
    delay(10000);
    exit(0); // seems fair to exit the test harness at this point!
  }
  if (MachineOne== true) Serial.print("M1 ");
  if (refreshSet== true) Serial.print("REFRESH ");
  if (busAckSet== true) Serial.print("BUSAK ");   
}

void printFirst32MemeoryLocations()
{
  Serial.println();
  Serial.println("Memory contents");
  for (int i = 0; i < 32; i++)
  {
    Serial.print("address = 0x");
    Serial.print(i, HEX);
    Serial.print("\t0x");
    Serial.println(Z80_RAM[i], HEX);
  }
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
  Serial.print("Address bus = 0x");
  Serial.print(addressBus,HEX); 
  if(writeEn)  
    Serial.print(" W "); 
  else
    Serial.print(" R "); 
  Serial.print(" DataBus = 0x");
  Serial.print(dataBus,HEX); 
  Serial.println();
}

// we need to keep track of Timing cycle and Machine cycle 
int T_cycle = 1; // cycles from 1 to 4 on an instruction fetch, other cycles may be longer
int M_cycle = 1; // M state cycles through (as far as I can tell M1 opcode fetch, M2 - memory read,  M3 - memory write


void initialiseProgram()
{
/*
  // s simple machine code program adds two 8 bit numbers and store in 3 locations
  
  Z80_RAM[0] = 0x06;  // ld b, n  (take conents of next memory address as operand)
  Z80_RAM[1] = 0x11;  
  Z80_RAM[2] = 0x3e;  // ld a, n  (take conents of next memory address as operand)
  Z80_RAM[3] = 0x22;  
  Z80_RAM[4] = 0x80;  // add, a,b  stores result in a
  Z80_RAM[5] = 0x32;  // ld (nn), a  (takes next two memory locations as 
  Z80_RAM[6] = 0x1c;  // 
  Z80_RAM[7] = 0x00;  // 
  Z80_RAM[8] = 0x32;  // ld (nn), a  (takes next two memory locations as 
  Z80_RAM[9] = 0x1d;  // 
  Z80_RAM[10] = 0x00;  // 
  Z80_RAM[11] = 0x32;  // ld (nn), a  (takes next two memory locations as 
  Z80_RAM[12] = 0x1e;  // 
  Z80_RAM[13] = 0x00;  // 
  Z80_RAM[14] = 0x76;
*/  

// s simple machine code program loops 255  incrementing "a" and 
// using djnz (auto decrement b and test zero  
  Z80_RAM[0] = 0x06;  // ld b, n  (take conents of next memory address as operand)
  Z80_RAM[1] = 0xff;  
  Z80_RAM[2] = 0xaf;  // xor a , this zeros a
  Z80_RAM[3] = 0x3c;  // inc c
  Z80_RAM[4] = 0x10;  // djnz 
  Z80_RAM[5] = 0xfd;  // relative jump location (twos compliment = -2)
  Z80_RAM[6] = 0x32;  // ld (nn), a  (takes next two memory locations as 
  Z80_RAM[7] = 0x1d;  // 
  Z80_RAM[8] = 0x00;  // 
  Z80_RAM[9] = 0x32;  // ld (nn), a  (takes next two memory locations as 
  Z80_RAM[10] = 0x1e;  // 
  Z80_RAM[11] = 0x00;  // 
  Z80_RAM[12] = 0x76;
  
/*   simple nop and halt code to test databus
  Z80_RAM[0] = 0x00;
  Z80_RAM[1] = 0x00;
  Z80_RAM[2] = 0x00;
  Z80_RAM[3] = 0x00;
  Z80_RAM[4] = 0x76;
*/
  
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
      //Serial.print(clockTransitionLowHigh++);
      //Serial.print("\t\t");
     
      readStatus();
      //printStatus(); 
      //Serial.print("\t");
      //readAddressBus();
      //Serial.print("\t");
      //printAddressAndDataBus();
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
    
    MachineOne = false;
 
    waitExternalClock();
    
    if (refreshSet == true)
    {
      // do nothing!!!
    }
    else
    {
      if ((MachineOne) && (memRequest) && (readEn))
      {   
        Serial.print("****OP FETCH\t");
        printStatus(); 
        setDataToOutput();  
        readAddressBus();
        dataBus = Z80_RAM[addressBus];
        if (addressBus > SIZE_OF_RAM)
        {
          Serial.print("got address outside RAM::");
          printAddressAndDataBus();
          delay(250);
          exit(0);
        }    
        outputToDataPins(dataBus);
        printAddressAndDataBus();
      }     
      else if ((readEn) && (memRequest))
      {
          Serial.print("****READ\t");
          printStatus();
          readAddressBus();
           
          if (addressBus > SIZE_OF_RAM)
          {
            Serial.print("got address outside RAM");
            printFirst32MemeoryLocations();
            delay(250);
            exit(0);
          }
          dataBus = Z80_RAM[addressBus];        
          outputToDataPins(dataBus);
          printAddressAndDataBus();
      }
      else if ((writeEn) && (memRequest))
      {
        Serial.print("****WRITE\t");
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
