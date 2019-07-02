int ar[] = { D1, D2, D0, D4, D5, D6, D7, D8 };
char* led;
bool is_first_time = 1;
int delaytime = 200;

//用來偵測 D3 腳抓引擎轉速的部分
const int firePin = D3; //RPM 透過 7414 載入，13腳來自引擎訊號，12腳接到 D0
const int fireInt = 0;
const unsigned int pulsesPR = 1;
unsigned long lastPT = 0;
unsigned long rpm = 0;
int rpm_int = 0;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  //pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(firePin, INPUT);
  attachInterrupt(fireInt, &fireIsr, RISING);
}
void fireIsr()
{
  unsigned long now = micros();
  unsigned long duringInt = now - lastPT;
  if (duringInt > 5000) 
  {
    rpm = 60000000UL / (duringInt * pulsesPR); //轉速訊號會在這取得!!!
    lastPT = now;
    rpm_int = int(rpm);
  }
}
void show_led(char* a)
{
  for (int i = 0; i < 8; i++)
  {
    switch (a[i])
    {
      case '0':
        digitalWrite(ar[i], LOW);
        break;
      case '1':
        digitalWrite(ar[i], HIGH);
        break;
    }
  }
}
void rpm_to_led()
{
  if(rpm<1000)
  {
    show_led("1111111");
  }
  else if(rpm>=1000&&rpm<2000)
  {
    show_led("0111111");
  }
  else if(rpm>=2000&&rpm<3000)
  {
    show_led("0011111");
  }
  else if(rpm>=3000&&rpm<4000)
  {
    show_led("00011111");
  }
  else if(rpm>=4000&&rpm<5000)
  {
    show_led("00001111");
  }
  else if(rpm>=5000&&rpm<6000)
  {
    show_led("00000111");
  }
  else if(rpm>=7000&&rpm<8000)
  {
    show_led("00000011");
  }
  else if(rpm>=9000&&rpm<10000)
  {
    show_led("00000001");
  }
  else if(rpm>=10000)
  {
    show_led("00000000");
  }
  
}
void loop() {
  // put your main code here, to run repeatedly:
  if (is_first_time)
  {
    delay(2000);
    show_led("11111111");
    for (int i = 0; i < 2; i++)
    {
      show_led("00000000");
      delay(delaytime);
      show_led("11111111");
      delay(delaytime);
      show_led("01111111");
      delay(delaytime);
      show_led("00111111");
      delay(delaytime);
      show_led("00011111");
      delay(delaytime);
      show_led("00001111");
      delay(delaytime);
      show_led("00000111");
      delay(delaytime);
      show_led("00000011");
      delay(delaytime);
      show_led("00000001");
      delay(delaytime);
      show_led("00000000");
      delay(delaytime);
      show_led("00000000");
      delay(delaytime);
      show_led("00000001");
      delay(delaytime);
      show_led("00000011");
      delay(delaytime);
      show_led("00000111");
      delay(delaytime);
      show_led("00001111");
      delay(delaytime);
      show_led("00011111");
      delay(delaytime);
      show_led("00111111");
      delay(delaytime);
      show_led("01111111");
      delay(delaytime);
      show_led("11111111");
      delay(delaytime);
    }
    is_first_time = false;
    show_led("11111111");
    delay(delaytime);
  }
  //依D0取得引擎轉速
  noInterrupts();
  interrupts();  
  
  //Serial.print(duringInt);
  //Serial.print(" ");  
  Serial.print("Now RPM :");
  if(rpm_int<10000)
  {
    Serial.print(" ");
    Serial.println(rpm_int);
  }
  else
  {
    Serial.println(rpm_int);
  }
  
  //超過一段時間沒變動，rpm刷 0  
  if (micros()-lastPT > 300000) 
  { 
    //Serial.println("超過N秒沒變動,rpm 刷 0");
    rpm=0;
    rpm_int=0;    
  }
  rpm_to_led();
}
