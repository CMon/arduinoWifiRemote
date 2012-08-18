
#include <RedFly.h>
#include <RedFlyServer.h>

// The pins we want to remote control through wifi
int redLedPin   =  9;
int greenLedPin = 10;
int blueLedPin  = 11;

// brightness levels (0-255)
int rBrightness = 0;
int gBrightness = 0;
int bBrightness = 0;
char brightness[4] = {0,0,0,0};

// network information
uint8_t ip[]      = { 192,168,  1, 31 }; //ip from shield (server)
uint8_t netmask[] = { 255,255,255,  0 }; //netmask
RedFlyServer server(80);

// buffer
char * readbuffer;

void setup() 
{
  readbuffer = (char *) calloc(32, sizeof(char));

  pinMode(redLedPin,   OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin,  OUTPUT);
  
  //init the WiFi module on the shield
  // ret = RedFly.init(br, pwr) //br=9600|19200|38400|57600|115200|200000|230400, pwr=LOW_POWER|MED_POWER|HIGH_POWER
  // ret = RedFly.init(pwr) //9600 baud, pwr=LOW_POWER|MED_POWER|HIGH_POWER
  // ret = RedFly.init() //9600 baud, HIGH_POWER  uint8_t ret;
  if (RedFly.init() == 0) {
    RedFly.scan();
    if (RedFly.join("essid", "pass", INFRASTRUCTURE) == 0) {
      if (RedFly.begin(ip, 0, 0, netmask) == 0) {
        server.begin();
        analogWrite(greenLedPin, 100);
        delay(100);
        analogWrite(greenLedPin, 0);
      }
    }
  }
}

void loop()
{
  char * path;
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
          rBrightness = setBrightness;
          server.print_P(PSTR("r: "));
        } else if (path[0] == 'g') {
          gBrightness = setBrightness;
          server.print_P(PSTR("g: "));
        } else if (path[0] == 'b') {
          bBrightness = setBrightness;
          server.print_P(PSTR("b: "));
        } else {
          server.print_P(PSTR("fail"));
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
  analogWrite(redLedPin,   rBrightness);
  analogWrite(greenLedPin, gBrightness);
  analogWrite(blueLedPin,  bBrightness);
  delay(1); //stability
}
