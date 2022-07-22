# PR3KTR
NPK , PR-3000-TR-NPK
## Installation
Just use Arduino Library Manager and search "PR-3000-TR-NPK Library" in Sensors category.
## Main assumptions
- easy as possible,
- minimal memory consumption,
- non-blocking functions,
- supporting Device ID 1 -254.

As a data source you can use **any object** that implements the **Stream class**, such as Wire, Serial, EthernetClient, e.t.c.
## Basic example
```
#include "PR3KTR.h"
PR3KTR npk(Serial2);    // decare class to other name.
PR3KTR::DATA data;      // decare DATA structure to other name
void setup() {
  Serial.begin(9600);   // GPIO1, GPIO3 (TX/RX pin on ESP-12E Development Board)
  Serial2.begin(4800);  // GPIO15, GPIO13 (TX/RX pin on ESP-12E Development Board)
  npk.setID(0x01);      // Set Slave ID
}

void loop() {
  if (npk.getNPK(data))	// Get NPK at once
  {
    Serial.print("N (mg/L): ");
    Serial.println(data.NITROGEN);

    Serial.print("P (mg/L): ");
    Serial.println(data.POTASSIUM);

    Serial.print("K (mg/L): ");
    Serial.println(data.PHOSPHORUS);

    Serial1.println();
  }
  delay(5000);
}
```
## Output
```
N (mg/L):  103
P (mg/L):  108
K (mg/L):  203
```
## Additional remarks
Tested with PR-3000-TR-NPK-N01 and ESP-12E Development Board (ESP8266 nodeMCU V3.0).
