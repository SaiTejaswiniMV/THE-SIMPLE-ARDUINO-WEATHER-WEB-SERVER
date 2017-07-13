/*

 Arduino with Ethernet Shield */

#include <SPI.h>
#include <Ethernet.h>
#include <dht11.h>
#include <Wire.h>
int led = 4;
#define DHT11PIN 2
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };


byte ip[] = {192,168,137, 120};                      // ip in lan (that's what you need to use in your browser. ("192.168.1.178")
//byte gateway[] = { 192, 168, 1, 1 };                   // internet access via router
//byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask
EthernetServer server(80);    //server port
dht11 DHT11;     
String readString;

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  pinMode(led, OUTPUT);
 
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {   
      if (client.available()) {
        char c = client.read();
     
        //read char by char HTTP request
        if (readString.length() < 100) {
          //store characters to string
          readString += c;
          //Serial.print(c);
         }

         //if HTTP request has ended
         if (c == '\n') {          
           Serial.println(readString); //print to serial monitor for debuging
     
           client.println("HTTP/1.1 200 OK"); //send new page
           client.println("Content-Type: text/html");
           client.println();     
           client.println("<HTML>");
           client.println("<HEAD>");
           client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
           client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
           client.println("<link rel='stylesheet' type='text/css' href='http://randomnerdtutorials.com/ethernetcss.css' />");
           client.println("<TITLE>Ethernet Shield Project</TITLE>");
           client.println("</HEAD>");
           client.println("<BODY>");
           client.println("<H1>A Simple Arduino Web Server For Home Automation</H1>");
           client.println("<hr />");
           client.println("<br />");  
           client.println("<H2>Monitoring Lights</H2>");
           client.println("<br />");  
           client.println("<a href=\"/?button1on\"\">Turn On Light</a>");
           client.println("<a href=\"/?button1off\"\">Turn Off Light</a><br />");   
           client.println("<br />");     
           client.println("<br />"); 
           int chk = DHT11.read(DHT11PIN);

          Serial.print("Read sensor: ");
          switch (chk)
          {
          case 0: 
            Serial.println("OK"); 
            break;
          case -1: 
            Serial.println("Checksum error"); 
            break;
          case -2: 
            Serial.println("Time out error"); 
            break;
          default: 
            Serial.println("Unknown error"); 
            break;
          }  
          client.print("<center>");
          client.println("<h3> Weather Report </h2>");
          client.print("<font color=blue size=4>");
          client.print("Temperature (C): ");
          client.println((float)DHT11.temperature, 1);  
          client.println("<br />");  

          client.print("<font color=blue size=4>");
          client.print("Temperature (F): ");
          client.println(Fahrenheit(DHT11.temperature), 1);
          client.println("<br />");  
          
          client.print("<font color=blue size=4>");
          client.print("Humidity (%): ");
          client.println((float)DHT11.humidity, 0);  
          client.println("<br />");  
          
          client.print("<font color=blue size=4>");
          client.print("Temperature (K): ");
          client.println(Kelvin(DHT11.temperature), 1);
          client.println("<br />");   
          client.print("<font color=blue size=4>");
          client.print("Dew Point (C): ");
          client.println(dewPoint(DHT11.temperature, DHT11.humidity));
          client.println("<br />");   
          client.print("<font color=blue size=4>");
         client.print("Dew PointFast (C): ");
         client.println(dewPointFast(DHT11.temperature, DHT11.humidity));
         client.println("<br />");   
         client.print("</center>");
     
           delay(1);
           //stopping client
           client.stop();
           //controls the LED lights connected to Arduino if you press the buttons
           if (readString.indexOf("?button1on") >0){
               
               digitalWrite(led, HIGH);
               client.println("<center><h4>TURNED ON</h4></center>");
           }
           if (readString.indexOf("?button1off") >0){
               digitalWrite(led, LOW);
               client.println("<center><h4>TURNED OFF</h4></center>");
           }
           client.println("</BODY>");
           client.println("</HTML>");
            //clearing string for next read
            readString="";  
           
         }
       }
    }
}
}
double Fahrenheit(double celsius)
{
  return 1.8 * celsius + 32;
}

//Celsius to Kelvin conversion
double Kelvin(double celsius)
{
  return celsius + 273.15;
}

// dewPoint function NOAA
// reference: http://wahiduddin.net/calc/density_algorithms.htm 
double dewPoint(double celsius, double humidity)
{
  double A0= 373.15/(273.15 + celsius);
  double SUM = -7.90298 * (A0-1);
  SUM += 5.02808 * log10(A0);
  SUM += -1.3816e-7 * (pow(10, (11.344*(1-1/A0)))-1) ;
  SUM += 8.1328e-3 * (pow(10,(-3.49149*(A0-1)))-1) ;
  SUM += log10(1013.246);
  double VP = pow(10, SUM-3) * humidity;
  double T = log(VP/0.61078);   // temp var
  return (241.88 * T) / (17.558-T);
}

// delta max = 0.6544 wrt dewPoint()
// 5x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
  double a = 17.271;
  double b = 237.7;
  double temp = (a * celsius) / (b + celsius) + log(humidity/100);
  double Td = (b * temp) / (a - temp);
  return Td;
}
/* ( THE END ) */
