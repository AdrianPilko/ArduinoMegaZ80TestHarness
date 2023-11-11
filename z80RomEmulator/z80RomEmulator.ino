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


it's possible assemble small programs using https://www.asm80.com/onepage/asmz80.html and
add them to the "simulated ROM"
*/


#include <avr/sleep.h>


//#define DISABLE_SERIAL_OUTPUT

const int programMode = 6;
const int SIZE_OF_RAM = 512;
const int NUMBER_OF_IO_PORTS = 3;
//uint16_t addressBus = 0;
uint8_t addressBus = 0;
uint8_t dataBus = 0;

const int TEN_SECONDS = 10000;
#define NUMBER_OF_ADDRESS_PINS 9
#define NUMBER_OF_DATA_PINS 8
int addressPins[NUMBER_OF_ADDRESS_PINS] = {36,38,40,42,44,46,48,50,52};
// pins on arduino are in sequence but z80 are not so have make sure Z80 is connected properly
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

const int HALF_CLOCK_RATE = 1;

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
  writeEn = !digitalRead(WR);    /// active low so not added before
  readEn = !digitalRead(RD);  /// active low so not added before
  memRequest = !digitalRead(MREQ);
  refreshSet = !digitalRead(REFRESH);
  ioRequest = !digitalRead(IORQ);
  haltSet = !digitalRead(HALT);
}



void printStatus()
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

void readAddressBus()
{    
    addressBus = 0;
    for (int i = 0; i < NUMBER_OF_ADDRESS_PINS; i++)
    {
        addressBus <<= 1;
        addressBus |= digitalRead(addressPins[i]);       
    }
}

void toggleClock()
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

void resetCPU()
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

void printAddressAndDataBus()
{
  #ifndef DISABLE_SERIAL_OUTPUT
  
  Serial.print("\t\tAddress bus = 0x");
  Serial.print(addressBus,HEX); 
  Serial.print(" ");
  Serial.print(addressBus,BIN); 
  if(writeEn)  
    Serial.print("\t\tW "); 
  else
    Serial.print("\t\tR "); 
  Serial.print("DataBus = 0x");
  Serial.print(dataBus,HEX); 
  Serial.print(" ");
  Serial.print(dataBus,BIN); 
  
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
  // simple machine code program adds two 8 bit numbers and store in 3 locations
  if (programMode == 1)
  {
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
  }
  else if (programMode == 2)
  {
    // s simple machine code program loops 0x0f  incrementing "a" and 
    // using djnz (auto decrement b and test zero  
      Z80_RAM[0] = 0xaf;  // xor a , this zeros a
      Z80_RAM[1] = 0x06;  // ld b, n  (take conents of next memory address as operand)
      Z80_RAM[2] = 0x0f;  // loop counter in b   
      Z80_RAM[3] = 0x3c;  // inc a
      Z80_RAM[4] = 0x32;  // ld (nn), a  (takes next two memory locations as operand)
      Z80_RAM[5] = 0x1d;  // just write registger a to 0x1d in the loop so we can see it in the serial
      Z80_RAM[6] = 0x00;  //   
      Z80_RAM[7] = 0x10;  // djnz 
      Z80_RAM[8] = 0xfa;  // relative jump location (twos compliment = -5 == 250 = 0xfa)
      Z80_RAM[9] = 0x32;  // ld (nn), a  (takes next two memory locations as operand)
      Z80_RAM[10] = 0x1d;  // 
      Z80_RAM[11] = 0x00;  // 
      Z80_RAM[12] = 0x32;  // ld (nn), a  (takes next two memory locations as operand)
      Z80_RAM[13] = 0x1e;  // 
      Z80_RAM[14] = 0x00;  // 
      Z80_RAM[15] = 0x76;
  }
  else if (programMode == 3)
  {
    // nop and halt code to test databus
    Z80_RAM[0] = 0x00;  // nop
    Z80_RAM[1] = 0x00;   // nop
    Z80_RAM[2] = 0x76;   // halt
  } 
  else if (programMode == 4)
  {
    Z80_RAM[0] = 0x3e;  // ld a, 0x0f
    Z80_RAM[1] = 0b01010101;  // 
    Z80_RAM[2] = 0x32;  // ld (nn), a  (takes next two memory locations as operand)
    Z80_RAM[3] = 0x18; 
    Z80_RAM[4] = 0b00000000;
    Z80_RAM[5] = 0x76;  //   halt
  }   
  else if (programMode == 5)
  {
//0000   3E 02                  LD   a,0x55       
//0004   D3 0F 01               OUT   (0x01),a     ;; output 55 on io port 1
//0009   76                     HALT       
    Z80_RAM[0] = 0x3e;
    Z80_RAM[1] = 0x01;
    Z80_RAM[2] = 0xD3;
    Z80_RAM[3] = 0x01;
    Z80_RAM[4] = 0x76;        
  }
  else if (programMode == 6)
  {  // this outputs to an port twice switching on and off if led connected 
     // then sets the led on the port zero when it's complete
      //0000   06 FF                  LD   b,0xff   ; load f into b for loop
      //0002                OUTERLOOP:   
      //0002   3E 01                  LD   a,0x01   ; load 1 into a
      //0004   D3 01                  OUT   (0x01),a   ; output 1 to port 1
      //0006   AF                     XOR   a   ; zero a
      //0007   D3 01                  OUT   (0x01),a   ; output zero to port 1
      //0009   10 F7                  DJNZ   outerLoop   

      //Z80_RAM[11]=0x3e;
      //Z80_RAM[12]=0xff;     
      //Z80_RAM[7]=0xD3;   // output ff to io port 0
      //Z80_RAM[8]=0x00;      
      //000B   76                     HALT   

      Z80_RAM[0]=0x06;
      Z80_RAM[1]=0x2F;
      //Z80_RAM[1]=0x03;  // changed to 3 temporarily
      Z80_RAM[2]=0x3e;
      Z80_RAM[3]=0xff;
      Z80_RAM[4]=0xd3;
      Z80_RAM[5]=0x01;
      Z80_RAM[6]=0xaf;
      Z80_RAM[7]=0xD3;
      Z80_RAM[8]=0x01;
      Z80_RAM[9]=0x10;
      Z80_RAM[10]=0xf7;
      Z80_RAM[11]=0x3e;
      Z80_RAM[12]=0xff;     
      Z80_RAM[13]=0xD3;   // output ff to io port 0
      Z80_RAM[14]=0x00;
      Z80_RAM[15]=0x76;      
  }  
}

void printMemory()
{
  Serial.println();
  Serial.println("Memory contents");
  for (int i = 0; i < SIZE_OF_RAM; i++)
  {
    if (i % 8 == 0) {      
      Serial.println();
      Serial.print("address = 0x");
      Serial.print(i, HEX);    
      Serial.print(":");
      Serial.print(Z80_RAM[i], HEX);
      Serial.print(":");
    }    
    else
    {      
      Serial.print(Z80_RAM[i], HEX);
      Serial.print(":");
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
        Serial.println("IO FAULT attempt to write to non existant port");
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
        Serial.flush();
        sleep_mode();
      }      
      printAddressAndDataBus();
      printIOPorts();
    }       
  }
}
