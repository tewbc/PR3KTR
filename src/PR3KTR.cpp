#include "Arduino.h"
#include "PR3KTR.h"

PR3KTR::PR3KTR(Stream& stream)
{
  this->_stream = &stream;
}

void PR3KTR::setID(uint8_t ID)
{
	a_ID = ID;
}

bool PR3KTR::getN(DATA& data)
{
  uint8_t command[] = { 0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c };
  RTU_CRC(command, 6, &command[6], &command[7]);
  _recvLen = sentDataLen(command);
  _stream->write(command, sizeof(command));
  delay(100);
  _mode = g_N;
  _index = 0;
  _data = &data;
  uint16_t timeout = SINGLE_RESPONSE_TIME;
  uint32_t start = millis();
  do
  {
    loop();
    if (_status == STATUS_OK) 
		break;
  } while (millis() - start < timeout);
/*
	Serial.print("N: ");
	showMSG();
*/
  killArray();
  return _status == STATUS_OK;
}

bool PR3KTR::getP(DATA& data)
{
  uint8_t command[] = { 0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc };
  RTU_CRC(command, 6, &command[6], &command[7]);
  _recvLen = sentDataLen(command);
  _stream->write(command, sizeof(command));
  delay(100);
  _mode = g_P;
  _index = 0;
  _data = &data;
  uint16_t timeout = SINGLE_RESPONSE_TIME;
  uint32_t start = millis();
  do
  {
    loop();
    if (_status == STATUS_OK) 
		break;
  } while (millis() - start < timeout);
/*
	Serial.print("P: ");
	showMSG();
*/
  killArray();
  return _status == STATUS_OK;
}

bool PR3KTR::getK(DATA& data)
{
  uint8_t command[] = { 0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0 };
  RTU_CRC(command, 6, &command[6], &command[7]);
  _recvLen = sentDataLen(command);
  _stream->write(command, sizeof(command));
  delay(100);
  _mode = g_K;
  _index = 0;
  _data = &data;
  uint16_t timeout = SINGLE_RESPONSE_TIME;
  uint32_t start = millis();
  do
  {
    loop();
    if (_status == STATUS_OK) 
		break;
  } while (millis() - start < timeout);
/*
	Serial.print("K: ");
	showMSG();
*/
  killArray();
  return _status == STATUS_OK;
}

bool PR3KTR::getNPK(DATA& data)
{
  uint8_t command[] = { 0x01, 0x03, 0x00, 0x1e, 0x00, 0x03, 0x65, 0xCD };
  RTU_CRC(command, 6, &command[6], &command[7]);
  _recvLen = sentDataLen(command);
  _stream->write(command, sizeof(command));
  delay(100);
  _mode = g_NPK;
  _index = 0;
  _data = &data;
  uint16_t timeout = SINGLE_RESPONSE_TIME;
  uint32_t start = millis();
  do
  {
    loop();
    if (_status == STATUS_OK) 
		break;
  } while (millis() - start < timeout);
 /* 
	Serial.print("NPK: ");
	showMSG();
*/	  
  killArray();
  return _status == STATUS_OK;
}

void PR3KTR::loop() {
  _status = STATUS_WAITING;
  
  if (_stream->available()) {
    uint8_t ch = _stream->read();
	_payload[_index] = ch;
    switch (_index) {
    case 0:
      if (ch != a_ID) {
        return;
      }
      break;
    case 1:
      if (ch != 0x03) {
        _index = 0;
        return;
      }
      break;
    case 2:
      _frameLen = ch;
	  if(_frameLen != _recvLen){
        _index = 0;
        return;
      }
      break;
    default: {
		if(_index == _recvLen + 4 ) {
		  uint8_t _LSB = 0;
		  uint8_t _MSB = 0;
		  RTU_CRC(_payload, _recvLen + 3, &_LSB, &_MSB);
		  _checksum           = makeWord(_payload[_recvLen+3], _payload[_recvLen+4]);
		  _calculatedChecksum = makeWord(_LSB, _MSB);
		  if (_calculatedChecksum == _checksum) {
			_status = STATUS_OK;
			switch (_mode){
			case g_N:
			  _data->NITROGEN   = makeWord(_result[0], _result[1]);
			  break;
			case g_P:
			  _data->POTASSIUM  = makeWord(_result[0], _result[1]);
			  break;
			case g_K:
			  _data->PHOSPHORUS = makeWord(_result[0], _result[1]);
			  break;
			case g_NPK:
			  _data->NITROGEN   = makeWord(_result[0], _result[1]);
			  _data->POTASSIUM  = makeWord(_result[2], _result[3]);
			  _data->PHOSPHORUS = makeWord(_result[4], _result[5]);
			  break;
			}
		  }
		} else if(_index != _frameLen + 3) {
			uint8_t payloadIndex = _index - 3;
			if (payloadIndex < sizeof(_result))
			  _result[payloadIndex] = ch;
		}
	}
	}	  
  }
    _index++;
}

uint8_t PR3KTR::recvPacketLen(byte buf[]) {
	return buf[2];
}

uint8_t PR3KTR::sentDataLen(byte buf[]) {
	return buf[5] * 2;
}

void PR3KTR::killArray() {
	memset(_payload, 0, sizeof(_payload));
	memset(_result, 0, sizeof(_result));
}

void PR3KTR::showMSG() {
  for(uint8_t i = 0; i < sizeof(_payload); i++) {
	  Serial.print(_payload[i], HEX);
	  if(i < sizeof(_payload)-1)
		Serial.print(", ");
  }
  Serial.print(" ANS: ");
  for(uint8_t i = 0; i < sizeof(_result); i++) {
	  Serial.print(_result[i], HEX);
	  if(i < sizeof(_payload)-1)
		Serial.print(", ");
  }	  
  Serial.println();
}

void PR3KTR::RTU_CRC(byte buf[], int len, uint8_t *LB, uint8_t *HB) {
/*
	len       = length of Addr+Func+[Ret.Number]+COMMAND - 2bytes (CRC-CHECK)
	*LB / *HB = Position in array of command's sent or receieved
*/
  uint16_t crc = 0xFFFF;
  for (int pos = 0; pos < len; pos++) {
    crc ^= (uint16_t)buf[pos];          // XOR byte into least sig. byte of crc
    for (int i = 8; i != 0; i--) {      // Loop over each bit
      if ((crc & 0x0001) != 0) {        // If the LSB is set
        crc >>= 1;                      // Shift right and XOR 0xA001
        crc ^= 0xA001;
      } else                            // Else LSB is not set
        crc >>= 1;                      // Just shift right
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  *HB = (crc & 0xFF00) >> 8;
  *LB =  crc & 0x00FF; 
/*
    Serial.printf("LB:%02X HB:%02X\n", *LB, *HB);
*/
}