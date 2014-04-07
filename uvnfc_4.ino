/********************************************
/UVNFC ARDUINO CODE V4.0
/7TH APRIL 2014
/JAMIE LOGAN AND RORY LAMBERT
/*********************************************/


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

RF430CL330H_Shield nfc(IRQ, RESET);
volatile byte into_fired = 0;
uint16_t flags = 0;



void setup(void) 
{
    //STOP MEKEING MEASURMENTS
    //reset RF430
    nfc.begin();
    delay(1000);
    //Serial.begin(115200);
}

/*********************************************************************/

void nfc_send(byte message[]){
     int PAY_LEN; 
     PAY_LEN=sizeof(message);                    //find the length of the payload
     /*sets the length of the NDEF message, depending upon the payload size*/
     byte NDEF_MSG[PAY_LEN + PRE_PAY_LEN-1];     
     int NDEF_LEN = sizeof(NDEF_MSG);            //store its length in an int
     //Function call prepares the full NDEF message
     NDEF_prep(NDEF_MSG, PAY_LEN, message[]);   
}


//function which checks if 2 arrays are equal to each other
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
  nfc.Read_Continuous(0, from_phone, 99);  //Get nfc data
  byte data[7]= {0,0,0,0,0,0,0};
  byte mime[27];
  for (x=31; x<58; x++){        //get mime type from data
    mime[x-31]=from_phone[x];
  }
  corr_app=array_cmp(aar, mime_type, 27);  //check if data is from correct app
  if(corr_app==true){                      //if from correct app
    //Serial.println("correct app");
    for (x=58; x<65; x++){                //get data payload
      data[x-58]=from_phone[x];
    }
    //Ushowarray(data, sizeof(data)-1); 
    
    
    /****FOR DEV ONLY*** send header back through NFC*/
     nfcsend(data);
    
    //PUT HEADER IN EEPROM 
  
    //SETUP TIMER INTERUPTS FOR MEASURMENT INTERVAL DATA[6]   

  }else{      //data comes from wrong phone app
    //Serial.println("wrong app");
  }
}


//ON TIMER INTERUPT
//MAKE READING
//ADD READING TO EEPROM
//UPDATE MEAS COUNT IN EEPROM
//COPY ENTIRE EEPROM TO NFC
//RESTART TIMER


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
            // STOP MAKEING MEASURMENTS
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

  
  
 

//ISR for NFC message
void RF430_Interrupt()            
{
    into_fired = 1;
    detachInterrupt(1);//cancel interrupt
}

