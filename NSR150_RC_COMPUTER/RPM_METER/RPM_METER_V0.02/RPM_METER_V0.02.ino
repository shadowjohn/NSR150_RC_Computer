/************************************
  RPM Tachometer
  Author:3WA 羽山秋人(http://3wa.tw)
  Request: IC 74595 x 2
  Release Date: 2019-07-09
  Version：0.02
*************************************/
#include <Servo.h>
Servo myservo; // 建立Servo物件，控制伺服馬達
// D0 大平台
// D1 活塞
// D2 Data
// D3 Singal Input
// code 就是 燈號，低->高 ，共16顆 LED，0是暗，1是亮
static String code = "0000000000000000";
byte LED_A = 0;
byte LED_B = 0;
//取得的rpm不停的寫入 clean_rpms，0~9 陣列，排序後，取中間，減少判差
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
 int firePin = D3; //RPM 透過 7414 載入，13腳來自引擎訊號，12腳接到 D3
 int fireInt = 0;
unsigned int pulsesPR = 1;
unsigned long lastPT = 0;
unsigned long rpm = 0;
unsigned long rpm_int = 0;
int last_r = 0;
unsigned long lastTime = 0;

void setup() {
  digitalWrite(LED_BUILTIN,LOW);
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(D0, OUTPUT); //大平台
  pinMode(D1, OUTPUT); //活塞
  pinMode(D2, OUTPUT); //資料   
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
  //泡沫排序
  //排序連續取得的轉速值，取中間值，減少誤差
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
  //觀察中，不確定會不會因為CPU不夠快，導至高轉讀不到 
  if(rpm>=10000)
  {
    return clean_rpms[9];
  }
  else
  {
    return clean_rpms[5];
  }
}
void show_led()
{
  Serial.println("");
  //將 code 的值，轉成 byte 再送到 74HC595
  LED_A=0;
  LED_B=0;
  for (int i = 0; i < 8; i++) {
    bitWrite(LED_A, i, (code[i]=='1')?LOW:HIGH);    
    bitWrite(LED_B, i, (code[i+8]=='1')?LOW:HIGH);       
    Serial.print(code[i]);
  }
  Serial.println("");
  Serial.println(LED_A);
  Serial.println(LED_B);
  digitalWrite(D0, LOW);
 
  // 先送高位元組 (Hight Byte), 給離 Arduino 較遠的那顆 74HC595
  shiftOut(D2, D1, MSBFIRST, LED_B); 
  // 再送低位元組 (Low Byte), 給離 Arduino 較近的那顆 74HC595
  shiftOut(D2, D1, MSBFIRST, LED_A); 

  // 送完資料後要把 latchPin 拉回成高電位
  digitalWrite(D0, HIGH);  
}

void rpm_to_led()
{
  //將 rpm 轉成 led 顯示，每1000轉一個燈，如:
  //1500rpm = 1500/1000 = 1.5 無條件進位 = 2 ，顯示 2 個燈
  //低於1000rpm 1個燈
  int rpm = get_rpm();
  int i=0;
  for(i = 0 ; i < ceil(rpm / 1000.0 );i++)
  {    
    code[i]='1';
  }
  for(int j=i;j<16;j++)
  {
    code[j]='0';
  }  
  show_led(); 
}
void rpm_to_servo(){
  //這是 rpm 轉 伺服馬達的，暫時用不到
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

  //初始測試 LED 二次
  //暗亮暗亮 低->高，高->低
  if (is_first_time)
  {    
    for (int i = 0; i < 2; i++)
    {     
      code = "0000000000000000";
      show_led();
      delay(500);        
      code = "1111111111111111";            
      show_led();
      delay(500);
      code = "0000000000000000";
      show_led();      
      delay(500);
      code = "1111111111111111";            
      show_led();
      delay(500);
      code = "0000000000000000";
      show_led();      
      delay(500);
      for(int j=0;j<16;j++)
      {        
        for(int k=0;k<=j;k++)
        {
          code[k]='1';
        }
        for(int k=j+1;k<16;k++)
        {              
          code[k]='0';     
        }
        show_led();
        delay(delaytime);
      }

      //反
      for(int j=15;j>=0;j--)
      {        
        for(int k=0;k<j;k++)
        {
          code[k]='1';
        }
        for(int k=j+1;k<16;k++)
        {              
          code[k]='0';     
        }
        show_led();
        delay(delaytime);
      }
                
    }
    is_first_time = false; 
    code = "0000000000000000";
    show_led();
    delay(delaytime);
  }

  //測試結束了
  
  //開始工作
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
  //rpm_to_servo();
  
}

