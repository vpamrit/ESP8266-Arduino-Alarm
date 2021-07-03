#define ESP8266

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPIClient.h>

#include <abstracted/NotifierClient.h>

String urlencode(String str);
void sendDataToArduino(char buf[]);
uint8_t _ss_pin = 15;

char buff[] = "Ello M8, no slave\n";

// create SPI client
static NotifierClient client;

void setup()
{
  Serial.begin(9600);
  Serial.println();

  SPI.begin();
  //  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.setFrequency(100000);

  // WiFi.begin("Starbucks-Wifi", "saibaba123");
  // Serial.println("Connecting...");

  // while (WiFi.status() != WL_CONNECTED)
  // {
  //   delay(500);
  //   Serial.println("...");
  // }

  // Serial.println();

  // Serial.print("Connected, with IP address: ");
  // Serial.println(WiFi.localIP());

  Serial.println("READY");
}

void loop()
{
  client.update();

  if (!client.readMessage())
  {
    Serial.println("got nothing");
  }

  delay(1000);

  // sendDataToArduino(buff);

  // if (WiFi.status() == WL_CONNECTED)
  // {
  //   HTTPClient http; //Object of class HTTPClient
  //   http.begin("http://192.168.1.13:5000/");
  //   int httpCode = http.GET();

  //   if (httpCode > 0)
  //   {
  //     Serial.print("Name:");
  //     Serial.println(http.getString());
  //   }
  //   else {
  //     Serial.println("Failure");
  //   }

  //   http.addHeader("Content-Type", "application/json");
  //   httpCode = http.POST("{\"username\":\"liljimothy\"}");
  //   http.end(); //Close connection
  // }
}

void readDataFromArduino(char buf[]){

};

// void sendDataToArduino(char buf[]) {
//   for (int i = 0; i < int(sizeof(buff)) - 1; i++)
//   {
//     char response = SPI.transfer(buff[i]);
//     if(i != 0 && response == buff[i-1]) {
//       Serial.println("DIFFERENT");
//     }
//     else if(response == buff[i]) {
//       Serial.println("SAME");
//     }
//     else if(i == 0 && response == '\n') {
//      Serial.println("DIFFERENT");
//     }
//     else {
//       Serial.println("WHAT JUST HAPPENED");
//     }
//     delay(1000);
//   }
//   Serial.println();
//   Serial.println("Completed transfer");
// }

String urlencode(String str)
{
  String encodedString = "";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i = 0; i < str.length(); i++)
  {
    c = str.charAt(i);
    if (c == ' ')
    {
      encodedString += '+';
    }
    else if (isalnum(c))
    {
      encodedString += c;
    }
    else
    {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9)
      {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9)
      {
        code0 = c - 10 + 'A';
      }
      code2 = '\0';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
      //encodedString+=code2;
    }
    yield();
  }
  return encodedString;
}