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
int i = 0;

//reset
#define RESET_PIN 52

CRGB leds[NUM_LEDS];

//****************************************  SETUP
void setup() {
  Serial.begin(9600);

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
      game_mode = HvsC;
      sequence = player_white;
      Serial.println("Done with Start!");
      break;

    case player_white:
      detect_human_movement();
      //if (button(END) == true) {  // White player end turn
        player_displacement();
        // if (game_mode == HvsH) { // no HvH
        //   AI_HvsH();  // Check is movement is valid
        //   if (no_valid_move == false) sequence = player_black;
        //   else lcd_display();
        // }
        //else if (game_mode == HvsC) {
        AI_HvsC();
        sequence = player_black;
        //}
        Serial.println("Done with White!");
        break;
      //}

      case player_black:
        black_player_movement();  //  Move the black chess piece
        sequence = player_white;
        //}
        Serial.println("Done with Black!");
        break;
  }

  // detect game over
  if (game_over == true) {
    Serial.print("White wins?");
  }
  // resets arduino
  if (button(END)) {
    digitalWrite(RESET_PIN, LOW);
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

  //  Update the reed sensors states with the Black move
  reed_sensor_status_memory[convert_table[departure_coord_Y]][departure_coord_X - 1] = 1;
  reed_sensor_status_memory[convert_table[arrival_coord_Y]][arrival_coord_X - 1] = 0;
  reed_sensor_status[convert_table[departure_coord_Y]][departure_coord_X - 1] = 1;
  reed_sensor_status[convert_table[arrival_coord_Y]][arrival_coord_X - 1] = 0;
}

//************************  DETECT HUMAN MOVEMENT
void detect_human_movement() {
  //edit reed_sensor_record
  if (i == 0) {
    for (byte i = 2; i < 6; i++ ) {
      for (byte j = 0; j < 8; j++) {
        reed_sensor_record[i][j] = 1;
      }
    }
    reed_sensor_record[3][3] = 0;
    reed_sensor_record[1][3] = 1;
  }

  if (i == 1) {
    reed_sensor_record[4][3] = 0;
    reed_sensor_record[3][3] = 1;
    reed_sensor_record[7][1] = 1;
    reed_sensor_record[5][2] = 0;
  }

  if (i == 2) {
    reed_sensor_record[3][4] = 0;
    reed_sensor_record[1][4] = 1;
    reed_sensor_record[5][2] = 1;
    reed_sensor_record[4][4] = 0;
  }

  if (i == 3) {
    reed_sensor_record[3][5] = 0;
    reed_sensor_record[1][5] = 1;
    reed_sensor_record[4][2] = 0;
    reed_sensor_record[6][2] = 1;
  }

  if (i == 4) {
    reed_sensor_record[4][0] = 0;
    reed_sensor_record[7][3] = 1;
    reed_sensor_record[1][3] = 0;
    reed_sensor_record[0][2] = 1;
  }

  if (i == 5) {
    reed_sensor_record[3][0] = 0;
    reed_sensor_record[4][0] = 1;
    reed_sensor_record[0][1] = 1;
    reed_sensor_record[2][2] = 0;
  }

  if (i == 6) {
    reed_sensor_record[3][0] = 1;
    reed_sensor_record[3][1] = 0;
    reed_sensor_record[3][5] = 1;
    reed_colone[1] = 3;
    reed_line[1] = 4;
  }

  if (i == 7) {
    reed_sensor_record[3][1] = 1;
    reed_sensor_record[0][0] = 1;
    reed_sensor_record[0][1] = 0;
  }

  if (i == 8) {
    reed_sensor_record[1][1] = 1;
    reed_sensor_record[2][0] = 0;
    reed_sensor_record[2][2] = 1;
    reed_sensor_record[4][1] = 0;
  }

  if (i == 9) {
    reed_sensor_record[2][0] = 1;
    reed_sensor_record[4][1] = 1;
    reed_sensor_record[6][2] = 0;
  }

  if (i == 10) {
    reed_sensor_record[7][4] = 1;
    reed_sensor_record[7][3] = 0;
    reed_sensor_record[6][2] = 1;
    reed_colone[1] = 0;
    reed_line[1] = 0;
  }

  if (i == 11) {
    reed_sensor_record[6][5] = 1;
    reed_sensor_record[4][5] = 0;
    reed_sensor_record[0][1] = 1;
    reed_sensor_record[0][0] = 0;
  }

  if (i == 12) {
    reed_sensor_record[1][0] = 1;
    reed_sensor_record[1][1] = 0;
    reed_sensor_record[0][0] = 1;
    reed_colone[1] = 1;
    reed_line[1] = 0;
  }

  if (i == 13) {
    reed_sensor_record[1][1] = 1;
    reed_sensor_record[1][3] = 1;
    reed_sensor_record[4][0] = 0;
  }

  if (i == 14) {
    reed_sensor_record[7][3] = 1;
    reed_sensor_record[7][4] = 0;
    reed_sensor_record[0][6] = 1;
    reed_sensor_record[2][5] = 0;
  }

  if (i == 15) {
    reed_sensor_record[4][4] = 1;
    reed_sensor_record[0][5] = 1;
    reed_sensor_record[1][4] = 0;
  }

  if (i == 16) {
    reed_sensor_record[7][6] = 1;
    reed_sensor_record[5][5] = 0;
    reed_sensor_record[7][0] = 1;
    reed_sensor_record[6][2] = 0;
  }

  if (i == 17) {
    reed_sensor_record[7][4] = 1;
    reed_sensor_record[6][5] = 0;
    reed_sensor_record[2][5] = 1;
    reed_sensor_record[4][6] = 0;
  }

  //captures queen
  if (i == 18) {
    reed_sensor_record[6][5] = 1;
    reed_sensor_record[5][6] = 0;
    reed_sensor_record[4][6] = 1;
    reed_colone[1] = 4;
    reed_line[1] = 4;
  }

  if (i == 19) {
    reed_sensor_record[5][5] = 1;
    reed_sensor_record[1][4] = 1;
    reed_sensor_record[4][7] = 0;
  }

  if (i == 20) {
    reed_sensor_record[5][6] = 1;
    reed_sensor_record[5][5] = 0;
    reed_sensor_record[6][2] = 1;
    reed_sensor_record[7][4] = 0;
  }

  if (i == 21) {
    reed_sensor_record[5][5] = 1;
    reed_sensor_record[4][4] = 0;
    reed_sensor_record[0][4] = 1;
    reed_sensor_record[0][6] = 0;
    reed_sensor_record[0][5] = 0;
    reed_sensor_record[0][7] = 1;
  }

  if (i == 22) {
    reed_sensor_record[6][1] = 1;
    reed_sensor_record[4][1] = 0;
    reed_sensor_record[0][3] = 1;
    reed_sensor_record[0][0] = 0;
  }

  if (i == 23) {
    reed_sensor_record[4][4] = 1;
    reed_sensor_record[4][3] = 0;
    reed_sensor_record[0][5] = 1;
    reed_colone[1] = 3;
    reed_line[1] = 5;
  }

  if (i == 24) {
    reed_sensor_record[6][4] = 1;
    reed_sensor_record[4][4] = 0;
    reed_sensor_record[0][0] = 1;
    reed_colone[1] = 3;
    reed_line[1] = 4;
  }

  if (i == 25) {
    reed_sensor_record[4][3] = 1;
    reed_sensor_record[3][2] = 0;
    reed_sensor_record[4][4] = 1;
    reed_colone[1] = 4;
    reed_line[1] = 4;
  }

  for (byte i = 0; i < 8; i++) {
    for (byte j = 0; j < 8; j++) {
      reed_sensor_status_memory[7 - i][j] = reed_sensor_record[i][j];
    }
  }

  //  Compare the old and new status of the reed switches

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
        }
      }
    }
  }

  if (i == 21) {
    reed_colone[0] = 7;
    reed_line[0] = 4;
  }
  //  Set the new status of the reed sensors
  for (byte i = 0; i < 8; i++) {
    for (byte j = 0; j < 8; j++) {
      reed_sensor_status[i][j] = reed_sensor_status_memory[i][j];
    }
  }
  i += 1;
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