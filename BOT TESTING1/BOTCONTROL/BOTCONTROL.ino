#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

//T stands for Trigger E stands for Echo F for Front B for Back L for Left R for Right

#define TF A0
#define EF A1

#define TB A2
#define EB A3

#define TL A4
#define EL A5


#define TR 8
#define ER 9











// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 25); // IP address, may need to change depending on network
EthernetServer server(80);  // create a server at port 80

File webFile;

String HTTP_req;            // stores the HTTP request
String Sensorvalue;

uint8_t dangerFront=0; //WILL BE SET IF THERE IS SOMETHING IN FRONT
uint8_t dangerBack=0; //WILL BE SET IF THERE IS SOMETHING IN BACK
uint8_t dangerLeft=0; //WILL BE SET IF THERE IS SOMETHING IN LEFT
uint8_t dangerRight=0;  //WILL BE SET IF THERE IS SOMETHING IN RIGHT

byte buf=0; //FOR STORING THE VALUES OF PORT WHILE USING THE MOTOR;


void setup()
{
  Ethernet.begin(mac, ip);  // initialize Ethernet device
    server.begin();
    Serial.begin(9600);       // for diagnostics
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
      webFile=SD.open("rishav.htm");
      Serial.println(webFile);
  

    pinMode(TF,OUTPUT);
    pinMode(TB,OUTPUT);
    pinMode(TL,OUTPUT);
    pinMode(TR,OUTPUT);

    pinMode(EF,INPUT);
    pinMode(EB,INPUT);
    pinMode(EL,INPUT);
    pinMode(ER,INPUT);

   

}





void loop()
{
    
    
    long front,back,left,right;
    
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
                    Serial.println("GOT REQUEST");
                    if (HTTP_req.indexOf("sensorval") > -1) {
                          front=getFront();
                          back=getBack();
                          left=getLeft();
                          right=getRight();
                          
                          checkdanger(right,back,front,left);
        
                          Serial.println(front);
                          Serial.println(back);
                          Serial.println(left);
                          Serial.println(right);

                          Sensorvalue=front;
                          Sensorvalue+=",";
                          Sensorvalue+=back;
                          Sensorvalue+=",";
                          Sensorvalue+=left;
                          Sensorvalue+=",";
                          Sensorvalue+=right;
                          
                          
                          Serial.println(Sensorvalue);
                          client.println(Sensorvalue);  
                    }

                  else if (HTTP_req.indexOf("forward") > -1) {
                          Forward();
                    }

                  else  if (HTTP_req.indexOf("backward") > -1) {
                          Backward(); 
                    }

                   else if (HTTP_req.indexOf("left") > -1) {
                          Left();
                    }
                    
                   else if (HTTP_req.indexOf("right") > -1) {
                          Right();
                    }
                    else
                    {
                        //webFile=SD.open("rishav.htm");  
                        if(webFile){
                          while(webFile.available())
                          {
                            client.write(webFile.read());
                           }
                          
                          webFile.close();
                          }
                      
                      
                      }
                    
                    
                    
                    // display received HTTP request on serial port
                   // Serial.print(HTTP_req);
                    HTTP_req = "";            // finished with request, empty string
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)

}








// send the state of the switch to the web browser

long getFront()
{
   long duration;
   long distance;
  digitalWrite(TF, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(TF, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(TF, LOW);
  duration = pulseIn(EF, HIGH);
  distance = (duration/2) / 29.1;
  //Serial.println(distance);
  return distance;
}
long getBack()
{
  
  long duration;
   long distance;
  digitalWrite(TB, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(TB, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(TB, LOW);
  duration = pulseIn(EB, HIGH);
  distance = (duration/2) / 29.1;
  //Serial.println(distance);
  return distance;
   
}

long getLeft()
{
  long duration;
   long distance;
  digitalWrite(TL, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(TL, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(TL, LOW);
  duration = pulseIn(EL, HIGH);
  distance = (duration/2) / 29.1;
  //Serial.println(distance);
  return distance; 
}

long getRight()
{
  long duration;
   long distance;
  digitalWrite(TR, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(TR, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(TR, LOW);
  duration = pulseIn(ER, HIGH);
  distance = (duration/2) / 29.1;
  //Serial.println(distance);
  return distance; 
}




void checkdanger(uint8_t right,uint8_t back,uint8_t front,uint8_t left)
{
 if(right<=30)
       {dangerRight=1; }
       else
       {dangerRight=0;}
       
       if(left<=30)
       {dangerLeft=1;}
        else
        {dangerLeft=0;}
        
        if(front<=30)
        {dangerFront=1;}
         else  
         {dangerFront=0;}
          
          if(back<=30)
          {dangerBack=1;}
           else
           {dangerBack=0;}  
  
}


void Forward()
{
  if(!dangerFront)
  {
    buf=0x0F&PORTB; //MASK THE LAST FOUR BITS AND SET THE FIRST FOUR BITS TO 0
    buf=buf|0xF0; //SETTING THE FIRST FOUR BITS TO ONE
    PORTB=buf;
   }  
  
  
}



void Backward()
{
   
   if(!dangerBack)
  {
    buf=0x0F&PORTB; //MASK THE LAST FOUR BITS AND SET THE FIRST FOUR BITS TO 0
    buf=buf|0xF0; //SETTING THE FIRST FOUR BITS TO ONE
    PORTB=buf;
    
  }  
 
  
  
}

void Left()
{
 if(!dangerLeft)
  {
   buf=0x0F&PORTB; //MASK THE LAST FOUR BITS AND SET THE FIRST FOUR BITS TO 0
    buf=buf|0xF0; //SETTING THE FIRST FOUR BITS TO ONE
    PORTB=buf;
     
  }  
  
}

void Right()
{
  
 if(!dangerRight)
  {
   buf=0x0F&PORTB; //MASK THE LAST FOUR BITS AND SET THE FIRST FOUR BITS TO 0
    buf=buf|0xF0; //SETTING THE FIRST FOUR BITS TO ONE
    PORTB=buf;
     
   }  
  
  
}


