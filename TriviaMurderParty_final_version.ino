#include <LiquidCrystal.h>
#include "LedControl.h" //  need the library
#include <EEPROM.h>

#include <SPI.h>
#include <SD.h>


const byte dinPin = A3; // 13 -> A3 -> 8 -> 11
const byte clockPin = 6;
const byte loadPin = 9;

const byte RS = A4; // 12 -> A4
const byte E = A5; // 11 -> A5
const byte contrastPin = 10;

const byte D4 = 5;
const byte D5 = 7; // 4
const byte D6 = 3;
const byte D7 = 2;

const byte pinX = A0;
const byte pinY = A1;
const byte pinSW = A2; 

const byte pinBuzz = 8;


byte minThreshold = 100;
short maxThreshold = 900;

byte contrast = 100;
byte ledBrightness = 100;
byte matrixBrightness = 5;

bool swState = LOW;
bool joyMove = false;

bool sound = true;

byte state = 0;
byte prevState = 255;

byte nameState = 1;

byte clicked = 0;
byte left = 0;
byte right = 0;

bool difficulty = 0;
byte hearts = 5;
byte score = 0;

byte topicIndex;
byte questionIndex;

byte choice = 0;

String choiceS() {
  return String(choice);
}

byte currentHSItem = 0; //int
byte previousHSItem = 1; //int

byte aboutPos = 0;
bool leftScroll = true;

byte lettersCursor = 0;
char letters[6] = "AAAAA";

String menuItems(byte index) {
  switch(index) {
    case 0:
      return "NEW GAME";
    case 1:
      return "CHANGE NAME";
    case 2:
      return "DIFFICULTY";
    case 3:
      return "HIGHSCORE";
    case 4:
      return "CONTRAST";
    case 5:
      return "LED BRIGHTNESS";
    case 6:
      return "MTX BRIGHTNESS";
    case 7:
      return "SOUND";
    case 8:
      return "ABOUT";
  }
}


String questionItems(byte index) {
  switch(index) {
    case 0:
      return "HP:";
    case 1:
      return "QUESTION";
    case 2:
      return "ANSWER 1";
    case 3:
      return "ANSWER 2";
    case 4:
      return "ANSWER 3";
    case 5:
      return "ANSWER 4";
    case 6:
      return "BACK";
  }
}

// DO NOT CHANGE
byte currentMenuItem = 0;
byte previousMenuItem = 1;
byte button_flag = 0;
unsigned long previousMillis = millis();
byte currentQuestionItem = 0; //int
byte previousQuestionItem = 1; //int

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER

bool generateQuestion = true;

String topics(byte index) {
  switch(index) {
    case 0:
      return "anime";
    case 1:
      return "gaming";
    case 2:
      return "geo";
    case 3:
      return "music";
    case 4:
      return "cult";
  }
}

const uint64_t ICONS[] = {
  0x001e3c78783c1e00, // New Game
  0x7e7e183c7e7e7e3c, // Change Name
  0x3e0c1c3e7fff0000, // Difficulty
  0x3c183c7ebdbd7e3c, // Highscores
  0x182c4e8f9f5e3c18, // Contrast 
  0x001e0c0c1e00120c, // Led Brightness
  0x3f212d2d2d2d2d2d, // Matrix Brightness
  0x066eecc88898f000, // Sound
  0x080008081022221c // About 
};

unsigned long newrandom(unsigned long howsmall, unsigned long howbig)
{
  return howsmall + random() % (howbig - howsmall);
}

String getTopic(bool x, byte pos) {
  if(x == 0) {
    return topics(newrandom(0, 5));
  }
  else {
    return topics(pos);
  }
}

byte generateQuestionIndex(String topic) {
  if(topic == "gaming") {
    return newrandom(0, 3);
  }
  if(topic == "anime") {
    return newrandom(0, 3);
  }
  if(topic == "geo") {
    return newrandom(0, 3);
  }
  if(topic == "music") {
    return newrandom(0, 3);
  }
  if(topic == "cult") {
    return newrandom(0, 3);
  }
}

void generateQuestionData() {
  topicIndex = newrandom(0, 5);
  questionIndex = newrandom(0, 3);
}

/* 
 Returns the question as a String; First, it builds the path of the file containing the question,
 then it tries opening the file at that path: if it succedes, it returns the question, if it doesn't,
 it returns "Error".
 */
String getQuestion() {
  File myFile;
  char chr;
  String fullPath = "x";

  Serial.print("fullPath: ");
  Serial.println(fullPath);
  
  if(difficulty == 0) {
    fullPath = "easy/";
  }
  else {
    fullPath = "hard/";
  }
  Serial.println(fullPath);
  fullPath.concat(getTopic(1, topicIndex));
  Serial.println(fullPath);
  fullPath.concat("/");
  Serial.println(fullPath);
  fullPath.concat(String(questionIndex));
  Serial.println(fullPath);
  fullPath.concat(".txt");
  Serial.println(fullPath);

//  fullPath = "easy/anime/2.txt";

  myFile = SD.open(fullPath);
  if (myFile) {
    Serial.print("opened: ");
    Serial.println(fullPath);
    fullPath = "";
  
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      chr = myFile.read();
      if(chr != '/') {
         fullPath.concat(chr);
      }
      else {
        Serial.print("Block: ");
        Serial.println(fullPath);
        myFile.close();
        return fullPath;
        fullPath = "";
      }
    }
    Serial.println();
  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening: ");
    Serial.println(fullPath);
    return "Error";
  }
}

/* 
 Returns the index of the correct answer as a String; First, it builds the path of the file containing the solution,
 then it tries opening the file at that path: if it succedes, it returns the correct answer's index, if it doesn't,
 it returns "Error".
 */
String getSolution() {
  File myFile;
  char chr;
  byte cnt = 0;
  String fullPath = "x";

  Serial.print("fullPath: ");
  Serial.println(fullPath);
  
  if(difficulty == 0) {
    fullPath = "easy/";
  }
  else {
    fullPath = "hard/";
  }
  Serial.println(fullPath);
  fullPath.concat(getTopic(1, topicIndex));
  Serial.println(fullPath);
  fullPath.concat("/");
  Serial.println(fullPath);
  fullPath.concat(questionIndex);
  Serial.println(fullPath);
  fullPath.concat(".txt");
  Serial.println(fullPath);

  myFile = SD.open(fullPath);
  if (myFile) {
    Serial.print("opened: ");
    Serial.println(fullPath);
    fullPath = "";
  
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      chr = myFile.read();
      if(chr != '/') {
         fullPath.concat(chr);
      }
      else {
        if(cnt == 1) {
          Serial.print("Block: ");
          Serial.println(fullPath);
          myFile.close();
          return fullPath;
        }
        cnt++;
        fullPath = "";
      }
    }
    Serial.println();
  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening: ");
    Serial.println(fullPath);
  }
}

/* 
 Returns the the first possible answer as a String; First, it builds the path of the file containing the option,
 then it tries opening the file at that path: if it succedes, it returns the option, if it doesn't,
 it returns "Error".
 */
String getAnswer1() {
  File myFile;
  char chr;
  byte cnt = 0;
  String fullPath = "x";

  Serial.print("fullPath: ");
  Serial.println(fullPath);
  
  if(difficulty == 0) {
    fullPath = "easy/";
  }
  else {
    fullPath = "hard/";
  }
  Serial.println(fullPath);
  fullPath.concat(getTopic(1, topicIndex));
  Serial.println(fullPath);
  fullPath.concat("/");
  Serial.println(fullPath);
  fullPath.concat(questionIndex);
  Serial.println(fullPath);
  fullPath.concat(".txt");
  Serial.println(fullPath);

  myFile = SD.open(fullPath);
  if (myFile) {
    Serial.print("opened: ");
    Serial.println(fullPath);
    fullPath = "";
  
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      chr = myFile.read();
      if(chr != '/') {
         fullPath.concat(chr);
      }
      else {
        if(cnt == 2) {
          Serial.print("Block: ");
          Serial.println(fullPath);
          myFile.close();
          return fullPath;
        }
        cnt++;
        fullPath = "";
      }
    }
    Serial.println();
  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening: ");
    Serial.println(fullPath);
  }
}

/* 
 Returns the the second possible answer as a String; First, it builds the path of the file containing the option,
 then it tries opening the file at that path: if it succedes, it returns the option, if it doesn't,
 it returns "Error".
 */
String getAnswer2() {
  File myFile;
  char chr;
  byte cnt = 0;
  String fullPath = "x";

  Serial.print("fullPath: ");
  Serial.println(fullPath);
  
  if(difficulty == 0) {
    fullPath = "easy/";
  }
  else {
    fullPath = "hard/";
  }
  Serial.println(fullPath);
  fullPath.concat(getTopic(1, topicIndex));
  Serial.println(fullPath);
  fullPath.concat("/");
  Serial.println(fullPath);
  fullPath.concat(questionIndex);
  Serial.println(fullPath);
  fullPath.concat(".txt");
  Serial.println(fullPath);

  myFile = SD.open(fullPath);
  if (myFile) {
    Serial.print("opened: ");
    Serial.println(fullPath);
    fullPath = "";
  
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      chr = myFile.read();
      if(chr != '/') {
         fullPath.concat(chr);
      }
      else {
        if(cnt == 3) {
          Serial.print("Block: ");
          Serial.println(fullPath);
          myFile.close();
          return fullPath;
        }
        cnt++;
        fullPath = "";
      }
    }
    Serial.println();
  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening: ");
    Serial.println(fullPath);
  }
}

/* 
 Returns the the third possible answer as a String; First, it builds the path of the file containing the option,
 then it tries opening the file at that path: if it succedes, it returns the option, if it doesn't,
 it returns "Error".
 */
String getAnswer3() {
  File myFile;
  char chr;
  byte cnt = 0;
  String fullPath = "x";

  Serial.print("fullPath: ");
  Serial.println(fullPath);
  
  if(difficulty == 0) {
    fullPath = "easy/";
  }
  else {
    fullPath = "hard/";
  }
  Serial.println(fullPath);
  fullPath.concat(getTopic(1, topicIndex));
  Serial.println(fullPath);
  fullPath.concat("/");
  Serial.println(fullPath);
  fullPath.concat(questionIndex);
  Serial.println(fullPath);
  fullPath.concat(".txt");
  Serial.println(fullPath);

  myFile = SD.open(fullPath);
  if (myFile) {
    Serial.print("opened: ");
    Serial.println(fullPath);
    fullPath = "";
  
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      chr = myFile.read();
      if(chr != '/') {
         fullPath.concat(chr);
      }
      else {
        if(cnt == 4) {
          Serial.print("Block: ");
          Serial.println(fullPath);
          myFile.close();
          return fullPath;
        }
        cnt++;
        fullPath = "";
      }
    }
    Serial.println();
  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening: ");
    Serial.println(fullPath);
  }
}

/* 
 Returns the the fourth possible answer as a String; First, it builds the path of the file containing the option,
 then it tries opening the file at that path: if it succedes, it returns the option, if it doesn't,
 it returns "Error".
 */
String getAnswer4() {
  File myFile;
  char chr;
  byte cnt = 0;
  String fullPath = "x";

  Serial.print("fullPath: ");
  Serial.println(fullPath);
  
  if(difficulty == 0) {
    fullPath = "easy/";
  }
  else {
    fullPath = "hard/";
  }
  Serial.println(fullPath);
  fullPath.concat(getTopic(1, topicIndex));
  Serial.println(fullPath);
  fullPath.concat("/");
  Serial.println(fullPath);
  fullPath.concat(questionIndex);
  Serial.println(fullPath);
  fullPath.concat(".txt");
  Serial.println(fullPath);

  myFile = SD.open(fullPath);
  if (myFile) {
    Serial.print("opened: ");
    Serial.println(fullPath);
    fullPath = "";
  
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      chr = myFile.read();
      if(chr != '/') {
         fullPath.concat(chr);
      }
      else {
        if(cnt == 5) {
          Serial.print("Block: ");
          Serial.println(fullPath);
          myFile.close();
          return fullPath;
        }
        cnt++;
        fullPath = "";
      }
    }
    Serial.println();
  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening: ");
    Serial.println(fullPath);
  }
}

// -----------

void setup() {
  pinMode(contrastPin, OUTPUT);

  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(pinSW, INPUT_PULLUP);

  pinMode(53, OUTPUT);
  
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen

  analogWrite(contrastPin, contrast);
  
  lcd.begin(16, 2);  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Trivia");
  lcd.setCursor(3, 1);
  lcd.print("Murder Party");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("by Burta");
  lcd.setCursor(3, 1);
  lcd.print("Mihai-Catalin");
  delay(3000);
  lcd.clear();

  byte heart[8] = {
    B00000,
    B01010,
    B11111,
    B11111,
    B01110,
    B00100,
    B00000,
  };

  byte arrows[8] = {
    B00100,
    B01110,
    B11111,
    B00000,
    B11111,
    B01110,
    B00100,
  };

  lcd.createChar(0, heart);
  lcd.createChar(1, arrows);

  randomSeed(analogRead(0));

  displayImage(0x0000000000000000);

  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
}

/*
  state:
  0 = menu
  1 = name
  2 = highscore
  3 = gameInit
  4 = map
  5 = preroom
  6 = room
  7 = question
  8 = gameOver
  9 = newHighscore
*/


/*
  The menu and main engine of the game is build using states. Each state has an id, as seen above. 
 In each state, we check the movement of the joystick and change the states accordingly. State 0 is the menu itself,
 state 1 is in charge of the name changing and storing, state 2 is in charge of the highscore storing and displaying,
 state 3 initializes a few variables, state 4 is unused, state 5 is not in use anymore, but was used to fix a previous bug,
 state 6 is in charge of selecting the answer, state 7 is in charge of displaying HP, score and details about the current question,
 state 8 is in charge of the game over screen, and finally state 9 is in charge of displaying and storing a newly obtained highscore.

 (Convention: "from state X you can go to state Y = X -> Y)
 0 -> 1, 2, 3
 1 -> 0
 2 -> 0
 3 -> 7
 4 - unused
 5 - unused
 6 -> 7, 8
 7 -> 6
 8 -> 9, 0
 9 -> 0

  Each item of the menu has a unique icon, shown on the 8x8 LED Matrix.
 */
void loop() {
  int valueX = analogRead(pinX);
  int valueY = analogRead(pinY);
  int valueSW = !digitalRead(pinSW);

  int ok = 1;
  for(int i = 1; i <= 16; i++) {
    if(valueSW != !digitalRead(pinSW)) ok = 0;
  }

  if(ok == 0) valueSW = LOW;

  analogWrite(contrastPin, contrast);

  if(state != prevState){
    Serial.println(state);
    prevState = state;
  }
  
  if(state == 0) { // menu

    if(currentMenuItem == 8) {
      scrollAbout();
    }
    
    if(valueX >= maxThreshold && button_flag == 0)
    {
      menuFunctions(currentMenuItem + 1, 1, 0, clicked);
      button_flag = 1;
      right = 1;
      left = 0;
      previousMillis = millis();
    }
    if(valueX <= minThreshold && button_flag == 0)
    {
      menuFunctions(currentMenuItem + 1, 0, 1, clicked);
      button_flag = 1;
      right = 0;
      left = 1;
      previousMillis = millis();
    }
    if(valueY >= maxThreshold && button_flag == 0)
    {
      ++currentMenuItem;
      if(currentMenuItem > 8 )
      {
        currentMenuItem = 8;
      }
      button_flag = 1;
      previousMillis = millis();

      if(sound == true) {
        menuDing();
      }
    }
    else if(valueY <= minThreshold && button_flag == 0)
    {
      if(currentMenuItem != 0) {
        currentMenuItem--;
      }
      if(currentMenuItem < 0)
      {
        currentMenuItem = 0;
      }
      button_flag = 1;
      previousMillis = millis();

      if(sound == true) {
        menuDing();
      }
    }
    if(minThreshold <= valueY && valueY <= maxThreshold) {
      if(valueSW == HIGH) {
        clicked = 1;
        Serial.println("AM FACUT CLICKED = 1");
        menuFunctions(currentMenuItem + 1, right, left, clicked);
        left = 0;
        right = 0;
        clicked = 0;
      }
    }
    if(currentMenuItem != previousMenuItem)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.write(byte(1));
      lcd.print(menuItems(currentMenuItem));
      
      Serial.println(menuItems(currentMenuItem));

      displayImage(ICONS[currentMenuItem]);

      if(currentMenuItem == 8) {
        lcd.setCursor(0, 3);
        lcd.print("https://github.com/cbrt-mihai");
        aboutPos = 0;
      }
      
      menuFunctions(currentMenuItem + 1, 0, 0, clicked);
      left = 0;
      right = 0;
      clicked = 0;
      previousMenuItem = currentMenuItem;
    }
    if(millis() - previousMillis >= 400) 
    {
      previousMillis = millis();
      button_flag = 0;
    }
  }
  
  if(state == 1) { // name
    if (nameState == 0) { // selecting the  display
      if(valueX <= minThreshold && joyMove == false) {
        lettersCursor--;
        joyMove = true;

        lcd.setCursor(lettersCursor + 1, 3);
        lcd.write(" ");
        lcd.setCursor(lettersCursor, 3);
        lcd.write("^");
  
        if(lettersCursor < 0) {
          lettersCursor = 0;
        }
      }
  
      if(valueX >= maxThreshold && joyMove == false) {
        lettersCursor++;
        joyMove = true;

        lcd.setCursor(lettersCursor - 1, 3);
        lcd.write(" ");
        lcd.setCursor(lettersCursor, 3);
        lcd.write("^");
  
        if(lettersCursor > 13) {
          lettersCursor = 13;
        }
      }
  
      if(minThreshold <= valueX && valueX <= maxThreshold) {
        joyMove = false;
        if(valueSW == HIGH) {
          if(lettersCursor > 8) {
            state = 0;
            lettersCursor = 0;
            lcd.clear();
          }
          nameState = 1 - nameState;
        }
      }
    }
    else { // selecting the digit
      if(valueY <= minThreshold && joyMove == false) {
        if(0 <= lettersCursor && lettersCursor <= 4) { 
          letters[lettersCursor]++;
        }
        joyMove = true;
  
        if(letters[lettersCursor] < 65) {
          letters[lettersCursor] = 90;
        }
      }
  
      if(valueY >= maxThreshold && joyMove == false) {
        if(0 <= lettersCursor && lettersCursor <= 4) { 
          letters[lettersCursor]--;
        }
        joyMove = true;
  
        if(letters[lettersCursor] > 90) {
          letters[lettersCursor] = 65;
        }
      }
  
      if(minThreshold <= valueY && valueY <= maxThreshold) {
        joyMove = false;
        if(valueSW == HIGH) {
          nameState = 1 - nameState;
        }
      }
    }

    showName();
    showBack();
  }
  
  if(state == 2) { // highscore
    if(valueY >= maxThreshold && button_flag == 0)
    {
      Serial.println("++");
      ++currentHSItem;
      if(currentHSItem > 4)
      {
        currentHSItem = currentHSItem - 1;
      }
      button_flag = 1;
      previousMillis = millis();
    }
    else if(valueY <= minThreshold && button_flag == 0)
    {
      Serial.println("--");
      if(currentHSItem != 0) {
        currentHSItem--;
      }
      if(currentHSItem < 0)
      {
        currentHSItem = 0;
      }
      button_flag = 1;
      previousMillis = millis();
    }
    if(minThreshold <= valueY && valueY <= maxThreshold) {
      if(valueSW == HIGH && currentHSItem == 3) {
        clicked = 1;
        state = 0;
        currentMenuItem = 0;
        currentHSItem = 0;
        previousHSItem = 1;
        resetHighscore();
        menuFunctions(currentMenuItem + 1, right, left, clicked);
        left = 0;
        right = 0;
        clicked = 0;
      }
      if(valueSW == HIGH && currentHSItem == 4) {
        clicked = 1;
        state = 0;
        currentMenuItem = 0;
        currentHSItem = 0;
        previousHSItem = 1;
        menuFunctions(currentMenuItem + 1, right, left, clicked);
        left = 0;
        right = 0;
        clicked = 0;
      }
    }
    if(currentHSItem != previousHSItem)
    {
      if(currentHSItem < 3) {
        showHighscore(currentHSItem);
      }
      if(currentHSItem == 3) {
        lcd.clear();
        lcd.print("CLEAR SCORES");
      }
      if(currentHSItem == 4) {
        lcd.clear();
        lcd.print("BACK");
      }
      previousHSItem = currentHSItem;
    }
    if(millis() - previousMillis >= 400) 
    {
      previousMillis = millis();
      button_flag = 0;
    }
  }
  
  if(state == 3) { // gameInit
    if(difficulty == 0) {
      hearts = 5;
    }
    if(difficulty == 1) {
      hearts = 3;
    }
    state = 7;
    clicked = 0;
  }
  
  if(state == 4) { // map
    //ceva
  }
  
  if(state == 5) { // preroom
    if(minThreshold <= valueY && valueY <= maxThreshold && button_flag == 0) {
      if(valueSW == HIGH) {
        state = 6;
        clicked = 1;
        currentQuestionItem = 0;
        previousQuestionItem = -1;
        button_flag = 1;
        clicked = 0;
      }
    }
    if(millis() - previousMillis >= 400) 
    {
      previousMillis = millis();
      button_flag = 0;
    }
  }
  
  if(state == 6) { // room
    //Serial.println(choice);
    if(valueX >= maxThreshold && button_flag == 0)
    {
      choice = 4;
      displayImage(0x30307e3234383000);
      button_flag = 1;
      previousMillis = millis();
    }
    if(valueX <= minThreshold && button_flag == 0)
    {
      choice = 2;
      displayImage(0x7e060c3060663c00);
      button_flag = 1;
      previousMillis = millis();
    }
    if(valueY >= maxThreshold && button_flag == 0)
    {
      choice = 3;
      displayImage(0x3c66603860663c00);
      button_flag = 1;
      previousMillis = millis();
    }
    else if(valueY <= minThreshold && button_flag == 0)
    {
      choice = 4;
      button_flag = 1;
      displayImage(0x7e1818181c181800);
      previousMillis = millis();
    }
    if(minThreshold <= valueY && valueY <= maxThreshold && button_flag == 0) {
      if(valueSW == HIGH) {
        state = 7;
        clicked = 1;
        currentQuestionItem = 0;
        previousQuestionItem = -1;
        button_flag = 1;
        clicked = 0;
        
        displayImage(0x0000000000000000);
        
        checkAnswer();
        generateQuestion = true;
      }
    }
    if(millis() - previousMillis >= 400) 
    {
      previousMillis = millis();
      button_flag = 0;
    }
  }
  
  if(state == 7) { // question 
    if(hearts == 0) {
      state = 8;
    }
    if(generateQuestion == true) {
      generateQuestionData();
      generateQuestion = false;
    }
    if(valueX >= maxThreshold && button_flag == 0)
    {
      questionFunctions(currentQuestionItem + 1, clicked);
      button_flag = 1;
      previousMillis = millis();
    }
    if(valueX <= minThreshold && button_flag == 0)
    {
      questionFunctions(currentQuestionItem + 1, clicked);
      button_flag = 1;
      previousMillis = millis();
    }
    if(valueY >= maxThreshold && button_flag == 0)
    {
      ++currentQuestionItem;
      if(currentQuestionItem > 6)
      {
        currentQuestionItem = 6;
      }
      button_flag = 1;
      previousMillis = millis();
    }
    else if(valueY <= minThreshold && button_flag == 0)
    {
      if(currentQuestionItem != 0) {
        currentQuestionItem--;
      }
      if(currentQuestionItem < 0)
      {
        currentQuestionItem = 0;
      }
      button_flag = 1;
      previousMillis = millis();
    }
    if(minThreshold <= valueY && valueY <= maxThreshold && button_flag == 0) {
      if(currentQuestionItem == 1) {
        scrollAbout();
      }
      else {
        aboutPos = 0;
      }
      if(valueSW == HIGH) {
        clicked = 1;
        button_flag = 1;
        questionFunctions(currentQuestionItem + 1, clicked);
        clicked = 0;
      }
    }
    if(currentQuestionItem != previousQuestionItem)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(questionItems (currentQuestionItem));
      questionFunctions(currentQuestionItem + 1, clicked);
      clicked = 0;
      previousQuestionItem = currentQuestionItem;
    }
    if(millis() - previousMillis >= 400) 
    {
      previousMillis = millis();
      button_flag = 0;
    }
  }
  
  if(state == 8) { //gameOver
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Game over, ");
    lcd.print(letters);
    lcd.setCursor(0, 3);
    lcd.print("Your score is ");
    lcd.print(score);
    delay(7000);

    state = 0;
    lcd.clear();
    
    currentMenuItem = 0;
    previousMenuItem = -1;

    bool beaten = beatenAnyHighscores();

    if(beaten == true) {
      state = 9;
      button_flag = 0;
      clicked = 0;
      putHighscore(letters, score);
    }
  }
  
  if(state == 9) { // newHighscore
    lcd.setCursor(0, 0);
    lcd.print("New highscore!!");
    lcd.setCursor(0, 3);
    lcd.print(letters);
    lcd.print(" ");
    lcd.print(score);

    delay(2000);

    if(minThreshold <= valueY && valueY <= maxThreshold && button_flag == 0) {
      if(valueSW == HIGH) {
        clicked = 1;
        button_flag = 1;
        state = 0;
        clicked = 0;
      }
    }
    if(millis() - previousMillis >= 400) 
    {
      previousMillis = millis();
      button_flag = 0;
    }
  }
}

void menuFunctions(int menu, byte right, byte left, byte clicked)  // Your menu functions
{
  if(menu == 1) // Start a new game?
  {
    if(right == 1)
    {
      lcd.setCursor(0, 3);
      lcd.print("Yes   ");
      lcd.setCursor(10, 3);
      lcd.print("No  <<");

      Serial.println("SELECTAT NO NEW GAME");
    }
    if(left == 1)
    {
      lcd.setCursor(0, 3);
      lcd.print("Yes <<");
      lcd.setCursor(10, 3);
      lcd.print("No    ");

      Serial.println("SELECTAT YES NEW GAME");

      if(clicked == 1)
      {
        Serial.println("APASAT YES NEW GAME");
        state = 3;
        clicked = 0;
      }
    }
  }
  if(menu == 2) // Change name?
  {
    if(right == 1)
    {
      lcd.setCursor(0, 3);
      lcd.print("Yes   ");
      lcd.setCursor(10, 3);
      lcd.print("No  <<");
    }
    if(left == 1)
    {
      lcd.setCursor(0, 3);
      lcd.print("Yes <<");
      lcd.setCursor(10, 3);
      lcd.print("No    ");

      if(clicked == 1)
      {
        state = 1;
        lcd.clear();
        clicked = 0;
      }
    }
  }
  if(menu == 3) // Change difficulty?
  {
    if(right == 1)
    {
      lcd.setCursor(0, 3);
      lcd.print("Easy  ");
      lcd.setCursor(9, 3);
      lcd.print("Hard <<");

      if(clicked == 1)
      {
        difficulty = 1;
        clicked = 0;
      }
    }
    if(left == 1)
    {
      lcd.setCursor(0, 3);
      lcd.print("Easy <<");
      lcd.setCursor(10, 3);
      lcd.print("Hard    ");

      if(clicked == 1)
      {
        difficulty = 0;
        clicked = 0;
      }
    }
  }
  if(menu == 4) // Check highscores?
  {
    if(right == 1)
    {
      lcd.setCursor(0, 3);
      lcd.print("Yes   ");
      lcd.setCursor(10, 3);
      lcd.print("No  <<");
    }
    if(left == 1)
    {
      lcd.setCursor(0, 3);
      lcd.print("Yes <<");
      lcd.setCursor(10, 3);
      lcd.print("No    ");

      if(clicked == 1)
      {
        state = 2;
        clicked = 0;
      }
    }
  }
  if(menu == 5) // Contrast
  {
    if(right == 1)
    {
      lcd.setCursor(0, 3);
      lcd.print("+++   ");
      lcd.setCursor(10, 3);
      lcd.print("--  <<");
      contrast -= 5;
      if(contrast < 75) {
        contrast = 75;
      }
    }
    if(left == 1)
    {
      lcd.setCursor(0, 3);
      lcd.print("+++ <<");
      lcd.setCursor(10, 3);
      lcd.print("--    ");
      contrast += 5;
      if(contrast > 125) {
        contrast = 125;
      }
    }
  }
  if(menu == 6) // LED Brightness
  {
    if(right == 1)
    {
      lcd.setCursor(0, 3);
      lcd.print("+++   ");
      lcd.setCursor(10, 3);
      lcd.print("--  <<");
      ledBrightness -= 5;
    }
    if(left == 1)
    {
      lcd.setCursor(0, 3);
      lcd.print("+++ <<");
      lcd.setCursor(10, 3);
      lcd.print("--    ");
      ledBrightness += 5;
    }
  }
  if(menu == 7) // Matrix Brightness
  {
    if(right == 1)
    {
      lcd.setCursor(0, 3);
      lcd.print("+++   ");
      lcd.setCursor(10, 3);
      lcd.print("--  <<");
      if(matrixBrightness != 0) {
        matrixBrightness -= 1;
      }
    }
    if(left == 1)
    {
      lcd.setCursor(0, 3);
      lcd.print("+++ <<");
      lcd.setCursor(10, 3);
      lcd.print("--    ");
      if(matrixBrightness != 255) {
        matrixBrightness += 1;
      }
    }
  }
  if(menu == 8) { // Sound
    if(right == 1)
    {
      lcd.setCursor(0, 3);
      lcd.print("Yes   ");
      lcd.setCursor(10, 3);
      lcd.print("No  <<");

      if(clicked == 1)
      {
        sound = false;
      }
    }
    if(left == 1)
    {
      lcd.setCursor(0, 3);
      lcd.print("Yes <<");
      lcd.setCursor(10, 3);
      lcd.print("No    ");

      if(clicked == 1)
      {
        sound = true;
      }
    }
  }
  if(menu == 9) // About
  {
    
    scrollAbout();
    
  }
  // and so on... 
}

void questionFunctions(int menu, byte clicked)  // Your menu functions
{
  if(menu == 1) // Hearts
  {
    lcd.setCursor(4, 0);
    for(int i = 0; i<hearts; i++) {
      lcd.write(byte(0));
    }

    lcd.setCursor(0, 3);
    lcd.print(letters);
    lcd.print(": ");
    lcd.print(score);
  }
  if(menu == 2) // Question
  {
    lcd.setCursor(0, 3);
    lcd.print(getQuestion());
  }
  if(menu == 3) // 1
  {
    lcd.setCursor(0, 3);
    lcd.print(getAnswer1());
  }
  if(menu == 4) // 2
  {
    lcd.setCursor(0, 3);
    lcd.print(getAnswer2());
  }
  if(menu == 5) // 3
  {
    lcd.setCursor(0, 3);
    lcd.print(getAnswer3());
  }
  if(menu == 6) // 4
  {
    lcd.setCursor(0, 3);
    lcd.print(getAnswer4());
  }
  if(menu == 7) // Back
  {
    lcd.setCursor(0, 3);
    lcd.print("Click to go back");
    
    if(clicked == 1)
    {
      state = 6;
      clicked = 0;
      delay(25);
    }
  }
  // and so on... 
}

void writePlayer(uint16_t startingAddress, char *playerName, uint8_t playerScore)
{
   for (int i = startingAddress; i < startingAddress + 16; i++)
      EEPROM.update(i, playerName[i - startingAddress]);
   EEPROM.update(startingAddress + 18, playerScore);
}

void readPlayer(uint16_t startingAddress, char *playerName, uint8_t* playerScore)
{
  for (int i = startingAddress; i < startingAddress + 16; i++)
      playerName[i - startingAddress] = EEPROM.read(i);
  *playerScore = EEPROM.read(startingAddress + 18);
}

uint8_t printHighscores() {
  uint16_t address = 45;
  char otherName[16] = "None";
  uint8_t otherScore = 0;

  for(int i = 0; i < 3; i++) {
    readPlayer(address, otherName, &otherScore);
    Serial.print(i+1);
    Serial.print(". ");
    Serial.print(otherName);
    Serial.print(": ");
    Serial.println(otherScore);
    address += 36;
  }
  Serial.println("=======");
}

//void printHighscores() {
//  getHighscores();
//
//  for(int i=0; i<3; i++) {
//    Serial.print(i+1);
//    Serial.print(". ");
//    Serial.print(playerNames[i]);
//    Serial.print(": ");
//    Serial.println(playerScores[i]);
//  }
//  Serial.println("=======");
//}

void scrollAbout() {
  if(aboutPos < 20 && leftScroll == true) {
    lcd.scrollDisplayLeft();
    aboutPos++;
    delay(250);
  }
  else {
    leftScroll = false;
  }

  if(aboutPos > 0 && leftScroll == false) {
    lcd.scrollDisplayRight();
    aboutPos--;
    delay(250);
  }
  else {
    leftScroll = true;
  }
}

void menuDing() {
  tone(pinBuzz, 75, 100);
}

void correctAns() {
  tone(pinBuzz, 25, 200);
  delay(200);
  tone(pinBuzz, 45, 100);
}

void incorrectAns() {
  tone(pinBuzz, 50, 500);
}

void checkAnswer() {
  if(choiceS() == getSolution()) {
    Serial.println("Correct");
    score++;
    if(sound == true) {
      correctAns();
    }
  }
  else {
    Serial.println("Incorrect");
    hearts--;
    if(sound == true) {
      incorrectAns();
    }
  }
}

bool beatenAnyHighscores() {
  for(int i = 0; i < 3; i++) {
    if(score == getScore(i)) return false;
    if(score > getScore(i)){
      return true;
    }
  }

  return false;
}

void showHighscore(int pos) {
  char theName[16];
  lcd.clear();
  lcd.setCursor(0,0);
  getName(theName, pos);
  lcd.print(theName);
  Serial.print("HS Name: ");
  Serial.println(theName);
  lcd.setCursor(0,1);
  lcd.print(getScore(pos));
}

void showName() {
  lcd.setCursor(0,0);
  lcd.print(letters);
}

void showBack() {
  lcd.setCursor(9, 0);
  lcd.print("BACK");
}

void displayImage(uint64_t image) {
  for (int i = 0; i < 8; i++) {
    byte row = (image >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, bitRead(row, j));
    }
  }
}

//void getHighscores() {
//  uint16_t address = 45;
//
//  for(int i = 0; i < 3; i++) {
//    char plrName[16] = "None";
//    uint8_t plrScore = 0;
//    readPlayer(address, plrName, &plrScore);
//    playerNames[i] = String(plrName);
//    playerScores[i] = plrScore;
//
//    address += 36;
//  }
//}

uint8_t getScore(byte index) {
  uint16_t address = 45;
  char otherName[16] = "None";
  uint8_t otherScore = 0;

  for(int i = 0; i < 3; i++) {
    readPlayer(address, otherName, &otherScore);
    if(i == index) {
      return otherScore;
    }
    else {
      address += 36;
    }
  }
}

void getName(char *theName, byte index) {
  uint16_t address = 45;
  char otherName[16] = "None";
  uint8_t otherScore = 0;

  for(int i = 0; i < 3; i++) {
    readPlayer(address, otherName, &otherScore);
    if(i == index) {
      strcpy(theName, otherName);
    }
    else {
      address += 36;
    }
  }
}

void putHighscore(char *plrName, uint8_t plrScore) {
  uint16_t address = 45;

  Serial.println("IN PUT");
  printHighscores();
  
  for(int i = 0; i < 3; i++) {
    if(plrScore == getScore(i)) return;
    if(plrScore > getScore(i)){
      writePlayer(address, plrName, plrScore);
      return;
    }
    else {
      address += 36;
    }
  }
}

void resetHighscore() {
  uint16_t address = 45;
  char resetName[16] = "None";
  uint8_t resetScore = 0;
  
  for(int i = 0; i < 3; i++) {
    writePlayer(address, resetName, resetScore);
    address += 36;
  }
}
