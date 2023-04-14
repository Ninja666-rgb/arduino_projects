#define firewall 6
#define trigpin  13  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define echopin     12  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define maxdistance 100 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

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

int aver(int n, int num[100]){
  int s;
  for(int i = 0; i < n; i++){
    s += num[i];
  }
  s = s/n;
  return s;
}

int lasts[firewall];

void setup(){
	pinMode(echo, INPUT);
	pinMode(trig, OUTPUT);
  for(int i = 2; i < 12; i++){
    pinMode(i, OUTPUT);
  }
}

int level;

void loop(){
  int a = ultra();
  if(abs(aver(firewall, lasts)-a) < 20){
    level = map(a, 0, 100, 2, 11);
  }
  for(int i = 2; i < level-1; i++){
    digitalWrite(i, HIGH);
  }
  for(int i = level-1; i < 12; i++){
    digitalWrite(i, LOW);
  }
  for(int i = 0; i < firewall; i++){
    lasts[i] = lasts[i+1];
  }
  lasts[firewall-1] = a;
}
