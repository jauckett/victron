#include "VEDirect.h"

VEDirectHandler::VEDirectHandler(char *n, int i) {
  //  this.name = name;
  id = i;
  strcpy(mpptname, n);
}

int VEDirectHandler::getId() {
  return id;
}

char *VEDirectHandler::getName() {
  return mpptname;
}

VEDirectHandler::handleChar(char inbyte) {
  //  counter++;
  if ( (inbyte == ':') && (mState != CHECKSUM) ) {
    mState = RECORD_HEX;
  }
  if (mState != RECORD_HEX) {
    mChecksum += inbyte;
  }
  inbyte = toupper(inbyte);
  //  Serial.print(inbyte);
  // Serial.print("Handle char : ");  Serial.print(inbyte); Serial.print(" STATE : "); Serial.println(mState);

  switch (mState) {
    case IDLE_STATE:
      //  Serial.println("IDLE_STATE");
      /* wait for \n of the start of an record */

      switch (inbyte) {
        case '\n':

          //Serial.println("BEGIN RECORD");

          mState = RECORD_BEGIN;
          //      Serial.println("STATE CHANGE RECORD_BEGIN");
          break;
        case '\r': /* Skip */
        default:
          break;
      }
      break;
    case RECORD_BEGIN:
      //  Serial.println("RECORD_BEGIN");
      mTextPointer = mName;
      *mTextPointer++ = inbyte;
      mState = RECORD_NAME;
      break;
    case RECORD_NAME:
      // The record name is being received, terminated by a \t
      switch (inbyte) {
        case '\t':
          //Serial.println(mName);
          // the Checksum record indicates a EOR
          if ( mTextPointer < (mName + sizeof(mName)) ) {
            *mTextPointer = 0; /* Zero terminate */
            //Serial.print("Checking : "); Serial.print(checksumTagName); Serial.print(" with "); Serial.println(mName);
            if (strcmp(mName, "CHECKSUM") == 0) {
              mState = CHECKSUM;
              //Serial.println(mName);
              break;
            }
          } else {
            //Serial.println("mTextPointer < (mName + sizeof(mName)");
          }
          mTextPointer = mValue; /* Reset value pointer */
          mState = RECORD_VALUE;
          // Serial.println("STATE CHANGE RECORD_VALUE");
          break;
        default:
          // add byte to name, but do no overflow
          if ( mTextPointer < (mName + sizeof(mName)) )
            *mTextPointer++ = inbyte;
          break;
      }
      break;
    case RECORD_VALUE:
      // The record value is being received.  The \r indicates a new record.
      switch (inbyte) {
        case '\n':
          // forward record, only if it could be stored completely
          if ( mTextPointer < (mValue + sizeof(mValue)) ) {
            *mTextPointer = 0; // make zero ended
            // textRxEvent(mName, mValue);

            handleRecord(mName, mValue);
            // @TODO handle name value pair
            //        Serial.print(mName);
            //        Serial.print(" = ");
            //        Serial.println(mValue);
          }
          mState = RECORD_BEGIN;
          break;
        case '\r': /* Skip */
          break;
        default:
          // add byte to value, but do no overflow
          if ( mTextPointer < (mValue + sizeof(mValue)) )
            *mTextPointer++ = inbyte;
          break;
      }
      break;
    case CHECKSUM:
      {
        bool valid = mChecksum == 0;
        // Serial.print(F("DEBUG Calculated Checksum : ")); Serial.println(mChecksum);
        if (!valid)
          Serial.println(F("DEBUG : [CHECKSUM] Invalid frame"));
        mChecksum = 0;
        mState = IDLE_STATE;
        validFrame = true;
        handleFrame(valid);

        //Serial.print("SET VALID FRAME :"); Serial.println(validFrame);
        // Serial.print(F("DEBUG COUNTER = ")); Serial.println(counter);
        //        counter = 0;
        break;
      }
    case RECORD_HEX:
      //    if (hexRxEvent(inbyte)) {
      //      mChecksum = 0;
      //      mState = IDLE;
      //    }
      mState = IDLE_STATE;
      break;
  }
}

VEDirectHandler::handleRecord(char *_name, char *value) {
  // Serial.print("Handle Record : "); Serial.print(_name); Serial.print(" = "); Serial.println(value);

  int i = findKeyword(_name);
  if (i < 0) {
    Serial.print(F("DEBUG : Unknown keyword : ")); Serial.println(_name);
  } else {
//    Serial.print("DEBUG : "); Serial.print(_name); Serial.print("="); Serial.println(value);
    // Serial.print("DEBUG : "); Serial.print(_name); Serial.print("="); Serial.println(i);

    strcpy(values[i], value);
    // Serial.print(F("DEBUG AVP : "));  Serial.print(keywords[i]);Serial.print(F(" = ")); Serial.println(values[i]);
  }
  // @TODO cache records until end of frame


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
}

boolean VEDirectHandler::isValidFrame() {
  // Serial.print("ISVALID FRAME : "); Serial.println(validFrame);
  boolean v = validFrame;
  validFrame = false;

  return v;
}

boolean VEDirectHandler::isAverageValid() {
  boolean v = validAverage;
  validAverage = false;
  return v;
}

/*
char *VEDirectHandler::getRecord() {
  
  //   V 51480
  //    I 1600
  //    VPV 84400
  //    PPV 86
  
  //sprintf(recordBuf, "V=%d I=%d VPV=%d PPV=%d", avgV, avgI, avgVPV, avgPPV);
  validAverage = false;
  return recordBuf;
}
*/

/*
boolean VEDirectHandler::checkTime() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
*/
/*
long VEDirectHandler::getLong(char *n) {
  int i = findKeyword(n);
  long v = atol(values[i]);

  //Serial.print("GETLONG "); Serial.print(n); Serial.print(" I="); Serial.print(i); Serial.print(" val=");Serial.print(v);
  //Serial.print(" values[i]=");Serial.println(values[i]);
  return v;
}
*/

void VEDirectHandler::getRecord() {
        /*
      Serial.print("CAL AVERAGES "); Serial.print("AVGV="); Serial.println(avgV);
      Serial.print("CAL AVERAGES "); Serial.print("AVGI="); Serial.println(avgI);
      Serial.print("CAL AVERAGES "); Serial.print("AVGVPV="); Serial.println(avgVPV);
      Serial.print("CAL AVERAGES "); Serial.print("AVGPPV="); Serial.println(avgPPV);
*/
//      Serial.print("SAMPLES "); Serial.println(numSamples);
//      Serial.print("VReading="); Serial.println(getLong("V"));
      avgV = avgV / numSamples;
      avgI = avgI / numSamples;
      avgVPV = avgVPV / numSamples;
      avgPPV = avgPPV / numSamples;
      validAverage = true;
      numSamples = 0;
//      Serial.print("DEBUG V="); Serial.print(avgV); Serial.print(" I="); Serial.print(avgI); Serial.print(" VPV="); Serial.print(avgVPV); Serial.print(" PPV="); Serial.println(avgPPV);

/*    H19 1326    Yield Total
      H20 42      Yield Today
      H21 321     Max Power Today
      H22 69      Yield Yesterday
      H23 323     Max Power Yesterday
          HSDS  13
          */

      // ERR, MPPT, HSDS, CS
      // 364500 RECORD : ID=2 V=55149 I=3231 VPV=87886 PPV=185 H19=21858 H20=233 H21=1417 H22=261 H23=1588    
      sprintf(recordBuf, "ID=%d V=%ld I=%ld VPV=%ld PPV=%ld H19=%ld H20=%ld H21=%ld H22=%ld H23=%ld ERR=%ld MPPT=%ld HSDS=%ld CS=%ld" , 
              getId(),
              avgV, avgI, avgVPV, avgPPV, atol(values[H19]), atol(values[H20]), atol(values[H21]), atol(values[H22]), atol(values[H23]),
              atol(values[ERR]), atol(values[MPPT]), atol(values[HSDS]), atol(values[CS]));
      eToday = atof(values[H20]) / 100;
      ppv = (float)avgPPV;
      i = (float)avgI / 1000;
      vpv = (float)avgVPV / 1000;
      cs = atoi(values[CS]);
      Serial.print("DEBUG : EToday="); Serial.print(eToday); Serial.print(" Ppv="); Serial.println(ppv);
      Serial.print("DEBUG : "); Serial.println(millis()); 
      Serial.print("RECORD : "); Serial.println(recordBuf);

//      Serial.print(ERR); Serial.print(" ERR="); Serial.println(values[ERR]);
//      Serial.print(HSDS); Serial.print(" HSDS="); Serial.println(values[HSDS]);
//      Serial.print(MPPT); Serial.print(" MPPT="); Serial.println(values[MPPT]);

//      Serial.print(F("DEBUG AVP : ")); Serial.print(mpptname); Serial.print(F(" = ")); Serial.println(values[i]);
//      Serial.print("CS="); Serial.println(cs);
      avgV = 0;
      avgI = 0;
      avgVPV = 0;
      avgPPV = 0;
}

VEDirectHandler::handleFrame(boolean valid) {

  // Serial.println("Handle Frame");
  if (valid) {

    // on the first frame send a CONFIG record 
    if (firstFrame) {
      Serial.print("CONFIG :");
      Serial.print(" ID="); Serial.print(getId());
      Serial.print(" FW="); Serial.print(values[FW]);
      Serial.print(" PID="); Serial.print(values[PID]);
      Serial.print(" SER="); Serial.println(values[SER]);
      firstFrame = false;
    }
    // Serial.print(F("DEBUG Handling Valid Frame : ")); Serial.println(mpptname);
    validFrame = true;

    numSamples++;
    avgV += atol(values[VOLTS]);
    avgI += atol(values[I]);
    avgVPV += atol(values[VPV]);
    avgPPV += atol(values[PPV]);
    //if (checkTime()) {

    //}
  } else {
    Serial.println(F("DEBUG : Handling Invalid Frame"));
  }

  // @TODO perform calulations and or transmit to server
  // must all be complete within 1 frame time
}

int VEDirectHandler::findKeyword(char *w) {
  //    Serial.print("FINDING KEYWORD : "); Serial.println(w);
  for (int i = 0; i < num_keywords; i++) {
    //      Serial.print("CHECKING : "); Serial.println(keywords[i]);
    if (strcmp(w, keywords[i]) == 0) {
      return i;
    }
  }
  return -1;
}

    float VEDirectHandler::getV() {
      return v;
    }
    float VEDirectHandler::getI() {
      return i;
    }
    float VEDirectHandler::getPpv() {
      return ppv;
    }
    float VEDirectHandler::getEtoday() {
      return eToday;;
    }
    float VEDirectHandler::getVpv() {
      return vpv;
    }
    int VEDirectHandler::getCs() {
      return cs;
    }
    
