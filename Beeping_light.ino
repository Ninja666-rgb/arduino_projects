void setup(){
  Serial.begin(115200);
  delay(500);
  pinMode(8, OUTPUT);
}

int highest = -10000, lowest = 10000;

void loop(){
  int a = analogRead(A0);
  if(a > highest){
    highest = a;
  }
  if(a < lowest){
    lowest = a;
  }
  if(a > lowest+(highest-lowest)/2){
    tone(8, 800);
    delay(map(a, lowest+(highest-lowest)/2, highest, 500, 0));
    noTone(8);
    delay(map(a, lowest+(highest-lowest)/2, highest, 500, 0));
  }
}
