#include "PinChangeInterrupt.h"

volatile int level = 0;
volatile int levelsToGo[3] = {0, 0, 0};
volatile int doorOpen = false;
volatile int doorClosing = false;
volatile int commoingFrom = 0;
volatile int doorClosed = true;
volatile int moving = false;
//funcation all false 
int doorDelay = 2000;

void setup()
{
  displayDigitInit();

  limitSwitchersInit();

  upDownInit();

  buttonsInit();

  closeOpenInit();

  Serial.begin(9600);
  Serial.println("starting ....");

  levelInfo();
}

void loop()
{
  if (doorOpen && !doorClosing)
  {
    delay(doorDelay);
    closeDoor();
    delay(50);
  }
}

void levelInfo()
{
  Serial.print("on level : ");
  Serial.println(level + 1);

  displayDigit();

  Serial.print("levels to go : ");
  Serial.print(levelsToGo[0]);
  Serial.print(" ");
  Serial.print(levelsToGo[1]);
  Serial.print(" ");
  Serial.println(levelsToGo[2]);
}

/** 7 segments **/

uint8_t a = 22;
uint8_t b = 23;
uint8_t c = 24;
uint8_t d = 25;
uint8_t e = 26;
uint8_t f = 27;
uint8_t g = 28;

void displayDigitInit()
{
  pinMode(a, OUTPUT); //A
  pinMode(b, OUTPUT); //B
  pinMode(c, OUTPUT); //C
  pinMode(d, OUTPUT); //D
  pinMode(e, OUTPUT); //E
  pinMode(f, OUTPUT); //F
  pinMode(g, OUTPUT); //G
}

void displayDigit()
{
  int digit = level + 1;

  displayDigitOff();
  //Conditions for displaying segment a
  if (digit != 1 && digit != 4)
    digitalWrite(a, HIGH);

  //Conditions for displaying segment b
  if (digit != 5 && digit != 6)
    digitalWrite(b, HIGH);

  //Conditions for displaying segment c
  if (digit != 2)
    digitalWrite(c, HIGH);

  //Conditions for displaying segment d
  if (digit != 1 && digit != 4 && digit != 7)
    digitalWrite(d, HIGH);

  //Conditions for displaying segment e
  if (digit == 2 || digit == 6 || digit == 8 || digit == 0)
    digitalWrite(e, HIGH);

  //Conditions for displaying segment f
  if (digit != 1 && digit != 2 && digit != 3 && digit != 7)
    digitalWrite(f, HIGH);
  if (digit != 0 && digit != 1 && digit != 7)
    digitalWrite(g, HIGH);
}

void displayDigitOff()
{
  digitalWrite(a, LOW);
  digitalWrite(b, LOW);
  digitalWrite(c, LOW);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
  digitalWrite(g, LOW);
}

/** 7 segments **/

/** limit switches pins **/

uint8_t LV1Swtich = 2;
uint8_t LV2Swtich = 3;
uint8_t LV3Swtich = 18;

uint8_t DoorSwtich = 19;

void limitSwitchersInit()
{
  pinMode(LV1Swtich, INPUT_PULLUP);
  pinMode(LV2Swtich, INPUT_PULLUP);
  pinMode(LV3Swtich, INPUT_PULLUP);

  pinMode(DoorSwtich, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(LV1Swtich), floor1reach, RISING);
  attachInterrupt(digitalPinToInterrupt(LV2Swtich), floor2reach, RISING);
  attachInterrupt(digitalPinToInterrupt(LV3Swtich), floor3reach, RISING);

  attachInterrupt(digitalPinToInterrupt(DoorSwtich), stopDoor, RISING);
}

void floorReach(int lv)
{
  level = lv;

  if (levelsToGo[lv])
  {
    stopMoving();
    levelsToGo[lv] = 0;
  }

  levelInfo();
}

void floor1reach()
{
  floorReach(0);
}

void floor2reach()
{
  floorReach(1);
}

void floor3reach()
{
  floorReach(2);
}

/** limit switches pins **/

/** up and down **/

volatile uint8_t upPin = 9;
volatile uint8_t downPin = 8;

void upDownInit()
{
  pinMode(upPin, OUTPUT);
  pinMode(downPin, OUTPUT);
}

void goUp()
{
  Serial.println("going up ....");
  digitalWrite(downPin, LOW);
  digitalWrite(upPin, HIGH);
  moving = true;
}

void goDown()
{
  Serial.println("going down ...");
  digitalWrite(upPin, LOW);
  digitalWrite(downPin, HIGH);
  moving = true;
}

void stopMoving()
{
  Serial.println("stopping ...");
  digitalWrite(upPin, LOW);
  digitalWrite(downPin, LOW);

  moving = false;

  openDoor();
}

/** buttons **/

uint8_t LV1btn = 53;
uint8_t LV2btn = 52;
uint8_t LV3btn = 51;

void buttonsInit()
{
  pinMode(LV1btn, INPUT_PULLUP);
  pinMode(LV2btn, INPUT_PULLUP);
  pinMode(LV3btn, INPUT_PULLUP);

  attachPCINT(digitalPinToPCINT(LV1btn), LV1btnClick, CHANGE);
  attachPCINT(digitalPinToPCINT(LV2btn), LV2btnClick, CHANGE);
  attachPCINT(digitalPinToPCINT(LV3btn), LV3btnClick, CHANGE);
}

void LV1btnClick(void)
{
  if (level == 0 & !moving)
    openDoor();
  else
  {

    levelsToGo[0] = 1;
    levelInfo();
    move();
  }
}

void LV2btnClick(void)
{
  if (level == 1 & !moving)
    openDoor();
  else
  {
    levelsToGo[1] = 1;
    levelInfo();
    move();
  }
}

void LV3btnClick(void)
{
  if (level == 2 & !moving)
    openDoor();
  else
  {

    levelsToGo[2] = 1;
    levelInfo();
    move();
  }
}

void move()
{
  if (doorClosed)
  {
    if (level == 0 && (levelsToGo[1] || levelsToGo[2]))
      goUp();

    if (level == 2 && (levelsToGo[0] || levelsToGo[1]))
      goDown();

    if (level == 1 && levelsToGo[0])
      if (levelsToGo[2] && commoingFrom == 0)
        goUp();
      else
        goDown();

    if (level == 1 && levelsToGo[2])
      goUp();

    commoingFrom = level;
  }
}

/** open and close door **/

volatile uint8_t openPin = 7;
volatile uint8_t closePin = 6;

void closeOpenInit()
{
  pinMode(openPin, OUTPUT);
  pinMode(closePin, OUTPUT);
}

void openDoor()
{
  doorClosing = false;
  doorClosed = false;

  Serial.println("opening door ....");
  digitalWrite(closePin, LOW);
  digitalWrite(openPin, HIGH);
}

void closeDoor()
{
  doorClosing = true;
  Serial.println("closing door ...");
  digitalWrite(openPin, LOW);
  digitalWrite(closePin, HIGH);
}

void stopDoor()
{
  Serial.println("stopping door ...");
  digitalWrite(openPin, LOW);
  digitalWrite(closePin, LOW);

  if (doorClosing)
  {
    doorOpen = false;
    doorClosing = false;
    doorClosed = true;
    move();
  }
  else
  {
    doorOpen = true;
  }
}
