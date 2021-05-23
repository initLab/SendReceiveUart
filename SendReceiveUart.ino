#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

const byte numChars = 64;
char receivedChars[numChars]; // an array to store the received data
boolean newData = false;

void setup() {
  Serial.begin(115200);
  Serial.println("INIT");
  mySwitch.enableReceive(0); // Receiver on interrupt 0 => that is pin #2
  mySwitch.enableTransmit(10); // Transmitter is connected to Arduino Pin #10
  Serial.println("READY");
}

void loop() {
  switchReceivePrint();
  recvWithEndMarker();
  parseSerialCommand();
}

void switchReceivePrint() {
  if (mySwitch.available()) {
    Serial.print("RECEIVE ");
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
 char endMarker = '\n';
 char rc;
 
  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (rc == '\r') {
      continue;
    }

    if (rc == endMarker) {
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

    Serial.println("SENT");
    
    return;
  }

  if (strcasecmp(strtokIndx, "SETRECEIVE") == 0) {
    strtokIndx = strtok(NULL, " "); // this continues where the previous call left off
    arg = atoi(strtokIndx); // convert this part to an integer
    
    if (arg) {
      mySwitch.enableReceive();
      Serial.println("SETRECEIVE ON");
    }
    else {
      mySwitch.disableReceive();
      Serial.println("SETRECEIVE OFF");
    }

    return;
  }

  if (strcasecmp(strtokIndx, "HELP") == 0 || strcasecmp(strtokIndx, "?") == 0) {
    Serial.println("SendReceiveUart by Vencislav Atanasov (user890104)");
    Serial.println("https://github.com/initLab/SendReceiveUart");
    Serial.println();
    Serial.println("Command list:");
    Serial.println("HELP - this message");
    Serial.println("SEND - send RF command.");
    Serial.println("Format: SEND <protocol> <pulse length (ms)> <repeat times> <type/length> <value>");
    Serial.println("For protocol ID, check RCSwitch.cpp");
    Serial.println("Type/Length = 0 means that value is a string of zeroes and ones");
    Serial.println("Type/Length > 1 indicates the number of bits, value is the actual data as long int");
    Serial.println("Example: SEND 1 350 15 24 4242");
    Serial.println("Example: SEND 1 350 15 0 0000111100001111");
    Serial.println();
    Serial.println("SETRECEIVE - enable/disable monitoring for RF commands");
    Serial.println("Format: SETRECEIVE <flag>");
    Serial.println("Flag = 0 means disable, everything else means enable");
    Serial.println("Example: SETRECEIVE 0");
    Serial.println("Example: SETRECEIVE 1");
    Serial.println();
    Serial.println("Received commands format:");
    Serial.println("RECEIVE <protocol> <pulse length (ms> <number of bits> <value (long)>");
    Serial.println();
    Serial.println("End of HELP");
    
    return;
  }
  
  Serial.println("ERROR COMMAND UNKNOWN");
}
