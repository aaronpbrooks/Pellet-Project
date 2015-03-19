/*
TODO:
  - Add ability to change units
  - Layout for FRAM (variables and logs)





*/
//#include <MenuBackend.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>
#include <Hackscribble_Ferro.h>
//#include <EEPROMex.h>

#define DEBUG

#define RIGHT_BUTTON  2
#define LEFT_BUTTON   3
#define DOWN_BUTTON   4
#define UP_BUTTON     5
#define SELECT_BUTTON 6

#define HALL_SENSOR1 A0
#define HALL_SENSOR2 A1
#define HALL_SENSOR3 A2
#define HALL_SENSOR4 A3

#define OLED_RESET 8
Adafruit_SSD1306 display(OLED_RESET);

Hackscribble_Ferro myFerro(MB85RS64);

int lastButton = 0;
int currentState = 0;
int nominalUpVal = 0;
float nCal = 1236.2;
float pelletDia = 0.0;
//byte sensorStatus = B00000000;

#define MAIN_MENU_MEASURE       0
#define MAIN_MENU_RESULTS       1
#define MAIN_MENU_SETTINGS      2
#define MEASUREMENT_SCREEN1     3
#define MEASUREMENT_SCREEN2     4
#define MEASURING_SCREEN        5
#define MEASURING               5
#define MEASURE_RESULT_SCREEN   6
#define RESULTS_MENU_VIEW       7
#define RESULTS_MENU_CLEAR      8
#define RESULTS_SCREEN          9
#define SETTINGS_MENU_CALIBRATE 10
#define CALIBRATE_SCREEN1       11
#define CALIBRATE_SCREEN2       12
#define CALIBRATING_SCREEN      13
#define CALIBRATING             13
#define CALIBRATE_RESULT_SCREEN 14
#define CANCEL_SCREEN           15

int freeRam () {
	extern int __heap_start, *__brkval;
	int v;
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void setPrescaler()
{
  const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
  const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  //The below ADC changes improve measurement time from approximately 1000-1400usec to 64-72usec
  ADCSRA &= ~PS_128;  // remove bits set by Arduino library
  // PS_16, PS_32, PS_64 or PS_128
  ADCSRA |= PS_64;    // set our own prescaler to 64 
}

int setupFRAM()
{
  if (myFerro.begin() == ferroBadResponse)
  {
    Serial.println(F("FRAM response not OK"));
  } else {
    Serial.println(F("FRAM response OK"));
    ferroResult myResult = ferroUnknownError;
    unsigned int myBufferSize = myFerro.getMaxBufferSize(); 
    unsigned int myBottom = myFerro.getBottomAddress();
    unsigned int myTop = myFerro.getTopAddress();
  }		  
}

int checkButtons()
{
  int buttonPressed = 0;
  if (digitalRead(RIGHT_BUTTON) == HIGH) {
    buttonPressed = RIGHT_BUTTON;
  } else if (digitalRead(LEFT_BUTTON) == HIGH) {
    buttonPressed = LEFT_BUTTON;
  } else if (digitalRead(DOWN_BUTTON) == HIGH) {
    buttonPressed = DOWN_BUTTON;
  } else if (digitalRead(UP_BUTTON) == HIGH) {
    buttonPressed = UP_BUTTON;
  } else if (digitalRead(SELECT_BUTTON) == HIGH) {
    buttonPressed = SELECT_BUTTON;
  }
  if (buttonPressed != lastButton) {
    lastButton = buttonPressed;
    return buttonPressed;
  } else {
    return 0;
  }
}

void displaySetup()
{
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  display.display();  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  updateState(MAIN_MENU_MEASURE);
}


void updateState(int newState)
{
  switch(newState)
  {
    case MAIN_MENU_MEASURE:
      display.clearDisplay();
      display.setTextColor(BLACK, WHITE); // 'inverted' text
      display.setCursor(0,0);
      display.println(F("Measure   "));
      display.setTextColor(WHITE, BLACK);
      display.setCursor(0,21);
      display.println(F("Results  >")); 
      display.setCursor(0,42);
      display.println(F("Settings >"));  
      display.display(); 
      break;
    case MAIN_MENU_RESULTS:
      display.clearDisplay();
      display.setCursor(0,0);
      display.setTextColor(WHITE, BLACK);
      display.println(F("Measure   "));
      display.setCursor(0,21);
      display.setTextColor(BLACK, WHITE); // 'inverted' text
      display.println(F("Results  >")); 
      display.setCursor(0,42);
      display.setTextColor(WHITE, BLACK);
      display.println(F("Settings >"));  
      display.display();  
      break;
    case MAIN_MENU_SETTINGS:
      display.clearDisplay();
      display.setCursor(0,0);
      display.setTextColor(WHITE, BLACK);      
      display.println(F("Measure   "));
      display.setCursor(0,21);
      display.println(F("Results  >")); 
      display.setCursor(0,42);
      display.setTextColor(BLACK, WHITE); // 'inverted' text
      display.println(F("Settings >"));  
      display.display(); 
      break;
    case MEASUREMENT_SCREEN1:
      display.setTextColor(WHITE, BLACK); 
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(22,20);
      display.println(F("Ready piston &"));
      display.setCursor(30,30);
      display.println(F("load pellet"));
      display.display();
      break;
    case MEASUREMENT_SCREEN2:
      display.setTextColor(WHITE, BLACK); 
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(16,20);
      display.println(F("Release piston to"));
      display.setCursor(16,30);
      display.println(F("begin measurement"));
      display.display();    
      break;
    case MEASURING_SCREEN:
      display.setTextColor(WHITE, BLACK);
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(24,25);
      display.println(F("Measuring..."));
      display.display();    
      break;
    case MEASURE_RESULT_SCREEN:
    
      break;
    case RESULTS_MENU_VIEW:
      display.setCursor(60,21);
      //display.clearDisplay();
      display.setTextColor(BLACK, WHITE);
      display.println(F(" View Log "));  
      display.setTextColor(WHITE, BLACK);
      display.setCursor(60,32);
      display.println(F(" Clear All "));
      display.display();     
      break;
    case RESULTS_MENU_CLEAR:
      display.setCursor(60,21);
      //display.clearDisplay();
      display.setTextColor(WHITE, BLACK);
      display.println(F(" View Log "));  
      display.setTextColor(BLACK, WHITE);
      display.setCursor(60,32);
      display.println(F(" Clear All "));
      display.display();  
      break;
    case RESULTS_SCREEN:
      display.setTextColor(WHITE, BLACK);
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(48,25);
      display.println(pelletDia, 4);
      display.display();       
      break;
    case SETTINGS_MENU_CALIBRATE:
      display.setCursor(60,42);
      //display.clearDisplay();
      display.setTextColor(BLACK, WHITE);
      display.println(F(" Calibrate "));  
      display.display();  
      break;
    case CALIBRATE_SCREEN1:
      display.setTextColor(WHITE, BLACK);
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(22,20);
      display.println(F("Ready piston &"));
      display.setCursor(1,30);
      display.println(F("load calibration ball"));
      display.display();   
      break;
    case CALIBRATE_SCREEN2:
      display.setTextColor(WHITE, BLACK); 
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(16,20);
      display.println(F("Release piston to"));
      display.setCursor(16,30);
      display.println(F("begin calibration"));
      display.display();  
      break;
    case CALIBRATING_SCREEN:
      display.setTextColor(WHITE, BLACK);
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(24,25);
      display.println(F("Calibrating..."));
      display.display();    
      break;
    case CALIBRATE_RESULT_SCREEN:
      display.setTextColor(WHITE, BLACK);
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(48,25);
      display.println(nCal, 4);
      display.display();    
      break;
    case CANCEL_SCREEN:
      display.setTextColor(WHITE, BLACK);
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(32,25);
      display.println(F(" Cancelled "));
      display.display();   
      break;   
  }
  if(newState != CANCEL_SCREEN) currentState = newState;
}

void monitorInput()
{
  int buttonPress = checkButtons();
  
  if (buttonPress > 0) {
    switch(currentState)
    {
      case MAIN_MENU_MEASURE:
        switch(buttonPress)
        {
          case RIGHT_BUTTON:
            //Serial.println(F("Right"));
            break;
          case LEFT_BUTTON:
             //Serial.println(F("Left"));         
            break;
          case DOWN_BUTTON:
            updateState(MAIN_MENU_RESULTS);
            //Serial.println(F("Down"));        
            break;
          case UP_BUTTON:
            //Serial.println(F("Up")); 
            updateState(MAIN_MENU_SETTINGS);
            break;
          case SELECT_BUTTON:
            //Serial.println(F("Select"));
            updateState(MEASUREMENT_SCREEN1);         
            break;
        } 
        break;
      case MAIN_MENU_RESULTS:
        switch(buttonPress)
        {
          case RIGHT_BUTTON:
            //Serial.println(F("Right"));
            updateState(RESULTS_MENU_VIEW);            
            break;
          case LEFT_BUTTON:
            //Serial.println(F("Left"));         
            break;
          case DOWN_BUTTON:
            //Serial.println(F("Down"));
            updateState(MAIN_MENU_SETTINGS);         
            break;
          case UP_BUTTON:
            //Serial.println(F("Up"));
            updateState(MAIN_MENU_MEASURE);          
            break;
          case SELECT_BUTTON:
            //Serial.println(F("Select"));     
            break;
        } 
        break;
      case MAIN_MENU_SETTINGS:
        switch(buttonPress)
        {
          case RIGHT_BUTTON:
            //Serial.println(F("Right"));
            updateState(SETTINGS_MENU_CALIBRATE);
            break;
          case LEFT_BUTTON:
            //Serial.println(F("Left"));         
            break;
          case DOWN_BUTTON:
            //Serial.println(F("Down"));   
            updateState(MAIN_MENU_MEASURE);       
            break;
          case UP_BUTTON:
            //Serial.println(F("Up")); 
            updateState(MAIN_MENU_RESULTS);         
            break;
          case SELECT_BUTTON:
            //Serial.println(F("Select"));
            break;
        }
        break;
      case MEASUREMENT_SCREEN1:
        updateState(CANCEL_SCREEN);
        delay(2000);
        updateState(MAIN_MENU_MEASURE);
        break;
      case MEASUREMENT_SCREEN2:
        updateState(CANCEL_SCREEN);
        delay(2000);
        updateState(MAIN_MENU_MEASURE);
        break;
      case MEASURING_SCREEN:
        switch(buttonPress)
        {
          case RIGHT_BUTTON:
            //Serial.println(F("Right"));
            break;
          case LEFT_BUTTON:
            //Serial.println(F("Left"));         
            break;
          case DOWN_BUTTON:
            //Serial.println(F("Down"));          
            break;
          case UP_BUTTON:
            //Serial.println(F("Up"));          
            break;
          case SELECT_BUTTON:
            //Serial.println(F("Select"));
            break;
        } 
        break;
      case MEASURE_RESULT_SCREEN:
        switch(buttonPress)
        {      
          case RIGHT_BUTTON:
            //Serial.println(F("Right"));
            break;
          case LEFT_BUTTON:
            //Serial.println(F("Left"));         
            break;
          case DOWN_BUTTON:
            //Serial.println(F("Down"));          
            break;
          case UP_BUTTON:
            //Serial.println(F("Up"));          
            break;
          case SELECT_BUTTON:
            //Serial.println(F("Select"));
            break;
        } 
        break;
      case RESULTS_MENU_VIEW:
        switch(buttonPress)
        {
          case RIGHT_BUTTON:
            //Serial.println(F("Right"));
            break;
          case LEFT_BUTTON:
            //Serial.println(F("Left"));
            updateState(MAIN_MENU_RESULTS);        
            break;
          case DOWN_BUTTON:
            //Serial.println(F("Down")); 
            updateState(RESULTS_MENU_CLEAR);         
            break;
          case UP_BUTTON:
            //Serial.println(F("Up")); 
            updateState(RESULTS_MENU_CLEAR);         
            break;
          case SELECT_BUTTON:
            //Serial.println(F("Select"));
            break;
        } 
        break;
      case RESULTS_MENU_CLEAR:
        switch(buttonPress)
        {
          case RIGHT_BUTTON:
            //Serial.println(F("Right"));
            break;
          case LEFT_BUTTON:
            //Serial.println(F("Left"));
            updateState(MAIN_MENU_RESULTS);         
            break;
          case DOWN_BUTTON:
            //Serial.println(F("Down"));
            updateState(RESULTS_MENU_VIEW);          
            break;
          case UP_BUTTON:
            //Serial.println(F("Up")); 
            updateState(RESULTS_MENU_VIEW);         
            break;
          case SELECT_BUTTON:
            //Serial.println(F("Select"));
            break;
        } 
        break;
      case RESULTS_SCREEN:
        switch(buttonPress)
        {
          case RIGHT_BUTTON:
            updateState(MAIN_MENU_MEASURE);
            //Serial.println(F("Right"));
            break;
          case LEFT_BUTTON:
            updateState(MAIN_MENU_MEASURE);
            //Serial.println(F("Left"));         
            break;
          case DOWN_BUTTON:
            updateState(MAIN_MENU_MEASURE);
            //Serial.println(F("Down"));          
            break;
          case UP_BUTTON:
            updateState(MAIN_MENU_MEASURE);
            //Serial.println(F("Up"));          
            break;
          case SELECT_BUTTON:
            updateState(MAIN_MENU_MEASURE);
            //Serial.println(F("Select"));
            break;
        } 
        break;
      case SETTINGS_MENU_CALIBRATE:
        switch(buttonPress)
        {
          case RIGHT_BUTTON:
            //Serial.println(F("Right"));
            break;
          case LEFT_BUTTON:
            //Serial.println(F("Left"));
            updateState(MAIN_MENU_SETTINGS);         
            break;
          case DOWN_BUTTON:
            //Serial.println(F("Down"));          
            break;
          case UP_BUTTON:
            //Serial.println(F("Up"));          
            break;
          case SELECT_BUTTON:
            //Serial.println(F("Select"));
            updateState(CALIBRATE_SCREEN1);
            break;
        }
        break;
      case CALIBRATE_RESULT_SCREEN:
        switch(buttonPress)
        {
          case RIGHT_BUTTON:
            updateState(MAIN_MENU_MEASURE);
            //Serial.println(F("Right"));
            break;
          case LEFT_BUTTON:
            updateState(MAIN_MENU_MEASURE);
            //Serial.println(F("Left"));         
            break;
          case DOWN_BUTTON:
            updateState(MAIN_MENU_MEASURE);
            //Serial.println(F("Down"));          
            break;
          case UP_BUTTON:
            updateState(MAIN_MENU_MEASURE);
            //Serial.println(F("Up"));          
            break;
          case SELECT_BUTTON:
            updateState(MAIN_MENU_MEASURE);
            //Serial.println(F("Select"));
            break;
        }
        break;
      case CALIBRATE_SCREEN1:
        updateState(CANCEL_SCREEN);
        delay(2000);
        updateState(MAIN_MENU_MEASURE);
        break;
      case CALIBRATE_SCREEN2:
        updateState(CANCEL_SCREEN);
        delay(2000);
        updateState(MAIN_MENU_MEASURE);
        break;
      }
    }
}

void performMeasurement(int mode)
{
  int sensorVal = 0;
  int maxVal = 0;
  long timeStamp_1 = micros();
  long timeStamp_2 = 0;
  long timeStamp_3 = 0;
  long timeStamp_4 = 0;
  
  sensorVal = analogRead(HALL_SENSOR2);
  
  while(sensorVal > 512) {
    sensorVal = analogRead(HALL_SENSOR2);
    
    //ADD A COUNTER IN HERE, TO SEE WHAT THE MEASUREMENT RESOLUTION IS
    
    if(sensorVal >= maxVal) {
      maxVal = sensorVal;
      //DOES ELIMINATING THE SUBTRACTION STEP BELOW MAKE THIS ANY FASTER?
      timeStamp_2 = micros() - timeStamp_1;
      #ifdef DEBUG
      Serial.print(timeStamp_2);
      Serial.print(",");
      Serial.println(maxVal);
      #endif
    } 
  }
  #ifdef DEBUG
  Serial.print("Max Val 2: ");
  Serial.print(maxVal);
  Serial.print(" at: ");
  Serial.println(timeStamp_2);
  #endif
  maxVal = 0;
  sensorVal = analogRead(HALL_SENSOR3);
  
  while(sensorVal > 512) {
    sensorVal = analogRead(HALL_SENSOR3);
    #ifdef DEBUG
    Serial.print(micros());
    Serial.print(",");
    Serial.println(sensorVal);    
    #endif
    if(sensorVal >= maxVal) {
      maxVal = sensorVal;
      timeStamp_3 = micros() - timeStamp_1;
      #ifdef DEBUG
      Serial.print(timeStamp_3);
      Serial.print(",");
      Serial.println(maxVal);
      #endif
    }
  } 
  #ifdef DEBUG
  Serial.print("Max Val 3: ");
  Serial.print(maxVal);
  Serial.print(" at: ");
  Serial.println(timeStamp_3);
  #endif
  maxVal = 0;
  sensorVal = analogRead(HALL_SENSOR4);

  while(sensorVal > 512) {
    sensorVal = analogRead(HALL_SENSOR4);
    #ifdef DEBUG
    Serial.print(micros());
    Serial.print(",");
    Serial.println(sensorVal);
    #endif
    
    if(sensorVal >= maxVal) {
      maxVal = sensorVal;
      timeStamp_4 = micros() - timeStamp_1;
      #ifdef DEBUG
      Serial.print(timeStamp_4);
      Serial.print(",");
      Serial.println(maxVal);
      #endif
    }
  }
  #ifdef DEBUG
  Serial.print("Max Val 4: ");
  Serial.print(maxVal);
  Serial.print(" at: ");
  Serial.println(timeStamp_4);
  #endif
  //time from sensor 2 to sensor 4 - could also just use 3-4, if more accurate
  long totalTime = timeStamp_4 - timeStamp_3;
  
  if (mode == MEASURING) {
    pelletDia = calculateResults(totalTime);
    //store result in FRAM
  } else if (mode == CALIBRATING) {
    nCal = performCalibration(totalTime);
    //store calibration in FRAM
  }
}

float calculateResults(long timeMeasured)
{
  //Does timeMeasured need to be a float for the math operation below?
  float boreDia = 0.17905;
  float result = sqrt((timeMeasured * sq(boreDia) - nCal) / timeMeasured);
  return result;
}

float performCalibration(long timeMeasured)
{
  float boreDia = 0.17905;
  float ballDia = 0.17403;
  //n = Tball(Dbore^2-Dball^2)
  float newCal = timeMeasured*(sq(boreDia) - sq(ballDia));

  return newCal;
}

double averageVal(int hallSensor, int numReadings)
{
  double averageValue = 0;
  unsigned int i;
  
  for (i = 1; i <= numReadings; i++) {
    averageValue = (averageValue + analogRead(hallSensor));
  }
  return averageValue / numReadings;
}

bool pistonUp()
{
  //Checks to see if piston is in up position
  //Read values to look for in FRAM, and compare to current values
  
  //If sensor 2 > 512 and < 600, and sensor 1 > 1000 and slope = 0
  /*
  int sensorOneLL = 925;
  int sensorTwoLL = 500;
  int sensorTwoUL = 600;
  */
  int sensorOneLL = 840;
  int sensorTwoLL = 500;
  int sensorTwoUL = 600;
  
  double sensorOneVal = averageVal(HALL_SENSOR1, 3);
  delayMicroseconds(100);
  double sensorOneDelta = (averageVal(HALL_SENSOR1, 3) - sensorOneVal);

  double sensorTwoVal = averageVal(HALL_SENSOR2, 3);
  delayMicroseconds(100);
  double sensorTwoDelta = (averageVal(HALL_SENSOR2, 3) - sensorTwoVal);  
  
  nominalUpVal = sensorOneVal;
  
  return (sensorOneVal > sensorOneLL && sensorOneDelta == 0 && sensorTwoVal > sensorTwoLL && sensorTwoVal < sensorTwoUL);
  //return false;
}

bool pistonFalling()
{
  double sensorOneVal = averageVal(HALL_SENSOR1, 3);
  //delayMicroseconds(200);
  //double sensorOneDelta = (averageVal(HALL_SENSOR1, 3) - sensorOneVal);
  //Serial.println(sensorOneVal);
  //Serial.println(sensorOneDelta);
  return (sensorOneVal - nominalUpVal < -50);
}

long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  
 
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring
 
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both
 
  long result = (high<<8) | low;
 
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}

void measureStuff()
{
    unsigned long values[5];
    unsigned int i;
//  unsigned long startTime = micros();
  // capture the values to memory

    values[0] = analogRead(A0);
    values[1] = analogRead(A1);
    values[2] = analogRead(A2);
    values[3] = analogRead(A3);

//  unsigned long stopTime = micros();
  // print out the results
  //Serial.println("\n\n--- Results ---");

  for(i=0;i<4;i++) {
    Serial.print(values[i]);
    Serial.print(",");
  }
  Serial.println();
}

void setup() {
  #ifdef DEBUG
  Serial.begin(115200);
  #endif
  
  pinMode(HALL_SENSOR1, INPUT);
  pinMode(HALL_SENSOR2, INPUT);
  pinMode(HALL_SENSOR3, INPUT);
  pinMode(HALL_SENSOR4, INPUT);
  
  //setupFRAM();
  
  displaySetup();
 
//  Serial.print("Vcc: ");
//  Serial.println(readVcc());
 
}

void loop() {
  measureStuff();
  if (currentState == MEASUREMENT_SCREEN1 || currentState == CALIBRATE_SCREEN1) {
    monitorInput();
    if (currentState == MEASUREMENT_SCREEN1 && pistonUp()) {
      updateState(MEASUREMENT_SCREEN2);
    } else if (currentState == CALIBRATE_SCREEN1 && pistonUp()) {
      updateState(CALIBRATE_SCREEN2);
    }
  } else if (currentState == MEASUREMENT_SCREEN2 || currentState == CALIBRATE_SCREEN2) {
    monitorInput();
    if (currentState == MEASUREMENT_SCREEN2 && pistonFalling()) {
      //Serial.println(analogRead(HALL_SENSOR1));
      updateState(MEASURING);
      performMeasurement(MEASURING);
      updateState(RESULTS_SCREEN);
    } else if (currentState == CALIBRATE_SCREEN2 && pistonFalling()) {
      updateState(CALIBRATING);
      performMeasurement(CALIBRATING);
      updateState(CALIBRATE_RESULT_SCREEN);
    } 
  } else if (currentState == RESULTS_SCREEN && pistonUp() || currentState == CALIBRATE_RESULT_SCREEN && pistonUp()) {
      updateState(MEASUREMENT_SCREEN2);      
  } else monitorInput();
}
