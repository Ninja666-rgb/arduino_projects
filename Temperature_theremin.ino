float R1 = 10000;

void setup(){
  Serial.begin(115200);
  delay(500);
  for(int i = 2; i < 14; i++){
    pinMode(i, OUTPUT);
  }
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
  int level = map(tempC, lowest, highest, 2, 13);
  for(int i = 2; i < level+1; i++){
    digitalWrite(i, HIGH);
  }
  for(int i = level+1; i < 14; i++){
    digitalWrite(i, LOW);
  }
}
