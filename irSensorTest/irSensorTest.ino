#include <IRLib.h>

#define PROTOCOL NEC

IRrecv reciever(11);
IRdecode decoder;

void setup(){
  Serial.begin(9600);
  reciever.No_Output();
  reciever.enableIRIn();  
}

void loop(){
  
    if(reciever.GetResults(&decoder)){
      decoder.decode();
      if(decoder.decode_type==PROTOCOL && decoder.value != 0xFFFFFFFF){
          Serial.println(decoder.value,HEX);
        }
        reciever.resume();
      }
  }

        switch (decoder.value) {
        case   0xFF38C7: //ok
          motorForward = false;
          motorBackward = false;
          motorLeft = false;
          motorRight = false;
          break;
        case 0xFF18E7: //^
          motorForward = true;
          motorBackward = false;
          motorLeft = false;
          motorRight = false;
          break;
        case  0xFF5AA5:  //>
          motorForward = false;
          motorBackward = false;
          motorLeft = true;
          motorRight = false;
          break;
        case  0xFF10EF:  //<
          motorForward = false;
          motorBackward = false;
          motorLeft = false;
          motorRight = true;
          break;
        case  0xFF4AB5:  //V
          motorForward = false;
          motorBackward = true;
          motorLeft = false;
          motorRight = false;
          break;
      }
