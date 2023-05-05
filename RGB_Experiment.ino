#define greenpin 9
#define bluepin 10
#define redpin 11

void setup(){
  Serial.begin(115200);
	pinMode(redpin, OUTPUT);
	pinMode(greenpin, OUTPUT);
	pinMode(bluepin, OUTPUT);
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

int i = 0;

void loop(){
  colour_pick(i, 255);
  delay(20);
  int a = analogRead(A0);
  Serial.println(a);
  i = (i+1)%360;
}
