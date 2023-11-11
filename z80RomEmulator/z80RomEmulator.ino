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
const int programMode = 1;
const int SIZE_OF_RAM = 32;
//uint16_t addressBus = 0;
uint8_t addressBus = 0;
uint8_t dataBus = 0;

#define NUMBER_OF_ADDRESS_PINS 9
#define NUMBER_OF_DATA_PINS 8
int addressPins[NUMBER_OF_ADDRESS_PINS] = {36,38,40,42,44,46,48,50,52};
int dataPins[NUMBER_OF_DATA_PINS] = {23,25,27,29,31,33,35,37};

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

const int HALF_CLOCK_RATE = 200;

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
  if (readEn == true) Serial.print("RD ");
  if (writeEn == true) Serial.print("WR ");
  if (memRequest == true) Serial.print("MREQ ");
  if (refreshSet == true) Serial.print("REFRESH ");
  if (ioRequest == true) Serial.print("IORQ ");
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
    delay(HALF_CLOCK_RATE);
    toggleClock();
    delay(HALF_CLOCK_RATE);
    Serial.println("Reset");
  }   
  digitalWrite(RESET, HIGH);
}

void printAddressAndDataBus()
{
  Serial.println();
  Serial.print("Address bus = 0x");
  Serial.print(addressBus,HEX); 
  if(writeEn)  
    Serial.print("\t\tW "); 
  else
    Serial.print("\t\tR "); 
  Serial.print("DataBus = 0x");
  Serial.print(dataBus,HEX); 
  Serial.println();
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
    Z80_RAM[0] = 0x00;
    Z80_RAM[1] = 0x00;
    Z80_RAM[2] = 0x76;
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
    }    
    else
    {      
      Serial.print(Z80_RAM[i], HEX);
      Serial.print(":");
    }
  }
  Serial.println();
}

void loop() 
{  
  //while(1)
  //{    
    delay(HALF_CLOCK_RATE);
    toggleClock();
    delay(HALF_CLOCK_RATE);
    toggleClock();
    readStatus();
    // this is halt instruction so stop and print memory
    if (haltSet == true)
    {
       printMemory();
       delay(1000);
    }
    readAddressBus();               
    // the z80 will assert the RD and MREQ when reading from RAM, bvut we also have to check that REFRSH is not active
    // the logic here is active high (reverse when read from pins), the z80 in reality uses active low for cpu control pins
    if ((readEn) && (memRequest) && (!refreshSet))
    {
      printStatus();     

      if (addressBus < SIZE_OF_RAM)
      {
        dataBus = Z80_RAM[addressBus];
      }
      else
      {
        Serial.println("SEG FAULT attempt to read off end of memory");
        delay(100);
      }
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
        delay(100);
      }      
      printAddressAndDataBus();
    }    
//  }
}
