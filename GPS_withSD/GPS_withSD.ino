#include<SD.h>
#include<SoftwareSerial.h>

const int chipSelect = 10;
SoftwareSerial gps(8,9);

void setup() 
{
  Serial.println("Starting program...");
  Serial.begin(9600);    // シリアルモニターのボーレートを9600に設定
  gps.begin(9600); // GPSシールドのボーレートを設定
  delay(2000);

  while(!gps.available()){}
  
  Serial.print("Initializing SD card");

  if(!SD.begin(chipSelect)){
    Serial.println(F("Card failed, or not present"));
    while(1){}
  }
  Serial.println("OK! Monitoring start...");
}

void loop() 
{
  File dataFile = SD.open("datagps.txt",FILE_WRITE);
  if(dataFile){
    while(gps.available()){
      Serial.write(gps.read());
      dataFile.print((char)gps.read());
    }
    dataFile.close();
  }
  else{
    Serial.println("error opening datalog.txt");
    delay(2000);
  }
}
