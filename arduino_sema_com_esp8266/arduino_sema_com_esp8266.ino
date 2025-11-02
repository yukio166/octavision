

int verde = 8, amarelo = 9, vermelho = 10;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(vermelho, OUTPUT);
  pinMode(amarelo, OUTPUT);
  pinMode(verde, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    char c = Serial.read();
    switch (c) {
      case 'r':
        digitalWrite(vermelho, HIGH);
        digitalWrite(amarelo, LOW);
        digitalWrite(verde, LOW);
        break;
      case 'y':
        digitalWrite(vermelho, LOW);
        digitalWrite(amarelo, HIGH);
        digitalWrite(verde, LOW);
        break;
      case 'g':
        digitalWrite(vermelho, LOW);
        digitalWrite(amarelo, LOW);
        digitalWrite(verde, HIGH);
        break;
    }
  }
}
