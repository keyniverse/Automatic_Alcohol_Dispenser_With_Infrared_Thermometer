#include <Servo.h> //導入伺服馬達函式庫-
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#define IRPin 4 //紅外線模組
#define ServoMotorPin 5 //伺服馬達 板機(yellow)
#define LEDPin 13 //宣告第 13 pin為LED用-
Servo myservo;
boolean readDetected = false; //記錄判斷是否真的有觸發紅外線模組-
byte minAngle = 35;//板機初始角度-
byte maxAngle = 80;//板機扣下角度-
int count = 0;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();  //溫度測量MLX90614
LiquidCrystal_I2C lcd(0x27,16,2);
SoftwareSerial mySoftwareSerial(10, 11);  //RX,TX，記得線要接相反
DFRobotDFPlayerMini myDFPlayer;
void printPlayerDetail(uint8_t type, int value);

void setup() {
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);
  Serial.println("init");
  //DFPlayer 資訊
  Serial.println();
//  Serial.println(F("DFRobot DFPlayer Mini Demo"));
//  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
//  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
//    Serial.println(F("Unable to begin:"));
//    Serial.println(F("1.Please recheck the connection!"));
//    Serial.println(F("2.Please insert the SD card!"));
//    while(true);
//  }
//  Serial.println(F("DFPlayer Mini online."));
//  myDFPlayer.volume(10);
//  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
//
//  myDFPlayer.volume(10);  //Set volume value. From 0 to 30
//  myDFPlayer.play(1);  //Play the first mp3
  
  mlx.begin();
  lcd.init();
  lcd.backlight();
  pinMode(2,INPUT);   //遠端開關腳位，我不確定2有沒有用，如果有用要換腳位(esp8266那邊也要改)，會接收esp8266的訊號
  pinMode(LEDPin, OUTPUT);//設定板上LED燈為輸出-
  pinMode(IRPin, INPUT_PULLUP);//設定第4PIN為輸入上拉腳位-
  myservo.attach(ServoMotorPin);//設定伺服馬達輸出腳位-
  myservo.write(minAngle);//設定初始伺服馬達角度-
}

void loop() {
  Serial.println("update");
  //避免誤觸-
  if(!digitalRead(IRPin)){
    Serial.println("read");
  }else{
    Serial.println("no read");
  }
  int open = digitalRead(2);
  
  if(open == HIGH){
    count = 0;
    Serial.println("switch is on");
    if (!digitalRead(IRPin)) {
      readDetected = true;  //IR有被觸發-
    
      //判斷IR 200次，都有觸發的話，才算真正有觸發-
      for (int i = 0 ; i < 100 ; i++) {
        if (!digitalRead(IRPin)) {
          delay(1);//讀取間隔1ms-
        } else {
          readDetected = false; //如果是誤判或是有人在玩機器，將判斷觸發無效-
          break;  //離開FOR迴圈-
        }
      }
    }
  

    //觸發執行噴霧-
    if (readDetected) {
      lcd.clear();
      float tmpNow = mlx.readAmbientTempC();
      readDetected = false; //成功觸發後要記得改FALSE，避免重複操作-
      
      if(tmpNow > 30){  //溫度上限，記得要改
        lcd.setCursor(0,0);
        lcd.print("fucking high.");
        lcd.setCursor(0,1);
        lcd.print(" ");
        lcd.print(tmpNow);
        lcd.print(" C");
//        myDFPlayer.play(3); //記得要將SD的檔名設為 0003.mp3
      }else{
        lcd.setCursor(0,0);
        lcd.print("Save :)");
        lcd.setCursor(0,1);
        lcd.print("");
        lcd.print(tmpNow+10);
        lcd.print(" C");
//        myDFPlayer.play(1); //記得要將SD的檔名設為 0001.mp3
      }
      delay(150);
      myservo.write(maxAngle);  //噴霧-
      delay(500);//馬達運轉時間-
      myservo.write(minAngle);  //回覆扳機初始位置-
      while (!digitalRead(IRPin));  //如果IR持續觸發，則程式停留在此，等到手離開紅外線的偵測範圍-    
      delay(5000);
    }
    
    delay(50);
    lcd.clear();
    lcd.print("     Hello");
    delay(500);
  }
  else{
    count += 1;
    Serial.println("switch is off");
//    if(count == 1)
//      myDFPlayer.play(4);   //看要不要錄機器已關閉的聲音之類的
  }
  
  
  //player debug
//  if (myDFPlayer.available()) {
//    printPlayerDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
//  }

}


//  playerdebug
void printPlayerDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

//  static unsigned long timer = millis();
//  
//  if (millis() - timer > 3000) {
//    timer = millis();
//    myDFPlayer.next();  //Play next mp3 every 3 second.
//  }
