#include <ESP8266WiFi.h>
//如果要燒錄GPIO 0要接地，燒不起來要reset(lab2的pdf都有寫)
//燒錄完要重新reset，GPIO 0要接回去正確的位置
const char *ssid = "Yao";  //wifi
const char *password = "mehandsome";  //password

WiFiServer server(80);
char html[2500];
String output0State = "off";

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(0, OUTPUT);   //GPIO 0 連燈泡看有沒有收到訊息
  pinMode(2, OUTPUT);   //GPIO 2 輸出到Arduino的訊號，要確認兩邊code的腳位都是一樣
  digitalWrite(0, 0);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));

  server.begin();
  Serial.println(F("Sever started"));

  Serial.println(WiFi.localIP());   //這個print出來的是web(開關)的網址
}

void loop(){
  // put your main code here, to run repeatedly:
  WiFiClient client = server.available();
  if (!client){
    return;
  }
  Serial.println(F("new client"));

  client.setTimeout(5000);

  String req = client.readStringUntil('\r');
  Serial.println(F("request: "));
  Serial.println(req);

  int val;
  // 將 GPIOs 開或關
  if (req.indexOf("GET /5/on") >= 0){
    Serial.println("GPIO 0 on");
    output0State = "on";
    digitalWrite(0, HIGH);
    digitalWrite(2, HIGH);
  }
  else if (req.indexOf("GET /5/off") >= 0){
    Serial.println("GPIO 0 off");
    output0State = "off";
    digitalWrite(0, LOW);
    digitalWrite(2, LOW);
  }

  //digitalWrite(0, val);

  while (client.available()){
    client.read();
  }

  // HTTP 表頭開始時，會有回應碼 response code (如： HTTP/1.1 200 OK)
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Content-Type: text/html\r\n\r\n<!DOCTYPE HTML>");

  // 顯示 HTML 網頁
  client.println("<html>");
  client.println("<head>");
  client.println("<link rel=\"icon\" href=\"data:,\">");

  // 設定 on/off 按鈕的CSS
  client.println("<style>html, body{ background-color: #272727; display: inline-block; margin: 0px auto; text-align: center;\
  font-family: sans-serif;}");
  client.println(".button { background-color:#86d993; border: none; color: white; padding: 16px 40px;");
  client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
  client.println(".button2 {background-color: rgb(225, 76, 76);}");
  client.println("h1 {color: #999;}</style></head>");

  // 網頁表頭
  client.println("<body><h1>Automatic alcohol dispenser with infrared thermometer</html>");

  // 顯示現在GPIO 5 按鈕的狀態是 ON/OFF
  client.println("<p>Machine - State " + output0State + "</p></h1>");

  // 按鈕假使狀態是 off, 就要顯示 ON
  if (output0State == "off"){
    client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
  }
  else{
    client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
  }
  client.println("</body></html>");
}


  // client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"));
  // client.println("<!doctype html>");
  // client.println("<html>");
  // client.println("<head>");
  // client.println('<meta charset="utf-8">');
  // client.println("<title>自動體溫測量消毒機</title>");
  // snprintf(html, 2500,
  //   "<style>\
  //   html,\
  //   body {\
  //     background-color: #272727;\
  //     display: -webkit-box;\
  //     display: -ms-flexbox;\
  //     display: flex;\
  //     min-height: 100%;\
  //     -webkit-box-pack: center;\
  //         -ms-flex-pack: center;\
  //             justify-content: center;\
  //     -webkit-box-align: center;\
  //         -ms-flex-align: center;\
  //             align-items: center;\
  //     -webkit-box-orient: vertical;\
  //     -webkit-box-direction: normal;\
  //         -ms-flex-direction: column;\
  //             flex-direction: column;\
  //     font-family: sans-serif;\
  //   }\
  //   ul,\
  //   li {\
  //     list-style: none;\
  //     margin: 0;\
  //     padding: 0;\
  //   }\
  //   .tg-list {\
  //     text-align: center;\
  //     display: -webkit-box;\
  //     display: -ms-flexbox;\
  //     display: flex;\
  //     -webkit-box-align: center;\
  //         -ms-flex-align: center;\
  //             align-items: center;\
  //   }\
  //   .tg-list-item {\
  //     margin: 0 2em;\
  //   }\
  //   h2 {\
  //     color: #777;\
  //   }\
  //   h4 {\
  //     color: #999;\
  //   }\
  //   .tgl {\
  //     display: none;\
  //   }\
  //   .tgl, .tgl:after, .tgl:before, .tgl *, .tgl *:after, .tgl *:before, .tgl + .tgl-btn {\
  //     box-sizing: border-box;\
  //   }\
  //   .tgl::-moz-selection, .tgl:after::-moz-selection, .tgl:before::-moz-selection, .tgl *::-moz-selection, .tgl *:after::-moz-selection, .tgl *:before::-moz-selection, .tgl + .tgl-btn::-moz-selection {\
  //     background: none;\
  //   }\
  //   .tgl::selection, .tgl:after::selection, .tgl:before::selection, .tgl *::selection, .tgl *:after::selection, .tgl *:before::selection, .tgl + .tgl-btn::selection {\
  //     background: none;\
  //   }\
  //   .tgl + .tgl-btn {\
  //     outline: 0;\
  //     display: block;\
  //     width: 4em;\
  //     height: 2em;\
  //     position: relative;\
  //     cursor: pointer;\
  //     -webkit-user-select: none;\
  //       -moz-user-select: none;\
  //         -ms-user-select: none;\
  //             user-select: none;\
  //   }\
  //   .tgl + .tgl-btn:after, .tgl + .tgl-btn:before {\
  //     position: relative;\
  //     display: block;\
  //     content: "";\
  //     width: 50%;\
  //     height: 100%;\
  //   }\
  //   .tgl + .tgl-btn:after {left: 0;}\
  //   .tgl + .tgl-btn:before {display: none;}\
  //   .tgl:checked + .tgl-btn:after {\
  //     left: 50%;\
  //   }\
  //   .tgl-light + .tgl-btn {\
  //     background: #f0f0f0;\
  //     border-radius: 2em;\
  //     padding: 2px;\
  //     -webkit-transition: all .4s ease;\
  //     transition: all .4s ease;\
  //   }\
  //   .tgl-light + .tgl-btn:after {\
  //     border-radius: 50%;\
  //     background: #fff;\
  //     -webkit-transition: all .2s ease;\
  //     transition: all .2s ease;\
  //   }\
  //   .tgl-light:checked + .tgl-btn {\
  //     background: #9FD6AE;\
  //   }\
  //   </style>");
  // client.println(html);
  // client.println("</head>");
  // client.println("<body>");
  // client.println("<h2>Super Idol</h2>");
  // client.println("<ul class='tg-list'>");
  // client.println("<li class='tg-list-item'>");
  // client.println("<h4>Light</h4>");
  // client.println('<input class="tgl tgl-light" id="cb1" type="checkbox">');
  // client.print(F("<a href= 'http://"));
  // client.print(WiFi.localIP());
  // client.println(F("/on'><label class="tgl-btn" for="cb1">123</label>"));
  // client.println("</li>");
  // client.println("</ul>");
  // client.println("</body>");
  // client.println("</html>");
