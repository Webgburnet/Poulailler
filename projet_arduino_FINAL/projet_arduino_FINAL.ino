#include <SPI.h>
#include <Ethernet2.h>
#include <Servo.h>
#include "DS1307.h" 

Servo servo_door;
Servo servo_food;

DS1307 clock;//define a object of DS1307 class

//--------------------------------------------------------------------------------------------------------------------------------------------
//int hour;
//int minute;
//int second;
//--------------------------------------------------------------------------------------------------------------------------------------------

//byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0x21, 0xAD}; //adresse mac de votre carte
byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0xFD, 0xAD}; //adresse mac de votre carte
byte ip[] = {192, 168, 1, 123}; //adresse IP
EthernetServer server(80);                             //default server port
String readString;

int pinRelais = 2; //variable de pin pour le Relais
boolean etatRelais = 0; //etat du pin
int porte_fermee=180;
int porte_ouverte=0;
int food_ok=20;
int Food_Pas_Ok=2;

//////////////////////

void setup(){

  pinMode(pinRelais, OUTPUT); //pin selected to control
  pinMode(6, OUTPUT); //pin selected to control
  pinMode(5, OUTPUT); //pin selected to control
  servo_door.attach(A1);  //the pin for the servo control
  servo_food.attach(A0);
  //myservo.write(110);
  delay(100);
  //start Ethernet
  Ethernet.begin(mac, ip);
  server.begin();

  clock.begin();
  clock.fillByYMD(2018,5,30);//mar 15,2018
  clock.fillByHMS(15,59,40);//15:59 00"
  clock.fillDayOfWeek(WED);//Saturday
  clock.setTime();//write time to the RTC chip
  

   //enable serial data print 
  Serial.begin(9600); 
  Serial.println("server servo/pin 5 test 1.0"); // so I can keep track of what is loaded
}

void loop(){
  clock.getTime();
    Serial.print(clock.hour, DEC);
    Serial.print("H");
    Serial.print(clock.minute, DEC);
    Serial.print("m");
    Serial.print(clock.second, DEC);
    Serial.print("s  ");
    Serial.print(clock.dayOfMonth, DEC);
    Serial.print("/");
    Serial.print(clock.month, DEC);
    Serial.print("/");
    Serial.print(clock.year, DEC);
    Serial.print("    ");

//--------------------------------------------------------------------------------------------------------------------------------------------
//second = clock.second,DEC;
//minute = clock.minute,DEC;
//hour = clock.hour,DEC;
//--------------------------------------------------------------------------------------------------------------------------------------------

    switch (clock.dayOfWeek)// Friendly printout the weekday    
    {
        case MON:
        Serial.print("LUNDI");
        break;
        case TUE:
        Serial.print("MARDI");
        break;
        case WED:
        Serial.print("MERCREDI");
        break;
        case THU:
        Serial.print("JEUDI");
        break;
        case FRI:
        Serial.print("VENDREDI");
        break;
        case SAT:
        Serial.print("SAMEDI");
        break;
        case SUN:
        Serial.print("DIMANCHE");
        break;
    }
    Serial.println(" ");
 
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

          ///////////////
          Serial.println(readString); //print to serial monitor for debuging 

          client.println("HTTP/1.1 200 OK"); //send new page
          client.println("Content-Type: text/html");
          client.println();

          //balises d'entête
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head><title>Poullailler</title>");
           client.println("<h1 align=\"center\"> <font color=\"E00000\"> Poulailler automatise a distance</font></h1></head>");
          client.println("<body style=\"background-color:grey; color:white\">");
          
          client.print("<br><h3>Pompe a eau (cycle de 2s) : </h3>");
          client.println("<a href=\"/?relais\"\"><input type=\"button\" name=\"Marche \"value=\"Marche\"/></a>");
          
          client.print("<br><h3>Nourriture : </h3>");
          client.println("<a href=\"/?food\"\"><input type=\"button\" name=\"Marche \"value=\"Marche\"/></a>");
          
          client.print("<br><h3>Porte : </h3>"); 
          client.println("<a href=\"/?ouvert\"\"><input type=\"button\" name=\"Marche \"value=\"Ouverture\"/></a>");
          client.println("<a href=\"/?ferme\"\"><input type=\"button\" name=\"Marche \"value=\"Fermeture\"/></a>");
          
          client.print("<br><h3>Camera IP : </h3><br>"); 
          client.println("<img id=\"MJPEG_streaming\" src=\"http://192.168.1.210/snapshot.cgi\" alt=\"CAMERA IP NON CONNECTEE\"><br>");
          client.println("<a href=\"http://192.168.1.210\"\"> Cliquez ici</a><br>");
          client.println("</BODY>");
          client.println("</HTML>");
 
          delay(1);
          //stopping client
          client.stop();

          ///////////////////// control arduino pin
          if(readString.indexOf("?relais") >0)//checks for on
          {
            etatRelais = 1; //on change l'état du relais
            digitalWrite(pinRelais, etatRelais); //on met à jour le pin
            Serial.println(etatRelais);
            delay(2000);
            etatRelais = 0; //on change l'état du relais
            digitalWrite(pinRelais, etatRelais); //on met à jour le pin
            Serial.println(etatRelais);
            Serial.println("relais");
          }
          if(readString.indexOf("?food") >0)//checks for on
          {
            servo_food.write(food_ok);
            delay(1500);
            servo_food.write(Food_Pas_Ok);
          delay(1500);
          Serial.println("porte");
          }
          if(readString.indexOf("?ouvert") >0)//checks for off
          {
            servo_door.write(porte_ouverte);
            Serial.println("porte ouverte");
            digitalWrite(5, LOW);    // set pin 4 low
            Serial.println("Led Off");
          }
          if(readString.indexOf("?ferme") >0)//checks for off
          {
            servo_door.write(porte_fermee);
            Serial.println("porte ferme");
          }
          //clearing string for next read
          readString="";
/*
//--------------------------------------------------------------------------------------------------------------------------------------------
          if(hour == 8 || hour == 12 || hour == 16 && second == 0 && minute == 0)//checks for on
          {
            servo_food.write(food_ok);
            delay(1500);
            servo_food.write(Food_Pas_Ok);
          delay(1500);
          Serial.println("porte");
          }
          if(hour == 10  && second == 0 && minute == 0)//checks for off
          {
            servo_door.write(porte_ouverte);
            Serial.println("porte ouverte");
            digitalWrite(5, LOW);    // set pin 4 low
            Serial.println("Led Off");
          }
          if(hour == 19 && second == 0 && minute == 0)//checks for off
          {
            servo_door.write(porte_fermee);
            Serial.println("porte ferme");
          }
          //clearing string for next read
          readString="";
//--------------------------------------------------------------------------------------------------------------------------------------------
*/
        }
      }
    }
  }
} 
