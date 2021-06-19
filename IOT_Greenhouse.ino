#include <ESP8266WiFi.h>
#include "DHT.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#define DHTTYPE DHT11

/////////INPUT PINS///////////////////
uint8_t DHTPin = 5; //D1
uint8_t mostr_pin = 4; //D2
uint8_t l8_pin = 14; //D5
/////////OUTPUT PINS///////////////////
uint8_t fan = 0; //D3
uint8_t pump = 2; //D4
uint8_t SCL_DISPLAY = 12; //D6 display
uint8_t SDA_DISPLAY = 13; //D7 display


const char* ssid     = "KKV-WiFi";
const char* password = "icecream123";

int wifi_connected=0, flag, i,j =1;

DHT dht(DHTPin, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 4);               

float Temperature = 0,Humidity = 0,  l8 = 0, mostr = 0;
String moisture_state;

WiFiClient client;
WiFiServer server(80);

// Variable to store the HTTP request
String header;


// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
 
void setup() 
{
       pinMode(fan,OUTPUT);
       pinMode(pump,OUTPUT);
       
       Serial.begin(115200);
       delay(10);
       Serial.println("");        
       Serial.println("Connecting to ");
       Serial.println(ssid); 
       delay(500);

       Wire.begin(SDA_DISPLAY,SCL_DISPLAY);
       lcd.begin();
       lcd.home(); 

       lcd.clear();
       lcd.setCursor(0,1);
       lcd.print("       IOT       ");
       lcd.setCursor(0,2);
       lcd.print("   GREEN HOUSE  ");
       delay(3000);
       lcd.clear(); 
///////////////////////////////////////////////////////////////

      for(i =1; i<50; i++){
          WiFi.begin(ssid, password);
          delay(500);
          Serial.print(".");
          lcd_connect();          
          if (WiFi.status() == WL_CONNECTED){
            Serial.println("");
            Serial.println("WiFi connected");
            lcd.setCursor(0,1);
            lcd.print("   WiFi CONNECTED    ");
            delay(1000); 
            i=100;                
         }     
      }
      
      lcd.clear();  
}

void loop() {
    
    humidity();
    temperature();
    lights();
    moisture();

    if(flag) digitalWrite(fan,HIGH);
    if(!flag) digitalWrite(fan,LOW);
    
    WiFi_Connected(Temperature,Humidity,l8,moisture_state) ;

  Serial.println("done");
  delay(5000);
}


///////////////////////////////////////////////



void humidity()
{
  Humidity = dht.readHumidity();
  Serial.print("Humidity: ");
  Serial.println(Humidity);
  lcd.setCursor(0,0);
  lcd.print("Humidity : ");
  lcd.print(Humidity);
  lcd.print("%   ");

  if     (Humidity > 60)  flag =1;    
  else   flag = 0;   
  
}

void temperature()
{
  Temperature = dht.readTemperature();
  Serial.print("Temprature: ");
  Serial.println(Temperature);
  lcd.setCursor(0,1);
  lcd.print("Temperature : ");
  lcd.print(Temperature);
  lcd.print("C");

  if     (Temperature > 40)  digitalWrite(fan,HIGH);
  else   digitalWrite(fan,LOW);
}

void lights()
{
  l8 = map(analogRead(l8_pin),0,1024,0,100);
  lcd.setCursor(0,2);
  lcd.print("Light : ");
  lcd.print(l8);
  lcd.print("%");

}

void moisture()
{
  mostr = digitalRead(mostr_pin);
  Serial.print("moisture: ");
  Serial.println(mostr);
  if (mostr == 1){
    lcd.setCursor(0,3);
    lcd.print("Moisture : ");
    lcd.print("LOW  ");
    moisture_state = "INADEQUATE";
    digitalWrite(pump,HIGH);
  }
  else {
    lcd.setCursor(0,3);
    lcd.print("Moisture : ");
    lcd.print("AMPLE");
    moisture_state = "ADEQUATE";    
    digitalWrite(pump,LOW);
  }
  
}


void WiFi_Connected(float a, float b, float f, String d) {

   if (WiFi.status() == WL_CONNECTED){
    
    if(wifi_connected==0){

      wifi_connected = 1;
  
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
    }
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
                                   
            // Display the HTML web page
            client.println("<html lang=\"en\">");
            client.println("<head>  <meta http-equiv=\"refresh\" content=\"60\" > <meta charset=\"UTF-8\">    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">  ");
            client.println("<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css\" integrity=\"sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm\" crossorigin=\"anonymous\"> </head>");
            client.println("<title>IOT GREENHOUSE_DATA_PANEL</title>");
            client.println("<body>    <h1 class = \"text-center\">IOT GREENHOUSE</h1>");
            client.println("<div class=\"row\">");
            client.println("<div class=\"rounded col-6 text-right\"> <img src=\"https://i.imgur.com/Q6HxDam.jpg\" loading = \"lazy\" height = \"200\" width = \"200\">  <br> <p class = \"text-center\">VALUE : ");
            client.println(a);
            client.println(" Celsius</p></div>  <div class=\"rounded col-6 text-left\">   <img src=\"https://i.imgur.com/6fUBMaS.jpg\"  loading = \"lazy\" height = \"200\" width = \"200\">        <br><p class = \"text-center\">VALUE: ");
            client.println(b);
            client.println("% </p></div>  <div class=\"rounded col-6 text-right\">        <img src=\"https://i.imgur.com/giaUl73.jpg\"  loading = \"lazy\" height = \"200\" width = \"200\">        <br> <p class = \"text-center\">VALUE:");
            client.println(f);
            client.println(" lum</p> </div>        <div class=\"rounded col-6 text-left\">        <img src=\"https://i.imgur.com/8HoWIVT.jpg\"  loading = \"lazy\" height = \"200\" width = \"200\" >        <br><p class = \"text-center\">");
            client.println(d); 
            client.println(" </p></div>    </div>");
            client.println(" <div class=\"text-center\">    <br><br>  <a href=\"/refreshpage\" class=\"btn btn-primary\" role=\"button\">Refresh</a></div>");
            client.println("<script src=\"https://code.jquery.com/jquery-3.2.1.slim.min.js\"  integrity=\"sha384-KJ3o2DKtIkvYIK3UENzmM7KCkRr/rE9/Qpg6aAZGJwFDMVNA/GpGFF93hXpG5KkN\" crossorigin=\"anonymous\"></script>");
            client.println("<script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.12.9/umd/popper.min.js\" integrity=\"sha384-ApNbgh9B+Y1QKtv3Rn7W3mgPxhU9K/ScQsAP7hUibX39j7fakFPskvXusvfa0b4Q\"  crossorigin=\"anonymous\"></script>");
            client.println("<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/js/bootstrap.min.js\" integrity=\"sha384-JZR6Spejh4U02d8jOt6vLEHfe/JQGiRRSQQxSfFWpi1MquVdAyjUar5+76PVCmYl\" crossorigin=\"anonymous\"></script>");
            client.println("</body> </html>");

           
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
      }      
    }
  }

  void lcd_connect(){
    lcd.setCursor(0,1);
          lcd.print("Connecting tO WiFi");
          if(i-j==0){lcd.setCursor(18,1); lcd.print("  ");}
          if(i-j==1){lcd.setCursor(18,1); lcd.print(". ");}
          if(i-j==2){lcd.setCursor(18,1); lcd.print(".."); j+=3;}
    }
