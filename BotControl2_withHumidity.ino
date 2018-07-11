#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <Sensirion.h>
#define dataPin 8
#define clockPin 9


#define REQ_BUF_SZ   50

#define TF 2
#define EF 3

#define TB 5
#define EB 6

//#define TL A4
//#define EL A5


//#define TR 8
//#define ER 9


#define MOTOSPEED  2000 //Represents the delay

char HTTP_req[REQ_BUF_SZ] = {0}; // buffered HTTP request stored as null terminated string
char req_index = 0;  

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };// MAC address from Ethernet shield sticker under board
IPAddress ip(192, 168, 0, 25); // IP address, may need to change depending on network
EthernetServer server(80);  // create a server at port 80
Sensirion tempSensor = Sensirion(dataPin, clockPin);


File webFile;

String Sensorvalue,Sht11_val;
float tempC,humidity;
uint8_t dangerFront=0; //WILL BE SET IF THERE IS SOMETHING IN FRONT
uint8_t dangerBack=0; //WILL BE SET IF THERE IS SOMETHING IN BACK
//uint8_t dangerLeft=0; //WILL BE SET IF THERE IS SOMETHING IN LEFT
//uint8_t dangerRight=0;  //WILL BE SET IF THERE IS SOMETHING IN RIGHT

const uint8_t danger_limit=30;
byte buf=0; //FOR STORING THE VALUES OF PORT WHILE USING THE MOTOR;

uint8_t front,back,left,right; //CONTAINS THE GAP

void setup()
{
    Ethernet.begin(mac, ip);  // initialize Ethernet device
    server.begin();           // start to listen for clients
    Serial.begin(9600);       // for debugging
    
    // initialize SD card
    Serial.println("Initializing SD card...");
    if (!SD.begin(4)) {
        Serial.println("ERROR - SD card initialization failed!");
        return;    // init failed
    }
    Serial.println("SUCCESS - SD card initialized.");
    // check for index.htm file
    if (!SD.exists("rishav.htm")) {
        Serial.println("ERROR - Can't find rishav.htm file!");
        return;  // can't find index file
    }
    Serial.println("SUCCESS - Found rishav.htm file.");


    pinMode(TF,OUTPUT);
    pinMode(TB,OUTPUT);
  //  pinMode(TL,OUTPUT);
//    pinMode(TR,OUTPUT);

    pinMode(EF,INPUT);
    pinMode(EB,INPUT);
  //  pinMode(EL,INPUT);
//    pinMode(ER,INPUT);
/*
pinMode(A2,OUTPUT); //A2-A5 as OUTPUT A5-Green A4-Purple A3-Grey A2-Yellow
pinMode(A3,OUTPUT);
pinMode(A4,OUTPUT);
pinMode(A5,OUTPUT);
*/

DDRC=DDRC|B00111100;
Serial.println(DDRC);

}




void loop()
{
    EthernetClient client = server.available();  // try to get client
     
     front=getGap(TF,EF);
     back=getGap(TB,EB);
     checkdanger(front,back);
    
  if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            
            
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                // last line of client request is blank and ends with \n
                // respond to client only after last line received

                if (req_index < (REQ_BUF_SZ - 1)) {
                    HTTP_req[req_index] = c;          // save HTTP request character
                    req_index++;
                }
                
                
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: keep-alive");
                    client.println();
                    // send web page
                    //Serial.println(HTTP_req);
                    
                    if (StrContains(HTTP_req, "sensorval")) {
                        sendData(front,back,client);
                          front=getGap(TF,EF);
                          back=getGap(TB,EB);
                          checkdanger(front,back);
                          
                         }
                    
                    
                 else if (StrContains(HTTP_req, "forward")) {
                          client.println("FORWARD");
                          Forward();
                    }

                  else  if (StrContains(HTTP_req, "backward")) {
                          client.println("BACKWARD");
                          Backward(); 
                    }

                   else if (StrContains(HTTP_req, "left")) {
                          client.println("LEFT");
                          Left();
                    }
                    
                   else if (StrContains(HTTP_req, "right")) {
                          client.println("RIGHT");
                          Right();
                    }
                  
                  else if(StrContains(HTTP_req, "temp_humid"))
                  {
                      getHumid_data(humidity,tempC,client);
                  }
                  
                  else{  
                    webFile = SD.open("rishav.htm");        // open web page file
                    if (webFile) {
                        while(webFile.available()) {
                            client.write(webFile.read()); // send web page to client
                        }
                        webFile.close();
                    }
                    
                  } 
                    
                    req_index = 0;
                    StrClear(HTTP_req, REQ_BUF_SZ);
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



void StrClear(char *str, char length)
{
    for (int i = 0; i < length; i++) {
        str[i] = 0;
    }
}

// searches for the string sfind in the string str
// returns 1 if string found
// returns 0 if string not found
char StrContains(char *str, char *sfind)
{
    char found = 0;
    char index = 0;
    char len;

    len = strlen(str);
    
    if (strlen(sfind) > len) {
        return 0;
    }
    while (index < len) {
        if (str[index] == sfind[found]) {
            found++;
            if (strlen(sfind) == found) {
                return 1;
            }
        }
        else {
            found = 0;
        }
        index++;
    }

    return 0;
}

long getGap(int T,int E)
{
   long duration;
   long distance;
  digitalWrite(T, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(T, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(T, LOW);
  duration = pulseIn(E, HIGH);
  distance = (duration/2) / 29.1;
  //Serial.println(distance);
  return distance;
}




void checkdanger(uint8_t front,uint8_t back)
{

        if(front<=danger_limit)
        {dangerFront=1;}
         else  
         {dangerFront=0;}
          
          if(back<=danger_limit)
          {dangerBack=1;}
           else
           {dangerBack=0;}  
  
}


void Forward()
{

 Serial.println("FORWARD");

 /*

digitalWrite(A2,LOW);
digitalWrite(A3,HIGH);
digitalWrite(A4,LOW);
digitalWrite(A5,HIGH);
  
   */
    if(!dangerFront){Serial.println(PORTC);
    buf=B11000011&PORTC; //MASK THE FIRST AND LAST TWO BITS AND CLEARING THE MIDDLE 4 BITS FOR NEW OUTPUT
    buf=buf|B00010100; //SETTING THE MIDDLE FOUR BITS AS PER MOVEMENT REQUIREMENT
    PORTC=buf; //LOADING THE DATA TO PORT
    Serial.println(PORTC);
    delay(MOTOSPEED);//WAIT FOR THE MOTOR TO ROTATE
    buf=B11000011&PORTC; //MASK THE FIRST AND LAST TWO BITS AND CLEARING THE MIDDLE 4 BITS FOR NEW OUTPUT
    PORTC=buf;
    Serial.println(PORTC);
 }
    
  
}



void Backward()
{
   
   /*
   digitalWrite(A3,LOW);
digitalWrite(A2,HIGH);
digitalWrite(A5,LOW);
digitalWrite(A4,HIGH);
 
     */ 
   
   
   
    if(!dangerBack){
        Serial.println("BACK");
  
 
    Serial.println(PORTC);
    buf=B11000011&PORTC; //MASK THE FIRST AND LAST TWO BITS AND CLEARING THE MIDDLE 4 BITS FOR NEW OUTPUT
    buf=buf|B00101000; //SETTING THE MIDDLE FOUR BITS AS PER MOVEMENT REQUIREMENT
    PORTC=buf; //LOADING THE DATA TO PORT
    Serial.println(PORTC);
    delay(MOTOSPEED);//WAIT FOR THE MOTOR TO ROTATE
    buf=B11000011&PORTC; //MASK THE FIRST AND LAST TWO BITS AND CLEARING THE MIDDLE 4 BITS FOR NEW OUTPUT
    PORTC=buf;
    Serial.println(PORTC);
   
      
      }
 
   

  
  
}

void Left() //MAKES IT MOVE LEFT CIRCLE
{
    /*
  digitalWrite(A3,LOW);
digitalWrite(A2,HIGH);
digitalWrite(A5,LOW);
digitalWrite(A4,HIGH);


  */


  

  Serial.println("LEFT");

 
    Serial.println(PORTC);
    buf=B11000011&PORTC; //MASK THE FIRST AND LAST TWO BITS AND CLEARING THE MIDDLE 4 BITS FOR NEW OUTPUT
    buf=buf|B00100100; //SETTING THE MIDDLE FOUR BITS AS PER MOVEMENT REQUIREMENT
    PORTC=buf; //LOADING THE DATA TO PORT
    Serial.println(PORTC);
    delay(MOTOSPEED);//WAIT FOR THE MOTOR TO ROTATE
    buf=B11000011&PORTC; //MASK THE FIRST AND LAST TWO BITS AND CLEARING THE MIDDLE 4 BITS FOR NEW OUTPUT
    PORTC=buf;
    Serial.println(PORTC);
}

void Right() //MAKES IT MOVE RIGHT CIRCLE
{
  
  /*
  digitalWrite(A3,HIGH);
digitalWrite(A2,LOW);
digitalWrite(A5,LOW);
digitalWrite(A4,HIGH);

*/
  Serial.println("RIGHT");
  
    Serial.println(PORTC);
    buf=B11000011&PORTC; //MASK THE FIRST AND LAST TWO BITS AND CLEARING THE MIDDLE 4 BITS FOR NEW OUTPUT
    buf=buf|B00011000; //SETTING THE MIDDLE FOUR BITS AS PER MOVEMENT REQUIREMENT
    
    PORTC=buf; //LOADING THE DATA TO PORT
    Serial.println(PORTC);
    delay(MOTOSPEED);//WAIT FOR THE MOTOR TO ROTATE
    buf=B11000011&PORTC; //MASK THE FIRST AND LAST TWO BITS AND CLEARING THE MIDDLE 4 BITS FOR NEW OUTPUT
    PORTC=buf;
    Serial.println(PORTC);



}


void sendData(uint8_t front,uint8_t back, EthernetClient cl)
{
                          Sensorvalue=front;
                          Sensorvalue+=",";
                          Sensorvalue+=back;
                          cl.println(Sensorvalue);    
}

void getHumid_data(float humid,float temp,EthernetClient cl)
{                       float dewpoint;
                        tempSensor.measure(&temp, &humid, &dewpoint);
                        Sht11_val=humid;
                        Sht11_val+=",";
                        Sht11_val+=temp;
                        cl.println(Sht11_val);  

}


