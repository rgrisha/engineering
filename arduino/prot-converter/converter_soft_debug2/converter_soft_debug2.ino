#include <SoftwareSerial.h>
#define CMD_STATUS 'S'
#define CMD_WEIGHT 'B'

//Svarstykliu serial portas, naudojamas tik skaitymas
//Kadangi softwarinis, negalima daryti ilgu laukimo
//ciklu skaitant, kitaip krenta klaidos
SoftwareSerial svSerial(2, 3); // RX, TX

//debug portas, naudojamas tik rasymui
SoftwareSerial debugSerial(4, 8); // RX, TX

//Kadangi su kompu bendraujam Rx/Tx tai hardwarini
//seriala naudojam kompui

char inputBuffer[64];
char pcCommandBuffer[4];

int inputIndex = 0;
int commaCount = 0;
int dataAvailable = 0;
long data = 0;
int doWeightReading = 0;
int dataLineEnd = 0;

int pcCmdIdx = 0;

char WEIGHT_UNSTABLE[]    =  {'\x02','\x55','\x0D','\x0A','\x03'}; 
char WEIGHT_STABLE[]      =  {'\x02','\x53','\x0d','\x0a','\x03'}; 

char weightData[32];

void setup() {

  Serial.begin(9600);

  svSerial.begin(2400);
  debugSerial.begin(115200);

  debugSerial.println("Protocol converter started");

  svSerial.listen();

}

void svResetToReadStart() {
  inputIndex = 0;
  commaCount = 0;
}

void clearWeightData() {
  data = 0;
  dataAvailable = 0;
}

void dataCaptured() {
  
  //debugSerial.write("->");
  //for(int i = 0; i < inputIndex; i++) {
  //  debugSerial.write(inputBuffer[i]);
  //}
  //debugSerial.println("<-");

  if(inputIndex > 16) {

    if(inputBuffer[3] == 'S' && inputBuffer[4] == 'T') {
      data = atol(inputBuffer + 6);
      dataAvailable = 1;
      debugSerial.print("Weight converted: ");
      debugSerial.println(data);
    } else {
      data = 0;
      dataAvailable = 0;
    }
  
    svResetToReadStart();
  
  }
}


void pcAnswerUnstable() {
  for(int i = 0; i < sizeof(WEIGHT_UNSTABLE); i++) {
    delay(20);
    Serial.write(WEIGHT_UNSTABLE[i]);
  }  
}

void pcAnswerStable() {
  debugSerial.println("answering stable");

  for(int i = 0; i < sizeof(WEIGHT_STABLE); i++) {
    delay(20);
    Serial.write(WEIGHT_STABLE[i]);
  }
}


void pcAnswerWeight() {
  int i, bc;
  
  weightData[0] = 0x02;
  weightData[1] = 0x20;
  
  ltoa(data,weightData + 2,10);
  int len = strlen(weightData + 2);

  bc = len + 2;
    
  weightData[bc++] = ' ';
  weightData[bc++] = 'k';
  weightData[bc++] = 'g';
  weightData[bc++] = 13;
  weightData[bc++] = 10;
  weightData[bc++] = 3;
  weightData[bc] = 0;

  debugSerial.print("Will send to PC: ");
  for(i = 0; i < bc; i++) debugSerial.write(weightData[i]);

  for(i = 0; i < bc; i++) {
    delay(20);
    Serial.write(weightData[i]);
  }
  
}


void doPcCommand(char cmd) {

  debugSerial.print("Received command from PC: ");
  debugSerial.println(cmd);

  delay(100);

  if(cmd == CMD_WEIGHT) {

    pcAnswerWeight();
    return;
  } 
  
  if(cmd == CMD_STATUS) {

    if(dataAvailable) {
      pcAnswerStable();
      return;
    }
  }
  
  pcAnswerUnstable();
  
}

void loop() {
  
  if (svSerial.available()) {
   inputBuffer[inputIndex] = svSerial.read();
  
   if(inputIndex > 0 && inputBuffer[inputIndex - 1] == 13 && inputBuffer[inputIndex] == 10) {
     dataCaptured();
   }
  
   //debugSerial.print("[");
   //debugSerial.print(inputBuffer[inputIndex]);
   //debugSerial.print("]");
  
   inputIndex++;
   if(inputIndex >= sizeof(inputBuffer)) {
     inputIndex = 0;
   }   
  
  }


  if(Serial.available()) {
    debugSerial.println("pc serial available");
    pcCommandBuffer[pcCmdIdx] = Serial.read();
    debugSerial.print("got from pc Serial: ");
    debugSerial.println((int)pcCommandBuffer[pcCmdIdx]);
  
    if(pcCommandBuffer[0] == (char)0x1B) {
      pcCmdIdx++;
    } else {
      pcCmdIdx = 0;
    }
    if(pcCmdIdx > 2) {
      if(pcCommandBuffer[2] == (char)0x05) {
        doPcCommand(pcCommandBuffer[1]);
      }
      pcCmdIdx = 0;
    }
  }
   
}
