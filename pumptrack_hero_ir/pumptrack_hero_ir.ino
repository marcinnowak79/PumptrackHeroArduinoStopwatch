#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// define some values used by the panel and buttons
int lcdKey     = 0;
int prevLcdKey = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

// TODO: check pin number! 
#define IR_PIN 2

#define ACTIVATION 0
#define WAITING_FOR_NEW_MEASURE 1
#define STOP_DISABLED 2
#define STOP_ENABLED 3
#define RESULT 4

int programState = ACTIVATION;
unsigned long startTime=0;
unsigned long endTime=0;
float resultTime=0;

// read the buttons
int read_LCD_buttons(){
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  
 return btnNONE;  // when all others fail, return this...
}

void setup(){ 
 lcd.begin(16, 2);             
 lcdPrint("Pumptrack Hero!","Reset - start");
 pinMode(IR_PIN, INPUT_PULLUP);
 attachInterrupt(digitalPinToInterrupt(IR_PIN), riderDetected, RISING);
}

void lcdPrint(char firstLine[], char secondLine[]){
 lcd.clear();
 lcd.print(firstLine);
 lcd.setCursor(0,1);
 lcd.print(secondLine); 
}

void resetAction(){
  // reset action 
  startTime = 0;
  endTime = 0; 
  resultTime = 0;
  programState = WAITING_FOR_NEW_MEASURE;
  lcdPrint("Nowy pomiar","");
}

void enableStopAction(){
  // enable stop of counting 
  programState = STOP_ENABLED;
  lcdPrint("Trwa pomiar","Stop aktywny");
}

void startCountingAction(){
    startTime = millis();
    programState = STOP_DISABLED;
    lcdPrint("Trwa pomiar","Aktywuj stop");
}

void showResultAction(){
  endTime = millis();
  resultTime = (float)(endTime - startTime)/1000; 
  programState = RESULT;
  char buff[10];
  dtostrf(resultTime, 1, 2, buff);  
  lcdPrint("Wynik",buff);
}

void riderDetected(){
   if(programState != ACTIVATION){
    if(programState == WAITING_FOR_NEW_MEASURE){
        startCountingAction();
    }
    if(programState == STOP_ENABLED){
        showResultAction();
    }
  } 
}
 
void loop(){
  lcdKey = read_LCD_buttons();
  if(prevLcdKey != lcdKey){

    if (lcdKey == btnLEFT){
      resetAction();  
    }
    if (lcdKey == btnRIGHT && programState == STOP_DISABLED){
      enableStopAction();
    }
    
    prevLcdKey = lcdKey;
  }
}
