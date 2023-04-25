/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

/** NimBLE differences highlighted in comment blocks **/

/*******original********
  #include <BLEDevice.h>
  #include <BLEUtils.h>
  #include <BLEScan.h>
  #include <BLEAdvertisedDevice.h>
  #include "BLEEddystoneURL.h"
  #include "BLEEddystoneTLM.h"
  #include "BLEBeacon.h"
***********************/

#include <Arduino.h>

#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>
#include "NimBLEEddystoneURL.h"
#include "NimBLEEddystoneTLM.h"
#include "NimBLEBeacon.h"

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))

int scanTime = 5; //In seconds
BLEScan *pBLEScan;
const std::string mm_uuid = "b9407f30-f5f8-466e-aff9-25556b57fe6e";

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    /*** Only a reference to the advertised device is passed now
      void onResult(BLEAdvertisedDevice advertisedDevice) { **/
    void onResult(BLEAdvertisedDevice *advertisedDevice)
    {
        if (advertisedDevice->haveManufacturerData() == true)
        {
            std::string strManufacturerData = advertisedDevice->getManufacturerData();
            ;

            uint8_t cManufacturerData[100];
            strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);

            if (strManufacturerData.length() == 25 && cManufacturerData[0] == 0x4C && cManufacturerData[1] == 0x00)
            {

                Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>");
                
                Serial.println(" >>>>>Found an iBeacon!");
                BLEBeacon oBeacon = BLEBeacon();
                oBeacon.setData(strManufacturerData);

                if (oBeacon.getProximityUUID().toString().compare(mm_uuid) == 0){
                    Serial.printf(" >>>>>>Find Mamorio \n");
                }

                Serial.printf(" >>>>>>iBeacon Frame\n");
                Serial.printf("ID: %04X Major: %d Minor: %d UUID: %s Power: %d Address: %s RSSI: %d\n"
                    , oBeacon.getManufacturerId()
                    , ENDIAN_CHANGE_U16(oBeacon.getMajor())
                    , ENDIAN_CHANGE_U16(oBeacon.getMinor())
                    , oBeacon.getProximityUUID().toString().c_str()
                    , oBeacon.getSignalPower()
                    , advertisedDevice->getAddress().toString().c_str()
                    , advertisedDevice->getRSSI()
                );
                Serial.println("<<<<<<<<<<<<<<<<<<<<<<<<<<");
            }
        }
        return;

    }
};

void setup()
{
  Serial.begin(115200);
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(false); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // less or equal setInterval value
}

void loop()
{
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.println("------------------------------");
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  delay(1);
}