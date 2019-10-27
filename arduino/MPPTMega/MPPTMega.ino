#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "VEDirect.h"

boolean new_data = true;
long counter = 0;

int portNum = 0;

// initialize the library with the numbers of the interface pins
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

/*
  #define RECORD_HEX    9
  #define IDLE_STATE    0
  #define RECORD_BEGIN  1
  #define RECORD_NAME   2
  #define RECORD_VALUE  3
  #define CHECKSUM      4
*/

// int mState = IDLE_STATE;
byte mChecksum = 0;

VEDirectHandler mppt1("East String", 1);
VEDirectHandler mppt2("West String", 2);

void setup() {
  Serial.begin(115200); // USB console
  Serial1.begin(19200);
  Serial2.begin(19200);

  lcd.init(); //initialize the lcd
  lcd.backlight(); //open the backlight
  Serial.println("DEBUG : Setup Complete");
}

char rc;
unsigned int t;
long timeout = 0;
long lcdtime = millis();

long interval = 60000;
long previousMillis = millis();
boolean checkTime() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

void loop() {

  t = millis();

  /* noise on the serial ports when simulating */
  /* printing chars to see, turn off in production */
  if (Serial1.available()) {
    rc = Serial1.read();
//    mppt1.handleChar(rc);
//    Serial.print(rc);
  }

  if (Serial2.available()) {
    rc = Serial2.read();
//    mppt2.handleChar(rc);
//    Serial.print(rc);
  }

  if (checkTime()) {
    mppt1.getRecord();
    mppt2.getRecord();
  }

  if ((millis() - lcdtime) > 5000) {
    lcd.clear();
    updateLcd(mppt1, 0);
    updateLcd(mppt2, 2);
    lcdtime = millis();
  }
  /*
    while (ss1.available()) {
        rc = ss1.read();
          Serial.write(rc);
      //      handleChar(rc);
        timeout = 0;
    }
  */

   simulate();
  delay(50);
  //  timeout++;
  //  if (timeout > 2000) {
  //    Serial.print(F("DEBUG : No packet for 2 seconds on port ")); Serial.println(portNum);
  //    timeout = 0;
  //    switchPorts();
  //  }
}


void setupLCDPIV() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("V=");     // V=99.9 I=99.9
  lcd.setCursor(7, 0);
  lcd.print("I=");
  lcd.setCursor(0, 1); // P=9999.9 Vpv=999.9
  lcd.print("P=");
  lcd.setCursor(9, 1);
  lcd.print("Vpv=");
}

void setupLCDEnergy() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tday");
  lcd.setCursor(5, 0); 
  lcd.print("E=");     // Tday E=99.99 
  lcd.setCursor(13, 0);
  lcd.print("P=");     // Tday E=99.99 P=9.99
  lcd.setCursor(0, 1);
  lcd.print("Yday");
  lcd.setCursor(5, 1); 
  lcd.print("E=");     // Tday E=99.99 
  lcd.setCursor(13, 1);
  lcd.print("P=");     // Tday E=99.99 P=9.99
}

void updateLCDEnergy() {

  lcd.setCursor(7, 0); 
  lcd.print("99.99");     // Tday E=99.99 
  lcd.setCursor(15, 0);
  lcd.print("9.99");     // Tday E=99.99 P=9.99
  lcd.setCursor(7, 1); 
  lcd.print("99.99");     // Tday E=99.99 
  lcd.setCursor(15, 1);
  lcd.print("9.99");     // Tday E=99.99 P=9.99

}

void updateLCDPIV() {
  lcd.setCursor(2, 0); // V=99.9 I=99.9
  lcd.print(99.9, 1);
  lcd.setCursor(9, 0);
  lcd.print(99.9, 1);
  lcd.setCursor(2, 1); // P=9999.9 Vpv=999.9
  lcd.print(9999.9, 1);
  lcd.setCursor(13, 1);
  lcd.print(999.5, 1);
}

void updateLcd(VEDirectHandler mppt, int p) {
  lcd.setCursor(0, p);
  lcd.print(mppt.getId());
  lcd.print(" ");
  lcd.print("P=");
  lcd.print(mppt.getPpv(), 0);
  lcd.print(" E=");
  lcd.print(mppt.getEtoday(), 2);
  lcd.setCursor(0, p+1);
  lcd.print("Vpv=");
  lcd.print(mppt.getVpv(), 1);
  lcd.print(" I=");
  lcd.print(mppt.getI(), 2);

  int cs = mppt.getCs();
  switch(cs) {
    case 2: lcd.print(" ERR");
      break;
    case 3: lcd.print(" BLK");
      break;
    case 4: lcd.print(" ABS");
      break;
    case 5: lcd.print(" FLT");
      break;
    default: lcd.print(" UNK"); 
  }
}

// 364500 RECORD : ID=2 V=55149 I=3231 VPV=87886 PPV=185 H19=21858 H20=233 H21=1417 H22=261 H23=1588

long recCount = 0;
int h19 = 0;
int h20 = 1340;
int cs = 2;
byte checksum = 0;
long simTime = millis();
char buf1[10];
boolean toggleLCD = false;

void simSend(char *s) {
  // Serial.print("SIM SEND : ");  Serial.println(s);
  char *c = s;
  while (*c != 0) {
    mppt1.handleChar(*c);
    mppt2.handleChar(*c);
    checksum += *c;
    c++;
    delay(1);
  }
}

bool toggleppv = false;
void simulate() {
  if ((millis() - simTime) > 5000) {
    simSend("\r\nPID\t0x203");
    simSend("\r\nFW\t137");
    simSend("\r\nSER#\tHQ181184JGK");
    simSend("\r\nV\t51372");
    simSend("\r\nI\t37880");
    simSend("\r\nVPV\t84400");

    if (toggleppv) {
      simSend("\r\nPPV\t1286");
    } else {
      simSend("\r\nPPV\t500");
    }
    toggleppv = !toggleppv;

    
//    simSend("\r\nCS\t3");
    simSend("\r\nCS\t");
    simSend(itoa(cs, buf1, 10));
    cs++;
    if (cs > 5){
      cs = 2;
    }
    simSend("\r\nMPPT\t2");
    simSend("\r\nERR\t0");
    simSend("\r\nLOAD\tON");

    simSend("\r\nH19\t");
    simSend(itoa(h19, buf1, 10));
    h19++;
    simSend("\r\nH20\t");
    simSend(itoa(h20, buf1, 10));
    h20++;

//    simSend("\r\nH20\t442");
    simSend("\r\nH21\t321");
    simSend("\r\nH22\t69");
    simSend("\r\nH23\t323");
    simSend("\r\nHSDS\t13");
    simSend("\r\nChecksum\t");
    //    Serial.print("DEBUG : CHECKSUM "); Serial.println(checksum);
    //    Serial.print("DEBUG : CHECKSUM "); Serial.println(checksum-256);

    mppt1.handleChar(abs(checksum - 256));
    mppt2.handleChar(abs(checksum - 256));
    // ss1.write(abs(checksum - 256));
    //if (mppt1.isValidFrame()) {
      //Serial.print("DEBUG : Got valid frame from  "); Serial.println(mppt1.getName());
    //}
    if (mppt1.isAverageValid()) {
      recCount++;
      // Serial.print("RECORD : "); Serial.print(recCount); Serial.print(" "); Serial.print(millis()); Serial.print(" "); Serial.println(mppt1.getRecord());
    }
    if (mppt2.isAverageValid()) {
      recCount++;
      // Serial.print("RECORD : "); Serial.print(recCount); Serial.print(" "); Serial.print(millis()); Serial.print(" "); Serial.println(mppt1.getRecord());
    }
/*
    if (toggleLCD) {
      setupLCDPIV();
      updateLCDPIV();
    } else {
      setupLCDEnergy();
      updateLCDEnergy();
    }
    */
    toggleLCD = !toggleLCD;
    lcd.clear();
    updateLcd(mppt1, 0);
    updateLcd(mppt2, 2);

    simTime = millis();
    checksum = 0;
  }
}
  /*
    MPPT
    PID  0xA04E
    FW  137
    SER#  HQ181184JGK
    V 51480
    I 1600
    VPV 84400
    PPV 86
    CS  3
    MPPT  2
    ERR 0
    LOAD  ON
    H19 1326    Yeild Total
    H20 42      Teild Today
    H21 321     Max Power Today
    H22 69      Yeild Yesterday
    H23 323     Max Power Yesterday
    HSDS  13
  */
