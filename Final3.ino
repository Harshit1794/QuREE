//00   : Update - Basic template (Line follower + obstacle avoidance  + bluetooth control) setup
//01   : Update - Now it will stop if any obstacle is in from of it, even in rc/ line following mode
//02   : Update - Button added - force stop
//03   : Update - sends dist to app - 1
//04   : Update - sends dist to app - 2
//05   : Update - addition of another BT module, removed sending data to app
//06   : Update - Sends dist to app - 3 - Successful!!!
//07   : Update - bug fixes
//08   : Update - fixed issue of malfunctioning of bot speed
//09   : Update - Fixed bluetooth control
//10   : Update - Stops the bot from crashing if app forward button is used - 1
//11   : Update - Stops the bot from crashing if app forward button is used - 2 - Successful!!!
//12   : Update - Bug Fixes - Prevenyting the bot from malfunctioning while using app
//13   : Update - removed some delays
//14   : Update - If line ends, mode is set to manual 
//15   : Update - If both IR Sensors detect nothing, the bot will stop automatically
//16   : Update - compiling temp and humidity sensor
//17   : Update - debugging
//18   : Update - debugging #2
//19   : Update - 
//20   : Update - 
//21   : Update - 
//22   : Update - 
//23   : Update - 
//25   : Update - 
//26   : Update - 


#include <SoftwareSerial.h>
SoftwareSerial BT_Serial(2, 3); // RX, TX (bt rx pin 3 tx pin 2)

#include <Wire.h> 
#include "DHT.h"

#define DHTPIN A5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define LDR A2
#define relay A1

#define enA 10//Enable1 L298 Pin enA 
#define in1 9 //Motor1  L298 Pin in1 
#define in2 8 //Motor1  L298 Pin in1 
#define in3 7 //Motor2  L298 Pin in1 
#define in4 6 //Motor2  L298 Pin in1  
#define enB 5 //Enable2 L298 Pin enB 

#define LED 4


#define servo A0
//#define servo A1
//#define servo A2
#define trigger A3 //Trigger pin
#define echo A4    //Echo pin
//#define dht A5
#define L_S 11 //ir sensor Left
#define R_S 12 //ir sensor Right


int distance_L, distance_F = 30, distance_R;
long distance;
int set = 35;
int check = 0;


int bt_ir_data; // variable to receive data from the serial port
int lightdat = 11;
int Speed = 130;  
int varturn = 130;
int mode=0;

int timer = 0;
int timer2 = 0;

int btsend = 0;
int btdt = 0;

void setup(){ // put your setup code here, to run once
pinMode(LED, OUTPUT);                                        //led blinking
pinMode(LDR, INPUT);
pinMode(relay, OUTPUT);

pinMode(R_S, INPUT_PULLUP); // declare ir sensor as input  
pinMode(L_S, INPUT_PULLUP); // declare ir sensor as input
pinMode(servo, OUTPUT);

pinMode(echo, INPUT );// declare ultrasonic sensor Echo pin as input
pinMode(trigger, OUTPUT); // declare ultrasonic sensor Trigger pin as Output  

pinMode(enA, OUTPUT); // declare as output for L298 Pin enA 
pinMode(in1, OUTPUT); // declare as output for L298 Pin in1 
pinMode(in2, OUTPUT); // declare as output for L298 Pin in2 
pinMode(in3, OUTPUT); // declare as output for L298 Pin in3   
pinMode(in4, OUTPUT); // declare as output for L298 Pin in4 
pinMode(enB, OUTPUT); // declare as output for L298 Pin enB 


Serial.begin(9600); // start serial communication at 9600bps
BT_Serial.begin(9600); 
Serial.println(69);     

dht.begin();

 for (int angle = 70; angle <= 140; angle += 5)  {
   servoPulse(servo, angle);  }
 for (int angle = 140; angle >= 0; angle -= 5)  {
   servoPulse(servo, angle);  }

 for (int angle = 0; angle <= 70; angle += 5)  {
   servoPulse(servo, angle);  }
delay(500);
analogWrite(enA, Speed); // Write The Duty Cycle 0 to 255 Enable Pin A for Motor1 Speed 
analogWrite(enB, Speed); // Write The Duty Cycle 0 to 255 Enable Pin B for Motor2 Speed 
Stop();
}

void loop(){  

timer += 1;
timer2 += 1;
//Serial.println(timer);
Serial.println(digitalRead(L_S));
Serial.println(digitalRead(R_S));
Serial.println();


if(BT_Serial.available() > 0){  //if some date is sent, reads it and saves in state
Serial.println("bt");
bt_ir_data = BT_Serial.read(); 
Serial.println(bt_ir_data);

if((260 > bt_ir_data)&&(bt_ir_data > 39)){
  Speed = bt_ir_data;
  
//===============================================================================
//                             Speed controlling
//=============================================================================== 
analogWrite(enA, Speed); // Write The Duty Cycle 0 to 255 Enable Pin A for Motor1 Speed 
analogWrite(enB, Speed); // Write The Duty Cycle 0 to 255 Enable Pin B for Motor2 Speed 
//=============================================================================== 
}      
}

/*
if(timer == 50){
  bt_data_sending();
  timer = 0;
}*/

if(timer == 100){
  /*btsend = 69;
if (btsend > 200){btsend = 200;};*/
/*
  btdt = btsend + 271;
  BT_Serial.print(65);
  BT_Serial.print(";");
  BT_Serial.print(54); //send distance to MIT App
 BT_Serial.print(";");
 BT_Serial.print(btdt); //send distance to MIT App 
 BT_Serial.println(";"); */
  //Serial.println(btdt);

  //Serial.println(mode);
  //Serial.println(check);
  //Serial.println(Speed);
  //Serial.println(bt_ir_data);
  //Serial.println();
  ///Serial.println(digitalRead(L_S));
  //Serial.println(digitalRead(R_S));
  //Serial.println(Ultrasonic_read());
  //Serial.println();
  


  {int  h = dht.readHumidity();
  int  t = dht.readTemperature();
  int btsend = Ultrasonic_read();
  if (btsend > 200){btsend = 200;};
  btdt = btsend + 270;
 BT_Serial.print(t); //send distance to MIT App
 BT_Serial.print(";");
 BT_Serial.print(h); //send distance to MIT App 
 BT_Serial.print(";");
 BT_Serial.print(btdt); //send distance to MIT App 
 BT_Serial.println(";");
}
timer = 0;
}


    
//===============================================================================
//                             Mode Selection
//=============================================================================== 
     if(bt_ir_data == 8){mode=0; Stop();}    //Manual Android Application and IR Remote Control Command   
else if(bt_ir_data == 9){mode=1; Speed=130;} //Auto Line Follower Command
else if(bt_ir_data ==10){mode=2; Speed=130;} //Auto Obstacle Avoiding Command
//===============================================================================


if(mode==0){     
//===============================================================================
//                          Key Control Command
//=============================================================================== 

distance_F = Ultrasonic_read();
if((distance_F<set) && (check==1)){check = 2; Stop();}
if(distance_F>set){check = 0;}

     if((bt_ir_data == 1) && (check==0)){check = 1; forward(); }  // if the bt_data is '1' the DC motor will go forward
else if(bt_ir_data == 2){backward();}  // if the bt_data is '2' the motor will Reverse
else if(bt_ir_data == 3){turnLeft();}  // if the bt_data is '3' the motor will turn left
else if(bt_ir_data == 4){turnRight();} // if the bt_data is '4' the motor will turn right
else if(bt_ir_data == 5){Stop(); }     // if the bt_data '5' the motor will Stop

//===============================================================================
//                          Voice Control Command
//===============================================================================    
else if(bt_ir_data == 6){analogWrite(enA, 255); analogWrite(enB, 255); turnLeft();  delay(600);  analogWrite(enA, Speed); analogWrite(enB, Speed); bt_ir_data = 5;}
else if(bt_ir_data == 7){analogWrite(enA, 255); analogWrite(enB, 255); turnRight(); delay(600);  analogWrite(enA, Speed); analogWrite(enB, Speed); bt_ir_data = 5;}

if((digitalRead(R_S) == 1)&&(digitalRead(L_S) == 1)){backward();delay(300); Stop();}
}

if(mode==1){    
//===============================================================================
//                          Line Follower Control
//===============================================================================  

distance_F = Ultrasonic_read();
if((distance_F<set) && (check==1)){check = 2; Stop();}
if(distance_F>set){check = 0;}


if((digitalRead(R_S) == 0)&&(digitalRead(L_S) == 0) && (check==0)){check = 1;forward();Serial.println(1100);}  //if Right Sensor and Left Sensor are at White color then it will call forward function
if((digitalRead(R_S) == 1)&&(digitalRead(L_S) == 0)){turnRight();Serial.println(1110);}//if Right Sensor is Black and Left Sensor is White then it will call turn Right function  
if((digitalRead(R_S) == 0)&&(digitalRead(L_S) == 1)){turnLeft();Serial.println(1101);} //if Right Sensor is White and Left Sensor is Black then it will call turn Left function
if((digitalRead(R_S) == 1)&&(digitalRead(L_S) == 1)){Stop();Serial.println(1111);}     //if Right Sensor and Left Sensor are at Black color then it will call Stop function

if(bt_ir_data == 16){mode=0; Stop();delay(2000);} 

} 

if(mode==2){    
//===============================================================================
//                          Obstacle Avoiding Control
//===============================================================================     

 distance_F = Ultrasonic_read();
 Serial.print("S=");Serial.println(distance_F);
  if (distance_F > set){forward();}
    else{Check_side();}

    if(bt_ir_data == 16){mode=0; Stop();delay(2000);} 
    if((digitalRead(R_S) == 1)&&(digitalRead(L_S) == 1)){backward();delay(300); Stop();}
}

//===============================================================================
//                             Blinking LED
//=============================================================================== 
if (timer2 == 100){digitalWrite(LED, HIGH);}
else if (timer2 == 150){digitalWrite(LED, LOW);timer2 = 0;}
//===============================================================================

//===============================================================================
//                             Lights
//=============================================================================== 
 {if(bt_ir_data >= 11 && bt_ir_data <= 13){lightdat = bt_ir_data;}
     if(lightdat == 11){digitalWrite(relay, LOW);}                   //On
else if(lightdat == 12){digitalWrite(relay, HIGH);}                    //Off
else if(lightdat == 13){                                          //Auto
  int outputValue = digitalRead(LDR);

  if (outputValue == 1){digitalWrite(relay, LOW);}
  else {digitalWrite(relay, HIGH);}
  
 }
 }

delay(1);
}                                                                   //Void loop ends

void bt_data_sending(){
//===============================================================================
//                              BT data sending
//=============================================================================== 

  int  h = dht.readHumidity();
  int  t = dht.readTemperature();
  int btsend = Ultrasonic_read();
  if (btsend > 200){btsend = 200;};
 BT_Serial.print(t); //send distance to MIT App
 BT_Serial.print(";");
 BT_Serial.print(h); //send distance to MIT App 
 BT_Serial.print(";");
 BT_Serial.print(btsend); //send distance to MIT App 
 BT_Serial.println(";");


}

void servoPulse (int pin, int angle){
int pwm = (angle*11) + 500;      // Convert angle to microseconds
 digitalWrite(pin, HIGH);
 delayMicroseconds(pwm);
 digitalWrite(pin, LOW);
 delay(50);                   // Refresh cycle of servo
}


//**********************Ultrasonic_read****************************
long Ultrasonic_read(){
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  distance = pulseIn (echo, HIGH);
  return distance / 29 / 2;
}

void compareDistance(){
       if (distance_L > distance_R){
  turnLeft();
  delay(350*255/Speed);
  }
  else if (distance_R > distance_L){
  turnRight();
  delay(350*255/Speed);
  }
  else{
  backward();
  delay(300);
  turnRight();
  delay(700*255/Speed);
  }
}

void Check_side(){
    Stop();
    delay(100);
 for (int angle = 70; angle <= 140; angle += 5)  {
   servoPulse(servo, angle);  }
    delay(300);
    distance_L = Ultrasonic_read();
    delay(100);
  for (int angle = 140; angle >= 0; angle -= 5)  {
   servoPulse(servo, angle);  }
    delay(500);
    distance_R = Ultrasonic_read();
    delay(100);
 for (int angle = 0; angle <= 70; angle += 5)  {
   servoPulse(servo, angle);  }
    delay(300);
    compareDistance();

}






//===============================================================================
//                              Movement controls
//===============================================================================

void forward(){//forward
//Serial.println("fwd");
varturn = Speed;
digitalWrite(in1, HIGH); //Right Motor forward Pin 
digitalWrite(in2, LOW);  //Right Motor backward Pin 
digitalWrite(in3, LOW);  //Left Motor backward Pin 
digitalWrite(in4, HIGH); //Left Motor forward Pin 

}

void backward(){ //backward
//Serial.println("back");
varturn = Speed;
digitalWrite(in1, LOW);  //Right Motor forward Pin 
digitalWrite(in2, HIGH); //Right Motor backward Pin 
digitalWrite(in3, HIGH); //Left Motor backward Pin 
digitalWrite(in4, LOW);  //Left Motor forward Pin 
}

void turnRight(){ //turnRight
//Serial.println("right");
varturn = Speed;
Speed = Speed/1.5+85;
digitalWrite(in1, LOW);  //Right Motor forward Pin 
digitalWrite(in2, HIGH); //Right Motor backward Pin  
digitalWrite(in3, LOW);  //Left Motor backward Pin 
digitalWrite(in4, HIGH); //Left Motor forward Pin 
}

void turnLeft(){ //turnLeft
//Serial.println("left");
varturn = Speed;
Speed = Speed/1.5+85;
digitalWrite(in1, HIGH); //Right Motor forward Pin 
digitalWrite(in2, LOW);  //Right Motor backward Pin 
digitalWrite(in3, HIGH); //Left Motor backward Pin 
digitalWrite(in4, LOW);  //Left Motor forward Pin 
}

void Stop(){ //stop
//Serial.println("stop");
Speed = varturn;
digitalWrite(in1, LOW); //Right Motor forward Pin 
digitalWrite(in2, LOW); //Right Motor backward Pin 
digitalWrite(in3, LOW); //Left Motor backward Pin 
digitalWrite(in4, LOW); //Left Motor forward Pin 
}
