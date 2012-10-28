#include <RedFly.h>
#include <RedFlyServer.h>
#include "localConfig.h"

#define FAIL "fail"

// The pins we want to remote control through wifi
int redLedPin   =  9;
int greenLedPin = 10;
int blueLedPin  = 11;

// brightness levels (0-255)
uint8_t rBrightness = 0;
uint8_t gBrightness = 0;
uint8_t bBrightness = 0;
char brightness[4] = {0,0,0,0};

// network information
RedFlyServer server(serverPort);

// buffer
char * readbuffer = NULL;

void setup() 
{
  readbuffer = (char *) calloc(32, sizeof(char));

  pinMode(redLedPin,   OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin,  OUTPUT);
  
  if (RedFly.init() == 0) {
    RedFly.scan();
    if (RedFly.join(ESSID, WIFI_PASS, INFRASTRUCTURE) == 0) {
      if (RedFly.begin(ip, 0, 0, netmask) == 0) {
        server.begin();
        analogWrite(greenLedPin, 200);
        delay(1000);
        analogWrite(greenLedPin, 0);
      }
    }
  }
}

void fade(int pin, uint8_t from, uint8_t to)
{
#ifdef FADE
  if (from == to) return;
  if ( to > from) {
    for (uint8_t i = from; i <= to; ++i) {
      analogWrite(pin, i);
      delay(10);
      if ( i == 255 ) break;
    }
  } else {
    for (uint8_t i = from; i >= to; --i) {
      analogWrite(pin, i);
      delay(10);
      if ( i == 0 ) break;
    }
  }
#else
    analogWrite(pin, to);
#endif
}

void loop()
{
  char * path = NULL;
  uint8_t oldBrightness;
  
  //listen for incoming clients
  if(server.available()) {
    
    while (server.available()) {
      // get request and handle like it
      server.read((uint8_t *)readbuffer, 31);
      server.flush();

      if (strncmp(readbuffer, "GET /", 5) == 0) {
        path = strtok(readbuffer+5, " "); // +5 get behind /
        memcpy(brightness, readbuffer+6, 3); // +6 get behind the command (r,g,b)
        int setBrightness = atoi(brightness);
        if (path[0] == 'r') {
          oldBrightness = rBrightness;
          rBrightness = setBrightness;
          fade(redLedPin, oldBrightness, rBrightness);
          server.print_P(PSTR("r: "));
        } else if (path[0] == 'g') {
          oldBrightness = gBrightness;
          gBrightness = setBrightness;
          fade(greenLedPin, oldBrightness, gBrightness);
          server.print_P(PSTR("g: "));
        } else if (path[0] == 'b') {
          oldBrightness = bBrightness;
          bBrightness = setBrightness;
          fade(blueLedPin,  oldBrightness, bBrightness);
          server.print_P(PSTR("b: "));
        } else {
          server.print_P(PSTR(FAIL));
          break;
        }
        server.print(brightness);
      }
      break;
    }
    server.stop(); //close connection
  } else if (!server.connected()) {
    server.stop();
    server.begin();
  }
}
