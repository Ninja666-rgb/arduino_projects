#define trigpin  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define echopin     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define maxdistance 300 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define green 7
#define red 9
#define yellow 10
#define firewall 6

int thresh;

int ultra(){
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigpin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  int dist = pulseIn(echopin, HIGH, 30*maxdistance);
  return (int)((double)(dist)*0.01715);
}

void setup() {
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
  Serial.begin(115200); // Open serial monitor at 9600 baud to see ping results.
  delay(500);
}

int aver(int n, int num[100]){
  int s;
  for(int i = 0; i < n; i++){
    s += num[i];
  }
  s = s/n;
  return s;
}

int lasts[firewall];

void loop() {
  int a = ultra();
  if(abs(aver(firewall, lasts)-a) < 20){
    if(a < 5){
      digitalWrite(red, HIGH);
      digitalWrite(yellow, LOW);
      digitalWrite(green, LOW);
    }else if(a < 10){
      digitalWrite(red, LOW);
      digitalWrite(yellow, HIGH);
      digitalWrite(green, LOW);
    }else{
      digitalWrite(red, LOW);
      digitalWrite(yellow, LOW);
      digitalWrite(green, HIGH);
    }
  }
  for(int i = 0; i < firewall; i++){
    lasts[i] = lasts[i+1];
  }
  lasts[firewall-1] = a;
  switch(traffic){
    case 1:
      if(lighttime > millis()){
        digitalWrite(red, LOW);
        digitalWrite(yellow, LOW);
        digitalWrite(green, HIGH);
      }else{
        traffic = 2;
        lighttime = millis()+3000;
      }
      break;
    case 2:
      if(lighttime > millis()){
        digitalWrite(red, LOW);
        digitalWrite(yellow, HIGH);
        digitalWrite(green, LOW);
      }else{
        traffic = 3;
        lighttime = millis()+10000;
      }
      break;
    case 3:
      digitalWrite(green, LOW);
      digitalWrite(yellow, LOW);
      digitalWrite(red, HIGH);
      break;
  }
}
