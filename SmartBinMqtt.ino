/********************************************************************************
  Description: Le but du projet est de creer une poubelle connecté... Ceci afin *
  d'automatiser la collecte et les traitements des ordures ménagère.            *
  Concretement ce script renvoie à intervalle régullier des données (distance,  *
  Latitude, Longitude, statut etc...) d'une poubelle ceci afin de connaitre     *
  l'état de la poubelle (Vide - Semis-pleine - Rempli) ceci afin de prévoire    *
  les rondes pour la collecte et le traitement des ordures.                     *
 *                                                                              *
 * Author: KODZO A. Mawuena M.                                                  *
 * Last version available on https://github.com/devnullone/SmartBin.git         *
 ********************************************************************************
 * MIT License
 *
 *******************************************************************************/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <time.h>
#include <TZ.h>
#include <FS.h>
#include <LittleFS.h>
#include <CertStoreBearSSL.h>
#include <ArduinoJson.h>

// Additional  headers 
#include "config.h"


// Translate config.h defines into variables used by the 
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;
const char* mqtt_server = HIVEMQ_SERVER;
const short mqtt_port = HIVEMQ_PORT;
const char* mqtt_user = HIVEMQ_USER;
const char* mqtt_password = HIVEMQ_PASSWORD;
const char* client_id = CLIENT_ID;
const char* device_uid = DEVICE_UID;
const char *device_status_tab[5] = DEVICE_STATUS_TAB;
const char* device_current_status = DEVICE_CURRENT_STATUS;
const short hauteurPou = DEVICE_HAUTEUR; // en Cm 
const char* lat = LATITUDE_VALUE;
const char* lon = LONGITUDE_VALUE;


// A single, global CertStore which can be used by all connections.
// Needs to stay live the entire time any of the WiFiClientBearSSLs
// are present.
BearSSL::CertStore certStore;

WiFiClientSecure espClient;
PubSubClient * client;
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (500)
char msg[MSG_BUFFER_SIZE];
int value = 0;

// HC-SE04 <------> EPS8266
const int trigPin = 15;
const int echoPin = 13;

//Definissons la vitesse du Son en cm/uS
#define vitesseDuSon 0.034

long durer;
float distanceCm;
float valeur;

// TR
float tauxDeRemplissage;



void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void setDateTime() {
  // You can use your own timezone, but the exact time is not used at all.
  // Only the date is needed for validating the certificates.
  configTime(TZ_Europe_Berlin, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(100);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println();

  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.printf("%s %s", tzname[0], asctime(&timeinfo));
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  //USE THIS IF FOR LOGGIN
  // Switch on the LED if the first character is present
  if ((char)payload[0] != NULL) {
    digitalWrite(LED_BUILTIN, LOW); // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off by making the voltage HIGH
  } else {
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off by making the voltage HIGH
  }
}


void reconnect() {
  // Loop until we’re reconnected
  while (!client->connected()) {
    Serial.print("Attempting MQTT connection…");
    String deviceId = "ESP8266Client - MyClient";
    // Attempt to connect
    // Insert your password
    if (client->connect(deviceId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement…
      client->publish("testTopic", "hello world");
      // … and resubscribe
      client->subscribe("testTopic");
    } else {
      Serial.print("failed, rc = ");
      Serial.print(client->state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void deepSleep(){
  // Deep sleep mode for X min, the ESP8266 wakes up by itself when GPIO 16 (D0 in NodeMCU board) is connected to the RESET pin
  Serial.println("Passe en mode sommeil profond pendant 5 min");
  ESP.deepSleep(30e7); 
  
  }

void deconnection(){
  while(client->connected()){
      client->disconnect();
      Serial.print("deconnexion success, Bye!");
  }
 }

float tr(){
  // trigPin fermer
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // trigPin ouvert pendant 10 microsecondes
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Lire le echoPin et retourne la durer de l'onde en microsecondes
  durer = pulseIn(echoPin, HIGH);
  
  // Calcul la distance
  distanceCm = durer * vitesseDuSon/2;

  valeur = distanceCm / hauteurPou;
  tauxDeRemplissage = (1 - valeur)*100; // Calcul du Taux de Remplissage (TR) of trash
  return tauxDeRemplissage;
  } 
  
String playload(){
  StaticJsonDocument<200> payload;
  String msg;
  float taux_remp;
  taux_remp = tr();
  if(taux_remp <= 15){
    device_current_status = device_status_tab[0]; // vide
    }
    
    else if(taux_remp > 30 && taux_remp <= 75){
    device_current_status = device_status_tab[1]; // semi-plein
    }
    
    else if(taux_remp > 75 && taux_remp <= 98){
    device_current_status = device_status_tab[2]; // plein
    }
    
    else if(taux_remp > 98 && taux_remp <= 100){
    device_current_status = device_status_tab[3]; // debordement
    }
    
    else{
    device_current_status = device_status_tab[4]; // erreur
    }
    // FORMATAGE DU MSG AVANT ENVOIE <======================= JE SUIS ICI

    // Add values in the payload
    payload["TauxDeRemp"] = taux_remp;
    payload["Status"] = device_current_status;
    payload["Latitude"] = lat;
    payload["Longitude"] = lon;
    payload["Battery"] = "N/A";
    payload["TIMESTAMP"] = "193923823747821";

    serializeJson(payload, msg);

  return msg;
  }

void setup() {
  delay(500);
  // When opening the Serial Monitor, select 9600 Baud
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT); // Mets le trigPin comme Sortie (Output)
  pinMode(echoPin, INPUT); // Mets le echoPin comme Entrer (Input)
  
  delay(500);

  LittleFS.begin();
  setup_wifi();
  setDateTime();

  pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output

  // you can use the insecure mode, when you want to avoid the certificates
  //espclient->setInsecure();

  int numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
  Serial.printf("Number of CA certs read: %d\n", numCerts);
  if (numCerts == 0) {
    Serial.printf("No certs found. Did you run certs-from-mozilla.py and upload the LittleFS directory before running?\n");
    return; // Can't connect to anything w/o certs!
  }

  BearSSL::WiFiClientSecure *bear = new BearSSL::WiFiClientSecure();
  // Integrate the cert store with this connection
  bear->setCertStore(&certStore);

  client = new PubSubClient(*bear);

  client->setServer(mqtt_server, mqtt_port);
  client->setCallback(callback);
}

void loop() {
  if (!client->connected()) {
    reconnect();
  }
  client->loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client->publish("testTopic", msg);
  }
  deconnection(); // Deconnexion
  deepSleep(); // Passage en mode veille prolonger pour sauveguarder la batterie
}
