/************************************
  RPM Tachometer
  Author:3WA 羽山秋人(http://3wa.tw)
  Release Date: 2019-07-06
  Version：0.01
*************************************/
#include <Servo.h>
Servo myservo; // 建立Servo物件，控制伺服馬達
int ar[] = { D1, D2, D0, D4, D5, D6, D7 };
int clean_rpms[] = { 0,0,0,0,0,0,0,0,0,0 };
int rpm_step=0;
char* led;
bool is_first_time = 1;
int delaytime = 100;
unsigned long time_now = 0;
int delay1000 = 1000;
int delay100 = 100;
int delay300 = 300;
//用來偵測 D3 腳抓引擎轉速的部分
 int firePin = D3; //RPM 透過 7414 載入，13腳來自引擎訊號，12腳接到 D0
 int fireInt = 0;
unsigned int pulsesPR = 1;
unsigned long lastPT = 0;
unsigned long rpm = 0;
unsigned long rpm_int = 0;
int last_r = 0;
unsigned long lastTime = 0;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  digitalWrite(D3,HIGH);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  myservo.attach(D8); // 連接數位腳位D8，伺服馬達的訊號線
  pinMode(firePin, INPUT);
  attachInterrupt(fireInt, &fireIsr, RISING);
}
void fireIsr()
{
  unsigned long now = micros();
  unsigned long duringInt = now - lastPT;
  if (duringInt > 1000) 
  {
    rpm = 60000000UL / (duringInt * pulsesPR); //轉速訊號會在這取得!!!
    lastPT = now;
    rpm_int = int(rpm);
    //if(rpm_int>10000)
    //{
      //rpm_int=10000;
    //}
    //rpm_int-=rpm_int%100;
    rpm_int-=rpm_int%10;
    clean_rpms[rpm_step]=rpm_int;    
    rpm_step++;
    rpm_step%=10;
  }
}
int get_rpm()
{
  //排序
  for(int i=0;i<9;i++)
  {
    for(int j=i+1;j<10;j++)  
    {
      if(clean_rpms[i]>clean_rpms[j])
      {
        int tmp = clean_rpms[i];
        clean_rpms[i]=clean_rpms[j];
        clean_rpms[j]=tmp;
      }
    }
  }
  //int sum=0;
  //for(int i=4;i<=8;i++)
  //{
  //  sum+=clean_rpms[i];
  //}
  if(rpm>=10000)
  {
    return clean_rpms[9];
  }
  else
  {
    return clean_rpms[5];
  }
}
void show_led(char* a)
{
  for (int i = 0; i < 7; i++)
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
 
  int rpm = get_rpm();
  
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
  else if(rpm>=6000&&rpm<7000)
  {
    show_led("00000011");
    
  }
  else if(rpm>=7000&&rpm<8000)
  {
    show_led("00000001");
    
  }
  else if(rpm>=7000&&rpm<8000)
  {
    show_led("00000000");
    
  }
  else if(rpm>=8000&&rpm<9000)
  {
    show_led("00000000");
    
  }
  else if(rpm>=10000)
  {
    show_led("00000000");    
  } 
}
void rpm_to_servo(){
  int r = 175-int(get_rpm()/100*1.2);
  Serial.print("r :");
  Serial.println(r);
  if(r!=last_r)
  {
    //如果跟之前一樣，就不用一直叫馬達運作    
    myservo.write(r); 
    last_r=r;
  }
}
void loop() {
  
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

  
  //Serial.print(duringInt);
  //Serial.print(" ");  
  Serial.print("RPM : ");
  Serial.println(rpm);
  Serial.print("Now RPM :");  
  if(get_rpm()<10000)
  {
    Serial.print(" ");
    Serial.println(get_rpm());
  }
  else
  {
    Serial.println(get_rpm());
  }
  
  //超過一段時間沒變動，rpm刷 0
  unsigned long m = millis();  
  if(time_now ==0 )
  {
    time_now = m;
  }
  if (micros()-lastPT > 300000) 
  { 
    //Serial.println("超過N秒沒變動,rpm 刷 0");
    rpm=0;
    rpm_int=0; 
    for(int i=0;i<10;i++)
    {
      clean_rpms[i]=0;   
    }
  }
  
  if(m > time_now+delay100)
  {
    // put your main code here, to run repeatedly:
    //依D3取得引擎轉速
    //noInterrupts();
   // interrupts();  
    time_now=m;
  }
  rpm_to_led();  
  rpm_to_servo();
}
