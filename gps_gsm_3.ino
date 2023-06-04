#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

#define VIBRATION_SENSOR_PIN    A0
TinyGPSPlus gps;
SoftwareSerial SerialGPS(D2, D3); // GPS module connected to GPIO2 (D2) for Rx and GPIO3 (D3) for Tx on ESP8266
SoftwareSerial gsmSerial(D4, D5); // GSM module connected to GPIO4 (D4) for Rx and GPIO5 (D5) for Tx on ESP8266

const char* ssid = "Maitri";
const char* password = "12345678";

float Latitude, Longitude;
int year, month, date, hour, minute, second;
String DateString, TimeString, LatitudeString, LongitudeString;

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  SerialGPS.begin(9600);
  gsmSerial.begin(9600);

  pinMode(VIBRATION_SENSOR_PIN, INPUT);

  Serial.println();
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");
  Serial.println(WiFi.localIP());
}

void loop()
{
  
  

   while (SerialGPS.available() > 0)
    if (gps.encode(SerialGPS.read()))
    {
      if (gps.location.isValid())
      {
        Latitude = gps.location.lat();
        LatitudeString = String(Latitude , 6);
        Longitude = gps.location.lng();
        LongitudeString = String(Longitude , 6);
      }

      if (gps.date.isValid())
      {
        DateString = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();

        if (date < 10)
        DateString = '0';
        DateString += String(date);

        DateString += " / ";

        if (month < 10)
        DateString += '0';
        DateString += String(month);
        DateString += " / ";

        if (year < 10)
        DateString += '0';
        DateString += String(year);
      }

      if (gps.time.isValid())
      {
        TimeString = "";
        hour = gps.time.hour()+5; //adjust UTC
        minute = gps.time.minute()-30;
        second = gps.time.second();
    
        if (hour < 10)
        TimeString = '0';
        TimeString += String(hour);
        TimeString += " : ";

        if (minute < 10)
        TimeString += '0';
        TimeString += String(minute);
        TimeString += " : ";

        if (second < 10)
        TimeString += '0';
        TimeString += String(second);
      }

  }
  
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  //Response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE html> <html> <head> <title>NEO-6M GPS Readings</title> <style>";
  s += "table, th, td {border: 1px solid blue;} </style> </head> <body> <h1  style=";
  s += "font-size:300%;";
  s += " ALIGN=CENTER>NEO-6M GPS Readings</h1>";
  s += "<p ALIGN=CENTER style=""font-size:150%;""";
  s += "> <b>Location Details</b></p> <table ALIGN=CENTER style=";
  s += "width:50%";
  s += "> <tr> <th>Latitude</th>";
  s += "<td ALIGN=CENTER >";
  s += LatitudeString;
  s += "</td> </tr> <tr> <th>Longitude</th> <td ALIGN=CENTER >";
  s += LongitudeString;
  s += "</td> </tr> <tr>  <th>Date</th> <td ALIGN=CENTER >";
  s += DateString;
  s += "</td></tr> <tr> <th>Time</th> <td ALIGN=CENTER >";
  s += TimeString;
  s += "</td>  </tr> </table> ";
 

  if (gps.location.isValid())
  {
    s += "<p align=center><a style=""color:RED;font-size:125%;"" href=""http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
    s += LatitudeString;
    s += "+";
    s += LongitudeString;
    s += """ target=""_top"">Click here</a> to open the location in Google Maps.</p>";
  }

  s += "</body> </html> \n";

  client.print(s);
  delay(100);

 int sensorValue = analogRead(VIBRATION_SENSOR_PIN);
 Serial.println(sensorValue);
 if (sensorValue > 10) {
 // Send an SMS
  String phoneNumber = "+919328825122"; // Replace with the desired phone number
  String message = "Hello from ESP8266!"; // Replace with the desired message

  gsmSerial.println("AT+CMGF=1"); // Set SMS mode to text
  delay(100);
  gsmSerial.println("AT+CMGS=\"" + phoneNumber + "\""); // Specify the phone number
  delay(100);
  gsmSerial.println(message); // Specify the message content
  delay(100);
  gsmSerial.println("Latitude- " + LatitudeString); // Specify the message content
  delay(100);
  gsmSerial.println("Longitude- " + LongitudeString); // Specify the message content
  delay(100);
  gsmSerial.println("Date- " + DateString); // Specify the message content
  delay(100);
  gsmSerial.println("Time- " + TimeString); // Specify the message content
  delay(100);

  gsmSerial.write(26); // Send the Ctrl+Z character to indicate the end of the message
  delay(100);

}
}