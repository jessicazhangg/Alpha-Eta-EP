//  Chessboard
int reed_sensor_record [8][8];
int reed_sensor_status_memory [8][8];

//  Multiplexer
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

//  Button - Switch
const byte BUTTON_WHITE_SWITCH_MOTOR_WHITE (11);

//****************************************  SETUP
void setup() {
  Serial.begin(9600);

  //  Multiplexer
  for (byte i = 0; i < 4; i++) {
    pinMode (MUX_ADDR [i], OUTPUT);
    digitalWrite(MUX_ADDR [i], LOW);
    pinMode (MUX_SELECT [i], OUTPUT);
    digitalWrite(MUX_SELECT [i], HIGH);
  }
  pinMode (MUX_OUTPUT, INPUT_PULLUP);

  //  Arcade button - Limit Switch
  pinMode (BUTTON_WHITE_SWITCH_MOTOR_WHITE, INPUT_PULLUP);
}

//*****************************************  LOOP
void loop() {

  if (digitalRead(BUTTON_WHITE_SWITCH_MOTOR_WHITE) != HIGH) {
    reed_switch_status();
    reed_switch_display();
    delay(500);
  }
}

//**************************************  DISPLAY
void reed_switch_display() {

  Serial.println("  +-----------------+");
  for (int i = 0; i < 8; i++) {
    Serial.print(' ');
    Serial.print(8 - i);
    Serial.print("| ");
    for (int j = 0; j < 8; j++) {
      Serial.print(reed_sensor_status_memory[i][j]);
      Serial.print(" ");
    }
    Serial.println('|');
  }
  Serial.println("  +-----------------+");
  Serial.println("   a b c d e f g h");
}

//***************************************  STATUS
void reed_switch_status() {

  //  Record the reed switches status
  byte column = 6;
  byte row = 0;

  for (byte i = 0; i < 4; i++) {
    digitalWrite(MUX_SELECT[i], LOW);
    for (byte j = 0; j < 16; j++) {
      for (byte k = 0; k < 4; k++) {
        digitalWrite(MUX_ADDR [k], MUX_CHANNEL [j][k]);
      }
      reed_sensor_record[column][row] = digitalRead(MUX_OUTPUT);
      row++;
      if (j == 7) {
        column++;
        row = 0;
      }
    }
    for (byte l = 0; l < 4; l++) {
      digitalWrite(MUX_SELECT[l], HIGH);
    }
    if (i == 0) column = 4;
    if (i == 1) column = 2;
    if (i == 2) column = 0;
    row = 0;
  }
  for (byte i = 0; i < 8; i++) {
    for (byte j = 0; j < 8; j++) {
      reed_sensor_status_memory[7 - i][j] = reed_sensor_record[i][j];
    }
  }
}
