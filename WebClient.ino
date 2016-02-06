// Programa: Web Server com modulo ESP8266
// Alteracoes e adaptacoes: FILIPEFLOP
 
#include <SoftwareSerial.h>
 
//RX pino 2, TX pino 3
SoftwareSerial esp8266(2, 3);
 
#define DEBUG true
#define AP "ACCESS_POINT"
#define PW "PASSWORD"

#define BAUD 9600

int alive = 0;
int count = 0;
int waiting = 0;
 
void setup()
{
  Serial.begin(BAUD);
  esp8266.begin(BAUD);

  while (alive != 1 || count<2){
    alive = connectWifi(AP,PW);
    //Serial.println("Alive: "+alive);
    count=count+1;
  }

  if(alive != 1 && count > 2)
  {
    Serial.println("Initial WiFi connection failed!");  
  }else{
    Serial.println("Initial WiFi connected.");
  }
  
}
 
void loop()
{

  while (alive != 1 || count <2){
    alive = connectWifi(AP,PW);
    delay(5000);
    count=count+1;
  }

  if(alive != 1 && count > 2)
  {
    Serial.println("Loop WiFi connection failed!");  
  }else{
    Serial.println("Loop WiFi connected.");
  }

  delay(2000);
  

  count = 0;
    
  post();

  delay(60000);
}

int connectWifi(String ap, String password)
{
 
  sendDataTimeout("AT+RST\r\n", 2000, DEBUG);
  delay(500);
  
  //sendDataTimeout("ATE1\r\n", 3000, DEBUG);
  //delay(100);
  
  /*sendDataTimeout("AT+CWMODE=1\r\n", 5000, DEBUG);
  delay(2000);
  sendDataTimeout("AT+CWLAP\r\n", 5000, DEBUG);
  delay(2000);*/
  
  esp8266.print("AT+CWJAP=\""+ap+"\",\""+password+"\"\r\n");
  //Serial.print("Command: AT+CWJAP=\""+ap+"\",\""+password+"\"\r\n");
  delay(4000);
  
  esp8266.print("AT+CWJAP?\r\n");
  
  if(waitText("+CWJAP=\"RTCunha&BarbaraM\"",2,27)==true){
    //Serial.println("Connected");
    return 1;
  }else{
    //Serial.println("Not");
    return 0;
  }

}

boolean waitOk()
{
  boolean reading = 0;
  String response = "";
  
  while (reading == 0)
  {  
    if(esp8266.available()>0){
      response = response + char(esp8266.read());
      //Serial.println("Response: "+response);
      //Serial.println("LOOP: "+response.substring(2,4));
    }
    if(response.substring(2,4)=="OK"){
      reading = 1;
    }
    delay(50); 
  }
  return 1;
}

boolean waitText(String text,int beginning,int ending)
{
  boolean reading = 0;
  String response = "";
  
  while (reading == 0)
  {  
    if(esp8266.available()>0){
      response = response + char(esp8266.read());
      //Serial.println("Response: "+response);
      //Serial.println("TEXT: "+text);
      //Serial.println("LOOP: "+response.substring(beginning,ending));
    }
    if(response.substring(beginning,ending)==text){
      reading = 1;
    }
    delay(50); 
  }
  return 1;
}

void post(){
  sendData("AT+CIPSTART=\"TCP\",\"rtcunha.com.br\",80\r\n", DEBUG);
  delay(4000);

  String cmd = "GET http://rtcunha.com.br/ProjectPemberley/api/?";
  String cmd1 = "controller=Measures&action=create&timestamp=test";
  String cmd2 = "&temp=23&sensor1=150&sensor2=150&sensor3=150&sensor4=150&sensor5=150";
  String cmd3 = " HTTP/1.0\r\n\r\n";

  int stringsize = cmd.length() + cmd1.length() + cmd2.length() + cmd3.length();

  esp8266.print("AT+CIPSEND="+String(stringsize)+"\r\n");
  delay(3000);
  
  //Serial.print(cmd);
  //esp8266.print(cmd);
  
  sendData(cmd, DEBUG);
  sendData(cmd1, DEBUG);
  sendData(cmd2, DEBUG);
  sendData(cmd3, DEBUG);
  delay(6000);

  sendData("AT+CIPCLOSE\r\n", DEBUG);
}
 
String sendData(String command, boolean debug)
{
  // Envio dos comandos AT para o modulo
  String response = "";
  esp8266.print(command);
  
  while (esp8266.available())
   {  
      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
   }
  
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}

String sendDataTimeout(String command, const int timeout, boolean debug)
{
  // Envio dos comandos AT para o modulo
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}
