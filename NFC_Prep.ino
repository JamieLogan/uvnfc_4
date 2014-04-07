void NDEF_prep (byte arr[], int Nlen, byte payload[]){

  

  int p =0;                        //for loop counter
  byte aar[] = AAR;                //initialise the standard arrays
  byte mime_type[] = MIME_TYPE;
  byte msg_setup[] = MSG_SETUP;

  

  

    /****************Add the message setup*********************************/

    //fill with setup 

    for(p=0; p<=SETUP_LEN; p++){
      
      arr[p]=msg_setup[p];
      
    }


    /********************Add the mime type****************************/

    for (p; p<(SETUP_LEN+MIME_LEN); p++){

      arr[p]=mime_type[p-SETUP_LEN];

    }

   

    /********************Add the payload****************************/

    for (p; p<(SETUP_LEN+MIME_LEN+Nlen); p++){

      arr[p]=payload[p-SETUP_LEN-MIME_LEN];

    }

    

     /********************Add the AAR ****************************/



    for (p; p<(SETUP_LEN+MIME_LEN+Nlen+AAR_LEN); p++){

      arr[p]=aar[p-SETUP_LEN-MIME_LEN-Nlen];

    }


    delay(1000);

    
    arr[LEN_BYTE_PACK] = (PACK_LEN + Nlen);

    arr[LEN_BYTE_PAY] = Nlen;


}




