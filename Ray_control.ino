/*本项目致力于实现一款可由蓝牙控制的、低功耗的智能灯小车，可以搭配由阿Ray开发的app一起使用
 * 程序分为几个板块：灯光板块、智能行进板块，以及穿插其中的无限透传蓝牙通讯块
 * 
 * 
 */

#include<EEPROM.h>                       //加载EEPROM库，以保留必要的用户设置
#include<SoftwareSerial.h>               //加载SoftwareSerial库，通过其他引脚实现蓝牙通讯,保留0、1脚调试
const int PROGMEM Light_pin[3]={3,5,6};  //控制灯的引脚
const int PROGMEM Serial_pin[2]={2,8};   //蓝牙通讯引脚。{RX，TX}
const int PROGMEM Car_pin[2]={9,10};     //电机控制引脚
SoftwareSerial BTSerial(Serial_pin[0],Serial_pin[1]);
byte LED_R=0;byte LED_G=0;byte LED_B=0;bool turnState=0;int kaiguan;char act[2];
byte breathDelay=15;//呼吸延时（周期）
byte r;byte g;byte b;

void setup() {
 
  Serial.begin(9600);BTSerial.begin(9600);
  //灯光引脚
  for(int i=0;i<3;i++){
    pinMode(Light_pin[i],OUTPUT);
    analogWrite(Light_pin[i],255);
  }
  //电机
  for(int j=0;j<2;j++){
    pinMode(Car_pin[j],OUTPUT);
  }
}



void loop() {
  //接收蓝牙指令
  if(BTSerial.available()>0){
    char data=BTSerial.read();
    switch(data){
      //当指令为为灯信号
      case 'R':
            r=BTSerial.parseInt(); 
            EEPROM.write(0,r);delay(10);
            BTSerial.print(F("已设置R="));BTSerial.print(r);BTSerial.print(F("\n"));
            break;
      case 'G':
            g=BTSerial.parseInt(); 
            EEPROM.write(1,g);delay(10);
            BTSerial.print(F("已设置G="));BTSerial.print(g);BTSerial.print(F("\n"));
            break;
      case 'B':
            b=BTSerial.parseInt(); 
            EEPROM.write(2,b);delay(10);
            BTSerial.print(F("已设置B="));BTSerial.print(b);BTSerial.print(F("\n"));
            break;
      case 'K':
            kaiguan=BTSerial.parseInt();
            break; 
      //小车信号
      case 'l':
            act[2]="TL";
            break;
      case 'r':
            act[2]="TR";
            break;
      case 'f':
            act[2]="TF";
            break;
      case 'k':
            int zhuansu=BTSerial.parseInt();
            break;
        }
      
    }

 //定义灯的行为
    byte brightness[3]={EEPROM.read(0),EEPROM.read(1),EEPROM.read(2)};//读出上次设置的RGB值
    delay(10);
  //常亮
    if(kaiguan==1001){
      if(!turnState){
      turnON_LED(brightness[0],brightness[1],brightness[2]);
      turnState=1;
      //BTSerial.println(F("常亮"));
    }else{keep_LED(brightness[0],brightness[1],brightness[2]);}}
      
  //关闭
    else if(kaiguan==1000 && turnState){
      turnOFF_LED(0,0,0);
      turnState=0;
      //BTSerial.println(F("关闭"));
      }
  //呼吸灯
    else if(kaiguan==1010){
      turnON_LED(brightness[0],brightness[1],brightness[2]);
      turnOFF_LED(0,0,0);
      turnState=1;
      //BTSerial.println(F("呼吸"));
      }
  //随机灯光
    else if(kaiguan==1101){
      suiji_LED();//BTSerial.println(F("随机"));
      turnState=1;
    }

    
  
delay(10);
}


//点亮LED的函数，参数为点亮后的亮度
void turnON_LED(int r0,int g0,int b0){
  while(r0>LED_R || g0>LED_G || b0>LED_B){
    if(LED_R+1<r0){
      LED_R+=1;analogWrite(Light_pin[0],255-LED_R);}
    else{LED_R=r0;analogWrite(Light_pin[0],255-LED_R);}
    if(LED_G+1<g0){
      LED_G+=1;analogWrite(Light_pin[1],255-LED_G);}
    else{LED_G=g0;analogWrite(Light_pin[1],255-LED_G);}
    if(LED_B+1<b0){
      LED_B+=1;analogWrite(Light_pin[2],255-LED_B);}
    else{LED_B=b0;analogWrite(Light_pin[2],255-LED_B);}
    delay(breathDelay);
}}
//点亮后保持
void keep_LED(int r2,int g2,int b2){
  analogWrite(Light_pin[0],255-r2);
  analogWrite(Light_pin[1],255-g2);
  analogWrite(Light_pin[2],255-b2);
  delay(1000);}

//熄灭LED的函数，参数为熄灭后的亮度
void turnOFF_LED(int r1,int g1,int b1){
  while(r1<LED_R || g1<LED_G || b1<LED_B){
    if(LED_R>r1+1){
      LED_R-=1;analogWrite(Light_pin[0],255-LED_R);}
    else{LED_R=r1;analogWrite(Light_pin[0],255-LED_R);}
    if(LED_G>g1+1){
      LED_G-=1;analogWrite(Light_pin[1],255-LED_G);}
    else{LED_G=g1;analogWrite(Light_pin[1],255-LED_G);}
    if(LED_B>b1+1){
      LED_B-=1;analogWrite(Light_pin[2],255-LED_B);}
    else{LED_B=b1;analogWrite(Light_pin[2],255-LED_B);}
    delay(breathDelay);
}}
//随机颜色
void suiji_LED(){
  int brightness0[3]={random(0,255),random(0,255),random(0,255)};
  analogWrite(Light_pin[0],brightness0[0]);
  analogWrite(Light_pin[1],brightness0[1]);
  analogWrite(Light_pin[2],brightness0[2]);
  delay(1500);}
