#include <robot.h>
#include <SoftwareSerial.h>

#define rxPin 8 // connect to txpin/D2
#define txPin 2 // connect to rxpin/D1

#define echoPin 13
#define trigPin 12
#define firewall 8

int lasts[firewall];

int motor_speed_LOW = 85;
int motor_speed_MID = 100;
int motor_speed_HIGH = 140;

unsigned long cool;

Robot robot;

SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);

int aver(){
  int s = 0;
  for(int i = 0; i < firewall; i++){
    s += lasts[i];
  }
  s = s/firewall;
  return s;
}

int getUltraDistance(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 6000);
  int distance = (duration*0.01715);
  if(distance == 0){
    distance = 10000;
  }
  if(abs(aver()-distance) < 20){
    for(int i = 0; i < firewall; i++){
      lasts[i] = lasts[i+1];
    }
    lasts[firewall-1] = distance;
    return distance;
  }
  for(int i = 0; i < firewall; i++){
    lasts[i] = lasts[i+1];
  }
  lasts[firewall-1] = distance;
  return getUltraDistance();
}

void left(int speed){
  speed = map(speed, 0, 10, motor_speed_LOW, motor_speed_HIGH);
  robot.left_motor(-speed);
  robot.right_motor(speed);
}

void right(int speed){
  speed = map(speed, 0, 10, motor_speed_LOW, motor_speed_HIGH);
  robot.left_motor(speed);
  robot.right_motor(-speed);
}

void forw(int speed){
  speed = map(speed, 0, 10, motor_speed_LOW, motor_speed_HIGH);
  robot.left_motor(speed);
  robot.right_motor(speed);
}

void back(int speed){
  speed = map(speed, 0, 10, motor_speed_LOW, motor_speed_HIGH);
  robot.left_motor(-speed);
  robot.right_motor(-speed);
}

void stahp(){
  robot.left_motor(0);
  robot.right_motor(0);
}

void forwun(int speed, int distan){
  speed = map(speed, 0, 10, motor_speed_LOW, motor_speed_HIGH);
  robot.left_motor(speed);
  robot.right_motor(speed);
  int dist = getUltraDistance();
  while(dist > distan){
    dist = getUltraDistance();
    if(millis() > cool){
      mySerial.println("INFO"+String(dist));
      cool = millis()+100;
    }
  }
  stahp();
}

void delay2(unsigned long mil){
  mil = mil+millis();
  while(mil > millis()){
    if(millis() > cool){
      mySerial.println("INFO"+String(getUltraDistance()));
      cool = millis()+100;
    }
  }
}

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(115200);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  mySerial.begin(4800);
  delay(500);
  mySerial.println("Idle");
}

void loop() {
  if(millis() > cool){
    mySerial.println("INFO"+String(getUltraDistance()));
    cool = millis()+100;
  }
  if(mySerial.available()){
    String data = mySerial.readStringUntil('\n');
    mySerial.println("Running");
    int i = 0;
    while(i < data.length()-1){
      String str1;
      String str2;
      int i2 = i+1;
      while(data.charAt(i2) != 'U'){
        str1 = str1+data.charAt(i2);
        i2++;
      }
      i2++;
      while(data.charAt(i2) != 'U'){
        str2 = str2+data.charAt(i2);
        i2++;
      }
      if(data.charAt(i) == 'f'){
        forw(atoi(str1.c_str()));
        delay2(atof(str2.c_str())*1000);
      }else if(data.charAt(i) == 'b'){
        back(atoi(str1.c_str()));
        delay2(atof(str2.c_str())*1000);
      }else if(data.charAt(i) == 'l'){
        left(atoi(str1.c_str()));
        delay2(atof(str2.c_str())*1000);
      }else if(data.charAt(i) == 'r'){
        right(atoi(str1.c_str()));
        delay2(atof(str2.c_str())*1000);
      }else if(data.charAt(i) == 'F'){
        forwun(atoi(str1.c_str()), atof(str2.c_str()));
      }
      stahp();
      i = i2+1;
    }
    mySerial.println("Idle");
  }
}
