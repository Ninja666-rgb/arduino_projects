void setup(){
  Serial.begin(115200);
  pinMode(2, INPUT);
  for(int i = 3; i < 14; i++){
    pinMode(i, OUTPUT);
  }
}

unsigned long last_pressed;
int l_p, level;

void loop(){
  int p = digitalRead(2);
  if(l_p == HIGH && p == LOW){
    if(level < 11){
      level++;
    }
    last_pressed = millis();
  }
  if(last_pressed+(13-level)*40 < millis()){
    if(level > 0){
      level--;
      last_pressed = millis();
    }
  }
  for(int i = 3; i < level+3; i++){
    digitalWrite(i, HIGH);
  }
  for(int i = level+3; i < 14; i++){
    digitalWrite(i, LOW);
  }
  l_p = p;
}
