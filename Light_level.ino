void setup(){
  pinMode(5, INPUT);
  for(int i = 3; i < 14; i++){
    pinMode(i, OUTPUT);
  }
}

unsigned long last_pressed;
int l_p, level;

void loop(){
  int p = digitalRead(5);
  if(l_p == HIGH && p == LOW){
    if(level < 11){
      level++;
    }
    last_pressed = millis();
  }
  if(last_pressed+500 < millis()){
    if(level > 0){
      level--;
    }
  }
  for(int i = 3; i < level+3; i++){
    digitalWrite(i, HIGH);
  }
  for(int i = level+3; i < 14; i++){
    digitalWrite(i, LOW);
  }
}
