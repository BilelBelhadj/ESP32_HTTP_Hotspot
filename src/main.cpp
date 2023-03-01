/*
Auteur      : Bilel Belhadj
Description : le ESP32 en premier joue le role d'un point d'accees apres il affiche une page web pour l'utilisateur de remplir 
              le nom et le mot de passe de son Wifi pour que le est32 se connecte dessus 
Date        : 28/02/2023
Version     : 0.0.1
*/

#include <WiFi.h>
#include <string>

//declaration des constantes
const char* ssidHotspot = "ESP32_Bilel";    // ssid pour le point d'accees
const char* passwordHotsport = "bilel2023"; // mot de passe pour le point d'accees

//declaration des variables
String line = "", line1 = "", ssidWifiStr = "", pwdWifiStr = "";

WiFiServer server(80);  // creation d'un serveur web on port 80

void setup() {
  Serial.begin(9600);  // commence le moniteur serie
  delay(1000);

  // Connection sur le Wifi du ESP32
  Serial.println("Connecting to WiFi network...");
  WiFi.softAP(ssidHotspot, passwordHotsport);

  //ecrit l'adresse ip dans le moniteur serie
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.softAPIP());

  server.begin();  // commencer le serveur
}

void loop() {
  WiFiClient client = server.available();  // verifier l existance du client
  if (client) {
    Serial.println("New client connected.");

    String currentLine = "";  // ligne current 
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();  // lire les donnees char par char
        if (c == '\n') {

          // verifier si la ligne est vide CAD la requete HTTP est fini
          if (currentLine.length() == 0) {

            // Envoyer le HTTP header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // page html
            client.println("<html>");
            client.println("<head><title>ESP32 Hotspot</title></head>");
            client.println("<body>");
            client.println("<h1>ESP32Choose your WiFi </h1>");
            client.println("<form method=\"post\">");
            client.println("<label for=\"input1\">Input 1:</label>");
            client.println("<input type=\"text\" id=\"input1\" name=\"input1\"><br>");
            client.println("<label for=\"input2\">Input 2:</label>");
            client.println("<input type=\"password\" id=\"input2\" name=\"input2\"><br><br>");
            client.println("<input type=\"submit\" value=\"Submit\">");
            client.println("</form>");
            client.println("</body>");
            client.println("</html>");
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        // verifier si l envoi de la requete est fini
        if (currentLine.endsWith("POST / HTTP/1.1")) {
          Serial.println("Request received.");

          // lire les donnees du requete
          while (client.available()) {
            line = client.readStringUntil('\r');
            line1 = line.substring(1, line.length());
          }
          break;
        }
      }
    }

    // fermer la connexion
    client.stop();
    Serial.println("Client disconnected.");
  }
  
  //affichage les donnees dans le moniteur serie
  Serial.println("Data");
  Serial.println(line1);

  //garder le previous data
  String lastSsidWifi = ssidWifiStr;
  String lastPwdWifi  = pwdWifiStr;

  //recuperer les donnees dans les variables
  ssidWifiStr = line1.substring(line1.indexOf('=') + 1, line1.indexOf('&'));
  pwdWifiStr  = line1.substring(line1.lastIndexOf('=') + 1, line1.length() + 1);

  //connecter au Wifi
  if (ssidWifiStr != lastSsidWifi || pwdWifiStr != pwdWifiStr){
    const char* ssidWifi = ssidWifiStr.c_str();
    const char* passwordWifi = pwdWifiStr.c_str();
    //commencer l'essai de connection
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssidWifi, passwordWifi);
    
    while (WiFi.status() != WL_CONNECTED) {
      Serial.println("Connecting to WiFi....");
      delay(100);
    }
    Serial.println("Connected to the WiFi network");
  }
  delay(1000);
}