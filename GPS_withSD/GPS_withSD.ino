/******************************************************************************
 GPS_withSD

 *** NOTIFICATION ***
 Original program was made by SparkFun,
 https://github.com/sparkfun/GPS_Shield

 Modified by T. Kamimura, GitHub: https://github.com/t-kamimura
******************************************************************************/

#include <SPI.h>
#include <SD.h>
#include <TinyGPS++.h>

#define ARDUINO_USD_CS 10 //マイクロSDピン（GPSロガーシールド上）

/////////////////////////
//  ログファイルの設定      //
/////////////////////////

// ファイル名の設定．
// "gpslogXX.csv"という名前のファイルが生成されるが，"gpslog99.csv"が限界
#define LOG_FILE_PREFIX "gpslog"    // ファイル名
#define MAX_LOG_FILES 100           // 作成できるログファイル数の上限
#define LOG_FILE_SUFFIX "csv"       // ログファイルの拡張子
char logFileName[13]; // ログファイル名を入れるchar型変数．半角13文字．
// 保存されるデータ
#define LOG_COLUMN_COUNT 8
char * log_col_names[LOG_COLUMN_COUNT] = {
 "longitude", "latitude", "altitude", "speed", "course", "date", "time", "satellites"
}; // これはファイルの１行目に入る

//////////////////////
// ログ周期の設定       //
//////////////////////
#define LOG_RATE 5000 // Log every 5 seconds
unsigned long lastLog = 0; // Global var to keep of last time we logged

/////////////////////////
// TinyGPS Definitions //
/////////////////////////
TinyGPSPlus tinyGPS; // tinyGPSPlus object to be used throughout
#define GPS_BAUD 9600 // GPS module's default baud rate

/////////////////////////////////
// GPS Serial Port Definitions //
/////////////////////////////////
// ソフトウェアシリアルの設定
#include <SoftwareSerial.h>
#define ARDUINO_GPS_RX 9 // GPS TX, Arduino RX pin
#define ARDUINO_GPS_TX 8 // GPS RX, Arduino TX pin
SoftwareSerial ssGPS(ARDUINO_GPS_TX, ARDUINO_GPS_RX); // Create a SoftwareSerial

#define gpsPort ssGPS           // ソフトウェアシリアルの名前をわかりやすくgpsPortとする．
#define SerialMonitor Serial    // ハードウェアシリアルも名前を変えておく．

// I/Oシールド上のLEDを設定
int led = 3;

void setup()
{
  SerialMonitor.begin(9600);

  pinMode(led, OUTPUT); //ledと名付けたピンを，出力に設定
  gpsPort.begin(GPS_BAUD);

  SerialMonitor.println("Setting up SD card.");

  // SDカードが入っているかチェックする
  if (!SD.begin(ARDUINO_USD_CS))
  {
   SerialMonitor.println("Error initializing SD card.");
  }

  updateFileName(); // プログラムを始めるたびに新しいファイルを作成
  printHeader();    // ファイルの１行目を書き込む

}

void loop()
{
 if ((lastLog + LOG_RATE) <= millis())  //前回のデータからちょっと時間がたっていれば
 {
   if (tinyGPS.location.isUpdated()) // GPSデータが来ていたなら
   {
     if (logGPSData()) // GPSデータを保存
     {
       SerialMonitor.println("GPS logged."); // Print a debug message
       digitalWrite(led, HIGH);
       lastLog = millis(); // Update the lastLog variable
     }
     else // GPSログが保存できないなら
     {
       SerialMonitor.println("Failed to log new GPS data.");
       digitalWrite(led, LOW);
     }
   }
   else // GPSデータが来ていないなら
   {
     // 今いくつの衛星を拾っているかを表示.
     SerialMonitor.print("No GPS data. Sats: ");
     SerialMonitor.println(tinyGPS.satellites.value());
     digitalWrite(led, LOW);
   }
 }

 while (gpsPort.available())
   tinyGPS.encode(gpsPort.read());
}

byte logGPSData()
{
 File logFile = SD.open(logFileName, FILE_WRITE); // Open the log file

 if (logFile)
 { // Print longitude, latitude, altitude (in feet), speed (in mph), course
   // in (degrees), date, time, and number of satellites.
   logFile.print(tinyGPS.location.lng(), 6);
   logFile.print(',');
   logFile.print(tinyGPS.location.lat(), 6);
   logFile.print(',');
   logFile.print(tinyGPS.altitude.feet(), 1);
   logFile.print(',');
   logFile.print(tinyGPS.speed.mph(), 1);
   logFile.print(',');
   logFile.print(tinyGPS.course.deg(), 1);
   logFile.print(',');
   logFile.print(tinyGPS.date.value());
   logFile.print(',');
   logFile.print(tinyGPS.time.value());
   logFile.print(',');
   logFile.print(tinyGPS.satellites.value());
   logFile.println();
   logFile.close();

   return 1; // Return success
 }

 return 0; // If we failed to open the file, return fail
}

// printHeader() - prints our eight column names to the top of our log file
void printHeader()
{
 File logFile = SD.open(logFileName, FILE_WRITE); // Open the log file

 if (logFile) // If the log file opened, print our column names to the file
 {
   int i = 0;
   for (; i < LOG_COLUMN_COUNT; i++)
   {
     logFile.print(log_col_names[i]);
     if (i < LOG_COLUMN_COUNT - 1) // If it's anything but the last column
       logFile.print(','); // print a comma
     else // If it's the last column
       logFile.println(); // print a new line
   }
   logFile.close(); // close the file
 }
}

// updateFileName() - Looks through the log files already present on a card,
// and creates a new file with an incremented file index.
void updateFileName()
{
 int i = 0;
 for (; i < MAX_LOG_FILES; i++)
 {
   memset(logFileName, 0, strlen(logFileName)); // Clear logFileName string
   // Set logFileName to "gpslogXX.csv":
   sprintf(logFileName, "%s%d.%s", LOG_FILE_PREFIX, i, LOG_FILE_SUFFIX);
   if (!SD.exists(logFileName)) // If a file doesn't exist
   {
     break; // Break out of this loop. We found our index
   }
   else // Otherwise:
   {
     SerialMonitor.print(logFileName);
     SerialMonitor.println(" exists"); // Print a debug statement
   }
 }
 SerialMonitor.print("File name: ");
 SerialMonitor.println(logFileName); // Debug print the file name
}
