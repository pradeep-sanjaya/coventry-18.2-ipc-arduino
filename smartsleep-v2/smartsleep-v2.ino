#define HEART_IN 35
#define VIBRATOR_OUT 15
#define THRESHOLD 4080

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


BLEServer* pServer = NULL;
BLECharacteristic* pCharHeart = NULL;
//BLECharacteristic* pCharVibrate = NULL;
BLECharacteristic * pRxChar;

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

float x_value = 0.0;

#define SERVICE_UUID      "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHAR_RX           "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHAR_HEART_UUID   "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
//#define CHAR_VIBRATE_UUID "6E400004-B5A3-F393-E0A9-E50E24DCCA9E"

class HeartServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
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
    Serial.begin(115200);
    pinMode(HEART_IN, INPUT);
    pinMode(VIBRATOR_OUT, OUTPUT);

    // Create the BLE Device
    BLEDevice::init("SmartSleep-v2");

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new HeartServerCallbacks());

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Heart Characteristic
    pCharHeart = pService->createCharacteristic(
        CHAR_HEART_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );         
    pCharHeart->addDescriptor(new BLE2902());

    // Create a BLE Vibrate Characteristic
    /*
    pCharVibrate = pService->createCharacteristic(
        CHAR_VIBRATE_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharVibrate->addDescriptor(new BLE2902());
    */

    pRxChar = pService->createCharacteristic(
        CHAR_RX,
        BLECharacteristic::PROPERTY_WRITE
    );

    pRxChar->setCallbacks(new SleepCharacteristicCallbacks());
    
    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();
    Serial.println("Waiting a client connection to notify...");
}

void loop() {
    
    x_value = analogRead(HEART_IN);
    Serial.println(x_value);

    // vibrate on threshold
    if (x_value >= THRESHOLD) {
        vibrate();
    }
    
    // notify changed value
    if (deviceConnected) {
        pCharHeart->setValue(((String)x_value).c_str());
        pCharHeart->notify();
        delay(3); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
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
    Serial.println("... vibrating ...");
    
    // notify vibrate on
    notifyVibrate(true);
    
    digitalWrite(VIBRATOR_OUT, HIGH);
    delay(100);
    
    digitalWrite(VIBRATOR_OUT, LOW);
    delay(100);

    digitalWrite(VIBRATOR_OUT, HIGH);
    delay(200);
    
    digitalWrite(VIBRATOR_OUT, LOW);
    delay(100);

    digitalWrite(VIBRATOR_OUT, HIGH);
    delay(200);

    digitalWrite(VIBRATOR_OUT, LOW);

    // notify vibrate off
    notifyVibrate(false);
}

void notifyVibrate(bool value) {
    if (deviceConnected) {
        //pCharVibrate->setValue(String(value).c_str());
        //pCharVibrate->notify();
        delay(3); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
    }
}
