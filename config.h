// MIT License

// SIM800L <------> EPS8266
#define SIM800_RX_PIN 14
#define SIM800_TX_PIN 12
#define SIM800_RST_PIN 2 

// Server
//#define MY_IOTHUB_SERVER "Server"

//Device 
//#define DEVICE_ID "Device ID"

//Client
//#define CLIENT_ID "Client ID"

// Poubelle
//#define POUBELLE_ID "POUBELLE ID"
//#define LATITUDE "POUBELLE lat"
//#define LATITUDE "Longitude long"
/*
 * Functions
 * 
 * setup_wifi() -> met en route la connexion wifi
 * 
 * setDateTime() -> Se connecte au serveur de temps et recupere le temps precis
 * 
 * callback()    -> Allume ou eteind la led dela ESP en fonction du message recu
 * 
 * reconnect()    -> Se reconnect au broker mqtt
 */
