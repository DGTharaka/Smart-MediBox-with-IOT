//import libraries
#include <WiFi.h>
#include "DHTesp.h"
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP32Servo.h>


//define pins
#define DHT_PIN 15
#define BUZZER 12
#define SERVO 18
#define LEFT_LDR 34
#define RIGHT_LDR 35


//initialization
WiFiClient espClient;
PubSubClient mqttClient(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
DHTesp dhtSensor;
Servo servoMotor;


//for servo motor angle
float minAngle = 30;
float controlFactor = 0.75;

//predefined values for A,B & C tablets
float minAngleA = 40;
float controlFactorA = 0.6;

float minAngleB = 50;
float controlFactorB = 0.5;

float minAngleC = 60;
float controlFactorC = 0.3;


//arrays to store inputs as a string
char tempAr[6];
char lightAr[6];


bool isScheduledON = false;
unsigned long scheduledOnTime;
float servoAngle;
double D;
float tablet; //variable to assign value comes from ENTC-MEDIBOX-DROPDOWN



void setup() {

  Serial.begin(115200);
  setupWifi();
  setupMqtt();

  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);

  timeClient.begin();
  timeClient.setTimeOffset(5.5*3600); //setting time offset

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW); //at the begining buzzer is silent

  pinMode(LEFT_LDR, INPUT);
  pinMode(RIGHT_LDR, INPUT);


  //attach the servo motor to the specified pin, with a pulse width range of 500 to 2400 microseconds
  servoMotor.attach(SERVO, 500, 2400); 


}

void loop() {
  if(!mqttClient.connected()){
    connectToBroker();
  }

  mqttClient.loop();

  updateTemperature(); //read temperature from DHT sensor
  Serial.print("Temperature: ");
  Serial.println(tempAr);
  mqttClient.publish("ENTC-DASHBOARD-TEMP", tempAr); //publish temperature

  checkSchedule();

  updateLightIntensity(); //read light intensity from two LDRs and get maximum  
  mqttClient.publish("ENTC-DASHBOARD-LIGHT",lightAr); //publish that light intensity to MQTT topic
  

  delay(1000);

}

void updateTemperature(){
  TempAndHumidity data = dhtSensor.getTempAndHumidity();

  //convert the temperature value to a string with 2 decimal places, 
  //and store it as a character array with a maximum length of 6 characters
  String(data.temperature,2).toCharArray(tempAr, 6);
}


void updateLightIntensity() {
  const float analogMinIntensity = 0;   //minimum analogRead value
  const float analogMaxIntensity = 1023.0; //maximum analogRead value

  //read the analog values from the left and right LDRs and assign them to new variables
  float sensorValueLeft = analogRead(LEFT_LDR);
  float sensorValueRight = analogRead(RIGHT_LDR);
  
  double maxIntensity;
  String maxIntensitySensor;


  if(sensorValueLeft > sensorValueRight){
    maxIntensity = sensorValueLeft/analogMaxIntensity;
    maxIntensitySensor = "LEFT_LDR";
    D = 1.5;
  }else if(sensorValueRight > sensorValueLeft){
    maxIntensity = sensorValueRight/analogMaxIntensity;
    maxIntensitySensor = "RIGHT_LDR";
    D = 0.5;
  }else{
    //since it is not given, I assume that D = 0 when sensor values of left LDR and right LDR are same
    maxIntensity = sensorValueRight/analogMaxIntensity;
    maxIntensitySensor = "BOTH_LDR";
    D = 0;
  }

  maxIntensity = constrain(maxIntensity, 0.0, 1.0); //maxIntensity is constrained within the range [0.0, 1.0]
  //if maxIntensity is already within this range, it remains unchanged.

  String intensityString = String(maxIntensity, 2); //convert maxIntensity to a String with two decimal places  
  intensityString.toCharArray(lightAr, 6); //convert intensityString to a character array

  Serial.print(maxIntensitySensor);
  Serial.print(": ");

  //if maxIntensitySensor is "LEFT_LDR", print sensorValueLeft;
  //otherwise, print sensorValueRight.
  Serial.print((maxIntensitySensor == "LEFT_LDR") ? sensorValueLeft : sensorValueRight); 
  Serial.print("  Intensity: ");
  Serial.println(maxIntensity, 2);

  mqttClient.publish("ENTC-MEDIBOX-MAX-LDR", maxIntensitySensor.c_str());

  AdjustServoMotor(maxIntensity);


}


void AdjustServoMotor(double lightintensity){

  if(tablet == 1){

    double angle = minAngleA * D +(180.0-minAngleA)*lightintensity*controlFactorA;
    double motorAngle = constrain(angle, 0, 180);
    Serial.print("Motor Angle: ");
    Serial.println(motorAngle);
    servoMotor.write(motorAngle); //set servo motor angle

  }else if(tablet == 2){

    double angle = minAngleB * D +(180.0-minAngleB)*lightintensity*controlFactorB;
    double motorAngle = constrain(angle, 0, 180);
    Serial.print("Motor Angle: ");
    Serial.println(motorAngle);
    servoMotor.write(motorAngle);

  }else if(tablet == 3){

    double angle = minAngleC * D +(180.0-minAngleC)*lightintensity*controlFactorC;
    double motorAngle = constrain(angle, 0, 180);
    Serial.print("Motor Angle: ");
    Serial.println(motorAngle);
    servoMotor.write(motorAngle);

  }else{

    double angle = minAngle * D +(180.0-minAngle)*lightintensity*controlFactor;
    double motorAngle = constrain(angle, 0, 180);
    Serial.print("Motor Angle: ");
    Serial.println(motorAngle);
    servoMotor.write(motorAngle);

  }
}



void setupWifi(){
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println("Wokwi-GUEST");
  WiFi.begin("Wokwi-GUEST", "");

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupMqtt(){
  mqttClient.setServer("test.mosquitto.org", 1883); //set MQTT server and port
  mqttClient.setCallback(receiveCallback); 
}

void receiveCallback(char* topic, byte* payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  char payloadCharAr[length];

  for(int i= 0; i<length; i++){
    Serial.print((char)payload[i]);
    payloadCharAr[i] = (char)payload[i];
  }

  Serial.println();

  if (strcmp(topic, "ENTC-DASHBOARD-MAIN-ON-OFF") == 0){ //compare topic and ENTC-DASHBOARD-MAIN-ON-OFF
    buzzerOn(payloadCharAr[0] == '1');  //If the payload indicates "1", turn the buzzer on; otherwise, turn it off
  }else if(strcmp(topic, "ENTC-DASHBOARD-SCH-ON") == 0){
    if(payloadCharAr[0] == 'N'){ 
      isScheduledON = false;  //if the payload starts with 'N' (NOT), disable scheduled ON mode; otherwise, enable it
    }else{
      isScheduledON = true;
      scheduledOnTime = atol(payloadCharAr);  //set the scheduled ON time based on the payload
    }

  } 

  if (strcmp(topic,"ENTC-MEDIBOX-MINIMUM-ANGLE")==0){ //compare received MQTT topic with "ENTC-MEDIBOX-MINIMUM-ANGLE"
      minAngle = atof(payloadCharAr); //convert payload to a float
      Serial.println(minAngle);
  }
  if (strcmp(topic,"ENTC-MEDIBOX-CONTROL-FACTOR")==0){
      controlFactor = atof(payloadCharAr);
      Serial.println(controlFactor);
  }
  if (strcmp(topic,"ENTC-MEDIBOX-DROPDOWN")==0){
      tablet = atof(payloadCharAr);
      Serial.println(tablet);
  }

}

void connectToBroker(){
  while (!mqttClient.connected()){
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP32-456123789")){
      Serial.println("connected");

      //subscribe to relevant topics
      mqttClient.subscribe("ENTC-DASHBOARD-MAIN-ON-OFF");
      mqttClient.subscribe("ENTC-DASHBOARD-SCH-ON");
      mqttClient.subscribe("ENTC-MEDIBOX-MINIMUM-ANGLE");
      mqttClient.subscribe("ENTC-MEDIBOX-CONTROL-FACTOR");
      mqttClient.subscribe("ENTC-MEDIBOX-DROPDOWN");

    }else{
      Serial.print("failed");
      Serial.print(mqttClient.state());
      delay(5000);
    }
  }
}


void buzzerOn(bool on){
  if(on){
    tone(BUZZER, 256);
  }else{
    noTone(BUZZER);
  }
}

unsigned long getTime(){
  timeClient.update();
  return timeClient.getEpochTime();
}

void checkSchedule(){
  if(isScheduledON){
    unsigned long currentTime = getTime();
    if(currentTime > scheduledOnTime){
      buzzerOn(true);
      isScheduledON = false;

      mqttClient.publish("ENTC-MEDIBOX-MAIN-ON-OFF-ESP" , "1");
      mqttClient.publish("ENTC-MEDIBOX-SCH-ESP-ON" , "0");
      
      Serial.println("Scheduled ON");
    }

  }

}
