#include "PR3KTR.h"
PR3KTR npk(Serial2);                             // decare class to npk name.
PR3KTR::DATA data;                               // decare DATA structure to data

void setup() {
  Serial.begin(9600);                            // GPIO1,   GPIO3 (TX/RX pin on ESP-32 Development Board)
  Serial2.begin(4800);                           // GPIO17, GPIO16 (TX/RX pin on ESP-32 Development Board)
  npk.setID(0x01);                               // Set Slave ID
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
  // Do other stuff...
}
