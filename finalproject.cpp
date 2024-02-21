// C++ code
//
#include "timer.h"
#include <LiquidCrystal.h>

char gSegPins[] = {A5, A2, 5, 4, 3, A4, 6};

void displayNumTo7Seg(unsigned int targetNum, int digitPin) {


    // A map of integers to the respective values needed to display
    // a value on one 7 seg digit.
    unsigned char encodeInt[10] = {
        // 0     1     2     3     4     5     6     7     8     9
        0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67,
    };




    // Make sure the target digit is off while updating the segments iteratively
    digitalWrite(digitPin, HIGH);


    // Update the segments
    for (int k = 0; k < 7; ++k) {
        digitalWrite(gSegPins[k], encodeInt[targetNum] & (1 << k));
    }


    // Turn on the digit again
    digitalWrite(digitPin, LOW);


}

//declare notes, found 8-bit notes in online document
int c = 262; 
int d = 294;
int e = 330;
int f = 349;
int g = 392;
int a = 440;
int b = 494;
int C = 523;

int timerI = 0; //timer with period of 10 ms
int count = 9; //for timer countdown
int gameoverCount; //game over
int introTimer;
int songTimer;
int noteCount;
int noToneTimer;
int wonCount;
int inputTimer = 0;
bool waveStart = false;
bool reachGoal = false;
bool hitWall = false;

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
int xVal = analogRead(A0);
int yVal = analogRead(A3);
const int swPin = 7;
int switchState = 1;
int currentCol = 0;
int currentRow = 0;
int goalColumn = 15;
int goalRow = 1;
String playerChar = "*";
String mazeChar = "#";
String goalChar = "O";

enum States_gametimer { START, DISPLAYNUM, COUNTDOWN};
enum States_gameloop { STARTGAME, INSTRUCTONE, INSTRUCTTWO, FIRSTWAVE, GAMEOVER, GAMEOVERWALL, WONGAME};
enum States_ingameplayer { BEGIN, WAIT, READINPUT, MOVERIGHT, MOVELEFT, MOVEUP, MOVEDOWN};
enum States_printmaze { WAITWAVE, PRINT };
enum States_gamesong { WAITPLAY, NOTEC, NOTED, NOTEE, NOTEF, NOTEG, NOTONE };

void tick_gametimer ( void );
void tick_gameloop ( void );
void tick_ingameplayer ( void );
void tick_printmaze ( void );
void tick_gamesong ( void ); 

void tick_gametimer ( void ) {

	static enum States_gametimer state_gametimer = START;

	// transitions between states
	switch ( state_gametimer ) { 
    case START:
    count = 10;
    timerI = 0;

    if(waveStart == true) {
      state_gametimer = COUNTDOWN;
    }
    if(waveStart == false) {
      state_gametimer = START;
    }
    break;

    case DISPLAYNUM:
    if(timerI < 100) {
      ++timerI;
      state_gametimer = DISPLAYNUM;
    }
    if(!(timerI < 100)) {
      state_gametimer = COUNTDOWN;
    }
    if(reachGoal == true) {
      waveStart = false;
      state_gametimer = START;
    }
    if(hitWall == true) {
      waveStart = false;
      state_gametimer = START;
    }
    break;

    case COUNTDOWN:
    if(count < 1) {
      waveStart = false;
      state_gametimer = START;
    }
    if(!(count < 1)) {
      --count;
      timerI = 0;
      state_gametimer = DISPLAYNUM;
    }
    break;
  }

  switch( state_gametimer ) {
    case START:
    displayNumTo7Seg(0, A1);
    break;

    case DISPLAYNUM:
    displayNumTo7Seg(count, A1);
    break;

    case COUNTDOWN:
    break;
  }
}

void tick_gameloop ( void ) {
  static enum States_gameloop state_gameloop = STARTGAME;

  switch ( state_gameloop ) {
    case STARTGAME:
    gameoverCount = 500;
    wonCount = 500;
    introTimer = 500;
    waveStart = false;
    hitWall = false;
    reachGoal = false;
    switchState = digitalRead(swPin);

    if(switchState == 0) {
      lcd.clear();
      state_gameloop = INSTRUCTONE;
    }
    if(switchState == 1) {
      state_gameloop = STARTGAME;
    }
    break;

    case INSTRUCTONE:
    if(introTimer > 0) {
      --introTimer;
      state_gameloop = INSTRUCTONE;
    }
    if(!(introTimer > 0)) {
      introTimer = 500;
      lcd.clear();
      state_gameloop = INSTRUCTTWO;
    }
    break;

    case INSTRUCTTWO:
    if(introTimer > 0) {
      --introTimer;
      state_gameloop = INSTRUCTTWO;
    }
    if(!(introTimer > 0)) {
      waveStart = true;
      lcd.clear();
      state_gameloop = FIRSTWAVE;
    }
    break;

    case FIRSTWAVE:
    if(waveStart == true) {
      state_gameloop = FIRSTWAVE;
    }
    if(waveStart == false && reachGoal == false && hitWall == false) {
      lcd.clear();
      state_gameloop = GAMEOVER;
    }
    if(hitWall == true) {
      lcd.clear();
      state_gameloop = GAMEOVERWALL;
    }
    if(reachGoal == true) {
      lcd.clear();
      state_gameloop = WONGAME;
    }
    break; 

    case GAMEOVER:
    if(gameoverCount > 0) {
      --gameoverCount;
      state_gameloop = GAMEOVER;
    }
    if(!(gameoverCount > 0)) {
      state_gameloop = STARTGAME;
    }
    break;

    case GAMEOVERWALL:
    if(gameoverCount > 0) {
      --gameoverCount;
      state_gameloop = GAMEOVERWALL;
    }
    if(!(gameoverCount > 0)) {
      state_gameloop = STARTGAME;
    }
    break;

    case WONGAME:
    if(wonCount > 0) {
      --wonCount;
      state_gameloop = WONGAME;
    }
    if(!(wonCount > 0)) {
      state_gameloop = STARTGAME;
    }
  }

  switch ( state_gameloop ) {
    case STARTGAME:
    lcd.setCursor(0, 0);
    lcd.print("Start game?");

    lcd.setCursor(0, 1);
    lcd.print("Press in stick!");
    break;

    case INSTRUCTONE:
    lcd.setCursor(0, 0);
    lcd.print("* - Player");

    lcd.setCursor(0, 1);
    lcd.print("# - Maze Walls");
    break;

    case INSTRUCTTWO:
    lcd.setCursor(0, 0);
    lcd.print("O - Goal");

    lcd.setCursor(0, 1);
    lcd.print("Ready?");
    break;

    case FIRSTWAVE:
    break;

    case GAMEOVER:
    lcd.setCursor(0, 0);
    lcd.print("GAME OVER");
    lcd.setCursor(0, 1);
    lcd.print("RAN OUT OF TIME!");
    break;

    case GAMEOVERWALL:
    lcd.setCursor(0, 0);
    lcd.print("GAME OVER");
    lcd.setCursor(0, 1);
    lcd.print("HIT MAZE WALL!");
    break;

    case WONGAME:
    lcd.setCursor(0, 0);
    lcd.print("YOU WON!!!");
    lcd.setCursor(0, 1);
    lcd.print("CONGRATULATIONS!! :D");
    break;
  }
}

void tick_ingameplayer ( void ) {
  static enum States_ingameplayer state_ingameplayer = BEGIN;

  switch ( state_ingameplayer ) {
    case BEGIN:
    xVal = map(xVal, 0, 1023, 5, 0);
    yVal = map(yVal, 0, 1023, 5, 0);
    inputTimer = 0;
    currentCol = 0;
    currentRow = 0;
    if(waveStart == true) {
      state_ingameplayer = WAIT;
    }
    if(waveStart == false) {
      state_ingameplayer = BEGIN;
    }
    break;

    case WAIT:
    if(inputTimer < 26) {
      ++inputTimer;
      state_ingameplayer = WAIT;
    }
    if(!(inputTimer < 26)) {
      inputTimer = 0;
      state_ingameplayer = READINPUT;
    }
    if(waveStart == false) {
      state_ingameplayer = BEGIN;
    }
    if((currentCol == 2 && currentRow == 0) || (currentCol == 3 && currentRow == 0) || (currentCol == 5 && currentRow == 1) || (currentCol == 6 && currentRow == 1) || (currentCol == 8 && currentRow == 0) || (currentCol == 10 && currentRow == 1) || (currentCol == 12 && currentRow == 0) || (currentCol == 14 && currentRow == 1)) {
      hitWall = true;
      state_ingameplayer = BEGIN;
    }
    if(currentCol == 15 && currentRow == 1) {
      reachGoal = true;
      state_ingameplayer = BEGIN;
    }
    break;

    case READINPUT:
    xVal = analogRead(A0);
    yVal = analogRead(A3);
    xVal = map(xVal, 0, 1023, 5, 0);
    yVal = map(yVal, 0, 1023, 5, 0);
    if(xVal == 1) {
      lcd.clear();
      state_ingameplayer = MOVERIGHT;
    }
    if(xVal == 5) {
      lcd.clear();
      state_ingameplayer = MOVELEFT;
    }
    if(xVal == 3) {
      state_ingameplayer = READINPUT;
    }
    if(yVal == 1) {
      lcd.clear();
      state_ingameplayer = MOVEUP;
    }
    if(yVal == 5) {
      lcd.clear();
      state_ingameplayer = MOVEDOWN;
    }
    if(waveStart == false) {
      state_ingameplayer = BEGIN;
    }
    break;

    case MOVERIGHT:
    xVal = analogRead(A0);
    xVal = map(xVal, 0, 1023, 5, 0);
    if(xVal == 0 || xVal == 1) {
      state_ingameplayer = WAIT;
    }
    if(!(xVal == 0 || xVal == 1)) {
      state_ingameplayer = WAIT;
    }
    if(waveStart == false) {
      state_ingameplayer = BEGIN;
    }
    break;

    case MOVELEFT: 
    xVal = analogRead(A0);
    xVal = map(xVal, 0, 1023, 5, 0);
    if(xVal == 5) {
      state_ingameplayer = WAIT;
    }
    if(!(xVal == 5)) {
      state_ingameplayer = WAIT;
    }
    if(waveStart == false) {
      state_ingameplayer = BEGIN;
    }
    break;

    case MOVEDOWN:
    yVal = analogRead(A3);
    yVal = map(yVal, 0, 1023, 5, 0);

    if(yVal == 1) {
      state_ingameplayer = WAIT;
    }
    if(!(yVal == 1)) {
      state_ingameplayer = WAIT;
    }
    if(waveStart == false) {
      state_ingameplayer = BEGIN;
    }
    break;

    case MOVEUP:
    yVal = analogRead(A3);
    yVal = map(yVal, 0, 1023, 5, 0);

    if(yVal == 5) {
      state_ingameplayer = WAIT;
    }
    if(!(yVal == 5)) {
      state_ingameplayer = WAIT;
    }
    if(waveStart == false) {
      state_ingameplayer = BEGIN;
    }
    break;
  }

  switch( state_ingameplayer ) {
    case BEGIN:
    break;

    case WAIT:
    lcd.setCursor(currentCol, currentRow);
    lcd.print(playerChar);
    break;

    case READINPUT:
    //lcd.clear();
    lcd.setCursor(currentCol, currentRow);
    lcd.print(playerChar);
    break;

    case MOVERIGHT:
    ++currentCol;
    if(currentCol > 15) {
      currentCol = 15;
    }
    //lcd.clear();
    lcd.setCursor(currentCol, currentRow);
    lcd.print(playerChar);
    break;

    case MOVELEFT:
    --currentCol;
    if(currentCol < 0) {
      currentCol = 0;
    }
    //lcd.clear();
    lcd.setCursor(currentCol, currentRow);
    lcd.print(playerChar);
    break;

    case MOVEDOWN:
    --currentRow;
    if(currentRow < 0) {
      currentRow = 0;
    }
    //lcd.clear();
    lcd.setCursor(currentCol, currentRow);
    lcd.print(playerChar);
    break;

    case MOVEUP:
    ++currentRow;
    if(currentRow > 1) {
      currentRow = 1;
    }
    //lcd.clear();
    lcd.setCursor(currentCol, currentRow);
    lcd.print(playerChar);
    break;
  }
}

void tick_printmaze ( void ) {
  static enum States_printmaze state_printmaze = WAITWAVE;

  switch ( state_printmaze ) {
    case WAITWAVE: 
    if(waveStart == false) {
      state_printmaze = WAITWAVE;
    }
    if(waveStart == true) {
      state_printmaze = PRINT;
    }
    break;

    case PRINT:
    if(waveStart == true) {
      state_printmaze = PRINT;
    }
    if(waveStart == false) {
      state_printmaze = WAITWAVE;
    }
    break;
  }
  switch ( state_printmaze ) {
    case WAITWAVE:
    break;

    case PRINT:
    lcd.setCursor(2, 0);
    lcd.print(mazeChar);

    lcd.setCursor(3, 0);
    lcd.print(mazeChar);

    lcd.setCursor(5, 1);
    lcd.print(mazeChar);

    lcd.setCursor(6, 1);
    lcd.print(mazeChar);

    lcd.setCursor(8, 0);
    lcd.print(mazeChar);

    lcd.setCursor(10, 1);
    lcd.print(mazeChar);

    lcd.setCursor(12, 0);
    lcd.print(mazeChar);

    lcd.setCursor(14, 1);
    lcd.print(mazeChar);

    lcd.setCursor(15, 1);
    lcd.print(goalChar);
    break;
  }
}

void tick_gamesong ( void ) {
  static enum States_gamesong state_gamesong = WAITPLAY;

  switch ( state_gamesong ){
    case WAITPLAY:
    songTimer = 0;
    noToneTimer = 0;
    noteCount = 0;
    songTimer = 60;

    if(waveStart == false) {
      state_gamesong = WAITPLAY;
    }
    if(waveStart == true) {
      songTimer = 60;
      ++noteCount;
      state_gamesong = NOTEC;
    }
    break;

    case NOTEC:
    if(songTimer < 0) {
      ++noteCount;
      noToneTimer = 5;
      state_gamesong = NOTONE;
    }
    if(!(songTimer < 0)) {
      --songTimer;
      state_gamesong = NOTEC;
    }
    if(waveStart == false) {
      state_gamesong = WAITPLAY;
    }
    break;

    case NOTED:
    if(songTimer < 0) {
      ++noteCount;
      noToneTimer = 5;
      state_gamesong = NOTONE;
    }
    if(!(songTimer < 0)) {
      --songTimer;
      state_gamesong = NOTED;
    }
    if(waveStart == false) {
      state_gamesong = WAITPLAY;
    }
    break;

    case NOTEE:
    if(songTimer < 0) {
      ++noteCount;
      noToneTimer = 5;
      state_gamesong = NOTONE;
    }
    if(!(songTimer < 0)) {
      --songTimer;
      state_gamesong = NOTEE;
    }
    if(waveStart == false) {
      state_gamesong = WAITPLAY;
    }
    break;

    case NOTEF:
    if(songTimer < 0) {
      ++noteCount;
      noToneTimer = 5;
      state_gamesong = NOTONE;
    }
    if(!(songTimer < 0)) {
      --songTimer;
      state_gamesong = NOTEF;
    }
    if(waveStart == false) {
      state_gamesong = WAITPLAY;
    }
    break;

    case NOTEG:
    if(songTimer < 0) {
      ++noteCount;
      noToneTimer = 5;
      state_gamesong = NOTONE;
    }
    if(!(songTimer < 0)) {
      --songTimer;
      state_gamesong = NOTEG;
    }
    if(waveStart == false) {
      state_gamesong = WAITPLAY;
    }
    break;

    case NOTONE:
    if(noToneTimer < 0 && noteCount == 2) {
      songTimer = 60;
      state_gamesong = NOTEC;
    }
    if(noToneTimer < 0 && noteCount == 3) {
      songTimer = 60;
      state_gamesong = NOTED;
    }
    if(noToneTimer < 0 && noteCount == 4) {
      songTimer = 60;
      state_gamesong = NOTED;
    }
    if(noToneTimer < 0 && noteCount == 5) {
      songTimer = 60;
      state_gamesong = NOTEC;
    }
    if(noToneTimer < 0 && noteCount == 6) {
      songTimer = 60;
      state_gamesong = NOTEC;
    }
    if(noToneTimer < 0 && noteCount == 7) {
      songTimer = 60;
      state_gamesong = NOTEE;
    }
    if(noToneTimer < 0 && noteCount == 8) {
      songTimer = 60;
      state_gamesong = NOTEF;
    }
    if(noToneTimer < 0 && noteCount == 9) {
      songTimer = 60;
      state_gamesong = NOTEE;
    }
    if(noToneTimer < 0 && noteCount == 10) {
      songTimer = 60;
      state_gamesong = NOTEF;
    }
    if(noToneTimer < 0 && noteCount == 11) {
      songTimer = 60;
      state_gamesong = NOTEG;
    }
    if(noToneTimer < 0 && noteCount == 12) {
      songTimer = 70;
      state_gamesong = NOTEE;
    }
    if(noToneTimer < 0 && noteCount == 13) {
      songTimer = 90;
      state_gamesong = NOTEC;
    }
    if(noToneTimer < 0 && noteCount == 14) {
      state_gamesong = WAITPLAY;
    }
    if(!(noToneTimer < 0)) {
      --noToneTimer;
      state_gamesong = NOTONE;
    }
    if(waveStart == false) {
      state_gamesong = WAITPLAY;
    }
    break;
  }

  switch ( state_gamesong ){
    case WAITPLAY:
    noTone(2);
    break;

    case NOTEC:
    tone(2, c);
    break;

    case NOTED:
    tone(2, d);
    break;

    case NOTEE:
    tone(2, e);
    break;

    case NOTEF:
    tone(2, f);
    break;

    case NOTEG:
    tone(2, g);
    break;

    case NOTONE:
    noTone(2);
    break;
  }
}

void setup() {
  //digits display
  pinMode(A5, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(2, OUTPUT);

  //button
  pinMode(A0, INPUT);
  pinMode(A3, INPUT);
  pinMode(swPin, INPUT);
  digitalWrite(swPin, HIGH);

  lcd.begin(16, 2);
  
  //buzzer

  tone(2, 1000, 2000);
  Serial.begin(9600);

  TimerSet(10);
  TimerOn();
}

void loop() {
  tick_gametimer();
  tick_gameloop();
  tick_ingameplayer();
  tick_printmaze();
  tick_gamesong();
  while(!TimerFlag){}
  TimerFlag = 0;  


  /*Serial.print("Starting order of notes\n");
  delay(1000);
  
  tone(8, c); 
  delay(500);
  
  tone(8, d); 
  delay(500);
  
  tone(8, e); 
  delay(500);
  
  tone(8, f);
  delay(500);
  
  tone(8, g);
  delay(500);
  
  tone(8, a);
  delay(500);
  
  tone(8, b);
  delay(500);
  
  tone(8, C);
  delay(500);
  
  noTone(8);
  Serial.print("END\n");*/
  //delay(1000);
  
  /*Serial.print("Starting song attempt\n");
  delay(1000);
  
  tone(2, c); //1
  delay(600);
  noTone(2);
  delay(50);
  
  tone(2, c); //2 
  delay(600);
  noTone(2);
  delay(50);
  
  tone(2, d); //3 
  delay(600);
  noTone(2);
  delay(50);
  
  tone(2, d); //4
  delay(600);
  noTone(2);
  delay(50);
  
  tone(2, c); //5
  delay(600);
  noTone(2);
  delay(50);
  
  tone(2, c); //6
  delay(600);
  noTone(8);
  delay(50);
  
  tone(2, e); //7
  delay(600);
  noTone(2);
  delay(50);
  
  tone(2, f); //8
  delay(600);
  noTone(2);
  delay(50);
  
  tone(2, e); //9
  delay(600);
  noTone(2);
  delay(50);
  
  tone(2, f); //10
  delay(600);
  noTone(2);
  delay(50);
  
  tone(2, g); //11
  delay(600);
  noTone(2);
  delay(50);
  
  tone(2, e); //12
  delay(700);
  noTone(2);
  delay(50);
  
  tone(2, c); //13
  delay(900);
  noTone(2);
  delay(50);
  
  delay(3000);
  noTone(8);
  Serial.print("END\n");
  delay(1000); */
}