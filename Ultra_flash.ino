#define trig 12
#define echo 11
#define firewall 8
#define maxdistance 100

int lasts[firewall];

int ultra(){
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trig, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trig, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  int dist = pulseIn(echo, HIGH, 30*maxdistance);
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

void setup(){
  Serial.begin(15200);
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(5, OUTPUT);
  delay(500);
}

void loop(){
  int a = ultra();
  if(abs(aver(firewall, lasts)-a) < 20){
    if(a < 50){
      digitalWrite(5, HIGH);
      delay(map(a, 0, 50, 0, 300));
      digitalWrite(5, LOW);
      delay(map(a, 0, 50, 0, 300));
    }
  }
  for(int i = 0; i < firewall; i++){
    lasts[i] = lasts[i+1];
  }
  lasts[firewall-1] = a;
}
