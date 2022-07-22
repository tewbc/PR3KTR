#include "PR3KTR.h"
PR3KTR npk(Serial2);		// decare class to other name.
PR3KTR::DATA data;		  // decare DATA structure to other name

void setup() {
  Serial.begin(9600);		// GPIO1, GPIO3 (TX/RX pin on ESP-12E Development Board)
  Serial2.begin(4800);	// GPIO15, GPIO13 (TX/RX pin on ESP-12E Development Board)
  npk.setID(0x01);		  // Set Slave ID
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
  // Do other stuff...
}