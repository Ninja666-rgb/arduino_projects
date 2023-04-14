#define greenpin 9
#define bluepin 10
#define redpin 11

#define firewall 6
#define trigpin  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define echopin     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define maxdistance 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

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

int lasts[firewall];

void RGB(int r, int g, int b){
	analogWrite(redpin, r);
	analogWrite(greenpin, g);
	analogWrite(bluepin, b);
}

void setup(){
	pinMode(redpin, OUTPUT);
	pinMode(greenpin, OUTPUT);
	pinMode(bluepin, OUTPUT);
	pinMode(5, INPUT);
	pinMode(echo, INPUT);
	pinMode(trig, OUTPUT);
}

void loop(){
  int a = ultra();
  if(abs(aver(firewall, lasts)-a) < 20){
    colour_pick(a/200*120, 255);
  }
  for(int i = 0; i < firewall; i++){
    lasts[i] = lasts[i+1];
  }
  lasts[firewall-1] = a;
}
