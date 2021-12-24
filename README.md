# Trivia Murder Party, an Arduino trivia minigame
<p align="right" >  <i> author: Burța Mihai-Cătălin </i> </p>

## Architecture

  <p> The code is split up in many functions, depending on the functionality. The main function is the one in loop(), that is the main engine of the code. It's made up of several states that change depending on what was selected on the LCD. Now I will explain the functions: </p>
  <ul>
    <li> menuItems() and questionItems() hold the menu/question tabs. </li>
    <li> writePlayer() and readPlayer() communicate with the EEPROM.</li>
    <li> topics() returns the topic at a certain index. </li>
    <li> newrandom() generates a random number in a given interval. </li>
    <li> generateQuestionIndex() generates a random index. </li>
    <li> generateQuestionData() generates a topicIndex and a questionIndex. </li>
    <li> getQuestion(), getSolution(), getAnswer{1/2/3/4}() return the question/solution/answer respectively. </li>
    <li> menuFunctions() and questionFunctions() are in charge of displaying on the LCD the menu or information regarding the curent question, and in charge of reacting to user input in those two cases. </li>
    <li> printHighscores() prints in the Serial monitor the top 3 scores from EEPROM. </li>
    <li> scrollAbout() is used to scroll the text when needed. </li>
    <li> menuDing(), correctAns() and incorrectAns() each play a different tone when called. </li>
  </ul>

## Backstory

  <p> Since the beginning of the project, when we were tasked to think about a game to build, I knew I had to create a different type of game than my colleagues, not to flex or anything, but to stand out. </p>
  <p> The inspiration for this game is a minigame from Jackbox, a game series with tons of party minigames. The minigame that inspired me, was the one named just like this one: "Trivia Murder Party", in which you have to answer different trivia questions in order to gain money and not die. The ones who answer wrong, are forced to play a selection of "lethal" minigames in order to survive. </p>

## Description

  <p> Trivia Murder Party is a minigame where you need to answer as many questions right, if you want to survive as long as possible. The game has two disting difficulties: easy and hard, for both the casual and the knowledgeable. Game topics range from anime and games, to geography and music, so that anyone can enjoy the game. </p>
  
## How to play

  <p> Before you start a new game, you should select a difficulty and change your name. To change the difficulty, scroll down until you find the "Difficulty" tab and then using your joystick, swipe left or right to select your desired difficulty. To change your name, scroll down until you find the "Change name" tab and then select swipe left to select Yes. You will then see AAAAA as your default name; to change it go to the letter you want to change, click the joystick and then use the up/down motion to select the desired letter. Click again to disengage the letter. When you are done, scroll right to "BACK" and click on it to go to the main menu. </p>
  <p> If you scroll all the way down, you'll find additional settings such as: Contrast, Led Brightness etc. Do the same things you did with the Difficulty and Name tabs to select your desired settings. </p>
  
## Used Components

  <p> An arduino Mega 2560, a MAX driver for the matrix, a 16x2 LCD, an 8x8 LED Matrix, a buzzer, a joystick, an SD card module, a 2GB SD card, a cylindrical capacitor, a ceramic capacitor and 2 resistors. </p>

![proj1_setup](https://github.com/cbrt-mihai/ArduinoTriviaMurderParty/blob/main/setup/setup.jpg?raw=true)

<p> Link to a video showcasing the gameplay: https://youtu.be/dqw4e5JM85o </p>
<p> Link to 2 videos showcasing the menu: https://youtu.be/QPXtAkIn-W4 and https://youtu.be/houLG56wmqE </p>
<p> Link to a video showcasing the name changing feature: https://youtu.be/ufS9Q_PB2NE </p>
