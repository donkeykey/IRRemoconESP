#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#define RAWBUF 400
#define SIZE_OF_ARRAY(ary)  (sizeof(ary)/sizeof((ary)[0]))

ESP8266WebServer server ( 80 );
IRsend irsend(15);
const char* ssid     = "";
const char* password = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  irsend.begin();
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println("Connecting to WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("WiFi connected with ip ");
  Serial.println(WiFi.localIP());

  server.on("/send", handleSend);
  server.on("/get", handleGet);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

int split(String *result, size_t resultsize, String data, char delimiter){
    int index = 0;
    int datalength = data.length();
    for (int i = 0; i < datalength; i++) {
        char tmp = data.charAt(i);
        if ( tmp == delimiter ) {
            index++;
            if ( index > (resultsize - 1)) return -1;
        }
        else result[index] += tmp;
    }
    return (index + 1);
}

void handleSend() {
  String splitstring[200] = {"\0"}; 
  char delimiter = ',';
  size_t arraysize = SIZE_OF_ARRAY(splitstring);
  
  String data = String(server.arg("data"));
  String bits = String(server.arg("bit"));
  int index = split(splitstring, arraysize, data, delimiter);
  Serial.println("index = " + String(index));
  unsigned int rawData[index];
  for(int i = 0; i < index; i++){
    rawData[i] = splitstring[i].toInt();
    Serial.println(rawData[i]);
  }
  irsend.sendRaw(rawData, index, bits.toInt());
  server.send ( 200, "text/html", data);
}

void handleGet() {
  server.send ( 200, "text/html", "hoge");
}

void handleNotFound() {

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.send ( 404, "text/plain", message );

}

void loop() {
  server.handleClient();
}
