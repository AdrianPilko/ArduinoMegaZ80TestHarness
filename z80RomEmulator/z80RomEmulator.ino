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

//uint16_t addressBus = 0;
uint8_t addressBus = 0;
uint8_t dataBus = 0;
int errorResultCount = 0;
int programCorrectCount = 0;
int programMode = 1;

#define NUMBER_OF_ADDRESS_PINS 6
#define NUMBER_OF_DATA_PINS 8
int addressPins[NUMBER_OF_ADDRESS_PINS] = {42,44,46,48,50,52};
//int addressPins[NUMBER_OF_ADDRESS_PINS] = {52,50,48,46,44,42};
int dataPins[NUMBER_OF_DATA_PINS] = {23,25,27,29,31,33,35,37};

int RD = 2; // active low 
int WR = 3; // active low 
int CLK = 4; 
int RESET = 5; // PIN 26 on Z80  // INPUT to Z80
int MREQ = 6; // pin 19 on Z80 // output from z80
int REFRESH = 7;

bool writeEn = false;
bool readEn = false;
bool ioRequest = false;
bool memRequest =false;
bool haltSet = false;
bool MachineOne = false;
bool refreshSet = false;
bool busAckSet = false;
bool resetSet = true;

const int HALF_CLOCK_RATE = 500;

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
}



void printStatus()
{
  if (readEn == true) Serial.print("RD ");
  if (writeEn == true) Serial.print("WR ");
  if (memRequest == true) Serial.print("MREQ ");
  if (refreshSet == true) Serial.print("REFRESH ");
}

void readAddressBus()
{    
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

void loop() 
{  
  setDataToOutput();
  while(1)
  {    
    delay(HALF_CLOCK_RATE);
    toggleClock();
    delay(HALF_CLOCK_RATE);
    toggleClock();
    readStatus();
    readAddressBus();               
    if ((readEn) && (memRequest) && (!refreshSet))
    {
      printStatus();
      outputToDataPins(0);
      printAddressAndDataBus();
    }
  }
}
