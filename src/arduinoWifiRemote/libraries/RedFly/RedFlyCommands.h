#ifndef REDFLYCOMMANDS_h
#define REDFLYCOMMANDS_h


#define CMD_OK          "OK"
#define CMD_ERROR       "ERROR"


//Command
#define CMD_FWVERSION   "AT+RSI_FWVERSION?" //firmware version
  //ret: OKMajor.Minor1.Minor2

#define CMD_RESET       "AT+RSI_RESET" //soft reset
  //ret: OK

#define CMD_BAUDRATE    "AT+RSI_BAUDRATE=" //baud_rate
  //ret: OK

#define CMD_MAC         "AT+RSI_MAC?" //get MAC address of the module
  //ret: OKabcdef

#define CMD_RSSI        "AT+RSI_RSSI?" //get signal strength for current connection
  //ret: OKa

#define CMD_NWPARAMS    "AT+RSI_NWPARAMS?" //get module paramters
  //        32    1         32   1    6        1                     4       4       4       1
  //ret: OK<SSID><SEC_TYPE><PSK><Chn><MACaddr><DHCP_MODE/ERROR_CODE><IPaddr><SubNet><Gateway><NO_OF_OPEN_SOCKET>   
  //       [<S_ID1><S_TYPE1><Sport1><Dport1><Dip1>][<S_ID2><S_TYPE2><Sport2><Dport2><Dip2>] (...up to <NO_OF_OPEN_SOCKET>)
  //       <DNS_Server_Address> 

#define CMD_BAND        "AT+RSI_BAND=" //set band
#define BAND24          "0"            //set 2.4GHz
#define BAND5           "1"            //set 5GHz

#define CMD_INIT        "AT+RSI_INIT" //execut after BAND

#define CMD_SCAN        "AT+RSI_SCAN=" //chn,SSID (SSID for hidden Access Points, chn=0 -> all channels)
  //ret: OK<uSSID1uMode1uRSSIval1><uSSID2uMode2uRSSIval2>...
#define CMD_NEXTSCAN    "AT+RSI_NEXTSCAN"
  //ret: OK<uSSID1uMode1uRSSIval1><uSSID2uMode2uRSSIval2>...
#define CMD_NUMSCAN     "AT+RSI_NUMSCAN=" //set number
#define CMD_GETNUMSCAN  "AT+RSI_NUMSCAN?" //get number
#define CMD_NWTYPE      "AT+RSI_NWTYPE?" //get network type (0=Ad-hoc, 1=Infrastructure)
  //ret: OK<SSID1NwType1><SSID2NwType2>...
#define CMD_BSSID       "AT+RSI_BSSID?" //get network MAC
  //ret: OK<SSID1BSSID1><SSID2BSSID2>...
#define MODE_OPEN       "0"
#define MODE_WPA        "1"
#define MODE_WPA2       "2"
#define MODE_WEP        "3"
  //chn: 0=all, 
  //     2.4GHz 1...14,
  //     5GHz   36,40,44,48,52,56,60,64,100,104,108,112,116,136,140,149,153,157,161,165
  //ret: OK<uSSID1uMode1uRSSIval1><uSSID2uMode2uRSSIval2>� 

#define CMD_NETWORK     "AT+RSI_NETWORK=" //INFRASTRUCTURE or IBSS,type,chn
  //IBSS type: 0 indicates IBSS Joiner and 1 indicates IBSS Creator,
  //for Joiner mode, chn has to be 0

#define CMD_PSK         "AT+RSI_PSK=" //maximum length is 32 characters

#define CMD_AUTHMODE    "AT+RSI_AUTHMODE=" //0 Open or 1 Shared Key 
#define AUTHMODE_OPEN   "0"     //Open
#define AUTHMODE_SK     "1"     //Shared Key 

#define CMD_JOIN        "AT+RSI_JOIN=" //SSID,TxRate(1 byte),TxPower(1 byte) 
#define JOIN_LOWPW      ",0,0"   //auto data rate,       7dBm (TxPower)
#define JOIN_MEDPW      ",0,1"   //auto data rate,      10dBm (TxPower)
#define JOIN_HIGHPW     ",0,2"   //auto data rate, 16...17dBm (TxPower)

#define CMD_DISCONN     "AT+RSI_DISASSOC" //disassociate / disconnect

#define CMD_PWMODE      "AT+RSI_PWMODE=" //power mode 0, 1 or 2

#define CMD_SLEEPTIMER  "AT+RSI_SLEEPTIMER=" //milliseconds, max 10000


//TCP/IP Commands
/*
#define ERROR_1          (-1) //255 Waiting for the connection from peer.
#define ERROR_2          (-2) //254 Socket not available.
#define ERROR_3          (-3) //253 Deauthentication from the Access Point.
#define ERROR_4          (-4) //252 Illegal IP/Port parameters.
#define ERROR_5          (-5) //251 TCP/IP configuration failure.
#define ERROR_6          (-6) //250 Invalid socket.
#define ERROR_7          (-7) //249 Association not done.
#define ERROR_8          (-8) //248 Error in cmd.
#define ERROR_9          (-9) //247 Error with byte stuffing for escape characters.
#define ERROR_10        (-10) //246 IP Lease expired.
#define ERROR_11        (-11) //245 TCP Connection closed.
#define ERROR_12        (-12) //244 Pre-Shared Key not set for connection to a secure Access Point.
#define ERROR_13        (-13) //243 No Access Points scanned 
#define ERROR_14        (-14) //242 INIT cmd already issued. This error is sent when the INIT cmd is sent more than once.
#define ERROR_15        (-15) //241 JOIN cmd already issued. This error is sent when the JOIN cmd is sent more than once.
#define ERROR_16        (-16) //240 DHCP Failure.
#define ERROR_17        (-17) //239 Baud Rate Not Supported.
*/

#define CMD_IPCONF      "AT+RSI_IPCONF=" //DHCP_MODE(0=manual, 1=DHCP),IP,SUBNET,GATEWAY
  //ret: OKMACaddrIPaddrSUBNETGateway
#define IPCONF_DHCP     "1,0,0"
#define IPCONF_AUTOIP   "2,0,0"

#define CMD_DNSSERVER   "AT+RSI_DNSSERVER=" //DNS server
  //ret: OK
  
#define CMD_DNSGET      "AT+RSI_DNSGET=" // DNS Resolution
  //ret: OKx...
  //       x = number of IP addresses

#define CMD_TCP         "AT+RSI_TCP=" //host,port,lport         //Open and Connect to a TCP Socket
  //ret: OKsock_handle

#define CMD_LTCP        "AT+RSI_LTCP=" //lport                  //Open a Listening TCP Socket
  //ret: OKsock_handle

#define CMD_UDP         "AT+RSI_UDP=" //host,port,lport         //Open a UDP Socket
  //ret: OKsock_handle

#define CMD_LUDP        "AT+RSI_LUDP=" //lport                  //Open a Listening UDP Socket
  //ret: OKsock_handle

#define CMD_MCAST       "AT+RSI_ MULTICAST=" //host,port,lport  //Open a Multicast Socket
  //ret: OKsock_handle

#define CMD_CTCP        "AT+RSI_CTCP=" //hn                     //Get a Listening socket's active conn. status
  //ret: OKChnIP_AddressPort
  //       Chn >=0: A handle to an active connected socket 
  //       Chn =-1: No connection has been established

#define CMD_CLS         "AT+RSI_CLS=" //hn                      //Close a Socket
  //ret: OK

#define CMD_SEND        "AT+RSI_SND=" //hn,sz,Dip,Dport,Data    //Send a byte stream to a Socket
  //ret: OKsz (sz in 2bytes)

#define CMD_READ        "AT+RSI_READ" //hn,sz,Sip,Sport,Data    //Receive a byte stream on a Socket


#endif //REDFLYCOMMANDS_h
