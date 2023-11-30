

#define L1 4
#define L2 5
#define L3 6
#define L4 7
#define L5 8
#define L6 9
#define L7 10
#define L8 11


void setup() {
  
  pinMode(L1, INPUT);      
  pinMode(L2, INPUT);    
  pinMode(L3, INPUT);    
  pinMode(L4, INPUT);    
  pinMode(L5, INPUT);      
  pinMode(L6, INPUT);    
  pinMode(L7, INPUT);    
  pinMode(L8, INPUT);    

  Serial.begin(9600);
  Serial.println("START");
}

bool keypadLatch[8];

void loop() 
{
  int hadPinHigh[8] = {0,0,0,0,0,0,0,0};
  int countAll = 0;
  int looCount = 0;
  while(1)
  {
    keypadLatch[0] = digitalRead(L1);
    keypadLatch[1] = digitalRead(L2);
    keypadLatch[2] = digitalRead(L3);
    keypadLatch[3] = digitalRead(L4);
    keypadLatch[4] = digitalRead(L5);
    keypadLatch[5] = digitalRead(L6);
    keypadLatch[6] = digitalRead(L7);
    keypadLatch[7] = digitalRead(L8);
    
    for (int i = 0; i < 8; i++)
    {
      if (keypadLatch[i] == 1) 
      {
          hadPinHigh[i] = 1;
      }
    }
      

    looCount++;
    if (looCount >= 20000)
    {
      looCount = 0;

      for (int i = 0; i < 8; i++)
      {
        if (hadPinHigh[i] == 1) 
        {               
          Serial.print(i);
          Serial.println("  was set");
          hadPinHigh[i] = 0;
        }
      }
    }
  }
}
