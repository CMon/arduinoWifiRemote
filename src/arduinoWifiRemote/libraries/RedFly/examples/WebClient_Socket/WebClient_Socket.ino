/*
  Web Client
 
  This sketch connects to a website using a RedFly-Shield. 
 */

#include <RedFly.h>


byte ip[]        = { 192,168,  0, 30 }; //ip from shield (client)
byte netmask[]   = { 255,255,255,  0 }; //netmask
byte gateway[]   = { 192,168,  0,100 }; //ip from gateway/router
byte dnsserver[] = { 192,168,  0,100 }; //ip from dns server
byte server[]    = {   0,  0,  0,  0 }; //{  85, 13,145,242 }; //ip from www.watterott.net (server)
#define HOSTNAME "www.watterott.net"  //host

uint8_t http=INVALID_SOCKET; //socket handle
uint16_t http_len=0; //receive len
char http_buf[512];  //receive buffer


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
      ret = RedFly.begin(ip, dnsserver, gateway, netmask);
      if(ret)
      {
        debugoutln("BEGIN ERR");
        RedFly.disconnect();
        for(;;); //do nothing forevermore
      }
      else
      {
        if(RedFly.getip(HOSTNAME, server) == 0) //get ip
        {
          http = RedFly.socketConnect(PROTO_TCP, server, 80); //start connection to server on port 80
          if(http == 0xFF)
          {
            debugoutln("SOCKET ERR");
            RedFly.disconnect();
            for(;;); //do nothing forevermore
          }
          else
          {
            //send HTTP request
            RedFly.socketSendPGM(http, PSTR("GET / HTTP/1.1\r\nHost: "HOSTNAME"\r\n\r\n"));
          }
        }
        else
        {
          debugoutln("DNS ERR");
          RedFly.disconnect();
          for(;;); //do nothing forevermore
        }
      }
    }
  }
}


void loop()
{
  uint8_t sock, buf[32];
  uint16_t rd, len;

  if(http == INVALID_SOCKET) //no socket open
  {
    return;
  }

  sock = 0xFF; //0xFF = return data from all open sockets
  rd = RedFly.socketRead(&sock, &len, buf, sizeof(buf));
  if(sock == http)
  {
    if((rd != 0) && (rd != 0xFFFF))
    {
      if((http_len+rd) > sizeof(http_buf))
      {
        rd = sizeof(http_buf)-http_len;
      }
      memcpy(&http_buf[http_len], buf, rd);
      http_len += rd;
    }

    if((rd == 0xFFFF) || (len == 0)) //connection closed or all data received
    {
      //close connection
      RedFly.socketClose(sock);

      //show http buffer
      http_buf[sizeof(http_buf)-1] = 0;
      debugout(http_buf);
    }
  }
}
