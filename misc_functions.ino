
/*

void showarray (byte arr[], int length){
  int x;
  for (x=0; x<=length; x++){
    Serial.print(arr[x],HEX);Serial.print(",");
    //if x is divisible by ten print a new line
    if ((x+1)%10==0){
      Serial.println("");
    }
    //delay(100);
  }
  Serial.println("\nEND\n");
  //delay(3000);
} 



/*void Store_Header (byte arr[], int length){
  int x;
  for(x=0; x<=legnth; x++){
    StoreData(x, arr[x]);
  }
  for(x=legnth+1; x<=legnth+3, x++){
     StoreData(x, 0);
  }
  ee_address=10;
  storedcount=0;
}*//*

void showASCII (byte arr[], int length){
  int y;
  char z;
  for (y=0; y<=length; y++){
    if (arr[y] <=64){
      z=46;
    }
    else{
      
    z=arr[y];
    }
    Serial.print(z);
  }
  Serial.println("");
}*/
