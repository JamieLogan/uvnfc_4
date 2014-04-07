/* Copyright 2013 Ten Wong, wangtengoo7@gmail.com  
*  https://github.com/awong1900/RF430CL330H_Shield 
*  RF430CL330H datasheet reference http://www.ti.com/
*/

/*********************************************************
** sample: when reset the rf430, it will write the uri to 
** rf430 tag.
***********************************************************/
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <Wire.h>
#include <RF430CL330H_Shield.h>
#include "RLMSG_2.h"

#define IRQ   (3)
#define RESET (4)  
int led = 13;
RF430CL330H_Shield nfc(IRQ, RESET);




volatile byte into_fired = 0;
uint16_t flags = 0;



void setup(void) 
{
   
    //reset RF430
    nfc.begin();
    delay(1000);
    //Serial.begin(115200);
}

/*********************************************************************/


boolean array_cmp(byte a[], byte b[], int len){
  int n;
  for (n=0;n<len;n++) if (a[n]!=b[n]) return false; // test each element to be the same. if not, return false
  return true; //ok, if we have not returned yet, they are equal
}

void Data_From_Phone(){
  byte from_phone[100];
  byte mime_type[27] = {0x61, 0x70, 0x70, 0x6C, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x2F, 0x75, 0x6B, 0x2E,	\
                        0x61, 0x63, 0x2E, 0x67, 0x6C, 0x61, 0x2E, 0x75, 0x76, 0x6E, 0x66, 0x63};
  int x;
  boolean corr_app;
  nfc.Read_Continuous(0, from_phone, 99);
  byte data[7]= {0,0,0,0,0,0,0};
  byte aar[27];
  for (x=31; x<58; x++){
    aar[x-31]=from_phone[x];
  }
  corr_app=array_cmp(aar, mime_type, 27);
  if(corr_app==true){
    //Serial.println("correct app");
    for (x=58; x<65; x++){
      data[x-58]=from_phone[x];
    }
    //Ushowarray(data, sizeof(data)-1); 
    
    int PAY_LEN; 
    
     PAY_LEN=sizeof(data);                    //find the length of the payload
     /*sets the length of the NDEF message, depending upon the payload size*/
     byte NDEF_MSG[PAY_LEN + PRE_PAY_LEN-1];     
     int NDEF_LEN = sizeof(NDEF_MSG);            //store its length in an int
     //Function call prepares the full NDEF message
     NDEF_prep(NDEF_MSG, PAY_LEN, data);    
     

  }else{
    //Serial.println("wrong app");
  }
}

void loop(void) 
{
    while(!(nfc.Read_Register(STATUS_REG) & READY)); //wait until READY bit has been set
       

    //write NDEF memory with Capability Container + NDEF message
   

    //Enable interrupts for End of Read and End of Write
    nfc.Write_Register(INT_ENABLE_REG, EOW_INT_ENABLE + EOR_INT_ENABLE);

    //Configure INTO pin for active low and enable RF
    nfc.Write_Register(CONTROL_REG, INT_ENABLE + INTO_DRIVE + RF_ENABLE );

    //enable interrupt 1
    attachInterrupt(1, RF430_Interrupt, FALLING);
    

    while(1)
    {
        if(into_fired)
        {
            //clear control reg to disable RF
            nfc.Write_Register(CONTROL_REG, INT_ENABLE + INTO_DRIVE); 
            delay(750);
            
            //read the flag register to check if a read or write occurred 
            flags = nfc.Read_Register(INT_FLAG_REG); 
         
            
            //ACK the flags to clear
            nfc.Write_Register(INT_FLAG_REG, EOW_INT_FLAG + EOR_INT_FLAG); 
            
            if(flags & EOW_INT_FLAG)      //check if the tag was written to by phone
            {
            
             Data_From_Phone();
             
             
            }
            else if(flags & EOR_INT_FLAG) //check if the tag was read
            {
            // nfc.Write_Continuous(0, data_header, sizeof(data_header));
            }
            flags = 0;
            into_fired = 0; //we have serviced INT1

            //Enable interrupts for End of Read and End of Write
            nfc.Write_Register(INT_ENABLE_REG, EOW_INT_ENABLE + EOR_INT_ENABLE);

            //Configure INTO pin for active low and re-enable RF
            nfc.Write_Register(CONTROL_REG, INT_ENABLE + INTO_DRIVE + RF_ENABLE);

            //re-enable INTO
            attachInterrupt(1, RF430_Interrupt, FALLING);
        }
    }
}

  
  
 

/**
**  @brief  interrupt service
**/
void RF430_Interrupt()            
{
    into_fired = 1;
    detachInterrupt(1);//cancel interrupt
}

