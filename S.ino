#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>


// WiFi
const char *ssid = "nepaldigisys"; // Enter your WiFi name
const char *password = "NDS_0ffice";  // Enter WiFi password


// Mac address of the slave
uint8_t peer1[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
typedef struct message {
   int id;
   float distance;
   float returnTime;
   bool race;
};
int i = 0;

const int trigPin = 12;
const int echoPin = 14;

bool obs = false;

#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float start_time;
float first_detect;
float end_time;
float ReturnTime;




//for avg
float global_data[2];
int c =0;
int flag = 0;




struct message myMessage;
void onSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.println("Status:");
  Serial.println(sendStatus);
}
struct recMessage {
  int id;
  bool test;
  } recMessage;
struct recMessage rec;
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&rec, incomingData, sizeof(rec));
//  Serial.printf("ID: %d\n", rec.id);
//  Serial.printf("Test: %d \n", rec.test);
  Serial.println();
  if(rec.test == true) {
    flag = 0;
    }
 
}
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid,password);
  //Set device as a Wi-Fi Station
//  WiFi.mode(WIFI_STA);

  while (WiFi.status() !=WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
    
    }
  WiFi.mode(WIFI_STA);
  // Get Mac Add
  Serial.print("Mac Address: 9C:9C:1F:CB:42:04   ");
  Serial.print(WiFi.macAddress());
  Serial.println("ESP-Now Sender");
  // Initializing the ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Problem during ESP-NOW init");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  // Register the peer
  Serial.println("Registering a peer");
  esp_now_add_peer(peer1, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  Serial.println("Registering send callback function");
  esp_now_register_send_cb(onSent);
//  esp_now_register_recv_cb(OnDataRecv);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);
}
void loop() {

  
  myMessage.id = i+1;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_VELOCITY / 2;

  
// Obstacle
  
  
  if (distanceCm <100){
    if (first_detect == 0) {
      start_time = millis();
      first_detect = 1;
      Serial.println("obstacle detected!");
      
      
      }
      else if (first_detect ==1) {
        end_time = millis();
        first_detect = 0;
        
        }
        obs = true;
    } else {
      if (obs == true) {
          obs = false;
          Serial.println("Race Started.....");
          myMessage.race = true;
        } else {
          myMessage.race = false;
          }
    }
  esp_now_send(NULL, (uint8_t *) &myMessage, sizeof(myMessage));

delay(500);
  
}
