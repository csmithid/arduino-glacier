#include "epd.h"
#include <SPI.h>
#include <Ethernet2.h>

const int led = 13;                           //user led

float center = 252; //
float spread = 100; // Use to place text on 800x600 screen.
float indent = 20;  //

String e = String(); //empty/waste
String p = String("why cant i"); //prompt

String a = String(); //first output
String b = String(); //second output
String c = String(); //third output


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "suggestqueries.google.com";    // name address for Google (using DNS)
IPAddress ip(192, 168, 0, 177);

EthernetClient client;

void get_google(String p)
{
  Serial.begin(9600);
  //while (!Serial) {
  // ; // wait for serial port to connect. Needed for Leonardo only
  //}

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip); //Connect over direct IP instead of DHCP
  }
  // give the Ethernet shield a second to initialize:
  Serial.println("connecting...");
  delay(1000);

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    String q = p;
    q.replace(' ','+');
    client.println("GET /complete/search?q=" + q + "&client=firefox HTTP/1.1");
    client.println("Host: suggestqueries.google.com");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("connection failed");
  }

  // handle bytes incoming from the server
  e = pull_quoted(client);
  e = pull_quoted(client);
  a = pull_quoted(client);
  b = pull_quoted(client);
  c = pull_quoted(client);

  while(true){
    char r  = client.read();
    if (!client.connected()) {
      Serial.println("disconnecting.");
      Serial.end();
      client.stop();
      break;
    }
  }

}

String pull_quoted(EthernetClient client){
  String quoted = String();
  int quoteCount = 0;
  while (true)
  {
    if (client.available()) {
      char r = client.read();
      if (r=='\"'){
        r=client.read();
        quoteCount++;
        if(quoteCount == 2){
          break;
        }
      }
      if(quoteCount == 1){
        quoted.concat(r);
      }
    }
  }
  Serial.println("Pulled result: " + quoted);
  return cleanString(quoted);
}

String cleanString(String input){
  input.replace("\\u0027","\'");
  return input;
}

void epd_stop()
{
  char flag = 0;
  epd_enter_stopmode();
  while (1)
  {
    if(flag)
    {
      flag = 0;
      digitalWrite(led, LOW);
    }
    else
    {
      flag = 1;
      digitalWrite(led, HIGH);
    }
    delay(500);
  }
}

void write_google()
{
  epd_init();
  epd_wakeup();
  epd_set_memory(MEM_NAND);

  epd_set_color(BLACK,WHITE);
  epd_clear();

  epd_set_en_font(ASCII48);
  epd_disp_string(a.c_str(),indent,center-spread);
  epd_disp_string(b.c_str(),indent,center);
  epd_disp_string(c.c_str(),indent,center+spread);

  epd_udpate();

  ep_stop();
}

void setup()
{
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
}

void loop()
{
  get_google(p);

  write_google();
}
