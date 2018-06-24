#include<Ethernet.h>
#include<SPI.h>
#include<avr/pgmspace.h>
#define trigger A0
#define echo A1




const PROGMEM char LINE_0[]="<!DOCTYPE html>";
const PROGMEM char LINE_1[]="<html>";
const PROGMEM char LINE_2[]="<h1>BOT CONTROL </h1> ";
const PROGMEM char LINE_3[]="<body>";
const PROGMEM char LINE_4[]="<p>SENSOR1:<input type=\"text\" id=\"SENSOR1\" value=\" \">";
const PROGMEM char LINE_5[]="SENSOR2:<input type=\"text\" id=\"SENSOR2\" value=\" \">";
const PROGMEM char LINE_6[]="SENSOR4:<input type=\"text\" id=\"SENSOR3\" value=\" \">";
const PROGMEM char LINE_7[]="SENSROR5:<input type=\"text\" id=\"SENSOR4\" value=\" \"></p></br></br>";
const PROGMEM char LINE_8[]="<p><input type=\"button\" id=\"STREAM\" value=\"START STREAMING\" onclick=\"GETSENSORVAL();\">";
const PROGMEM char LINE_9[]="<input type=\"button\" id=\"FORWARD\" value=\"FORWARD\" onclick=\"FORWARD();\">";
const PROGMEM char LINE_10[]="<input type=\"button\" id=\"BACKWARD\" value=\"BACKWARD\" onclick=\"BACKWARD();\">";
const PROGMEM char LINE_11[]="<input type=\"button\" id=\"LEFT\" value=\"LEFT\" onclick=\"LEFT();\">";
const PROGMEM char LINE_12[]="<input type=\"button\" id=\"LEFT\" value=\"RIGHT\" onclick=\"RIGHT();\"></p>";
const PROGMEM char LINE_13[]="</body>";

const PROGMEM char LINE_14[]="<script>";
const PROGMEM char LINE_15[]="var request = new XMLHttpRequest();";
const PROGMEM char LINE_16[]="nocache = \"&nocache=\"\\+ Math.random() * 1000000;";

const PROGMEM char LINE_17[]="function GETSENSORVAL(){request.onreadystatechange = function() {if (this.readyState == 4)";
const PROGMEM char LINE_18[]="{if (this.status == 200) {";
const PROGMEM char LINE_19[]="if (this.responseText != null) {";
const PROGMEM char LINE_20[]="var myArr = JSON.parse(this.responseText);";
const PROGMEM char LINE_21[]="document.getElementById(\"SENSOR1\").value = this.responseText";
const PROGMEM char LINE_22[]="document.getElementById(\"SENSOR2\").value = this.responseText;";
const PROGMEM char LINE_23[]="document.getElementById(\"SENSOR3\").value = this.responseText;";
const PROGMEM char LINE_24[]="document.getElementById(\"SENSOR4\").value = this.responseText;}}}}";
const PROGMEM char LINE_25[]="request.open(\"GET\", \"sensorval\" + nocache, true);";
const PROGMEM char LINE_26[]="request.send(null);";
const PROGMEM char LINE_27[]="setTimeout('GETSENSORVAL()',100);}"; //BE READY TO CHANGE THE TIME IF THE ARDUINO CAN'T PROCESS FAST ENOUGH

const PROGMEM char LINE_28[]="function FORWARD(){request.onreadystatechange = function() {";
const PROGMEM char LINE_29[]="request.open(\"GET\", \"forward\" + nocache, true);";
const PROGMEM char LINE_30[]="request.send(null);}"; 

const PROGMEM char LINE_31[]="function BACKWARD(){request.onreadystatechange = function() {";
const PROGMEM char LINE_32[]="request.open(\"GET\", \"backward\" + nocache, true);";
const PROGMEM char LINE_33[]="request.send(null);}"; 

const PROGMEM char LINE_34[]="function LEFT(){request.onreadystatechange = function() {";
const PROGMEM char LINE_35[]="request.open(\"GET\", \"left\" + nocache, true);";
const PROGMEM char LINE_36[]="request.send(null);}"; 

const PROGMEM char LINE_37[]="function RIGHT(){request.onreadystatechange = function() {";
const PROGMEM char LINE_38[]="request.open(\"GET\", \"right\" + nocache, true);";
const PROGMEM char LINE_39[]="request.send(null);}"; 

const PROGMEM char LINE_40[]="</script>";
const PROGMEM char LINE_41[]="</html>";

int buffersize=strlen(LINE_18);
char *buffer=(char *)calloc(buffersize,sizeof(char));
const char* const string_table[] PROGMEM={LINE_0,LINE_1,LINE_2,LINE_3,LINE_4,LINE_5,LINE_6,LINE_7,LINE_8,LINE_9,LINE_10,LINE_11,LINE_12,LINE_13,LINE_14,LINE_15,LINE_16,LINE_17,LINE_18,LINE_19,LINE_20,
LINE_21,LINE_22,LINE_23,LINE_24,LINE_25,LINE_26,LINE_27,LINE_28,LINE_29,LINE_30,LINE_31,LINE_32,LINE_33,LINE_34,LINE_35,LINE_36,LINE_37,LINE_38,LINE_39,LINE_40,LINE_41};

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 0, 0, 20); // IP address, may need to change depending on network
EthernetServer server(80);
String HTTP_req;

void setup() {
  // put your setup code here, to run once:
    Ethernet.begin(mac, ip);  // initialize Ethernet device
    server.begin();           // start to listen for clients
    Serial.begin(9600);       // for diagnostics
    pinMode(echo, INPUT);
    pinMode(trigger,OUTPUT); 
}

void loop() {
  // put your main code here, to run repeatedly:
EthernetClient client = server.available();  // try to get client

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                HTTP_req += c;  // save the HTTP request 1 char at a time
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: keep-alive");
                    client.println();
                    // AJAX request for switch state
                    if (HTTP_req.indexOf("sensorval") > -1) {
                        // read switch state and send appropriate paragraph text
                        GetSensorVal(client);
                    }
                    else if(HTTP_req.indexOf("forward") > -1){
                      Serial.println("Forward");
                    }
                    else if(HTTP_req.indexOf("backward") > -1){
                      Serial.println("Backward");
                     }
                     else if(HTTP_req.indexOf("left") > -1)
                     {
                      Serial.println("Left");
                     }
                     else if(HTTP_req.indexOf("right") > -1){
                      Serial.println("Right");
                      }
                    else {  // HTTP request for web page
                        // send web page - contains JavaScript with AJAX calls
                        for(int i=0;i<42;i++) //34 no. of html lines are there
                        {   strcpy_P(buffer, (char*)pgm_read_word(&(string_table[i])));
                            client.println(buffer);
                            client.flush(); //MAY OR MAY NOT BE NEEDED
                            delay(50);  
                        }   
                           
                           
                           }
                    // display received HTTP request on serial port
                    Serial.print(HTTP_req);
                    HTTP_req = "";            // finished with request, empty string
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}
void GetSensorVal(EthernetClient cl)
{
  float time=0,distance=0;
  digitalWrite(trigger,LOW);
  delayMicroseconds(2);
  digitalWrite(trigger,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger,LOW);
  delayMicroseconds(2);
  time=pulseIn(echo,HIGH);
  distance=time*340/20000;  
  cl.println(distance);  
}

