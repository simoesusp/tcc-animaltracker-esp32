/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 5; //In seconds
BLEScan *pBLEScan;

void AdvertisingPayLoadReader(uint8_t *payload, size_t payloadSize)
{
  printf("\n Started Decoding, Size: %d \n \n\n ",payloadSize);

  uint8_t auxPayload[16] = {  payload[9],payload[10] ,payload[11],payload[12],
                              payload[13],payload[14],payload[15],payload[16],
                              payload[17],payload[18],payload[19],payload[20],
                              payload[21],payload[22],payload[23],payload[24] };
  
  String data;
  data.reserve(payloadSize+1); // prepare space for the buffer and extra termination character '\0'
  for (int i = 0; i<payloadSize; ++i) {
      data += (char)auxPayload[i]; // typecast because String takes uint8_t as something else than char
  }
  printf("\n Recontructed UUID: %s \n",data);

  printf("\n iBeacon Prefix(9 bytes): %02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX \n",payload[0],
                    payload[1],payload[2],payload[3],payload[4],payload[5],payload[6],payload[7],payload[8]);
  printf("\n UUID(16 bytes): %02hhX%02hhX%02hhX%02hhX - %02hhX%02hhX - %02hhX%02hhX - %02hhX%02hhX - %02hhX%02hhX%02hhX%02hhX%02hhX%02hhX \n",
                    payload[9],payload[10],payload[11],payload[12],payload[13],payload[14],payload[15],payload[16],
                    payload[17],payload[18],payload[19],payload[20],payload[21],payload[22],payload[23],payload[24]); 
  printf("\n Major Number(2 bytes): %02hhX%02hhX \n",payload[25],payload[26]);
  printf("\n Minor Number(2 bytes): %02hhX%02hhX \n",payload[27],payload[28]);
  printf("\n TX Power(1 bytes): %02hhX \n",payload[29]);
  printf("\n\n\n Finished Decoding \n\n\n ");

}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    Serial.printf("\n Advertised Device: %s \n", advertisedDevice.toString().c_str());
    Serial.printf("Advertised getPayloadLength: %d  \n", advertisedDevice.getPayloadLength());
    AdvertisingPayLoadReader(advertisedDevice.getPayload(), advertisedDevice.getPayloadLength());
  }
};

void setup()
{
  Serial.begin(115200);

  Serial.println("Scanning...");
  BLEAdvertisedDevice myDevice;
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // less or equal setInterval value
}

void loop()
{
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());

  Serial.println("Scan done!");
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  delay(2000);
}