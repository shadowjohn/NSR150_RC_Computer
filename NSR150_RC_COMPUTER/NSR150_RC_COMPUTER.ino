/*
 * NSR-150 RC電腦 V0.1
 * Release Date: 2018-09-04
 * By 羽山秋人 (http://3wa.tw)
 * GitHub: https://github.com/shadowjohn/NSR150_RC_Computer
 * 
 * 接腳參考
   #define enA D0
   #define in1 D1
   #define in2 D2
   #define rpm D3
*/

int analogPin= A0;
int raw= 0;
float Vin = 5.0;
float Vout = 0;
float R1 = 1000; //R1 是參考電阻
float R2 = 0; //記錄 RC 目前的電阻值，如 1000Ω~4300Ω
float buffer= 0;

//long randRPM;
long now;
long startMillis;
long currentMillis;



bool start_test_flag = true;
int min_data = 0;
int max_data = 0;


int step_10 = 0;

//用來測試爬梯子的轉速訊號
//int rpm_arr[7] = { 0,2000,4000,6000,8000,10000,12000 };
//int rs = 0;
//int rs_am = 1;

//用來偵測 D3 腳抓引擎轉速的部分
const int firePin = D3;
const int fireInt = 0;
const unsigned int pulsesPR = 1;
unsigned long lastPT = 0;
unsigned long rpm = 0;
int rpm_int;
int rpm_to_disp;

void setup()
{
  Serial.begin(9600);
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  randomSeed(analogRead(0)); 
  pinMode(firePin, INPUT);
  attachInterrupt(fireInt, &fireIsr, RISING); 
  
}
void fireIsr()
{
  unsigned long now = micros();
  unsigned long duringInt = now - lastPT;
  if (duringInt > 5000)
  {
     rpm = 60000000UL/(duringInt * pulsesPR); //轉速訊號會在這取得!!!
     lastPT = now;
     //rpm_int=int(rpm);
  }  
  
}
void rc_stop()
{
  //函式五，馬達停止
  Serial.println("Run rc_stop");
  digitalWrite(D0, HIGH);
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);      
}
void rc_add(){
  //函式三：馬達旋轉(RC開)
  Serial.println("Run rc_add");
  digitalWrite(D0, HIGH);  
  digitalWrite(D1, LOW);
  digitalWrite(D2, HIGH);
  delay(10);
}
void rc_minus(){
  //函式四：馬達旋轉(RC關)
  Serial.println("Run rc_minus");
  digitalWrite(D0, HIGH);
  digitalWrite(D1, HIGH);
  digitalWrite(D2, LOW);    
  delay(10);
}
void updateR2(){
  //取得目前RC姿態，將電阻值存到 R2
  raw= analogRead(analogPin);
  if(raw) 
  {
    buffer= raw * Vin;
    //Serial.print("Vin:");
    //Serial.println(Vin);
    //Serial.print("raw:");
    //Serial.println(raw);  
    
    Vout= (buffer)/1350.0;
    buffer= (Vin/Vout) -1;
    R2= R1 * buffer;
    //Serial.print("Vout: ");
    //Serial.println(Vout);
    //Serial.print("R2: ");
    //Serial.println(R2);
    //delay(200);
  }
}
int rpm_to_step(int rpm)
{
  //函式二：什麼樣的轉速要用第幾級
  if(rpm<5000)
  {
    return 1;
  }
  if(rpm>=5000 & rpm < 5500)
  {
    return 2;
  }
  if(rpm>=5500 & rpm < 6500)
  {
    return 3;
  }
  if(rpm >= 6500 && rpm < 7000)
  {
    return 4;
  }  
  if(rpm >= 7000)
  {
    return 5;
  }  
}
void rc_run_step(int step)
{
  //函式六：依轉速自動旋轉至對應級別
  int r2_now_step = (R2-min_data) / step_10; 
  Serial.println();
  Serial.print("R2 now step :");
  Serial.println(r2_now_step);
  Serial.println();
  
  if(r2_now_step==step)
  {
    rc_stop();
  }
  else if(r2_now_step < step)
  {
    if(r2_now_step < 5)
    {
      rc_add();
    }
    else
    {
      rc_stop();
    }
  }  
  else if(r2_now_step > step)
  {
    if( r2_now_step > 1)
    {
      rc_minus();
    }
    else
    {
      rc_stop();
    }
  }  
}
void loop()
{  
  //依D3取得引擎轉速
  noInterrupts();
  rpm_to_disp=int(rpm);
  interrupts();
  Serial.println();
  Serial.print("Now RPM :");
  Serial.println(rpm_to_disp);

  
  if(start_test_flag==true) ////測試模式
  {          
    //開機時進入測試模式， RC 開關二次，第二次記錄最低點電阻、最高點電阻當參考值
    rc_add(); 
    delay(1000);    
    rc_minus(); 
    delay(1000);
    rc_stop();
    updateR2();
    min_data=R2;             
    rc_add(); 
    delay(1000);
    rc_stop();
    updateR2();
    max_data=R2;
    rc_minus(); 
    delay(1000);
    start_test_flag=false;
    startMillis = millis();
    step_10 = (max_data - min_data) / 6;

    //用來測試隨機轉速
    //randRPM = random(0,11000);
  }    
  currentMillis = millis();
  /*Serial.println();
  Serial.print("R2:");
  Serial.println(R2);
  Serial.print("min_data:");
  Serial.println(min_data);
  Serial.print("max_data:");
  Serial.println(max_data);  
  */
  rc_stop();  
  updateR2();  
  //delay(100);
  /*if(currentMillis-startMillis>=1000)
  {
    startMillis=currentMillis;
    rs=rs+rs_am;
    randRPM = rpm_arr[rs];    
    if(rs> (sizeof(rpm_arr)/sizeof(int))-1) rs_am *=-1;
    if(rs<=0) rs_am*=-1;
    
  }  
  */
  int rpm_steps = rpm_to_step(rpm_to_disp);  
  Serial.println();
  //Serial.print("randRPM:");
  //Serial.print(randRPM);
  Serial.print(", step:");
  Serial.print(rpm_steps);
  rc_run_step(rpm_steps);

}

