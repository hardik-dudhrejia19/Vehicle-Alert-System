 #include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <ESP8266WebServer.h>


#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* host = "192.168.4.1";                 //Ip of the Host(Our Case esp8266-01 as server. Its the ip of the esp8266-01 as Access point)
const char *ssid = "ESPA";
const char *pass = "";
ESP8266WebServer server(80);
const int buzzer = D1;
int count = 0;
int hopcount;
int cs;



void handleRoot() {
//toggle=!toggle;               
//Toggling Led Variable
//digitalWrite(2,toggle);     
//Toggle Led
//char h = hopcount;
        Serial.println("Inside handleRoot");
        String s = "\r\n\r\nAlert! Make Way ";
        s = s + "SAVE THE SOUL! \r\n\r\n";
        server.send(200,"text/html",s);      //Reply to the client
        Serial.println("Message sent");
}

void serverr()
{
  delay(200);                           //Stable Wifi
  Serial.begin(9600);                 //Set Baud Rate
  pinMode(2, OUTPUT);                   //Led/Solenoid at pin 2
  WiFi.softAP(ssid,pass);//, password);      //In Access Point Mode

  IPAddress myIP = WiFi.softAPIP();     //Check the IP assigned. Put this Ip in the client host.
  Serial.print("AP IP address: ");
  Serial.println(myIP);                 //Print the esp8266-01 IP(Client must also be on th e save IP series)
  server.on("/", handleRoot);           //Checking client connection
  server.begin();                       // Start the server
  Serial.println("Server started");
  delay(120000); 
  cs = 0;
  WiFi.softAPdisconnect(true);
  clientt();
  
  }
void clientt() {
  Serial.begin(9600);          //Baud Rate for Communication
  delay(10);                     //Baud rate prper initialization
//  pinMode(13, INPUT);            //Pin D7 on NodeMcu Lua. Button to switch on and off the solenoid.
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);           //NodeMcu esp12E in station mode
  WiFi.begin("ESPA","");      //Connect to this SSID. In our case esp-01 SSID.

  while (WiFi.status() != WL_CONNECTED) {      //Wait for getting IP assigned by Access Point/ DHCP.
    //Our case  esp-01 as Access point will assign IP to nodemcu esp12E.
    delay(10);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());             //Check out the Ip assigned by the esp12E
  //Check out if Pin D7/13 is high. If high activate
  //the led connected to esp-01. On next press deactivate it.
  Serial.print("connecting to ");
  Serial.println(host);
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect("192.168.4.1", httpPort)) {
    Serial.println("connection failed");
    return;
  }
  //Request to server to activate the led
  client.print(String("GET ") + "/" + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(10);
  // Read all the lines of the reply from server and print them to Serial Monitor etc
  Wire.begin(2,0);
  lcd.init();   // initializing the LCD
  lcd.backlight(); 
//  lcd.setCursor(0,0);
//  Serial.println("Alert! Make Way");M
//  lcd.print("Alert! Make Way");
//  lcd.setCursor(0,1);
//  Serial.println("SAVE THE SOUL!");
//  lcd.print("SAVE THE SOUL!");// Enable or Turn On the backlight 
  String first = "";
  String second = "";
  Serial.println("Before while loop");
  while (client.available() || client.connected()) {
    
    //if(client.available()){
      
    String line = client.readStringUntil('\n');
    int i=0;
   // int hopcount;
    if(count == 7){
      for(i = 0;i < line.length()-2 ; i++)
      {
        if(i<16)
        first = first + line[i];
        else
        second = second + line[i];
        }
      char temp = line[i];
     // Serial.println(temp);
       hopcount = temp - '0';
   
      }
      Serial.println("First"+first);
      Serial.println("Second"+second);
      lcd.setCursor(0,0);
      lcd.print(first);
      lcd.setCursor(0,1);
      lcd.print(second);
    //}
    count = count + 1;
    }
  
  for(int i=0 ; i<850 ; i++){ // loop for to increase the sound frequency
  tone(buzzer, 2700);
  //Serial.println(i);
  delay(10); //You can change the delay if you want to make it longer or shorter
}
 noTone(buzzer);
// digitalWrite(led,LOW);
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
  Serial.print(hopcount);
  if(hopcount > 0)
  {
    hopcount = hopcount - 1;
    cs = 1;
    serverr();
    }
  //Close the Connection. Automatically
  Serial.println();
  Serial.println("closing connection");
  //End if
}
void setup()
{
   clientt();
  }

void loop() {
// Check if a client has connected. On first connection switch on the Solenoid on next switch off.
  server.handleClient();
}//End Loop
