//***********************************************
//
//            AUTOMATIC CHESSBOARD
//
//***********************************************

//******************************  INCLUDING FILES
#include "global.h"
#include "Micro_Max.h"
#include <Wire.h>
#include "FastLED.h"

//LED Setup
#define LED_PIN 2
#define NUM_LEDS 150
#define BRIGHTNESS 64
#define LED_TYPE WS2812
#define red         CRGB (130, 0, 0)
#define gold        CRGB (255, 144, 0)
#define black       CRGB (0, 0, 0)
#define green       CRGB (0, 130, 0);
#define white       CRGB (255, 255, 255);
#define blue        CRGB(0, 0, 255);

//reset
#define RESET_PIN 52

CRGB leds[NUM_LEDS];

//****************************************  SETUP
void setup() {
  Serial.begin(9600);

  
  FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  //reset button
  digitalWrite(RESET_PIN, HIGH);
  pinMode(RESET_PIN, OUTPUT);

  //  Multiplexer
  for (byte i = 0; i < 4; i++) {
    pinMode (MUX_ADDR [i], OUTPUT);
    digitalWrite(MUX_ADDR [i], LOW);
    pinMode (MUX_SELECT [i], OUTPUT);
    digitalWrite(MUX_SELECT [i], HIGH);
  }
  pinMode (MUX_OUTPUT, INPUT_PULLUP);

  //  Set the reed switches status
  for (byte i = 2; i < 6; i++) {
    for (byte j = 0; j < 8; j++) {
      reed_sensor_status[i][j] = 1;
      reed_sensor_status_memory[i][j] = 1;
    }
  }

  //  MicroMax
  lastH[0] = 0;

  //  Countdown
  timer = millis();

  //  Arcade button - Limit Switch
  pinMode (BUTTON_END_TURN, INPUT_PULLUP);
  pinMode (BUTTON_RESET, INPUT_PULLUP);
}

//*****************************************  LOOP
void loop() {
  switch (sequence) {
    case start:
      // if (button(WHITE) == true) {  // HvsH Mode (WE CAN IMPLEMENT LATER IF WANTED!!)
      //   game_mode = HvsH;
      //   sequence = player_white;
      // }

      // HvsC Mode
      // displayTT();
      // displayAH();
      start_check();
      game_mode = HvsC;
      sequence = player_white;
      Serial.println("Done with Start!");
      break;

    case player_white:
      detect_human_movement();
      if (button(END) == true) {  // White player end turn
        //led_reset();
        player_displacement();
        // if (game_mode == HvsH) { // no HvH
        //   AI_HvsH();  // Check is movement is valid
        //   if (no_valid_move == false) sequence = player_black;
        //   else lcd_display();
        // }
        //else if (game_mode == HvsC) {
        AI_HvsC();
        if (no_valid_move == true) { 
          led_invalid();
          break;
        }
        sequence = player_black;
        led_valid();
        //}
        Serial.println("Done with White!");
      }
      break;

      case player_black:
        //  Game mode HvsH
        // if (game_mode == HvsH) {  // Display the black player clock (NO HvH)
          // if (millis() - timer > 995) {
          //   countdown();
          //   //lcd_display();
          // }  
          //detect_human_movement();
          // if (button(END) == true) {  // Black human player end turn
          //   new_turn_countdown = true;
          //   player_displacement();
          //   AI_HvsH();  // Check is movement is valid
          //   if (no_valid_move == false) sequence = player_white;
          //   //else lcd_display();
          // }
        //}
        //  Game mode HvsC
        //else if (game_mode == HvsC) {
        black_player_movement();  //  Move the black chess piece
        sequence = player_white;
        //}
        Serial.println("Done with Black!");
        break;
  }

  // detect game over
  if (game_over == true) {
    led_gameover(white_win);
  }
  
}

//***************************************  SWITCH
boolean button(byte type) {

  if (type == END && digitalRead(BUTTON_END_TURN) != HIGH) {
    delay(250);
    return true;
  }
  if (type == RESET && digitalRead(BUTTON_RESET) != HIGH) {
    delay(250);
    return true;
  }
  return false;
}

// ***********************  BLACK PLAYER MOVEMENT
void black_player_movement() {

  //  Convert the AI characters in variables
  int departure_coord_X = lastM[0] - 'a' + 1;
  int departure_coord_Y = lastM[1] - '0';
  int arrival_coord_X = lastM[2] - 'a' + 1;
  int arrival_coord_Y = lastM[3] - '0';

  //  Trolley displacement to the starting position
  int convert_table [] = {0, 7, 6, 5, 4, 3, 2, 1, 0};

  led_black_move(departure_coord_X, departure_coord_Y, arrival_coord_X, arrival_coord_Y);

  // Give player time to move the piece
  //delay(5000);

  while(true) {
    // player uses button to signify they moved black piece
    if (button(END) == true) {
      byte column = 6;
      byte row = 0;
      // scans reed sensors
      for (byte i = 0; i < 4; i++) {
        digitalWrite(MUX_SELECT[i], LOW);
        for (byte j = 0; j < 16; j++) {
          for (byte k = 0; k < 4; k++) {
            digitalWrite(MUX_ADDR [k], MUX_CHANNEL [j][k]);
            // might need to add delay(5 / 10) here according to online bc sensing too fast causes errors
          }
          reed_sensor_black[column][row] = digitalRead(MUX_OUTPUT);
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
      if (reed_sensor_black[departure_coord_Y - 1][departure_coord_X - 1] == 1 && reed_sensor_black[arrival_coord_Y - 1][arrival_coord_X - 1] == 0) {
        Serial.println("CORRECT BLACK MOVE!");
        break;
      }
      else {
        led_invalid();
        led_black_move(departure_coord_X, departure_coord_Y, arrival_coord_X, arrival_coord_Y);
      }
    }
  }
  led_valid();
  
  //  Update the reed sensors states with the Black move
  reed_sensor_status_memory[convert_table[departure_coord_Y]][departure_coord_X - 1] = 1;
  reed_sensor_status_memory[convert_table[arrival_coord_Y]][arrival_coord_X - 1] = 0;
  reed_sensor_status[convert_table[departure_coord_Y]][departure_coord_X - 1] = 1;
  reed_sensor_status[convert_table[arrival_coord_Y]][arrival_coord_X - 1] = 0;
}

//************************  DETECT HUMAN MOVEMENT
void detect_human_movement() {

  //  Record the reed switches status
  byte column = 6;
  byte row = 0;

  for (byte i = 0; i < 4; i++) {
    digitalWrite(MUX_SELECT[i], LOW);
    for (byte j = 0; j < 16; j++) {
      for (byte k = 0; k < 4; k++) {
        digitalWrite(MUX_ADDR [k], MUX_CHANNEL [j][k]);
        // might need to add delay(5) here according to online bc sensing too fast causes errors
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

  //  Compare the old and new status of the reed switches
  led_reset();
  for (byte i = 0; i < 8; i++) {
    for (byte j = 0; j < 8; j++) {
      if (reed_sensor_status[i][j] != reed_sensor_status_memory[i][j]) {
        if (reed_sensor_status_memory[i][j] == 1) {
          reed_colone[0] = i;
          reed_line[0] = j;
        }
        if (reed_sensor_status_memory[i][j] == 0) {
          reed_colone[1] = i;
          reed_line[1] = j;
          leds[led_coord(i + 1, 8 - j)] = red;
          leds[led_coord(i + 1, 8 - j) + 1] = red;
          FastLED.show();
        }
      }
    }
  }
  //  Set the new status of the reed sensors
  for (byte i = 0; i < 8; i++) {
    for (byte j = 0; j < 8; j++) {
      reed_sensor_status[i][j] = reed_sensor_status_memory[i][j];
    }
  }

  reed_switch_display();
}

//**************************  PLAYER DISPLACEMENT
void player_displacement() {

  //  Convert the reed sensors switches coordinates in characters
  char table1[] = {'8', '7', '6', '5', '4', '3', '2', '1'};
  char table2[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

  mov[0] = table2[reed_line[0]];
  mov[1] = table1[reed_colone[0]];
  mov[2] = table2[reed_line[1]];
  mov[3] = table1[reed_colone[1]];
}

int led_coord(int x, int y) {
  x = x - 1;
  y = y - 1;
  if (x == 0) {return 133 + y * 2;}
  if (x == 1) {return 128 - y * 2;}
  if (x == 2) {return 95 + y * 2;}
  if (x == 3) {return 90 - y * 2;}
  if (x == 4) {return 57 + y * 2;}
  if (x == 5) {return 52 - y * 2;}
  if (x == 6) {return 19 + y * 2;}
  if (x == 7) {return 14 - y * 2;}
}

void displayTT() {
    for (int i = 0; i < 2; i++) {
      leds[50] = red;
      leds[51] = red;
      leds[59] = red;
      leds[60] = red;
      leds[88] = red;
      leds[89] = red;
      leds[95] = red;
      leds[96] = red;
      leds[97] = red;
      leds[98] = red;
      leds[99] = red;
      leds[100] = red;

      leds[40] = gold;
      leds[41] = gold;
      leds[69] = gold;
      leds[70] = gold;
      leds[78] = gold;
      leds[79] = gold;
      leds[105] = gold;
      leds[106] = gold;
      leds[107] = gold;
      leds[108] = gold;
      leds[109] = gold;
      leds[110] = gold;
      FastLED.show();
      delay(2000);
      led_reset();
      delay(1000);
  }
}

void displayAH() {
    leds[38] = red;
    leds[39] = red;
    leds[42] = red;
    leds[43] = red;
    leds[67] = red;
    leds[68] = red;
    leds[71] = red;
    leds[72] = red;
    leds[76] = red;
    leds[77] = red;
    leds[78] = red;
    leds[79] = red;
    leds[80] = red;
    leds[81] = red;
    leds[105] = red;
    leds[106] = red;
    leds[109] = red;
    leds[110] = red;

    leds[48] = gold;
    leds[49] = gold;
    leds[52] = gold;
    leds[53] = gold;
    leds[57] = gold;
    leds[58] = gold;
    leds[59] = gold;
    leds[60] = gold;
    leds[61] = gold;
    leds[62] = gold;
    leds[86] = gold;
    leds[87] = gold;
    leds[90] = gold;
    leds[91] = gold;
    leds[95] = gold;
    leds[96] = gold;
    leds[97] = gold;
    leds[98] = gold;
    leds[99] = gold;
    leds[100] = gold;
    FastLED.show();
    delay(1500);
    led_reset();
}

void led_reset() {
  for (int i = 0; i < 150; i++) {
      leds[i] = black;
    }
    FastLED.show();
}

void led_invalid() {
  for (int i = 0; i < 150; i++) {
    leds[i] = 150;
    FastLED.show();
    delay(500);
    led_reset();
    delay(500);
  }
}

void led_valid() {
  for (int i = 0; i < 150; i++) {
    leds[i] = green;
    FastLED.show();
    delay(500);
    led_reset();
    delay(500);
  }
}

void led_black_move(int ax, int ay, int dx, int dy) {
  int arrival_led = led_coord(ay, ax);
  int departure_led = led_coord(dy, dx);
  leds[arrival_led] = blue;
  leds[arrival_led + 1] = blue;
  leds[departure_led] = blue;
  leds[departure_led + 1] = blue; 
  FastLED.show();
}

void start_check() {
  bool wrong = true;
  while(wrong) {
    wrong = false;
    byte column = 6;
    byte row = 0;

    for (byte i = 0; i < 4; i++) {
      digitalWrite(MUX_SELECT[i], LOW);
      for (byte j = 0; j < 16; j++) {
        for (byte k = 0; k < 4; k++) {
          digitalWrite(MUX_ADDR [k], MUX_CHANNEL [j][k]);
          delay(3);
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

    for(int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        Serial.print(reed_sensor_record[i][j]);
      }
      Serial.println("");
    }
    Serial.println("");
    led_reset();
    for(int i = 0; i < 2; i++) {
      for(int j = 0; j < 8; j++) {
        if (reed_sensor_record[i][j] == 1) {
          leds[led_coord(8 - i, j + 1)] = red;
          leds[led_coord(8 - i, j + 1) + 1] = red;
          wrong = true;
        } 
        if (reed_sensor_record[7 - i][j] == 1) {
          leds[led_coord(i + 1, j + 1)] = red;
          leds[led_coord(i + 1, j + 1) + 1] = red;
          wrong = true;
        }
      }
    }
    FastLED.show();
    delay(500);
  }
  led_reset();
}

void led_gameover(boolean win) {
  if (win == true) {
    for (int i = 0; i < 150; i ++) {
      leds[i] = green;
    }
  }
  else {
    for (int i = 0; i < 150; i ++) {
      leds[i] = red;
    }
  }
  FastLED.show();
}

void reed_switch_display() {
  Serial.println("  +-----------------+");
  for (int i = 0; i < 8; i++) {
    Serial.print(' ');
    Serial.print(8 - i);
    Serial.print("| ");
    for (int j = 0; j < 8; j++) {
      Serial.print(reed_sensor_record[i][j]);
      Serial.print(" ");
    }
    Serial.println('|');
  }
  Serial.println("  +-----------------+");
  Serial.println("   a b c d e f g h");
}