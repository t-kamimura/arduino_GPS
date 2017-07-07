////////////////////////////////////////////////////////////////////////////////
// マスター側プログラム(テンプレート)
// 構成としては，マスターArduino（これ）に以下のものを接続する想定．
// ・GPS用Arduino　・方位センサー　・測距センサー　・モータードライバ
//
// 以下のライブラリをインストールしてください
// 1. DualVNH5019MotorShield
//    「スケッチ」→「ライブラリをインクルード」→「ライブラリの管理」
//     →「DualVNH5019MotorShield」と入力
//
// Template by T. Kamimura, GitHub: https://github.com/t-kamimura
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// 設定とかは以下に書き込んで下さい
////////////////////////////////////////////////////////////////////////////////

/* includeファイルはここに追加してください*/
#include <Wire.h>
#include "DualVNH5019MotorShield.h"

/* 各種変数・定数の設定はここに書いてください */
#define MotorPower 300    //モーターパワーは-400から400

int ledPin = 3;           // I/OシールドのLEDは3番ピン

int dir = 0;              // 現在の向き
int r = 0;                // 目標地点との距離
int theta = 0;            // 目標地点との角度（北から時計回りに何度か）

//モーター
DualVNH5019MotorShield md;

////////////////////////////////////////////////////////////////////////////////
// ユーザー関数は以下で定義して下さい
////////////////////////////////////////////////////////////////////////////////

// GPS用Arduinoからrとthetaを取ってくる
void getPositionData()
{
  // スレーブの8番に接続されているデバイスから1バイトのデータを取得
  // 送られてくる値は10進数で255以下、つまり1バイト以内であるため
  Wire.requestFrom(8, 2);

  // Wire.available()がfalseになるまでループ
  // 要求より短いデータがくる可能性あり
  while(Wire.available())
  {
      r = Wire.read(); // 1バイト受信
      theta = Wire.read(); // 1バイト受信
      Serial.print(r);
      Serial.print(", ");
      Serial.println(theta);
  }
}

// 方位センサーの値を取ってくる
int getDirection()
{
  // 自分の使う方位センサーにあわせてコードを書いてみよう

  return 0; //ここの0を，センサーの値に変えるのを忘れないように！
}

// モーターエラー関数
void stopIfFault()
{
  if (md.getM1Fault())
  {
    Serial.println("M1 error!");
    while(1);
  }
  if (md.getM2Fault())
  {
    Serial.println("M2 error!");
    while(1);
  }
}
////////////////////////////////////////////////////////////////////////////////
// setupは開始時に一回だけ実行される
////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);   //シリアル通信の開始
  Wire.begin();         // マスター側としてI2Cを開始
  pinMode(3,OUTPUT);    // I/OシールドのLEDを使えるようにする
  Serial.println("Beginning...");
}

////////////////////////////////////////////////////////////////////////////////
// 以下、メイン関数
////////////////////////////////////////////////////////////////////////////////
void loop() {
    //センサーデータの取得
    getPositionData();
    dir = getDirection();

    // デバッグ用
    Serial.print("r:     "); Serial.println(r);
    Serial.print("theta: "); Serial.println(theta);
    Serial.print("dir:   "); Serial.println(dir);

    // ここに自分のコードを書いてみよう！

    delay(100);
}
