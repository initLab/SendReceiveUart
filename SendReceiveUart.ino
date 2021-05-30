#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

const byte numChars = 64;
char receivedChars[numChars]; // an array to store the received data
boolean newData = false;

void enableReceive() {
  mySwitch.enableReceive(0); // Receiver on interrupt 0 => that is pin #2
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("INIT"));
  enableReceive();
  mySwitch.enableTransmit(10); // Transmitter is connected to Arduino Pin #10
  Serial.println(F("READY"));
}

void loop() {
  switchReceivePrint();
  recvWithEndMarker();
  parseSerialCommand();
}

void switchReceivePrint() {
  if (mySwitch.available()) {
    Serial.print(F("RECEIVE "));
    Serial.print(mySwitch.getReceivedProtocol());
    Serial.print(" ");
    Serial.print(mySwitch.getReceivedDelay());
    Serial.print(" ");
    Serial.print(mySwitch.getReceivedBitlength());
    Serial.print(" ");
    Serial.println(mySwitch.getReceivedValue());
    mySwitch.resetAvailable();
  }
}


void recvWithEndMarker() {
 static byte ndx = 0;
 char rc;
 
  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (rc == '\r' || rc == '\n') {
      if (ndx == 0) {
        continue;
      }
      
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
      return;
    }

    receivedChars[ndx] = rc;
    ndx++;
    
    if (ndx >= numChars) {
      ndx = numChars - 1;
    }
  }
}

void parseSerialCommand() {
  if (!newData) {
    return;
  }

  newData = false;
  char* strtokIndx; // this is used by strtok() as an index
  int arg;
  
  strtokIndx = strtok(receivedChars, " "); // get the first part - the string

  if (strcasecmp(strtokIndx, "SEND") == 0) {
    strtokIndx = strtok(NULL, " "); // this continues where the previous call left off
    arg = atoi(strtokIndx); // convert this part to an integer
    mySwitch.setProtocol(arg);

    strtokIndx = strtok(NULL, " "); // this continues where the previous call left off
    arg = atoi(strtokIndx); // convert this part to an integer
    mySwitch.setPulseLength(arg);

    strtokIndx = strtok(NULL, " "); // this continues where the previous call left off
    arg = atoi(strtokIndx); // convert this part to an integer
    mySwitch.setRepeatTransmit(arg);

    strtokIndx = strtok(NULL, " "); // this continues where the previous call left off
    arg = atoi(strtokIndx); // convert this part to an integer

    strtokIndx = strtok(NULL, " "); // this continues where the previous call left off

    if (arg) {
      unsigned long code = atol(strtokIndx); // convert this part to an integer
      mySwitch.send(code, arg);
    }
    else {
      mySwitch.send(strtokIndx);
    }

    Serial.println(F("SENT"));
    
    return;
  }

  if (strcasecmp(strtokIndx, "SETRECEIVE") == 0) {
    strtokIndx = strtok(NULL, " "); // this continues where the previous call left off
    arg = atoi(strtokIndx); // convert this part to an integer
    
    if (arg) {
      enableReceive();
      Serial.println(F("SETRECEIVE ON"));
    }
    else {
      mySwitch.disableReceive();
      Serial.println(F("SETRECEIVE OFF"));
    }

    return;
  }

  if (strcasecmp(strtokIndx, "PING") == 0) {
    Serial.println(F("PONG"));
    
    return;
  }

  if (strcasecmp(strtokIndx, "HELP") == 0 || strcasecmp(strtokIndx, "?") == 0) {
    Serial.println(F("SendReceiveUart by Vencislav Atanasov (user890104)"));
    Serial.println(F("https://github.com/initLab/SendReceiveUart"));
    Serial.println();
    Serial.println(F("POSSIBLE COMMANDS TO SEND:"));
    Serial.println(F("HELP or ? - this message"));
    Serial.println(F("SEND - send RF command."));
    Serial.println(F("Format: SEND <protocol> <pulse length (ms)> <repeat times> <type/length> <value>"));
    Serial.println(F("For protocol ID, check RCSwitch.cpp"));
    Serial.println(F("Type/Length = 0 means that value is a string of zeroes and ones"));
    Serial.println(F("Type/Length > 1 indicates the number of bits, value is the actual data as long int"));
    Serial.println(F("Example: SEND 1 350 15 24 4242"));
    Serial.println(F("Example: SEND 1 350 15 0 0000111100001111"));
    Serial.println(F("Response: SENT"));
    Serial.println();
    Serial.println(F("SETRECEIVE - enable/disable monitoring for RF commands"));
    Serial.println(F("Format: SETRECEIVE <flag>"));
    Serial.println(F("Flag = 0 means disable, everything else means enable"));
    Serial.println(F("Example: SETRECEIVE 0"));
    Serial.println(F("Example: SETRECEIVE 1"));
    Serial.println(F("Response: SETRECEIVE <ON/OFF>"));
    Serial.println();
    Serial.println(F("PING - check if device is responding to serial commands"));
    Serial.println(F("Format: PING"));
    Serial.println(F("Response: PONG"));
    Serial.println();
    Serial.println(F("POSSIBLE RESPONSE MESSAGES:"));
    Serial.println(F("INIT - program has started"));
    Serial.println(F("READY - receiving and transmitting is enabled"));
    Serial.println(F("RECEIVE - a new command has just been received over RF"));
    Serial.println(F("Format: RECEIVE <protocol> <pulse length (ms)> <number of bits> <value (long)>"));
    Serial.println();
    Serial.println(F("End of HELP"));
    
    return;
  }
  
  Serial.println(F("ERROR COMMAND UNKNOWN"));
}
