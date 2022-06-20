#include <SoftwareSerial.h>

//Svarstykliu serial portas, naudojamas tik skaitymas
//Kadangi softwarinis, negalima daryti ilgu laukimo
//ciklu skaitant, kitaip krenta klaidos
SoftwareSerial svSerial(2, 3); // RX, TX

//debug portas, naudojamas tik rasymui
SoftwareSerial debugSerial(4, 8); // RX, TX

//Kadangi su kompu bendraujam Rx/Tx tai hardwarini
//seriala naudojam kompui

#define ACK '\x05'

#define OUTPUT_DELAY_MS 1

#define BEGIN_READ 1
#define WAITING_T 2
#define READ_T_VAL 3 
#define WAITING_P 4
#define READ_P_VAL 5
#define WAITING_SEP 6 
#define READ_CK 8 

char inputState = BEGIN_READ;
char inputChar;

long weight = 0;

char answerBuffer[32];
char convertBuffer[16];
char weightT[8];
char weightP[8];
char inputBufCounter=0;
char checkSum[2];
char checkSumAcc;
int cnt = 0;
char prevState;
char hexOut[4];

void convertToHex(char c) {
  hexOut[0] = c >> 4;
  hexOut[0] = hexOut[0] > 9 ? hexOut[0] + 'A' : hexOut[0] + '0';
  hexOut[1] = c & '\x0F';
  hexOut[1] = hexOut[1] > 9 ? hexOut[1] + 'A' : hexOut[1] + '0';
  hexOut[2] = '\0';
}

void pcAnswerWeight() {
  int i, bc = 0;
  
  answerBuffer[bc++] = 'N';
  
  ltoa(weight, convertBuffer,10);
  int len = strlen(convertBuffer);

  for(i = 0; i < (8 - len); i++) {
    answerBuffer[bc++] = ' ';

  }
  for(i = 0; i < len; i++) {
    answerBuffer[bc++] = convertBuffer[i];
  }

  answerBuffer[bc++] = ' ';
    
  answerBuffer[bc++] = 'k';
  answerBuffer[bc++] = 'g';
  answerBuffer[bc++] = 13;
  answerBuffer[bc++] = 10;

  debugSerial.print("Will send to PC: ");
  for(i = 0; i < bc; i++) debugSerial.write(answerBuffer[i]);

  for(i = 0; i < bc; i++) {
    delay(OUTPUT_DELAY_MS);
    Serial.write(answerBuffer[i]);
  }
  
}

void resetInputState() {
  inputState = BEGIN_READ;
  checkSumAcc = 0;
}

void saveWeights() {
  weight = atol(weightP);
  debugSerial.print("converted weight: %d\n");
  debugSerial.println(weight); 
}

char hexCharToBin(char c) {
  if(c >= '0' && c <= '9') {
    return c - '0';
  }
  if(c >= 'a' && c <= 'f') {
    return c - 'a';
  }
  if(c >= 'A' && c <= 'F') {
    return c - 'A';
  }
  return -1;
}

char hexToBin(char* pc) {
  return hexCharToBin(*pc) << 4 | hexCharToBin(*(pc+1));
}



char checkSumValid() {
  char result = checkSumAcc == hexToBin(checkSum);
  if(result) debugSerial.println("Checksum valid");
  else { 
    debugSerial.print("Checksum invalid: calculated ");
    convertToHex(checkSumAcc);
    debugSerial.print(hexOut);
    debugSerial.print(" vs given ");
    convertToHex(hexToBin(checkSum));
    debugSerial.println(hexOut);
  }
  return result;
}

void onSvRead() {

  prevState = inputState;

  switch(inputState) {
  case BEGIN_READ:
    if(inputChar == '&') {
      inputState = WAITING_T;
    }
    debugSerial.print('.');
    break;

  case WAITING_T:
    if(inputChar == 'T') {
      inputState = READ_T_VAL;
      inputBufCounter = 0;
      checkSumAcc ^= inputChar;
    } else {
      resetInputState();
    }
    break;


  case READ_T_VAL:
    if(inputChar >= '0' && inputChar <= '9') {
      weightT[inputBufCounter++] = inputChar;
      weightT[inputBufCounter] = 0;
      checkSumAcc ^= inputChar;
    }
    if(inputBufCounter >= 6) {
      inputState = WAITING_P;
    }
    break;

  case WAITING_P:
    if(inputChar == 'P') {
      inputState = READ_P_VAL;
      inputBufCounter = 0;
      checkSumAcc ^= inputChar;
    } else {
      resetInputState();
    }

  case READ_P_VAL:
    if(inputChar >= '0' && inputChar <= '9') {
      weightP[inputBufCounter++] = inputChar;
      weightP[inputBufCounter] = 0;
      checkSumAcc ^= inputChar;
    }
    if(inputBufCounter >= 6) {
      inputState = WAITING_SEP;
    }
    break;

  case WAITING_SEP:
    if(inputChar == '\\') {
      inputState = READ_CK;
      inputBufCounter = 0;
    } else {
      resetInputState();
    }
    break;

  case READ_CK:
    checkSum[inputBufCounter++] = inputChar;
    if(inputBufCounter >= 2) {
      if(checkSumValid()) {
        saveWeights();
      }
      resetInputState();
    }
    break;

  }

  if(prevState != inputState) {
    debugSerial.print("State ");
    debugSerial.print((int)prevState);
    debugSerial.print("->");
    debugSerial.println(inputState);
  }

}

void setup() {

  Serial.begin(9600);

  svSerial.begin(2400);
  debugSerial.begin(115200);

  debugSerial.println("Protocol converter started");

  svSerial.listen();

  resetInputState();

}

void loop() {
  
  if (svSerial.available()) {

   inputChar = svSerial.read();

   onSvRead();

   debugSerial.print("*S");
  }


  char pcCommand;
  if(Serial.available()) {
    pcCommand = Serial.read();
    if(pcCommand == ACK) {
      debugSerial.println("Got ACK");
      pcAnswerWeight();
    } else {
      debugSerial.print("unknown command from PC: ");
      debugSerial.println((int)pcCommand);
    }
    debugSerial.print("*P");
  }

  if(cnt++ == 0) debugSerial.print('.');
   
}
