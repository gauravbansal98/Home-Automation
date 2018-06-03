#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ThingSpeak.h>

WiFiClient client;
unsigned long myChannelNumber = 341357;
const char * myWriteAPIKey = "EGW4V9JMTBN75SJ4";
const char* ssid = "gaurav";
const char* password = "12345678";
static boolean data_state = false;
WiFiServer server(80);


void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("...");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  Serial.print('X');
  ThingSpeak.begin(client);
}

int tem, humidity,local;

void loop()
{
 
  // Check if a client has connected
  int a=millis();
  if(a%1000==0)
  {
  if (Serial.available())
  {
    int value = Serial.read();
    if (local == 0)
    {
      tem = value;
      local++;
    }
    else
    {
      humidity = value;
      local = 0;
    }
    if(value==123)
    {
      sendthing();
    }
    Serial.println(value);
  }
  }
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  // Serial.println("new client");

  while (!client.available()) {
    delay(1);
  }
  String request = client.readStringUntil('\r');
  client.flush();
  if (request.indexOf("/z") != -1)
  {
    Serial.println('z');
  }
  else if (request.indexOf("/x") != -1)
  {
    Serial.println('x');
  }
  else if (request.indexOf("/c") != -1)
  {
    Serial.println('c');
  }
  else if (request.indexOf("/v") != -1)
  {
    Serial.println('v');
  }
  else if (request.indexOf("/b") != -1)
  {
    Serial.println('b');
  }
  else if (request.indexOf("/n") != -1)
  {
    Serial.println('n');
  }
  else if (request.indexOf("/m") != -1)
  {
    Serial.println('m');
  }
  else if (request.indexOf("/l") != -1)
  {
    Serial.println('l');
  }
  else if (request.indexOf("/k") != -1)
  {
    Serial.println('k');
  }
  else if (request.indexOf("/j") != -1)
  {
    Serial.println('j');
  }
  else if (request.indexOf("/a") != -1)
  {
    Serial.println('a');
  }
  else if (request.indexOf("/r") != -1)
  {
    Serial.println('r');
  }


  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  // client.println("Refresh: 15");  // refresh the page automatically every 5 sec
  client.println();
p  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("This is my home :)");
  client.println("<br><br>");
  client.println("temperature of room =  ");
  client.println("32");
  client.println("<br><br>");
  client.println("Humidity of room =  ");
  client.println("15");
  client.println("<br><br>");
  client.println("<br><br>");
  client.println("<a href=\"/z\"\"><button>Bulb 1 On </button></a>");
  client.println("<a href=\"/x\"\"><button>Bulb 1 Off </button></a><br />");
  client.println("<a href=\"/c\"\"><button>Bulb 2 On</button></a>");
  client.println("<a href=\"/v\"\"><button>Bulb 2 Off </button></a><br />");
  client.println("<a href=\"/b\"\"><button>Bulb 3 On</button></a>");
  client.println("<a href=\"/n\"\"><button>Bulb 3 Off </button></a><br />");
  client.println("<a href=\"/m\"\"><button>Bulb 4 On</button></a>");
  client.println("<a href=\"/l\"\"><button>Bulb 4 Off </button></a><br />");
  client.println("<a href=\"/k\"\"><button>Open door</button></a>");
  client.println("<a href=\"/j\"\"><button>Close door </button></a><br />");
  client.println("<a href=\"/a\"\"><button>Add ID</button></a>");
  client.println("<a href=\"/r\"\"><button>Remove ID </button></a><br />");
  client.println("</html>");
  delay(1);
}

void sendthing()
{
  if ( data_state )
  {
    ThingSpeak.writeField(myChannelNumber, 1, tem, myWriteAPIKey);
    data_state = false;
  }
  else
  {
    ThingSpeak.writeField(myChannelNumber, 2, humidity, myWriteAPIKey);
    data_state = true;
  }
  Serial.println("hii");
  delay(1000);
}
