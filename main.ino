#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

 const char* ssid = "REDACTED";
const char* password = "REDACTED";

ESP8266WebServer server(80);
 
// Define routing
void restServerRouting() {
    server.on("/", HTTP_GET, []() {
        server.send(200, F("text/html"),
            F("No, this is Patrick!"));
    });
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.print("\nConnected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Set server routing
    restServerRouting();

    // Start server
    server.begin();
    Serial.println("HTTP server started");
}

void loop(void) {
    server.handleClient();
}