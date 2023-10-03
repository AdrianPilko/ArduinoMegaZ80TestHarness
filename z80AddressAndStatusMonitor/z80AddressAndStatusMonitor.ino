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

*/

uint16_t addressBus = 0;
uint8_t dataBus;
int errorResultCount = 0;
int programCorrectCount = 0;
int programMode = 1;

#define NUMBER_OF_ADDRESS_PINS 16
#define NUMBER_OF_DATA_PINS 8
int addressPins[NUMBER_OF_ADDRESS_PINS] = {52,50,48,46,44,42,40,38,36,34,32,30,28,26,24,22};
int dataPins[NUMBER_OF_DATA_PINS] = {21,20,19,18,17,16,15,14};

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
  Serial.println("CPU is in reset, RESET held active");
  digitalWrite(RESET, LOW);      
  for (int i = 0; i < 12; i++)
  {
    waitExternalClock();
  }
  digitalWrite(RESET, HIGH); 
  Serial.println("CPU now reset");
}

void initialiseTest()
{  
  pinMode(CLK, INPUT);   // to grab 555 timer clock output

  
  pinMode(IORQ, INPUT);
  pinMode(MREQ, INPUT);
  pinMode(HALT, INPUT);
  pinMode(M1,   INPUT);
  pinMode(REFRESH,INPUT);
  pinMode(BUSAK,INPUT);
  
  pinMode(readEnable, INPUT);   
  pinMode(writeEnable, INPUT);

  setAddressPinsToInput();
  setDataToInput();
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
  if (haltSet == true) Serial.print("HALT ");
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

void loop() 
{  
  while(1)
  {    
    waitExternalClock();  
    readStatus();
    printStatus();          
    readAddressBus();    
    dataBus = readFromDataPins();
    printAddressAndDataBus();
  }
}
