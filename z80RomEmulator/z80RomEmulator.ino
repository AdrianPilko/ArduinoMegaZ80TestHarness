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
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRING EMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Ardiuno Mega test harness for Z80A CPU - this code just reads the addrress/data bus
and some of the CPU control pins, then outputs to the terminal
All clock, and control pins must be generated/handled externally


it's possible assemble small programs using https://www.asm80.com/ and then paste the contents 
of the hex file into programFile.h::Z80_ROM char[]
*/

//#define DISABLE_SERIAL_OUTPUT
//#define DEBUG_RUN_SLOW

#include <avr/sleep.h>
#include <stdio.h>
#include "programFile.h"

extern const char  ROM_image[];
const int SIZE_OF_RAM = 2048;  // bytes
//const int SIZE_OF_RAM = 512;  // bytes
const int NUMBER_OF_IO_PORTS = 3;
uint16_t addressBus = 0;
uint8_t dataBus = 0;

const int TEN_SECONDS = 10000;
#define NUMBER_OF_ADDRESS_PINS 11     // this gives memory size of 2^11 = 2048
#define NUMBER_OF_DATA_PINS 8
                                           // A10 pin  ======>    A0 pin
int addressPins[NUMBER_OF_ADDRESS_PINS] = {32,34,36,38,40,42,44,46,48,50,52};

// pins on arduino are in sequence but z80 are not so have make sure Z80 is connected properly
                                  // D0   ======>         D7 pin
int dataPins[NUMBER_OF_DATA_PINS] = {37,35,33,31,29,27,25,23};
/// we can use the many arduino pins as mock io ports and read/write to them from the z80
// we also need some logic to map them to the memory address space
int ioPortPinMapping[NUMBER_OF_IO_PORTS] = {10,11,12};
unsigned char  Z80_IO[NUMBER_OF_IO_PORTS];



/// these are the pins to connect the control lines to the Z80
int RD = 2; // active low 
int WR = 3; // active low 
int CLK = 4; 
int RESET = 5; // PIN 26 on Z80  // INPUT to Z80
int MREQ = 6; // pin 19 on Z80 // output from z80
int REFRESH = 7;
int HALT = 8;
int IORQ = 9;


bool writeEn = false;
bool readEn = false;
bool ioRequest = false;
bool memRequest =false;
bool haltSet = false;
bool MachineOne = false;
bool refreshSet = false;
bool busAckSet = false;
bool resetSet = true;

unsigned char Z80_RAM[SIZE_OF_RAM]; // 512 bytes of RAM should be plenty here :)

#ifdef DEBUG_RUN_SLOW
const int HALF_CLOCK_RATE = 50;   // slow mode for DEBUG
#else
const int HALF_CLOCK_RATE = 1;   // this becomes delay so 1 is delay 1 msec
#endif

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


void initialiseTest()
{  
  pinMode(CLK, OUTPUT);   // to grab 555 timer clock output  
  pinMode(RESET,OUTPUT);
  pinMode(RD, INPUT);   
  pinMode(WR, INPUT);
  pinMode(MREQ, INPUT); 
  pinMode(REFRESH, INPUT); 
  pinMode(IORQ, INPUT); 
  pinMode(HALT, INPUT);   
  memset(Z80_RAM,0, sizeof(char) * SIZE_OF_RAM);
  initialiseProgram();
  setAddressPinsToInput();
  setDataToInput();
  resetCPU();
}
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  set_sleep_mode(SLEEP_MODE_STANDBY);  
  initialiseTest();
}

inline uint8_t readFromDataPins()
{
    uint8_t rv = 0;
    setDataToInput();
    for (int i = 0; i < NUMBER_OF_DATA_PINS; i++)
    {
        rv |= digitalRead(dataPins[i]) << i;       
    }   
    return rv;
}

inline void readStatus()
{
  writeEn = !digitalRead(WR);    /// active low so not added before
  readEn = !digitalRead(RD);  /// active low so not added before
  memRequest = !digitalRead(MREQ);
  refreshSet = !digitalRead(REFRESH);
  ioRequest = !digitalRead(IORQ);
  haltSet = !digitalRead(HALT);
}



inline void printStatus()
{
  #ifndef DISABLE_SERIAL_OUTPUT
  if (readEn == true) Serial.print("RD ");
  if (writeEn == true) Serial.print("WR ");
  if (memRequest == true) Serial.print("MREQ ");
  if (refreshSet == true) Serial.print("REFRESH ");
  if (ioRequest == true) Serial.print("IORQ ");
  #endif  
  if (haltSet == true) 
  {
      Serial.println("PROCESSOR HALT");  
  }

}

inline void readAddressBus()
{    
    addressBus = 0;
    for (int i = 0; i < NUMBER_OF_ADDRESS_PINS; i++)
    {
        addressBus <<= 1;
        addressBus |= digitalRead(addressPins[i]);       
    }
}

inline void toggleClock()
{
  static int clockState = LOW;
  if (clockState == LOW)
  {
    digitalWrite(CLK, HIGH);
    clockState = HIGH;
  }
  else
  {
    digitalWrite(CLK, LOW);
    clockState = LOW;
  }
}

inline void resetCPU()
{
  digitalWrite(RESET, LOW);
  for (int i = 0; i < 10; i++)
  {
    toggleClock();
    delay(20);
    toggleClock();
    delay(20);    
  }   
  digitalWrite(RESET, HIGH);
  Serial.println("CPU is Reset");
}

inline void printAddressAndDataBus()
{
  #ifndef DISABLE_SERIAL_OUTPUT
  
  // address 
  Serial.print(" 0x");
  Serial.print(addressBus,HEX); 
  if(writeEn)  
    Serial.print("\tW "); 
  else
    Serial.print("\tR "); 
  Serial.print(" 0x");
  Serial.print(dataBus,HEX); 
  Serial.println();
  #endif  
}


int clockCount = 0;
bool lastClock = true;
bool currentClock = false;

void waitExternalClock()
{
  bool clockTransitioned = false;  
  

  while (!clockTransitioned)
  {    
    currentClock = digitalRead(CLK);
    
    if ((currentClock == false) && (lastClock == true))
    { 
      
      clockTransitioned = true;            
    }      
    lastClock = currentClock;
  }  
}

void initialiseProgram()
{
  // here we parse the data from programFile.h, it's a string of characters of bytes, 
  // zero must be fully expanded ie 00 or 01, 0 or 1
  //char ROM_image[] = "062F033effd301afD30110f73effD30076\0";  

  size_t progLengthInChars = strlen(ROM_image);
  char * ptrToROM = ROM_image;
  
  int programCounter = 0;

  for (int i = 0; i < progLengthInChars; i++)
  {
    char tempTwochars[3];
    tempTwochars[0]=*ptrToROM;
    ptrToROM++;
    tempTwochars[1]=*ptrToROM;
    ptrToROM++;
    tempTwochars[2]='\0';
    int theInstructionOrData = (int)strtol(tempTwochars, NULL, 16);

    Z80_RAM[programCounter] = theInstructionOrData;
    programCounter++;
  }
  
  Serial.println("ROM Image = ");
  Serial.println(ROM_image);
  printMemory();
  Serial.flush();
  
  Serial.print("loaded program ");
  Serial.print(progLengthInChars / 2);
  Serial.println(" bytes long");
  delay(500);
}

void printMemory()
{
  Serial.println();
  Serial.println("Memory contents");
  for (int i = 0; i < SIZE_OF_RAM; i++)
  {
    if (i % 16 == 0) 
    {      
      Serial.println();      
      Serial.print("address = 0x");
      Serial.print(i, HEX);    
      Serial.print("\t\t");
      if (Z80_RAM[i] < 16) // print a dummy zero
      {
        Serial.print("0");
      }      
      Serial.print(Z80_RAM[i], HEX);
    }    
    else
    { 
      Serial.print(":");     
      if (Z80_RAM[i] < 16) // print a dummy zero
      {
        Serial.print("0");
      }
      Serial.print(Z80_RAM[i], HEX);      
    }
  }
  Serial.println();
}

void printIOPorts()
{
  #ifndef DISABLE_SERIAL_OUTPUT
  Serial.println();
  Serial.println("IO ports contents:");
  for (int i = 0; i < NUMBER_OF_IO_PORTS; i++)
  {
      Serial.print("Port = ");
      Serial.print(i);    
      Serial.print("=");
      Serial.print(Z80_IO[i], HEX);
      Serial.println("\t");
  }
  Serial.println();
  #endif
}

void loop() 
{ 
  while(1)
  {
    delay(HALF_CLOCK_RATE);
    toggleClock();
    delay(HALF_CLOCK_RATE);
    toggleClock();
    readStatus();
    //printStatus();
    // this is halt instruction so stop and print memory
    if (haltSet == true)
    {
       printStatus();
       printMemory();
       Serial.flush();
       sleep_mode();
    }
    readAddressBus();
    // the z80 will assert the RD and MREQ when reading from RAM, bvut we also have to check that REFRSH is not active
    // the logic here is active high (reverse when read from pins), the z80 in reality uses active low for cpu control pins

    if ((readEn) && (memRequest) && (!refreshSet))
    {
      if (addressBus < SIZE_OF_RAM)
      {
        dataBus = Z80_RAM[addressBus];
      }
      else
      {
        Serial.println("SEG FAULT attempt to read off end of memory");
        printMemory();
        Serial.flush();
        sleep_mode();
      }
      printStatus();
      printAddressAndDataBus(); 
      outputToDataPins(dataBus);
      
    }
    if ((writeEn) && (memRequest) && (!refreshSet))
    {
      printStatus();
      dataBus = readFromDataPins();
      if (addressBus < SIZE_OF_RAM)
      {
        Z80_RAM[addressBus] = dataBus;
      }
      else
      {
        Serial.println("SEG FAULT attempt to read off end of memory");
        printMemory();
        Serial.flush();
        sleep_mode();
      }      
      printAddressAndDataBus();
    }    
    // handle the out (nn), a instructions
    if ((writeEn) && (ioRequest) && (!refreshSet))
    {
      printStatus();
      dataBus = readFromDataPins();
      addressBus &= 0x00ff;  // the z80 does put stuff in upper 16bits of the address for some reason on IORQ
      if (addressBus < NUMBER_OF_IO_PORTS)
      {
        Z80_IO[addressBus] = dataBus;
        // we only have a 1 bit io on the arduino, not full 8 but output digital 1 to that mapped pin if lowest bit set
        // the idea is to have an led connected to it and switch it on, ideally we'd have 8 leds per port!
        if (Z80_IO[addressBus] & 0x01 == 1)
        {
            digitalWrite(ioPortPinMapping[addressBus], HIGH);
        }
        else
        {
            digitalWrite(ioPortPinMapping[addressBus], LOW);
        }
      }
      else
      {
        Serial.print("IO FAULT attempt to write to non existant port at address = ");
        Serial.println(addressBus, HEX);
        Serial.print(" with data = ");
        Serial.println(dataBus, HEX);
        printIOPorts();
        printMemory();
        Serial.flush();
        sleep_mode();
      }      
      printAddressAndDataBus();
      printIOPorts();
    }        
    // handle the in (nn), a instructions
    if ((readEn) && (ioRequest) && (!refreshSet))
    {
      printStatus();
      dataBus = readFromDataPins();
      if (addressBus < NUMBER_OF_IO_PORTS)
      {
        dataBus = Z80_IO[addressBus];
        outputToDataPins(dataBus);
      }
      else
      {
        Serial.println("IO FAULT attempt to read from to non existant port");
        printMemory();
        Serial.flush();
        sleep_mode();
      }      
      printAddressAndDataBus();
      printIOPorts();
    }       
  }
}
