#define greenpin 9
#define bluepin 10
#define redpin 11

void setup(){
	pinMode(redpin, OUTPUT);
	pinMode(greenpin, OUTPUT);
	pinMode(bluepin, OUTPUT);
	pinMode(5, INPUT);
}

void RGB(int r, int g, int b){
	analogWrite(redpin, r);
	analogWrite(greenpin, g);
	analogWrite(bluepin, b);
}

void loop(){
  RGB(0, 255, 0);//green
  if(digitalRead(5) == LOW){
    RGB(255, 255, 0);//yellow
    delay(2000);
    RGB(255, 0, 0);//red
    while(digitalRead(5) == LOW){
    }
    while(digitalRead(5) == HIGH){
    }
    RGB(0, 255, 0);//green
    while(digitalRead(5) == LOW){
    }
  }
}
