#include <RedFly.h>
#include <ArdOSC.h>


byte myIp[] = { 192, 168, 0, 30 };
uint16_t serverPort = 10000;
uint16_t destPort = 12000;

int ledPin = 9;
int flag = 0;

OSCServer server;
OSCClient client;


//serial format: 9600 Baud, 8N2
void debugout(char *s)  { RedFly.disable(); Serial.print(s);   RedFly.enable(); }
void debugoutln(char *s){ RedFly.disable(); Serial.println(s); RedFly.enable(); }


void setup()
{ 
  uint8_t ret;

  //init the WiFi module on the shield
  // ret = RedFly.init(br, pwr) //br=9600|19200|38400|57600|115200|200000|230400, pwr=LOW_POWER|MED_POWER|HIGH_POWER
  // ret = RedFly.init(pwr) //9600 baud, pwr=LOW_POWER|MED_POWER|HIGH_POWER
  // ret = RedFly.init() //9600 baud, HIGH_POWER
  ret = RedFly.init();
  if(ret)
  {
    debugoutln("INIT ERR"); //there are problems with the communication between the Arduino and the RedFly
    for(;;); //do nothing forevermore
  }
  else
  {
    //scan for wireless networks (must be run before join command)
    RedFly.scan();

    //join network
    // ret = join("wlan-ssid", "wlan-passw", INFRASTRUCTURE or IBSS_JOINER or IBSS_CREATOR, chn, authmode) //join infrastructure or ad-hoc network, or create ad-hoc network
    // ret = join("wlan-ssid", "wlan-passw", IBSS_CREATOR, chn) //create ad-hoc network with password, channel 1-14
    // ret = join("wlan-ssid", IBSS_CREATOR, chn) //create ad-hoc network, channel 1-14
    // ret = join("wlan-ssid", "wlan-passw", INFRASTRUCTURE or IBSS_JOINER) //join infrastructure or ad-hoc network with password
    // ret = join("wlan-ssid", INFRASTRUCTURE or IBSS_JOINER) //join infrastructure or ad-hoc network
    // ret = join("wlan-ssid", "wlan-passw") //join infrastructure network with password
    // ret = join("wlan-ssid") //join infrastructure network
    ret = RedFly.join("wlan-ssid", "wlan-passw", INFRASTRUCTURE);
    if(ret)
    {
      debugoutln("JOIN ERR");
      for(;;); //do nothing forevermore
    }
    else
    {
      //set ip config
      // ret = RedFly.begin(); //DHCP
      // ret = RedFly.begin(1 or 2); //1=DHCP or 2=Auto-IP
      // ret = RedFly.begin(ip);
      // ret = RedFly.begin(ip, dnsserver);
      // ret = RedFly.begin(ip, dnsserver, gateway);
      // ret = RedFly.begin(ip, dnsserver, gateway, netmask);
      ret = RedFly.begin(myIp);
      if(ret)
      {
        debugoutln("BEGIN ERR");
        RedFly.disconnect();
        for(;;); //do nothing forevermore
      }
      else
      {
       //start server
       server.begin(serverPort);
       
       //set callback function & oscaddress
       server.addCallback("/ard/aaa",&func1);
       
       pinMode(ledPin, OUTPUT);  
      }
    }
  }
}

  
void loop()
{
  if(server.aviableCheck()>0)
  {
    //debugoutln("alive!"); //callback after process
  }
}


void func1(OSCMessage *_mes)
{
  int value = _mes->getArgInt32(0);
  if(value!=1) return;
  
  //create new osc message
  OSCMessage newMes;
  
  //set destination ip address & port no
  newMes.setAddress(_mes->getIpAddress(),destPort);
  newMes.beginMessage("/ard/aaa");
  
  if(flag==1)
  {
    flag=0;
    digitalWrite(ledPin, LOW);
  }
  else
  {
    flag=1;
    digitalWrite(ledPin, HIGH);
  }
  newMes.addArgInt32(flag);
  
  //send osc message
  server.stop(); //stop server while sending
  client.send(&newMes);
  server.begin(serverPort); //start server
}
