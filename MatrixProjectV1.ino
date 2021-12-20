#include <LiquidCrystal.h>
#include "LedControl.h" //  need the library
#include <EEPROM.h>

const int dinPin = 13;
const int clockPin = 6;
const int loadPin = 9;

const byte RS = 12;
const byte E = 11;
const byte contrastPin = 10;

const byte D4 = 5;
const byte D5 = 4;
const byte D6 = 3;
const byte D7 = 2;

const byte pinX = A0;
const byte pinY = A1;
const byte pinSW = A2; 

int minThreshold = 100;
int maxThreshold = 900;

int contrast = 100;
int ledBrightness = 100;
int matrixBrightness = 5;

bool swState = LOW;
bool joyMove = false;

String state = "menu";
String prevState = "none";

byte nameState = 1;

byte clicked = 0;
byte clicked2 = 0;
byte left = 0;
byte right = 0;

String difficulty = "easy";
byte hearts = 5;
byte score = 0;

String mainQuestion = "9 + 10 = ?";
String answer = "1";
String options[4] = {"21", "19", "3", "0"};

String choice = "0";

uint16_t currentNameAddress = 25;
uint16_t startingHighscoreAddress = 45;

byte currentHSItem = 0; //int
byte previousHSItem = 1; //int

byte aboutPos = 0;
bool leftScroll = true;

String playerNames[] = {
  "None1",
  "None2",
  "None3"
};

uint8_t playerScores[] = {
  1,
  2,
  3
};

char playerName[16] = "Default";
uint8_t playerScore = 0;

byte lettersCursor = 0;
char letters[6] = "AAAAA";
char prevLetters[6] = "BBBBB";
bool changedLetter = true;

const uint64_t IMAGES[] = {
  0x0000000000000000,
  0x7e1818181c181800,
  0x7e060c3060663c00,
  0x3c66603860663c00,
  0x30307e3234383000
};

String menuItems[] = {
  "    NEW GAME",
  "  CHANGE NAME.",
  "   DIFFICULTY",
  "   HIGHSCORES",
  "    CONTRAST",
  "LED BIGHTNESS",
  "MATRIX BIGHTNESS",
  "ABOUT"
};

String questionItems[] = {
  "HEARTS",
  "QUESTION",
  "ANSWER 1",
  "ANSWER 2",
  "ANSWER 3",
  "ANSWER 4",
  "BACK"
};

// DO NOT CHANGE
template< typename T, size_t NumberOfSize > 
size_t MenuItemsSize(T (&) [NumberOfSize]){ return NumberOfSize; }
byte numberOfMenuItems = MenuItemsSize(menuItems) - 1;
byte currentMenuItem = 0;
byte previousMenuItem = 1;
byte button_flag = 0;
unsigned long previousMillis = millis();

// DO NOT CHANGE
template< typename T2, size_t NumberOfSize2 > 
size_t QuestionItemsSize(T2 (&) [NumberOfSize2]){ return NumberOfSize2; }
byte numberOfQuestionItems = QuestionItemsSize(questionItems) - 1; //int
byte currentQuestionItem = 0; //int
byte previousQuestionItem = 1; //int
byte button_flag2 = 0;
unsigned long previousMillis2 = millis();

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER

byte heart[8] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
};

// -----------

//const byte nrTopics = 2;
//String topics[] = { "Games", "Anime" };
//
//const byte nrGameQ = 3;
//String gameQuestions[][6] = {
//  {"Geralt is from?", "4", "Age of Empires", "Starcraft", "Styx", "Witcher"},
//  {"Spyro is a?", "1", "Dragon", "Imp", "Human", "Flower"},
//  {"Who made Warcraft?", "3", "Riot", "EA", "Blizzard", "Valve"}
//};
//
//const byte nrAnimeQ = 3;
//String animeQuestions[][6] = {
//  {"4th JoJo?", "1", "Josuke", "Giorno", "Speedwagon", "Koichi"},
//  {"Dio's last name?", "3", "Cujoh", "Joestar", "Brando", "Hirose"},
//  {"Naruto is in team?", "2", "Team 10", "Team 7", "Team 1", "Team 4"}
//};
//
//int generateTopicIndex() {
//  random(nrTopics);
//}

//int generateQuestionIndex(String topic) {
//  if(topic == "Games") {
//    return random(nrGameQ);
//  }
//  if(topic == "Anime") {
//    return random(nrAnimeQ);
//  }
//}

bool generateQuestion = true;

byte nrQuestions = 3;
String demoQuestions[][6] = {
  {"10 + 42 = ?", "1", "52", "32", "420", "0"},
  {"4 x 3 = ?", "2", "7", "12", "1", "-1"},
  {"99 / 3 = ?", "3", "69", "102", "33", "420"},
};

int generateQuestionIndex() {
  return random(nrQuestions);
}

void extractQuestion(int index) {
  mainQuestion = demoQuestions[index][0];
  answer = demoQuestions[index][1];

  for(int i = 0; i < 4; i++) {
    options[i] = demoQuestions[index][i+2];
  }
}

// -----------

void setup() {
  pinMode(contrastPin, OUTPUT);

  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(pinSW, INPUT_PULLUP);

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

  lcd.createChar(0, heart);

  randomSeed(analogRead(0));

  Serial.begin(9600);
}

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
  
  if(state == "menu") {

    if(menuItems[currentMenuItem] == "ABOUT") {
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
      if(currentMenuItem > numberOfMenuItems )
      {
        currentMenuItem = numberOfMenuItems;
      }
      button_flag = 1;
      previousMillis = millis();
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
      lcd.print(menuItems[currentMenuItem]);
      
      Serial.println(menuItems[currentMenuItem]);

      if(menuItems[currentMenuItem] == "ABOUT") {
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
  if(state == "name") {
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
            state = "menu";
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
          changedLetter = true;
        }
      }
    }

    showName();
    showBack();
  }
  if(state == "highscore") {
    if(valueY >= maxThreshold && button_flag == 0)
    {
      Serial.println("++");
      ++currentHSItem;
      if(currentHSItem > 3)
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
        state = "menu";
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
      else {
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
  if(state == "gameInit") {
    if(difficulty == "easy") {
      hearts = 5;
    }
    if(difficulty == "hard") {
      hearts = 3;
    }
    state = "question";
    clicked = 0;
  }
  if(state == "map") {
    //
  }
  if(state == "preroom") {
    if(minThreshold <= valueY && valueY <= maxThreshold && button_flag == 0) {
      if(valueSW == HIGH) {
        state = "room";
        currentQuestionItem = 0;
        previousQuestionItem = -1;
        button_flag = 1;
        button_flag2 = 1;
        clicked = 0;
        clicked2 = 0;
      }
    }
    if(millis() - previousMillis >= 400) 
    {
      previousMillis = millis();
      button_flag = 0;
    }
  }
  if(state == "room") {
    //Serial.println(choice);
    if(valueX >= maxThreshold && button_flag == 0)
    {
      choice = "4";
      displayImage(IMAGES[4]);
      button_flag = 1;
      previousMillis2 = millis();
    }
    if(valueX <= minThreshold && button_flag == 0)
    {
      choice = "2";
      displayImage(IMAGES[2]);
      button_flag = 1;
      previousMillis2 = millis();
    }
    if(valueY >= maxThreshold && button_flag == 0)
    {
      choice = "3";
      displayImage(IMAGES[3]);
      button_flag2 = 1;
      previousMillis2 = millis();
    }
    else if(valueY <= minThreshold && button_flag == 0)
    {
      choice = "1";
      button_flag = 1;
      displayImage(IMAGES[1]);
      previousMillis2 = millis();
    }
    if(minThreshold <= valueY && valueY <= maxThreshold && button_flag == 0) {
      if(valueSW == HIGH) {
        state = "question";
        currentQuestionItem = 0;
        previousQuestionItem = -1;
        button_flag = 1;
        button_flag2 = 1;
        clicked = 0;
        clicked2 = 0;
        
        displayImage(IMAGES[0]);
        
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
  if(state == "question") {
    if(hearts == 0) {
      state = "gameOver";
    }
    if(generateQuestion == true) {
      int index = generateQuestionIndex();
      extractQuestion(index);
      generateQuestion = false;
    }
    if(valueX >= maxThreshold && button_flag2 == 0)
    {
      questionFunctions(currentQuestionItem + 1, clicked2);
      button_flag2 = 1;
      previousMillis2 = millis();
    }
    if(valueX <= minThreshold && button_flag2 == 0)
    {
      questionFunctions(currentQuestionItem + 1, clicked2);
      button_flag2 = 1;
      previousMillis2 = millis();
    }
    if(valueY >= maxThreshold && button_flag2 == 0)
    {
      ++currentQuestionItem;
      if(currentQuestionItem > numberOfQuestionItems )
      {
        currentQuestionItem = numberOfQuestionItems;
      }
      button_flag2 = 1;
      previousMillis2 = millis();
    }
    else if(valueY <= minThreshold && button_flag2 == 0)
    {
      if(currentQuestionItem != 0) {
        currentQuestionItem--;
      }
      if(currentQuestionItem < 0)
      {
        currentQuestionItem = 0;
      }
      button_flag2 = 1;
      previousMillis2 = millis();
    }
    if(minThreshold <= valueY && valueY <= maxThreshold && button_flag2 == 0) {
      if(valueSW == HIGH) {
        clicked2 = 1;
        button_flag2 = 1;
        button_flag = 1;
        questionFunctions(currentQuestionItem + 1, clicked2);
        clicked2 = 0;
      }
    }
    if(currentQuestionItem != previousQuestionItem)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(questionItems [currentQuestionItem]);
      questionFunctions(currentQuestionItem + 1, clicked2);
      clicked2 = 0;
      previousQuestionItem = currentQuestionItem;
    }
    if(millis() - previousMillis2 >= 400) 
    {
      previousMillis2 = millis();
      button_flag2 = 0;
    }
  }
  if(state == "gameOver") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Game over, ");
    lcd.print(letters);
    lcd.setCursor(0, 3);
    lcd.print("Your score is ");
    lcd.print(score);
    delay(7000);

    state = "menu";
    lcd.clear();
    
    currentMenuItem = 0;
    previousMenuItem = -1;

    bool beaten = beatenAnyHighscores();

    if(beaten == true) {
      state = "newHighscore";
      putHighscore(letters, score);
    }
  }
  if(state == "newHighscore") {
    lcd.setCursor(0, 0);
    lcd.print("New highscore!!");
    lcd.setCursor(0, 3);
    lcd.print(letters);
    lcd.print(" ");
    lcd.print(score);

    delay(2000);

    if(minThreshold <= valueY && valueY <= maxThreshold && button_flag2 == 0) {
      if(valueSW == HIGH) {
        clicked2 = 1;
        button_flag2 = 1;
        button_flag = 1;
        state = "menu";
        clicked2 = 0;
      }
    }
    if(millis() - previousMillis2 >= 400) 
    {
      previousMillis2 = millis();
      button_flag2 = 0;
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
        state = "gameInit";
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
        state = "name";
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
        difficulty = "hard";
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
        difficulty = "easy";
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
        getHighscores();
        state = "highscore";
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
      contrast -= 10;
    }
    if(left == 1)
    {
      lcd.setCursor(0, 3);
      lcd.print("+++ <<");
      lcd.setCursor(10, 3);
      lcd.print("--    ");
      contrast += 10;
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
  if(menu == 8) // About
  {
    
    scrollAbout();
    
  }
  // and so on... 
}

void questionFunctions(int menu, byte clicked)  // Your menu functions
{
  if(menu == 1) // Hearts
  {
    lcd.setCursor(0, 3);
    for(int i = 0; i<hearts; i++) {
      lcd.write(byte(0));
    }
  }
  if(menu == 2) // Question
  {
    lcd.setCursor(0, 3);
    lcd.print(mainQuestion);
  }
  if(menu == 3) // 1
  {
    lcd.setCursor(0, 3);
    lcd.print(options[0]);
  }
  if(menu == 4) // 2
  {
    lcd.setCursor(0, 3);
    lcd.print(options[1]);
  }
  if(menu == 5) // 3
  {
    lcd.setCursor(0, 3);
    lcd.print(options[2]);
  }
  if(menu == 6) // 4
  {
    lcd.setCursor(0, 3);
    lcd.print(options[3]);
  }
  if(menu == 7) // Back
  {
    lcd.setCursor(0, 3);
    lcd.print("Click to go back");
    
    if(clicked == 1)
    {
      state = "preroom";
      clicked = 0;
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

void printHighscores() {
  getHighscores();

  for(int i=0; i<3; i++) {
    Serial.print(i+1);
    Serial.print(". ");
    Serial.print(playerNames[i]);
    Serial.print(": ");
    Serial.println(playerScores[i]);
  }
  Serial.println("=======");
}

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

void checkAnswer() {
  if(choice == answer) {
    Serial.println("Correct");
    score++;
  }
  else {
    Serial.println("Incorrect");
    hearts--;
  }
}

bool beatenAnyHighscores() {
  getHighscores();

  for(int i = 0; i < 3; i++) {
    if(score == playerScores[i]) return false;
    if(score > playerScores[i]){
      return true;
    }
  }

  return false;
}

void showHighscore(int pos) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(playerNames[pos]);
  lcd.setCursor(0,1);
  lcd.print(playerScores[pos]);
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

void getHighscores() {
  uint16_t address = startingHighscoreAddress;

  for(int i = 0; i < 3; i++) {
    char plrName[16] = "None";
    uint8_t plrScore = 0;
    readPlayer(address, plrName, &plrScore);
    playerNames[i] = String(plrName);
    playerScores[i] = plrScore;

    address += 36;
  }
}

void putHighscore(char *plrName, uint8_t plrScore) {
  uint16_t address = startingHighscoreAddress;
  getHighscores();

  Serial.println("IN PUT");
  printHighscores();
  
  for(int i = 0; i < 3; i++) {
    if(plrScore == playerScores[i]) return;
    if(plrScore > playerScores[i]){
      playerScores[i] = plrScore;
      playerNames[i] = plrName;

      writePlayer(address, plrName, plrScore);
      return;
    }
    else {
      address += 36;
    }
  }
}

void resetHighscore() {
  uint16_t address = startingHighscoreAddress;
  char resetName[16] = "None";
  uint8_t resetScore = 0;
  
  for(int i = 0; i < 3; i++) {
    writePlayer(address, resetName, resetScore);
    address += 36;
  }
}
