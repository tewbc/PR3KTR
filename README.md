# PR3KTR
NPK , PR-3000-TR-NPK
## Installation
Just use Arduino Library Manager and search "PR-3000-TR-NPK Library" in Sensors category.
## Main assumptions
- easy as possible,
- minimal memory consumption,
- non-blocking functions,
- supporting Device ID 1 -254.

## Basic example
```
#include "PR3KTR.h"
PR3KTR npk(Serial2);                              // decare class to npk
PR3KTR::DATA data;                                // decare DATA structure to data
void setup() {
  Serial.begin(9600);                             // GPIO1, GPIO3 (TX/RX pin on ESP-32 Development Board)
  Serial2.begin(4800);                            // GPIO17, GPIO16 (TX/RX pin on ESP-32 Development Board)
  npk.setID(0x01);                                // Set Slave ID
}

void loop() {
  if (npk.getN(data))   {                         //Get N value
    Serial.print("N (mg/L): ");
    Serial.println(data.NITROGEN);
  }
  delay(200);
  if (npk.getP(data))  {                          //Get P value
    Serial.print("P (mg/L): ");
    Serial.println(data.POTASSIUM);
  }
  delay(200);
  if (npk.getK(data))   {                         //Get K value
    Serial.print("K (mg/L): ");
    Serial.println(data.PHOSPHORUS);
  }
  delay(200);
  Serial.println();
  // Do other stuff...
}
```
## Output
```
N (mg/L):  103
P (mg/L):  108
K (mg/L):  203
```
## Additional remarks
Tested with PR-3000-TR-NPK-N01 and ESP-32 WROOM Development Board.
