#include <EtherCard.h>  

static byte mymac[] = { 
  0x5A,0x5A,0x5A,0x5A,0x5A,0x5A };

byte Ethernet::buffer[900];
BufferFiller bfill;

int LedPins[] = {
  2,3,4,5};

boolean PinStatus[] = {
  1,2,3,4};

//-------------

const char http_OK[] PROGMEM =
"HTTP/1.0 200 OK\r\n"
"Content-Type: text/html\r\n"
"Pragma: no-cache\r\n\r\n";

const char http_Found[] PROGMEM =
"HTTP/1.0 302 Found\r\n"
"Location: /\r\n\r\n";

const char http_Unauthorized[] PROGMEM =
"HTTP/1.0 401 Unauthorized\r\n"
"Content-Type: text/html\r\n\r\n"
"<h1>401 Unauthorized</h1>";

//------------

// Ðîáèìî ôóíêö³þ äëÿ îôîðìëåííÿ íàøî¿ Web ñòîð³íêè. 
void homePage()
{
  bfill.emit_p(PSTR("$F"
    "<title>Microsek Webserver</title>" 
    "Micriosek Arduino ethernet webserver<br>"
    "<br>"
    "Relay 1: <a href=\"?ArduinoPIN1=$F\">$F</a><br />"
    "Relay 2: <a href=\"?ArduinoPIN2=$F\">$F</a><br />"  
    "Relay 3: <a href=\"?ArduinoPIN3=$F\">$F</a><br />"
    "Relay 4: <a href=\"?ArduinoPIN4=$F\">$F</a>"),   

  http_OK,
  PinStatus[1]?PSTR("off"):PSTR("on"),
  PinStatus[1]?PSTR("<font color=\"green\"><b>ON</b></font>"):PSTR("<font color=\"red\">OFF</font>"),
  PinStatus[2]?PSTR("off"):PSTR("on"),
  PinStatus[2]?PSTR("<font color=\"green\"><b>ON</b></font>"):PSTR("<font color=\"red\">OFF</font>"),
  PinStatus[3]?PSTR("off"):PSTR("on"),
  PinStatus[3]?PSTR("<font color=\"green\"><b>ON</b></font>"):PSTR("<font color=\"red\">OFF</font>"),
  PinStatus[4]?PSTR("off"):PSTR("on"),
  PinStatus[4]?PSTR("<font color=\"green\"><b>ON</b></font>"):PSTR("<font color=\"red\">OFF</font>"));
}

//------------------------

void setup () {
  Serial.begin(57600);
  Serial.println(F("\n[GET IP forn DHCP]"));

  Serial.print("MAC: ");
  for (byte i = 0; i < 6; ++i) {
    Serial.print(mymac[i], HEX);
    if (i < 5)
      Serial.print(':');
  }
  Serial.println();
  
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println(F("Failed to access Ethernet controller"));

  Serial.println(F("Setting up DHCP"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));
  
  ether.printIp("My IP: ", ether.myip);
  ether.printIp("Netmask: ", ether.netmask);
  ether.printIp("GW IP: ", ether.gwip);
  ether.printIp("DNS IP: ", ether.dnsip);
  for (int i = 0; i <= 3; i++)pinMode(LedPins[i],OUTPUT);
}

// --------------------------------------

void loop()
{

  delay(1); // Ñìèêàºì ì³êðîêîíòðîëåð.

  word len = ether.packetReceive();   // check for ethernet packet 
  word pos = ether.packetLoop(len);   // check for tcp packet 

  if (pos) {
    bfill = ether.tcpOffset();
    char *data = (char *) Ethernet::buffer + pos;
    if (strncmp("GET /", data, 5) != 0) {
      bfill.emit_p(http_Unauthorized);
    }
   
   
    else {

      data += 5;
      if (data[0] == ' ') {       
        homePage(); 
        for (int i = 0; i <= 3; i++)digitalWrite(LedPins[i],!PinStatus[i+1]);
      }

      // "16" = ê³ëüê³ñòü ñèìâîë³â "?ArduinoPIN1=on ".
      else if (strncmp("?ArduinoPIN1=on ", data, 16) == 0) {
        PinStatus[1] = true;        
        bfill.emit_p(http_Found);
      }
      else if (strncmp("?ArduinoPIN2=on ", data, 16) == 0) {
        PinStatus[2] = true;        
        bfill.emit_p(http_Found);
      }
      else if (strncmp("?ArduinoPIN3=on ", data, 16) == 0) {
        PinStatus[3] = true;        
        bfill.emit_p(http_Found);
      }
      else if (strncmp("?ArduinoPIN4=on ", data, 16) == 0) {
        PinStatus[4] = true;        
        bfill.emit_p(http_Found);
      
      }


      //------------------------------------------------------  


      else if (strncmp("?ArduinoPIN1=off ", data, 17) == 0) {
        PinStatus[1] = false;        
        bfill.emit_p(http_Found);
      }
      else if (strncmp("?ArduinoPIN2=off ", data, 17) == 0) {
        PinStatus[2] = false;        
        bfill.emit_p(http_Found);
      }
      else if (strncmp("?ArduinoPIN3=off ", data, 17) == 0) {
        PinStatus[3] = false;        
        bfill.emit_p(http_Found);
      }
      else if (strncmp("?ArduinoPIN4=off ", data, 17) == 0) {
        PinStatus[4] = false;        
        bfill.emit_p(http_Found);
      }
      


      //---------------------------


      else {
        // Page not found
        bfill.emit_p(http_Unauthorized);
      }
    }
    ether.httpServerReply(bfill.position());    // send http response
  }
}



