#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

/* ble */
BLEServer* pServer = NULL;
BLEService *pService = NULL;
BLECharacteristic * pTxCharacteristic;
BLECharacteristic * pRxCharacteristic;

bool deviceConnected = false;
bool oldDeviceConnected = false;
//uint8_t value = 0;


/* sensor reading */
#define sensorPin 32
#define vibrateMotorPin 2

int sensorValue = 0;

class SlepServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("... device connected ...");
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println("... device disconnected ...");
    }
};


class SleepCharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();

        if (rxValue.length() > 0) {
            Serial.println("*********");
            Serial.print("Received Value: ");
            for (int i = 0; i < rxValue.length(); i++)
                Serial.print(rxValue[i]);

            Serial.println();
            Serial.println("*********");
        }
    }
};

void setup() {
  
    pinMode(sensorPin, INPUT);
    pinMode(vibrateMotorPin, OUTPUT);  
  
    Serial.begin(115200);
    Serial.println("Starting BLE...");

    BLEDevice::init("SmartPillow");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new SlepServerCallbacks());

    
    pService = pServer->createService(SERVICE_UUID);
    
    // Create a BLE Characteristic
    pTxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY
    );
                      
    pTxCharacteristic->addDescriptor(new BLE2902());

    BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE
    );

    pRxCharacteristic->setCallbacks(new SleepCharacteristicCallbacks());


    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
    Serial.println("characteristic defined.");
}

void loop() {
    sensorValue = 0;
    sensorValue = analogRead(sensorPin);
    Serial.println("sensor value: " + (String)sensorValue);
    
    std::string notifyValue = ((String)sensorValue).c_str();

    if (sensorValue >= 300) {
        vibrate();
    }
    
    // notify changed value
    if (deviceConnected) {
        pTxCharacteristic->setValue(notifyValue);
        pTxCharacteristic->notify();
        delay(10); // bluetooth stack will go into congestion, if too many packets are sent
    }
    
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
    
    delay(1000);
}

void vibrate() {
    Serial.println("... vibrate start ...");
    
    digitalWrite(vibrateMotorPin, HIGH);
    delay(100);
    
    digitalWrite(vibrateMotorPin, LOW);
    delay(50);
    
    digitalWrite(vibrateMotorPin, HIGH);
    delay(100);
    
    digitalWrite(vibrateMotorPin, LOW);
    delay(50);
    
    digitalWrite(vibrateMotorPin,HIGH);
    delay(500);
    
    digitalWrite(vibrateMotorPin, LOW);
    delay(1000);
    
    Serial.println("... vibrate end ...");   
}
