```
SendReceiveUart by Vencislav Atanasov (user890104)
An Arduino project to send and receive RF commands over 433MHz using a pair of receiver
and transmitter boards.

Command list:
HELP - this message
SEND - send RF command.
Format: SEND <protocol> <pulse length (ms)> <repeat times> <type/length> <value>
For protocol ID, check RCSwitch.cpp
Type/Length = 0 means that value is a string of zeroes and ones
Type/Length > 1 indicates the number of bits, value is the actual data as long int
Example: SEND 1 350 15 24 4242
Example: SEND 1 350 15 0 0000111100001111

SETRECEIVE - enable/disable monitoring for RF commands
Format: SETRECEIVE <flag>
Flag = 0 means disable, everything else means enable
Example: SETRECEIVE 0
Example: SETRECEIVE 1

Received commands format:
RECEIVE <protocol> <pulse length (ms> <number of bits> <value (long)>
