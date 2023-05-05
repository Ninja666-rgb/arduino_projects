#define greenpin 9
#define bluepin 10
#define redpin 11

#define firewall 6
#define trigpin  13  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define echopin     12  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define maxdistance 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

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

int aver(){
  int s = 0;
  for(int i = 0; i < firewall; i++){
    s += lasts[i];
  }
  s = s/firewall;
  return s;
}

void RGB(int r, int g, int b){
	analogWrite(redpin, r);
	analogWrite(greenpin, g);
	analogWrite(bluepin, b);
}

void colour_pick(int i, int p){
  double r, g, b;
  double d = (double)i;
  if(i<60){
    r = 1; 
    g = d/60; 
    b = 0;
  }else if(i<120){
    d -= 60;
    r = (60-d)/60; 
    g = 1; 
    b = 0;
  }else if(i<180){
    d -= 120;
    r = 0; 
    g = 1; 
    b = d/60;
  }else if(i<240){
    d -= 180;
    r = 0; 
    g = (60-d)/60; 
    b = 1;
  }else if(i<300){
    d -= 240;
    r = d/60;
    g = 0; 
    b = 1;
  }else{
    d -= 300;
    r = 1;
    g = 0; 
    b = (60-d)/60;
  }
  RGB(r*p, g*p, b*p);
}

void setup(){
	pinMode(redpin, OUTPUT);
	pinMode(greenpin, OUTPUT);
	pinMode(bluepin, OUTPUT);
	pinMode(echopin, INPUT);
	pinMode(trigpin, OUTPUT);
}

void loop(){
  int a = ultra();
  if(abs(aver()-a) < 20 && a != 0){
    colour_pick(map(a, 1, 20, 0, 120), 255);
  }else{
    RGB(0, 0, 0);
  }
  for(int i = 0; i < firewall; i++){
    lasts[i] = lasts[i+1];
  }
  lasts[firewall-1] = a;
}
