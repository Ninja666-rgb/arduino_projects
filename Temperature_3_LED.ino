#define green 7
#define red 9
#define yellow 10

float R1 = 10000;

void setup() {
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  Serial.begin(115200); // Open serial monitor at 9600 baud to see ping results.
  delay(500);
}

void loop() {
  int Vo = analogRead(A0);
  float R2 = R1 / (1023.0 / (float)Vo - 1.0);
  float logR2 = log(R2);
  float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
  float T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  float tempC = T - 273.15;
  if(tempC > 20){
    digitalWrite(red, HIGH);
    digitalWrite(yellow, LOW);
    digitalWrite(green, LOW);
  }else if(tempC > 10 && tempC < 20){
    digitalWrite(red, LOW);
    digitalWrite(yellow, HIGH);
    digitalWrite(green, LOW);
  }else if(tempC < 10){
    digitalWrite(red, LOW);
    digitalWrite(yellow, LOW);
    digitalWrite(green, HIGH);
  }
}
