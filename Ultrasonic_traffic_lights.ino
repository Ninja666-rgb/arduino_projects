#define trigpin  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define echopin     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define maxdistance 300 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define green 7
#define red 9
#define yellow 10
#define firewall 6

int lasts[firewall];

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
  Serial.begin(115200);
  delay(500);
}

int aver(){
  int s = 0;
  for(int i = 0; i < firewall; i++){
    s += lasts[i];
  }
  s = s/firewall;
  return s;
}

void loop() {
  int a = ultra();
  if(abs(aver()-a) < 20){
    if(a < 10){
      digitalWrite(red, HIGH);
      digitalWrite(yellow, LOW);
      digitalWrite(green, LOW);
    }else if(a > 9 && a < 20){
      digitalWrite(red, LOW);
      digitalWrite(yellow, HIGH);
      digitalWrite(green, LOW);
    }else if(a > 19){
      digitalWrite(red, LOW);
      digitalWrite(yellow, LOW);
      digitalWrite(green, HIGH);
    }
  }
  for(int i = 0; i < firewall; i++){
    lasts[i] = lasts[i+1];
  }
  lasts[firewall-1] = a;
}
