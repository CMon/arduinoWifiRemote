/*
  Paint
 
  This sketch connects two RedFly-Shields with displays. 
  Requires RedFly + mSD-shield with MI0283QT-Adapter.
 */

#include <MI0283QT2.h>
#include <ADS7846.h>
#include <RedFly.h>
#include <RedFlyClient.h>
#include <RedFlyServer.h>


#define TP_EEPROMADDR (0x00) //eeprom address for calibration data
MI0283QT2 lcd;
ADS7846 tp;

byte ip_server[] = { 192,168,  0, 50 }; //ip from server shield
byte ip_client[] = { 192,168,  0, 51 }; //ip from client shield
byte netmask[]   = { 255,255,255,  0 }; //netmask
byte gateway[]   = {   0,  0,  0,  0 }; //ip from gateway/router (not needed)

uint16_t comm_port = 1000;

RedFlyClient client(ip_server, comm_port);
RedFlyServer server(comm_port);

uint8_t working_mode=0; //0=server, 1=client

static uint16_t last_x=0, last_y=0, color=COLOR_BLACK;


//display output functions
#define infoClear()  lcd.fillRect(0, (lcd.getHeight()/2)-15, lcd.getWidth()-1,(lcd.getHeight()/2)+5, COLOR_WHITE);
#define infoText(x)  lcd.fillRect(0, (lcd.getHeight()/2)-15, lcd.getWidth()-1,(lcd.getHeight()/2)+5, COLOR_BLACK); lcd.drawTextPGM((lcd.getWidth()/2)-60, (lcd.getHeight()/2)-10, PSTR(x), 1, COLOR_WHITE, COLOR_BLACK)
#define errorText(x) lcd.fillRect(0, (lcd.getHeight()/2)-15, lcd.getWidth()-1,(lcd.getHeight()/2)+5, COLOR_BLACK); lcd.drawTextPGM((lcd.getWidth()/2)-60, (lcd.getHeight()/2)-10, PSTR(x), 1, COLOR_RED, COLOR_BLACK)


uint8_t clearall(uint8_t send) //redraw complete screen
{
  if(send)
  {
    uint16_t buf[4]={0xBEEF,0x0000,0x0000,0x0000};
    if(working_mode == 0) //server
    {
      server.write((uint8_t*)buf, 8);
    }
    else
    {
      client.write((uint8_t*)buf, 8);
    }
  }

  lcd.fillRect(0, 0, 20, lcd.getHeight()-1, color);
  lcd.fillRect(lcd.getWidth()-20, 0, lcd.getWidth()-1, lcd.getHeight()-1, COLOR_BLACK);
  lcd.fillRect(21, 0, lcd.getWidth()-21, lcd.getHeight()-1, COLOR_WHITE);
}


uint8_t startwifi(void)
{
  uint8_t ret, i;

  //init the WiFi module
  infoText("Init WiFi...");
  ret = RedFly.init(115200, HIGH_POWER); //baudrate, LOW_POWER MED_POWER HIGH_POWER
  if(ret)
  {
    errorText("Init WiFi...Error"); //there are problems with the communication between the Arduino and the RedFly
    return 1;
  }

  //join/create network
  for(i=3; i!=0; i--) //try 3 times
  {
    if(working_mode == 0) //server
    {
      infoText("Create network...");
      ret = RedFly.join("DemoWLAN", "1A2B3C4D5E", IBSS_CREATOR, 1);
    }
    else
    {
      infoText("Scan...");
      RedFly.scan(); //scan for wireless networks (must be run before join command)
      infoText("Join network...");
      ret = RedFly.join("DemoWLAN", "1A2B3C4D5E", IBSS_JOINER);
    }
    if(ret)
    {
      errorText("Join network...Error");
    }
    else
    {
      break;
    }
  }
  if(ret)
  {
    return 2;
  }

  //set ip config
  infoText("Set IP config...");
  if(working_mode == 0) //server
  {
    ret = RedFly.begin(ip_server, 0, gateway, netmask);
  }
  else
  {
    ret = RedFly.begin(ip_client, 0, gateway, netmask);
  }
  if(ret)
  {
    errorText("Set IP config...Error");
    RedFly.disconnect();
    return 3;
  }

  if(working_mode == 0) //server
  {
    infoText("Start Server...");
    server.begin();
  }
  else
  {
    infoText("Start Client...");
    client.connect();
  }

  clearall(1);

  return 0;
}


void setup()
{
  //init display
  lcd.init(2); //spi-clk = Fcpu/2
  lcd.setOrientation(0);

  //init touch controller
  tp.init();
  tp.setOrientation(0);

  //clear screen
  lcd.clear(COLOR_WHITE);
  
  //touch-panel calibration
  tp.service();
  if(tp.getPressure() > 5)
  {
    tp.doCalibration(&lcd, TP_EEPROMADDR, 0); //dont check EEPROM for calibration data
  }
  else
  {
    tp.doCalibration(&lcd, TP_EEPROMADDR, 1); //check EEPROM for calibration data
  }

  //clear screen
  lcd.clear(COLOR_WHITE);

  //select working mode
  lcd.drawTextPGM(30, (lcd.getHeight()/2)-20, PSTR("Server"), 2, COLOR_BLACK, COLOR_WHITE);
  lcd.drawTextPGM((lcd.getWidth()/2)+30, (lcd.getHeight()/2)-20, PSTR("Client"), 2, COLOR_BLACK, COLOR_WHITE);
  lcd.fillRect((lcd.getWidth()/2)-1, 0, (lcd.getWidth()/2)+1, lcd.getHeight()-1, COLOR_BLACK);
  while(tp.getPressure() < 5){ tp.service(); }
  if(tp.getX() < (lcd.getWidth()/2))
  {
    working_mode = 0; //server
    lcd.drawTextPGM(30, (lcd.getHeight()/2)-20, PSTR("Server"), 2, COLOR_RED, COLOR_WHITE);
    color = COLOR_BLUE;
  }
  else
  {
    working_mode = 1; //client
    lcd.drawTextPGM((lcd.getWidth()/2)+30, (lcd.getHeight()/2)-20, PSTR("Client"), 2, COLOR_RED, COLOR_WHITE);
    color = COLOR_RED;
  }
  delay(1000);
  lcd.clear(COLOR_WHITE);
  
  //start WiFi
  while(startwifi() != 0){ delay(1000); }
}


void loop()
{
  uint16_t x=0, y=0;
  uint16_t buf[4];
  static uint8_t connection=0;

  if(working_mode == 0) //server
  {
    if(!server.connected())
    {
      if(server.connect() == 0) //open port for client (begin is the same as connect)
      {
        server.stop();
        if(++connection >= 3)
        {
          while(startwifi() != 0){ delay(1000); }
        }
      }
      else
      {
        connection = 0;
      }
    }
    else
    {
      while(server.available() >= 8) //data available?
      {
        //0xBEEF, x, y, color (all 16bit, total 8 bytes)
        buf[0]  = (server.read()<<0); //1
        buf[0] |= (server.read()<<8);
        if(buf[0] == 0xBEEF)
        {
          buf[1]  = (server.read()<<0); //2
          buf[1] |= (server.read()<<8);
          buf[2]  = (server.read()<<0); //3
          buf[2] |= (server.read()<<8);
          buf[3]  = (server.read()<<0); //4
          buf[3] |= (server.read()<<8);
          if((buf[1] == 0) &&
             (buf[2] == 0) &&
             (buf[3] == 0))
          {
            clearall(0);
          }
          else
          {
            lcd.fillRect(buf[1]-2, buf[2]-2, buf[1]+2, buf[2]+2, buf[3]);
          }
        }
      }
    }
  }
  else //client
  {
    if(!client.connected())
    {
      if(client.connect() == 0) //connect to server
      {
        client.stop();
        if(++connection >= 3)
        {
          while(startwifi() != 0){ delay(1000); }
        }
      }
      else
      {
        connection = 0;
      }
    }
    else
    {
      while(client.available() >= 8) //data available?
      {
        //0xBEEF, x, y, color (all 16bit, total 8 bytes)
        buf[0]  = (client.read()<<0); //1
        buf[0] |= (client.read()<<8);
        if(buf[0] == 0xBEEF)
        {
          buf[1]  = (client.read()<<0); //2
          buf[1] |= (client.read()<<8);
          buf[2]  = (client.read()<<0); //3
          buf[2] |= (client.read()<<8);
          buf[3]  = (client.read()<<0); //4
          buf[3] |= (client.read()<<8);
          if((buf[1] == 0) &&
             (buf[2] == 0) &&
             (buf[3] == 0))
          {
            clearall(0);
          }
          else
          {
            lcd.fillRect(buf[1]-2, buf[2]-2, buf[1]+2, buf[2]+2, buf[3]);
          }
        }
      }
    }
  }

  //service routine for touch panel
  tp.service();

  //touch press?
  if((tp.getPressure() > 10) &&
     (((working_mode == 0) && server.connected() && !server.available()) || //server
      ((working_mode == 1) && client.connected() && !client.available())))   //client
  {
    x = tp.getX();
    y = tp.getY();
 
    //change color
    if(x < 20)
    {
      last_x = 0;
      switch(color)
      {
        case COLOR_BLACK: color = COLOR_RED;   break;
        case COLOR_RED:   color = COLOR_GREEN; break;
        case COLOR_GREEN: color = COLOR_BLUE;  break;
        case COLOR_BLUE:  color = COLOR_BLACK; break;
      }
      lcd.fillRect(0, 0, 20, lcd.getHeight(), color);
      while(tp.getPressure() > 5){ tp.service(); }
    }

    //clear screen
    else if(x > (lcd.getWidth()-20))
    {
      clearall(1);
      while(tp.getPressure() > 5){ tp.service(); }
    }

    //draw line
    else if((last_x == 0) ||
            ((x >= (last_x-10)) && (x <= (last_x+10)) && (y >= (last_y-10)) && (y <= (last_y+10))))
    {
      if((x != last_x) || (y != last_y))
      {
        last_x = x;
        last_y = y;
        lcd.fillRect(x-2, y-2, x+2, y+2, color);

        //0xBEEF, x, y, color (all 16bit, total 8 bytes)
        buf[0] = 0xBEEF;
        buf[1] = x;
        buf[2] = y;
        buf[3] = color;
        if(working_mode == 0) //server
        {
          server.write((uint8_t*)buf, 8);
        }
        else
        {
          client.write((uint8_t*)buf, 8);
        }
      }
    }
  }
  else
  {
    last_x = 0;
  }
}
