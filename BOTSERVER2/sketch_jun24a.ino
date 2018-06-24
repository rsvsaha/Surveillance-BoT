#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 0, 0, 20); // IP address, may need to change depending on network
EthernetServer server(80);  // create a server at port 80

File webFile;
String HTTP_req;            // stores the HTTP request


int dangerFront=0; //WILL BE SET IF THERE IS SOMETHING IN FRONT
int dangerBack=0; //WILL BE SET IF THERE IS SOMETHING IN BACK
int dangerLeft=0; //WILL BE SET IF THERE IS SOMETHING IN LEFT
int dangerRight=0;  //WILL BE SET IF THERE IS SOMETHING IN RIGHT



void setup()
{
    Ethernet.begin(mac, ip);  // initialize Ethernet device
    server.begin();           // start to listen for clients
    Serial.begin(9600);       // for diagnostics
    Serial.println("Initializing SD card...");
    if (!SD.begin(4)) {
        Serial.println("ERROR - SD card initialization failed!");
        return;    // init failed
    }
    Serial.println("SUCCESS - SD card initialized.");
    // check for index.htm file
    if (!SD.exists("BOTSERVER.html")) {
        Serial.println("ERROR - Can't find index.htm file!");
        return;  // can't find index file
    }
    Serial.println("SUCCESS - Found BOTSERVER.html file.");
            // switch is attached to Arduino pin 3
}

void loop()
{
    
    
    
    
    
    long front,back,left,right;
     front=getFront();
     back=getBack();
     left=getLeft();
     right=getRight();
      
      
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




    
    EthernetClient client = server.available();  // try to get client

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            front=getFront();
            back=getBack();
            left=getLeft();
            right=getRight();

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
                    if (HTTP_req.indexOf("sensorval") > -1) {
                          front=getFront();
            back=getBack();
            left=getLeft();
            right=getRight();

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
                          
                          String Sensorvalue=front+",";
                          Sensorvalue=Sensorvalue+back+",";
                          Sensorvalue=Sensorvalue+left+",";
                          Sensorvalue=Sensorvalue+right;
                          client.println(Sensorvalue);  
                    }
                    else {  
                      webFile = SD.open("BOTSERVER.html");        // open web page file
                    if (webFile) {
                        while(webFile.available()) {
                            client.write(webFile.read()); // send web page to client
                        }
                        webFile.close();
                    }
                    }
                    // display received HTTP request on serial port
                    Serial.print(HTTP_req);
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
  
;  
}

long getBack()
{
  
  
  
;  
}

long getLeft()
{
  
;  
}

long getRight()
{
  
  
  
;  
}










