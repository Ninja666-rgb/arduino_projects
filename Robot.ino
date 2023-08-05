#include <robot.h>
#include "Mag.h"

#define echoPin2 9
#define trigPin2 10

#define echoPin 13
#define trigPin 12
#define lightSensor A0

int motor_speed_EXLOW = 80; 
int motor_speed_LOW = 85;
int motor_speed_MID = 100;
int motor_speed_HIGH = 120;

Robot robot;
volatile bool moved_back = false;
volatile int threshold;
volatile bool is_covered = false, was_covered = false, turn_on = true;

double zn, zbef, za;

long ltim = 0, chang = 0;
int ldistance2;

int getUltraDistance(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 6000);
  int distance = (duration * 0.01715);
  if(distance == 0){
    distance = 1000000;
  }
  return distance;
}

int getUltraDistance2(){
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  long duration2 = pulseIn(echoPin2, HIGH, 6000);
  int distance2 = (duration2 * 0.01715);
  if(distance2 == 0){
    distance2 = 1000000;
  }
  return distance2;
}

void setup() {
  // {Init
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  Serial.begin(115200);
  Wire.begin();
  i2c_init();
  int n = 20;
  int total = 0;
  // Init}
  // {Callibrating light sensor
  for(int i=0; i<n; i++){
    total += analogRead(lightSensor);
    delay(50);
  }
  threshold = (total/n) + 75;
  // Callibrating light sensor}
  while (!Serial) continue;
  // {Getting left ultrasonic reading
  ldistance2 = getUltraDistance2();
  // Getting left ultrasonic reading}
}

void loop() {
  // {Gyro reading
  for(int i = 0; i < 10; i++){
    GYRO = read_gyro();
    za += (double)GYRO[2];
  }
  // Gyro reading}
  za = za/10;
  zn = fmod(zn+za, 4300.0);
  int distance = getUltraDistance();
  int distance2 = getUltraDistance2();
  int light_val = analogRead(A0);
  if(light_val > threshold){
    is_covered = true;
  }else{
    is_covered = false;
  }
  // {Turning on or off
  // if(is_covered == false && was_covered){
  //   Serial.println("released");
  //   turn_on = !turn_on;
  // }
  // Turning on or off}
  // {Checking if the left wall opened
  if(ldistance2 < 20 && distance2 > 20){
    chang = millis()+300;
  }
  // Checking if the left wall opened}
  // {Checking if it is time to turn if the left wall opened
  if(chang-(long)millis() < 0 && chang != 0 && ltim-(long)millis() < 0){
    // {Jolt backwards
    robot.left_motor(-motor_speed_HIGH);
    robot.right_motor(-motor_speed_HIGH);
    delay(50);
    // Jolt backwards}
    // {Turn left approximately 90 degrees
    robot.left_motor(0);
    robot.right_motor(motor_speed_MID);
    zbef = fmod(zbef+1075.0, 4300.0);
    while(abs(zbef-zn) > 80){
      for(int i = 0; i < 10; i++){
        GYRO = read_gyro();
        za += (double)GYRO[2];
      }
      za = za/10;
      zn = fmod(zn+za, 4300.0);
    }
    // Turn left approximately 90 degrees}
    // {Set to go forward
    robot.left_motor(motor_speed_LOW);
    robot.right_motor(motor_speed_LOW);
    // Set to go forward}
    // {Setting the timeout for turning again to avoid repetitive turning
    ltim = millis()+1500;
    chang = 0;
    // Setting the timeout for turning again to avoid repetitive turning}
  }
  // Checking if it is time to turn if the left wall opened}
  if(turn_on == false){
    // {Stopped
    while(light_val <= threshold){
      robot.left_motor(0);
      robot.right_motor(0);
      light_val = analogRead(A0);
    }
    // Stopped}
  }else if(distance <= 10){
    // {Jolt backwards
    robot.left_motor(-motor_speed_HIGH);
    robot.right_motor(-motor_speed_HIGH);
    delay(200);
    // Jolt backwards}
    // {Turning left or right based on whether there is a wall on the left
    if(distance2 > 20){
      robot.left_motor(0);
      robot.right_motor(motor_speed_MID);
      zbef = fmod(zbef+1075.0, 4300.0);
    }else{
      robot.left_motor(motor_speed_MID);
      robot.right_motor(0);
      zbef = fmod(zbef-1075.0, 4300.0);
    }
    
    // Turning left or right based on whether there is a wall on the left}
    // {Waiting till it has turned aproximately 90 degrees
    while(abs(zbef-zn) > 80){
      for(int i = 0; i < 10; i++){
        GYRO = read_gyro();
        za += (double)GYRO[2];
      }
      za = za/10;
      zn = fmod(zn+za, 4300.0);
    }
    // Waiting till it has turned aproximately 90 degrees}
    // {Set to go forward
    robot.left_motor(motor_speed_LOW);
    robot.right_motor(motor_speed_LOW);
    // Set to go forward}
    // {Setting the timeout for turning again to avoid repetitive turning
    if(distance2 > 15){
      ltim = millis()+1500;
    }
    // Setting the timeout for turning again to avoid repetitive turning}
  }else if(distance < 30 ) {
    // {Set to move slowly
    robot.left_motor(motor_speed_LOW);
    robot.right_motor(motor_speed_LOW);
    // Set to move slowly}
    // {Adjusting to go straight
    if(abs(zbef-zn) > 70){
      if(zbef-zn < 0){
        robot.left_motor(motor_speed_EXLOW);
        robot.right_motor(-motor_speed_EXLOW);
        while(abs(zbef-zn) > 70){
          for(int i = 0; i < 10; i++){
            GYRO = read_gyro();
            za += (double)GYRO[2];
          }
          za = za/10;
          zn += za;
        }
        robot.left_motor(motor_speed_LOW);
        robot.right_motor(motor_speed_LOW);
      }else{
        robot.left_motor(-motor_speed_EXLOW);
        robot.right_motor(motor_speed_EXLOW);
        while(abs(zbef-zn) > 70){
          for(int i = 0; i < 10; i++){
            GYRO = read_gyro();
            za += (double)GYRO[2];
          }
          za = za/10;
          zn += za;
        }
        robot.left_motor(motor_speed_LOW);
        robot.right_motor(motor_speed_LOW);
      }
    }
    // Adjusting to go straight}
  }else {
    // {Set to move medium speed
    robot.left_motor(motor_speed_MID);
    robot.right_motor(motor_speed_MID);
    // Set to move medium speed}
    // {Adjusting to go straight
    if(abs(zbef-zn) > 70){
      if(zbef-zn < 0){
        robot.left_motor(motor_speed_EXLOW);
        robot.right_motor(-motor_speed_EXLOW);
        while(abs(zbef-zn) > 70){
          for(int i = 0; i < 10; i++){
            GYRO = read_gyro();
            za += (double)GYRO[2];
          }
          za = za/10;
          zn += za;
        }
        robot.left_motor(motor_speed_MID);
        robot.right_motor(motor_speed_MID);
      }else{
        robot.left_motor(-motor_speed_EXLOW);
        robot.right_motor(motor_speed_EXLOW);
        while(abs(zbef-zn) > 70){
          for(int i = 0; i < 10; i++){
            GYRO = read_gyro();
            za += (double)GYRO[2];
          }
          za = za/10;
          zn += za;
        }
        robot.left_motor(motor_speed_MID);
        robot.right_motor(motor_speed_MID);
      }
    }
    // Adjusting to go straight}
  }
  // {Updating values
  was_covered = is_covered;
  ldistance2 = distance2;
  // Updating values}
}