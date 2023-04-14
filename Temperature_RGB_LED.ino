#define greenpin 9
#define bluepin 10
#define redpin 11

float R1 = 10000;

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
  Serial.begin(115200);
  delay(500);
  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
}

float highest = -1000, lowest = 1000;

void loop(){
  int Vo = analogRead(A0);
  float R2 = R1 / (1023.0 / (float)Vo - 1.0);
  float logR2 = log(R2);
  float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
  float T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  float tempC = T - 273.15;
  if(tempC > highest){
    highest = tempC;
  }
  if(tempC < lowest){
    lowest = tempC;
  }
  colour_pick(map(tempC, lowest, highest, 0, 240), 255);
}
