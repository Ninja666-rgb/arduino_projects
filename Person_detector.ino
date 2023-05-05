#define trig 12
#define echo 11
#define firewall 8
#define maxdistance 400

int lasts[firewall];
int thresh;
int gonethrough;
unsigned long wentthrough;

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

int aver(){
  int s = 0;
  for(int i = 0; i < firewall; i++){
    s += lasts[i];
  }
  s = s/firewall;
  return s;
}

void setup(){
  Serial.begin(115200);
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  for(int i = 0; i < 100; i++){
    thresh += ultra();
  }
  thresh = thresh/100;
}

void loop(){
  int a = ultra();
  if(abs(aver()-a) < 20){
    if(a < thresh-20){
      gonethrough = 1;
    }else{
      if(gonethrough == 1){
        if(abs(wentthrough-millis()) > 70){
          Serial.println("Person detected!");
          gonethrough = 0;
          wentthrough = millis();
        }
      }
    }
  }
  for(int i = 0; i < firewall; i++){
    lasts[i] = lasts[i+1];
  }
  lasts[firewall-1] = a;
}
