const byte MUX_ADDR [4] = {A3, A2, A1, A0};
const byte MUX_SELECT [4] = {13, 9, 8, 7};
const byte MUX_OUTPUT (12);
const byte MUX_CHANNEL[16][4] = {
  {0, 0, 0, 0},
  {1, 0, 0, 0},
  {0, 1, 0, 0},
  {1, 1, 0, 0},
  {0, 0, 1, 0},
  {1, 0, 1, 0},
  {0, 1, 1, 0},
  {1, 1, 1, 0},
  {0, 0, 0, 1},
  {1, 0, 0, 1},
  {0, 1, 0, 1},
  {1, 1, 0, 1},
  {0, 0, 1, 1},
  {1, 0, 1, 1},
  {0, 1, 1, 1},
  {1, 1, 1, 1}
};

void setup() {
  for (byte i = 0; i < 4; i++) {
    pinMode (MUX_ADDR [i], OUTPUT);
    digitalWrite(MUX_ADDR [i], LOW);
    pinMode (MUX_SELECT [i], OUTPUT);
    digitalWrite(MUX_SELECT [i], HIGH);
  }
  pinMode (MUX_OUTPUT, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  for (byte i = 0; i < 4; i++) {
    digitalWrite(MUX_SELECT[i], LOW);
    for (byte j = 0; j < 16; j++) {
      for (byte k = 0; k < 4; k++) {
        digitalWrite(MUX_ADDR [k], MUX_CHANNEL [j][k]);
      }
      Serial.print("Value at channel ");
      Serial.print(i * 16 + j);
      Serial.print(" is : ");
      Serial.println(digitalRead(MUX_OUTPUT));
      delay(150);
    }
    for (byte l = 0; l < 4; l++) {
        digitalWrite(MUX_SELECT[l], HIGH);
    }
  }
}
