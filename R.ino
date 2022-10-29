
#include <esp_now.h>
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "nepaldigisys"; // Enter your WiFi name
const char *password = "NDS_0ffice";  // Enter WiFi password


// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "nds/race_test";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;
float global_data[100];
float global_return[100];
int c = 0;
WiFiClient espClient;
PubSubClient client(espClient);

//bool Time = true;
bool race_started=false;
float distance;
float start_time ;
float end_time ;
float actual_time;
float flag = 0;
float Time;
float pre_time;
#define SOUND_VELOCITY 0.034

long duration;
float distanceCm;

typedef struct struct_message {
  int id;
  float distance;
  float ReturnTime;
  bool race;
}struct_message;
const int trigpin = 5;
const int echopin = 18;

typedef struct send_Message{
  int id;
  bool test;
  } ;

struct send_Message sen;
int i=0;

// Create a struct_message called myData
struct_message myData;

// Create a structure to hold the readings from each board
struct_message board1;
struct_message board2;
struct_message board3;

// Create an array with all the structures
struct_message boardsStruct[3] = {board1, board2, board3};

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];

  memcpy(&myData, incomingData, sizeof(myData));

  Serial.println();
  if (myData.race == true){
      race_started = true;
      start_time = millis();
      flag = 0;
    }
}
 
void setup() {

  
  //Initialize Serial Monitor
  Serial.begin(115200);
  
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.begin(ssid,password);
  while (WiFi.status() !=WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
    
    }
  Serial.println("connected to the wifi");
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  while(!client.connected()){
    String client_id ="esp32-client";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(),mqtt_username,mqtt_password))
  {
      Serial.println("public eqmx mqtt broker connected:");
  }
  else{
    Serial.println("failed with state");
    Serial.print(client.state());
    delay(2000);
      
  }
  
  
  }
  
  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
  pinMode(trigpin, OUTPUT);
  pinMode(echopin,INPUT);

  client.publish(topic,"hello - world");
  client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  distanceCm = duration * SOUND_VELOCITY / 2;
  Serial.println(distanceCm);

  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}


 
void loop() {
 
  client.loop();
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);


  duration = pulseIn (echopin, HIGH);
  distanceCm = duration * SOUND_VELOCITY / 2;

  char test[30];
  
   if (distanceCm < 100){
      if(race_started == true && flag == 0) {
        end_time = millis();
        race_started == false;
        flag = 1;
       }
        Time = abs(end_time - start_time) * 0.001;
        if (Time !=0 ){
           if (pre_time != Time){
              Serial.println(String("Time: ")+String(Time));
              sprintf(test,"Time %f",Time);
              client.publish(topic,test);
              pre_time = Time;
            }
          }
//       
        
    }

    delay(500);
//  }
 }
