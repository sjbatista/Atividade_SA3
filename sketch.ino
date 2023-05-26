#include <DHTesp.h>
#include <WiFi.h>
#include <PubSubClient.h>

/*const int* armazenat [ ]
const int* armazenah [ ]*/

// Dados da rede Wi-Fi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Dados do sensor DHT
const int dhtPin = 13;
const DHTesp::DHT_MODEL_t dhtType = DHTesp::DHT22;

// Dados do HiveMQ
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttTopic = "testtopic/1";

DHTesp dhtSensor;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Protótipo da função connectToMQTT
void connectToMQTT();

void setup() {
  Serial.begin(115200);
  Serial.println("Inicializando Sistema");

  // Conectando à rede Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }

  // Inicializando o sensor DHT
  dhtSensor.setup(dhtPin, dhtType);
  pinMode(23, OUTPUT);
  pinMode(18, OUTPUT);
  Serial.println("Finalizando Sistema");

  // Conectando ao servidor MQTT
  connectToMQTT();
}

void loop() {
  // Verificando a conexão Wi-Fi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conexão WiFi perdida. Tentando reconectar...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Conectando ao WiFi...");
    }
    connectToMQTT(); // Reconectar ao servidor MQTT após a reconexão Wi-Fi
  }

  float temperatura = dhtSensor.getTemperature();
  float humidade = dhtSensor.getHumidity();

  Serial.println("Temperatura: " + String(temperatura) + "C");
  Serial.println("Humidade: " + String(humidade) + "%");
  Serial.println("--------------------------------------------");

  // Publicando dados no tópico MQTT do HiveMQ
  String temperaturaString = String(temperatura);
  String humidadeString = String(humidade);

  mqttClient.publish(mqttTopic, "{\"temperatura\" : 24,\"umidade\" : 40}");

  /*armazenat = temperatura
  armazenah = humidade*/

  delay(3000);

  digitalWrite(23, HIGH);
  digitalWrite(18, HIGH);
  delay(1000);

  digitalWrite(23, LOW);
  digitalWrite(18, LOW);
  delay(1000);
}

void connectToMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  while (!mqttClient.connected()) {
    Serial.println("Conectando ao servidor MQTT...");
    String clientId = "salmao-";
    clientId += String(random(0xffff), HEX);
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("Conectado ao servidor MQTT!");
    } else {
      Serial.print("Falha na conexão MQTT. Erro: ");
      Serial.println(mqttClient.state());
      delay(2000);
    }
  }
}