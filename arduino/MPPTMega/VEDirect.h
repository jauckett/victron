#ifndef VEDIRECT
#define VEDIRECT
#include <Arduino.h>

#define RECORD_HEX    9
#define IDLE_STATE    0
#define RECORD_BEGIN  1
#define RECORD_NAME   2
#define RECORD_VALUE  3
#define CHECKSUM_STATE      4

/*
      "PID",
      "FW",
      "SER#",
      "V",
      "I",
      "VPV",
      "PPV",
      "CS",
      "MPPT",
      "ERR",
      "LOAD",
      "H19",
      "H20",
      "H21",
      "H22",
      "H23",
      "HSDS",
      "CHECKSUM",
      "MPPT"
*/

#define PID 0
#define FW 1
#define SER 2 // Offically SER# but # does not play that well as macro
#define VOLTS 3     // ScV
#define I 4     // ScI
#define VPV 5   // PVV
#define PPV 6   // PVI = PVV / VPV
#define CS 7    // ScS
#define MPPT 8   // ScERR
#define ERR 9  // SLs
#define LOAD 10   // SLI
#define H19 11  // Yeild Total
#define H20 12  // Yield Today
#define H21 13  // Max Power Today
#define H22 14  // Yield Yesterday
#define H23 15  // Max Power Yesterday
#define HSDS 16 // Day Sequence Number (0..364)
#define SOC  17
#define CHECKSUM 18
// #define MPPT 19


/*
  char avgkeywords[num_keywords][label_bytes] = {
  "V",
  "I",
  "VPV",
  "PPV",
  };
*/
/*
        PID  0xA04E
        FW  137
        SER#  HQ181184JGK
    V 51480
    I 1600
    VPV 84400
    PPV 86
        CS  3   state of operation
        MPPT  2
        ERR 0
        LOAD  ON
    H19 1326    Yield Total
    H20 42      Yield Today
    H21 321     Max Power Today
    H22 69      Yield Yesterday
    H23 323     Max Power Yesterday
        HSDS  13
*/


class VEDirectHandler {

  private:
    char mpptname[16];
    int id;
    boolean firstFrame = true;

  public:
    VEDirectHandler(char *n, int i);
    int getId();
    handleChar(char c);
    boolean isValidFrame();
    char *getName();
    void getRecord();
    boolean isAverageValid();
    float getV();
    float getI();
    float getPpv();
    float getVpv();
    float getEtoday();
    int getCs();


  private:
    handleRecord(char *name, char *value);
    handleFrame(boolean valid);
    findKeyword(char *w);
    //long getLong(char *n);
    boolean checkTime();

    const byte buffsize = 32;
    static const byte value_bytes = 16;
    static const byte label_bytes = 9;
    static const byte num_keywords = 18;

    long interval = 60000;
    long numSamples = 0;
    unsigned long previousMillis = millis();

    boolean validFrame = false;
    int mState = IDLE_STATE;
    byte mChecksum = 0;
    char recv_label[num_keywords][label_bytes]; //  = {0};  // {0} tells the compiler to initalize it with 0.
    //long recv_value[num_keywords];  // That does not mean it is filled with 0's
    char values[num_keywords][value_bytes];
    char mValue[16];
    char mName[16];
    char *mTextPointer;
//    char checksumTagName[] = "CHECKSUM";


    long avgV = 0;
    long avgI = 0;
    long avgVPV = 0;
    long avgPPV = 0;
    int cs;
    
float v;
float i;
float vpv;
float ppv;
float eTot;
float eToday;
float ppToday;
float eYesterday;
float ppYesterday;

// V=55149 I=3231 VPV=87886 PPV=185 H19=21858 H20=233 H21=1417 H22=261 H23=1588

    
    boolean validAverage = false;
    char recordBuf[128];
/*
    V 51480
      I 1600
      VPV 84400
      PPV 86
*/
    
   


    char keywords[num_keywords][label_bytes] = {
      "PID",
      "FW",
      "SER#",
      "V",
      "I",
      "VPV",
      "PPV",
      "CS",
      "MPPT",
      "ERR",
      "LOAD",
      "H19",
      "H20",
      "H21",
      "H22",
      "H23",
      "HSDS",
      "CHECKSUM"
    };
};

/*
  config.h - config the keywords for Victron.DIRECT
*/

#endif
