//======================================== Including the libraries.
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "DHT.h"
//======================================== 

//======================================== Insert your network credentials.
#define WIFI_SSID "IoT"
#define WIFI_PASSWORD "@IoT@S3nh@S3gur@"
//======================================== 

//======================================== Defines the pin and type of DHT sensor and initializes the DHT sensor.
#define DHTPIN 21
#define DHTTYPE DHT22
DHT dht22(DHTPIN, DHTTYPE);
//======================================== 

// Defines the Digital Pin of the "On Board LED".
#define On_Board_LED 25

// Defines the Digital Pin of the LED.
#define LED_01_PIN 23

//Provide the token generation process info.
#include "addons/TokenHelper.h"

//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert Firebase project API Key
#define API_KEY "AIzaSyBBHZshB_tDY88x7sBndldW4WYbnI8T9t8"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://esp32-realtime-database-2728b-default-rtdb.firebaseio.com/" 

// Define Firebase Data object.
FirebaseData fbdo;

// Define firebase authentication.
FirebaseAuth auth;

// Definee firebase configuration.
FirebaseConfig config;

//======================================== Millis variable to send/store data to firebase database.
unsigned long sendDataPrevMillis = 0;
const long sendDataIntervalMillis = 10000; //--> Sends/stores data to firebase database every 10 seconds.
//======================================== 

// Boolean variable for sign in status.
bool signupOK = false;

float Temp_Val;
int Humd_Val;
int LED_01_State;

//________________________________________________________________________________ Get temperature and humidity values from the DHT11 sensor.
void read_DHT22() {
  Temp_Val = dht22.readTemperature();
  Humd_Val = dht22.readHumidity();

  //---------------------------------------- Check if any reads failed.
  if (isnan(Temp_Val) || isnan(Humd_Val)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    Temp_Val = 0.00;
    Humd_Val = 0;
  }
  //---------------------------------------- 

  Serial.println("---------------Read_DHT");
  Serial.print(F("Humidity   : "));
  Serial.print(Humd_Val);
  Serial.println("%");
  Serial.print(F("Temperature: "));
  Serial.print(Temp_Val);
  Serial.println("°C");
  Serial.println("---------------");
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Store data to firebase database.
void store_data_to_firebase_database() {
  Serial.println();
  Serial.println("---------------Store Data");
  digitalWrite(On_Board_LED, HIGH);
  
  // Write an Int number on the database path DHT22_Data/Temperature.
  if (Firebase.RTDB.setFloat(&fbdo, "DHT22_Data/Temperature", Temp_Val)) {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
  
  // Write an Float number on the database path DHT22_Data/Humidity.
  if (Firebase.RTDB.setInt(&fbdo, "DHT22_Data/Humidity", Humd_Val)) {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
  
  digitalWrite(On_Board_LED, LOW);
  Serial.println("---------------");
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Read data from firebase database.
void read_data_from_firebase_database() {
  Serial.println();
  Serial.println("---------------Get Data");
  digitalWrite(On_Board_LED, HIGH);
  
  if (Firebase.RTDB.getInt(&fbdo, "/LED_Data/LED_01")) {
    if (fbdo.dataType() == "int") {
      LED_01_State = fbdo.intData();
      digitalWrite(LED_01_PIN, LED_01_State);
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      Serial.print("LED_01_State : ");
      Serial.println(LED_01_State);
    }
  }
  else {
    Serial.println(fbdo.errorReason());
  }

  digitalWrite(On_Board_LED, LOW);
  Serial.println("---------------");
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID SETUP
void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  Serial.println();

  pinMode(On_Board_LED, OUTPUT);
  pinMode(LED_01_PIN, OUTPUT);

  //---------------------------------------- The process of connecting the WiFi on the ESP32 to the WiFi Router/Hotspot.
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("---------------Connection");
  Serial.print("Connecting to : ");
  Serial.println(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");

    digitalWrite(On_Board_LED, HIGH);
    delay(250);
    digitalWrite(On_Board_LED, LOW);
    delay(250);
  }
  digitalWrite(On_Board_LED, LOW);
  Serial.println();
  Serial.print("Successfully connected to : ");
  Serial.println(WIFI_SSID);
  //Serial.print("IP : ");
  //Serial.println(WiFi.localIP());
  Serial.println("---------------");
  //---------------------------------------- 

  // Assign the api key (required).
  config.api_key = API_KEY;

  // Assign the RTDB URL (required).
  config.database_url = DATABASE_URL;

  // Sign up.
  Serial.println();
  Serial.println("---------------Sign up");
  Serial.print("Sign up new user... ");
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  Serial.println("---------------");
  
  // Assign the callback function for the long running token generation task.
  config.token_status_callback = tokenStatusCallback; //--> see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  dht22.begin();

  delay(1000);
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID LOOP
void loop() {
  // put your main code here, to run repeatedly:
  
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > sendDataIntervalMillis || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    read_DHT22();
    store_data_to_firebase_database();
    read_data_from_firebase_database();
  }
}
