#include <LiquidCrystal_I2C.h>

// LCD object
LiquidCrystal_I2C lcd(0x27,20,4);

// Pins for joysticks
#define JOY1Y 0
#define JOY1X 1
#define JOY2Y 2
#define JOY2X 3
#define JOY3Y 10
#define JOY3X 11

// Pins for motor current sense
#define CUR1 4
#define CUR2 5
#define CUR3 6
#define CUR4 7
#define CUR5 8
#define CUR6 9

// Pins for motor speed control
#define PWM1 4
#define PWM2 5
#define PWM3 6
#define PWM4 7
#define PWM5 8
#define PWM6 9

// Pins for motor direction
#define DIR1 24
#define DIR2 25
#define DIR3 26
#define DIR4 27
#define DIR5 28
#define DIR6 29

// Number of motors
#define MOTOR_COUNT 6

// Number of joystick axis
#define JOYSTICK_COUNT 6

#define DEADZONE 40

int JOY[JOYSTICK_COUNT] = {JOY1X, JOY1Y, JOY2X, JOY2Y, JOY3X, JOY3Y};
int CUR[MOTOR_COUNT] = {CUR1, CUR2, CUR3, CUR4, CUR5, CUR6};
int PWM[MOTOR_COUNT] = {PWM1, PWM2, PWM3, PWM4, PWM5, PWM6};
int DIR[MOTOR_COUNT] = {DIR1, DIR2, DIR3, DIR4, DIR5, DIR6};
int cal[JOYSTICK_COUNT];

void setup() {
  //pinmode for test
  pinMode(10, INPUT_PULLUP);
  
  Serial.begin(9600);
    // Initialize LCD
    lcd.init();
    lcd.backlight();

    for (int i = 0; i < 6; i++) {
        pinMode(DIR[i], OUTPUT);
    }

    // Measure joystick initial position and set bias
    for (int i = 0; i < JOYSTICK_COUNT; i++) {
        cal[i] = analogRead(JOY[i]) - 511;
    }
}

//if stopped then motors shouldn't run
bool stopped = false;

// Measure joystick position
int joyIn[JOYSTICK_COUNT];

// Calculate motor speeds according to joystick position
int M[MOTOR_COUNT] = {0, 0, 0, 0, 0, 0};


void DisplayJoysticks() {
    
}
void loop() {
    if (digitalRead(10) == LOW) {
      TestMode();
    }
    
    for (int i = 0; i < JOYSTICK_COUNT; i++) {
        joyIn[i] = analogRead(JOY[i]) - 511 - cal[i];

        //display joysticks
        Serial.print(joyIn[i]); Serial.print(" ");
    }
    Serial.println("");
    
    ConvertJoystickInputToMotors();
//    
//    //if joystick 1 and 3's x and y values are below -300 then stop the motors
//    if (!stopped && joyIn[4] < -300 && joyIn[5] < -300 && joyIn[0] < -300 && joyIn[1] < -300) {
//      stopped = true;
//    } else if (stopped && joyIn[4] > 300 && joyIn[5] > 300 && joyIn[0] > 300 && joyIn[1] > 300) {
//      //start the motors again
//      stopped = false;
//    }
//    

    
//    if (stopped) {
//      StopMotors();
//    } else {
      ApplyMotorSpeed();
    //}
    DisplayMotorCurrent();
   
}
void StopMotors () {
  // set motors to 0 for everything
    for (int i = 0; i < MOTOR_COUNT; i++) {
        digitalWrite(DIR[i], 0);
        analogWrite(PWM[i], 0);
    }
}
void ConvertJoystickInputToMotors () {
    for (int i = 0; i < MOTOR_COUNT; i++) {
        M[i] = 0;
    }
  // Joystick 1, y-axis
    if (abs(joyIn[0]) > DEADZONE) {
        M[0] -= joyIn[0] / 1.5;
        M[2] -= joyIn[0] / 1.5;
    }

    // Joystick 1, x-axis
    if (abs(joyIn[1]) > DEADZONE) {
        M[0] -= joyIn[1] / 2;
        M[2] += joyIn[1] / 2;
    }

    // Joystick 2, y-axis
    if (abs(joyIn[2]) > DEADZONE) {
        M[1] -= joyIn[2] / 1.35;
        M[3] -= joyIn[2] / 1.35;
    }

    // Joystick 2, x-axis
    if (abs(joyIn[3]) > DEADZONE) {
        M[1] += joyIn[3] / 2;
        M[3] -= joyIn[3] / 2;
    }

    // Joystick 3, x-axis
    if (abs(joyIn[4]) > DEADZONE) {
        M[4] -= joyIn[4] / 2;
        M[5] += joyIn[4] / 2;
    }

    // Joystick 3, y-axis
    if (abs(joyIn[5]) > DEADZONE) {
        M[4] -= joyIn[5] / 1.5;
        M[5] += joyIn[5] / 1.5;
    }
    
}

void TestMode() {
  StopMotors();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TEST MODE");

  //24-29
  for(int i = 24; i <= 29; i++) {
    pinMode(i, OUTPUT);
    //strength of motorf
    analogWrite(i - 20, 100);
    //direction of motor
    digitalWrite(i, 1);

    delay(1000);

    //direction of motor
    digitalWrite(i, 0);

    delay(1000);

    //strength of motor
    analogWrite(i - 20, 0);
  }
  lcd.setCursor(0, 0);
  lcd.print("         ");
}
void ApplyMotorSpeed () {
  //some motors are set to a slightly lower value to make them work the same strength as the weaker motors
    int motorMaximums[] = {255, 255, 255, 255, 255, 255};
    // Apply motor speed and direction
    for (int i = 0; i < MOTOR_COUNT; i++) {
        int dir = M[i] > 0 ? HIGH : LOW;
        digitalWrite(DIR[i], dir); // 1 DIR? 2 
        int val = abs(M[i]) > motorMaximums[i] ? motorMaximums[i] : int(abs(M[i]));
        analogWrite(PWM[i], val);
    }

    
}
void DisplayMotorCurrent() {
    // Measure motor current
    int currentIn[MOTOR_COUNT];
    
    for (int i = 0; i < MOTOR_COUNT; i++) {
        currentIn[i] = analogRead(CUR[i]) / 2.2;
//        Serial.print(currentIn[i]); Serial.print(" ");
    }
//    Serial.println("");

    // Format motor current for display
    String lcdOutput[MOTOR_COUNT];
    for (int i = 0; i < MOTOR_COUNT; i++) {
        lcdOutput[i] = String(currentIn[i] * 5 / 1023.0);
        while (lcdOutput[i].length() < 2) {
        lcdOutput[i] = "0" + lcdOutput[i];
        }
    }
    if (!stopped) {
     // Display motor current
      lcd.setCursor(0, 0);
      lcd.print("M1: " + lcdOutput[0]);
      lcd.setCursor(10, 0);
      lcd.print("M2: " + lcdOutput[1]);
      lcd.setCursor(0, 1);
      lcd.print("M5: " + lcdOutput[4]);
      lcd.setCursor(10, 1);
      lcd.print("M6: " + lcdOutput[5]);
      lcd.setCursor(0, 2);
      lcd.print("M3: " + lcdOutput[2]);
      lcd.setCursor(10, 2);
      lcd.print("M4: " + lcdOutput[3]);
    } else {
      lcd.setCursor(0, 0);
    lcd.print("Stopped");
      }
}
