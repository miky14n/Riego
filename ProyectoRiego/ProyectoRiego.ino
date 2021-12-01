#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
const int TRIGGER_PIN = 13;
const int HUMITY_PIN = 32;
const int ESTATUS_HUMITY_PIN = 35;
const int DHT_PIN = 26;
const int DHT_TYPE = 11;
const int LED_GREEN = 12;
const int LED_RED = 14;

DHT dht(DHT_PIN, DHT_TYPE);

long lecturaHumity;
float temperature;
int statusHumity;
String estausAlexa;
String estatusAWS;

const char* WIFI_SSID = "COMTECO-95888697";
const char* WIFI_PASS = "FTDMP20072";
const char* MQTT_BROKER = "algxu5d14kq3f-ats.iot.us-east-2.amazonaws.com";
const int MQTT_PORT = 8883;

const char* CLIENT_ID = "Grupo8"; // unique client id
const char* IN_TOPIC = "ucb/grupo8/on";   // subscribe
//const char* IN_TOPIC_ALEXA = "ucb/grupo8/on";   // subscribe
const char* OUT_TOPIC = "ucb/iot/out"; // publish

const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUMFHnX62496G0biJ7xy4o6TlCJ6IwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIxMTAyMDIyNDY1
MFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOEieYvzWifWmvD3HbGB
MmTYGZTHlE56311j6XTQ8Qkv4ZDC4gkdrVO7rmkLQsd+q8HTYaoM8+n7TYvb5Odn
nvWYsvEJGWE25BbQKs9pWcR9ynosOHnBMBLjjoXW7rKX9wsTWSXLYfmI1POKhJhI
KyXps8fhSyLv0n3/6kHvuN12nZNN3SruUK1g7eCMIfKIjD088HYba2D4PN6sJOP0
SRng/Wk/9rIqgjXlRQE9BQW7xA4jxz2P0svL1TTYOymOzI/wMC0b5eEKPUuxsQhl
Jfn9NJkQIaRjnvmW9ENXUg7h4PTyZYCerQho1G40hm7CCoz4RqmsGhdiPRGt4M0r
UnsCAwEAAaNgMF4wHwYDVR0jBBgwFoAUL9Kab4ElL1fYE3PKSb75SN6uyK4wHQYD
VR0OBBYEFKMMFoJ+TgPY/XurcudJh3aj2lFTMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCiQrnpSncj5BM1j4o0DUu7H9+G
iHYxZ69ooEOPp+sZuPVmZJj1xttQ8I0Qm1TcYSjYMzGPcO9v7bXg2sbePQgYv86L
PIGvEB+4EVFGXDK3i4h9nXr/AjzTwkr7sIdExbMw/bUiMur0Ss3aCQ7clk43Pd8v
+idzetxM9FPEEMMdn1vp5mqURTdp78bRZ8ZtJPL6/YIW4Y3Cvh43WsndpHtWxhus
FiGxIWkiwIzDqYNvQPclm9mwRqa9cX+4dKnCp1N1vo7P627gSu3ynWALTFYkuSmM
kWVZcC5xT3j5tzhPI7dlzYeYl0qGqnE68exzeCYgrIzolVABQiQ7lI4xo8pQ
-----END CERTIFICATE-----
)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEA4SJ5i/NaJ9aa8PcdsYEyZNgZlMeUTnrfXWPpdNDxCS/hkMLi
CR2tU7uuaQtCx36rwdNhqgzz6ftNi9vk52ee9Ziy8QkZYTbkFtAqz2lZxH3Keiw4
ecEwEuOOhdbuspf3CxNZJcth+YjU84qEmEgrJemzx+FLIu/Sff/qQe+43Xadk03d
Ku5QrWDt4Iwh8oiMPTzwdhtrYPg83qwk4/RJGeD9aT/2siqCNeVFAT0FBbvEDiPH
PY/Sy8vVNNg7KY7Mj/AwLRvl4Qo9S7GxCGUl+f00mRAhpGOe+Zb0Q1dSDuHg9PJl
gJ6tCGjUbjSGbsIKjPhGqawaF2I9Ea3gzStSewIDAQABAoIBAQDSOcwACEx+eOIT
8t1SXE8/ZZ91k2wEYK5k2aAtSdJZ/tEFyUUiuAia8N1rD23bNYiTJJzFGvVE3zpJ
SBuWjeccdOtbMmD1qzkPqHDhXwHI/qFsthBCeUg3yy/SAKLCOC8is32CUkl5pvEb
PZSVUoagD3umQNVVmVYX/Dm2OnCIsrL47BB6NlGv92Su6RzmjNxVWLZ+tIEemdA6
lZpfMwFLb1T1DCMqraRdi/MEwYq4w+i/EheaVyWeBBTzhotDv/hiL+L7b+SOqKpX
ybwD8Lhb6JSCk+DEMmFtC95RMWSZAC8OT6xjB4lJ61DmtpYKjzutcvaE0i/5I8Dp
UMQ/+3OZAoGBAPVzyY+UvdGOYjPIUzmxFHSyj9dwytUGdC95mWQceuP59/2BWK21
YTmR9ajvaugkU6IbJtVoWHiGN1eMpieSD3Vc+5eIGs0Vx1bCmD/wViahxPn6j6og
CW2uqAL+gnt6xVSyOXPigyFgehBwZJ/17D/ILV73PcXb6JCkyRuC5vUFAoGBAOrP
LIRG3kSBNUga4jgm+YlyDKWqvWoPsOWmAXoABXJme4TnwsXsjD8PXi6fDU/Gvpjy
9rEvB+PccuZZA4KIlEB/ADzIt4XF0Fjy2+hVZv1zf6WvAMKlrNeCxhmpZaMCrcwQ
qb9adyPKA9QtGe1rcpx+0CrtIYr482ix17mck8F/AoGAB54qWUxyf7h9j8Hd42J9
kgmgsT0M7oOew4JwokzH+59cdP+4stt4QwMmrrGPjdjix5s0H1d3t3Xi7dKtrXi5
Bhl2rYNJ4aeVYjnz47gf1F8c7+043DzWThMnn8AqaI6s5S4K3m/fRf/qiewTE2vc
gzZBQjtGLYffte2HWFU9uckCgYEAyczrsXNGqxFwvVt9eEzmwR1niSnISFLb8UJO
mo9SHxO8rGiSmI/HqJqIkwDW6+0e+Uw2f+ivm0Fm7lkqDxFaeGOtzI+kSi/L8+Yk
4PQk40ZUOgzBVogUl9Modf2EACjZPTxE5tPU+MkxqTRpyAZEJgNG3N2XqlxytVPC
1x6eTfkCgYBpfO7Avi/PWr0ralUG3ZVqv6hUf3KgtljyrbaLsDwKov07KEpZxXU7
UMIQ9IFq8P2UDKU+jRjGilTcXiUpeKLXNEUHCNkgrGwYAgBVNkk+tBsRkpUF+zX4
8sdN7SOPwWYmyFZbvCKBBP9DVqZ4reVybw/zPcBs/44k38HsJ2KTjA==
-----END RSA PRIVATE KEY-----
)KEY";

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);


StaticJsonDocument<JSON_OBJECT_SIZE(1)> inputDoc;

// PubSubClient callback function
void callback(const char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += String((char) payload[i]);
  }
  if (String(topic) == IN_TOPIC) {
    //Serial.println("Message from topic " + String(IN_TOPIC) + ":" + message);
    

    DeserializationError err = deserializeJson(inputDoc, payload);
    if (!err) {
      String action = String(inputDoc["action"].as<char*>());
      if (action == "encendiendo riego"){
        estausAlexa=String(inputDoc["action"].as<char*>());
      }
      else if (action == "detener") {
        estatusAWS=action;
      }
      else Serial.println("Unsupported action received: " + action);
    }
  }
  
}



void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(32, INPUT);
  digitalWrite(32, HIGH);
  pinMode(35, OUTPUT);
  digitalWrite(35, HIGH);
  dht.begin();
  //Serial.begin(115200);
  Serial.begin(9600);
  Serial.println("Connecting to WiFi...");

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Couldn't connect to WiFi.");
    while(1) delay(100);
  }
  Serial.println("Connected to " + String(WIFI_SSID));

  wifiClient.setCACert(AMAZON_ROOT_CA1);
  wifiClient.setCertificate(CERTIFICATE);
  wifiClient.setPrivateKey(PRIVATE_KEY);

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(callback);
}

boolean mqttClientConnect() {
  Serial.println("Connecting to MQTT broker...");
  if (mqttClient.connect(CLIENT_ID)) {
    Serial.println("Connected to " + String(MQTT_BROKER));

    mqttClient.subscribe(IN_TOPIC);

    Serial.println("Subscribed to " + String(IN_TOPIC));
  } else {
    Serial.println("Couldn't connect to MQTT broker.");
  }
  return mqttClient.connected();
}

unsigned long previousConnectMillis = 0;
unsigned long previousPublishMillis = 0;

unsigned char counter = 0;

StaticJsonDocument<JSON_OBJECT_SIZE(1)> outputDoc;
char outputBuffer[128];

void publishMessage(unsigned char humedad) {
  outputDoc["humedad"] = humedad;
  serializeJson(outputDoc, outputBuffer);
  mqttClient.publish(OUT_TOPIC, outputBuffer);
}


float readDataFromThings() {
  float temperature = dht.readTemperature(); // Gets the values of the temperature
  float segs = millis();
  Serial.println(temperature);
  //publishMessageJson(OUT_SHADOW_HDT11, temperature, humidity, segs);
  return temperature;
}

void loop() {
  
  delay(100);
 unsigned long now = millis();
  if (!mqttClient.connected()) {
    if (now - previousConnectMillis >= 5000) {
      previousConnectMillis = now;
      if (mqttClientConnect()) {
        previousConnectMillis = 0;
      } else delay(1000);
    }
  }
  else {
     //readDataFromThings();
    mqttClient.loop();
    //Serial.println(lecturaHumity);
    if(estausAlexa=="encendiendo riego"){
      lecturaHumity = analogRead(HUMITY_PIN);
      statusHumity= digitalRead(ESTATUS_HUMITY_PIN);
      temperature= readDataFromThings();
      if(lecturaHumity>1500 && temperature>17 && temperature<31){
        digitalWrite(LED_RED, LOW);
        Serial.print("sepuede Regar");
        digitalWrite(LED_GREEN, HIGH);
        
      }
      else{
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, LOW);
      }
      Serial.print("La lectura es: ");
      delay(100);
      Serial.println(lecturaHumity);
      Serial.println();
      delay(100);
    }
    if(estatusAWS=="detener"){
      Serial.println("detene");
    }
    //Serial.println(statusHumity);
    delay(1000);
    publishMessage(lecturaHumity);
  }
   
}
