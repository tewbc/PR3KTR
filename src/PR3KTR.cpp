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
  uint8_t command[] = { 0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0x00, 0x00 };
  command[0] = a_ID;
  RTU_CRC(command, 6, &command[6], &command[7]);
  _recvLen = sentDataLen(command);
  _stream->write(command, sizeof(command));
  delay(100);
  _mode = g_N;
  _data = &data;
  uint16_t timeout = SINGLE_RESPONSE_TIME;
  uint32_t start = millis();
  do
  {
    loop();
    if (_status == STATUS_OK) 
		break;
  } while (millis() - start < timeout);
  return _status == STATUS_OK;
}

bool PR3KTR::getP(DATA& data)
{
  uint8_t command[] = { 0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0x00, 0x00 };
  command[0] = a_ID;
  RTU_CRC(command, 6, &command[6], &command[7]);
  _recvLen = sentDataLen(command);
  _stream->write(command, sizeof(command));
  delay(100);
  _mode = g_P;
  _data = &data;
    uint16_t timeout = SINGLE_RESPONSE_TIME;
  uint32_t start = millis();
  do
  {
    loop();
    if (_status == STATUS_OK) 
		break;
  } while (millis() - start < timeout);
  return _status == STATUS_OK;
}

bool PR3KTR::getK(DATA& data)
{
  uint8_t command[] = { 0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x00, 0x00 };
  command[0] = a_ID;
  RTU_CRC(command, 6, &command[6], &command[7]);
  _recvLen = sentDataLen(command);
  _stream->write(command, sizeof(command));
  delay(100);
  _mode = g_K;
  _data = &data;
  uint16_t timeout = SINGLE_RESPONSE_TIME;
  uint32_t start = millis();
  do
  {
    loop();
    if (_status == STATUS_OK) 
		break;
  } while (millis() - start < timeout);
  return _status == STATUS_OK;
}

bool PR3KTR::getNPK(DATA& data)
{
  uint8_t command[] = { 0x01,0x03, 0x00, 0x1e, 0x00, 0x03, 0x00, 0x00 };
  command[0] = a_ID;
  RTU_CRC(command, 6, &command[6], &command[7]);
  _recvLen = sentDataLen(command);
  _stream->write(command, sizeof(command));
  delay(100);
  _mode = g_NPK;
  _data = &data;
  uint16_t timeout = SINGLE_RESPONSE_TIME;
  uint32_t start = millis();
  do
  {
    loop();
    if (_status == STATUS_OK) 
		break;
  } while (millis() - start < timeout);
  return _status == STATUS_OK;
}

void PR3KTR::loop()
{
  uint8_t _startAddr = 0;
  uint8_t _packet[_recvLen+6];
  _status = STATUS_WAITING;
  if (_stream->available())
  {
    uint8_t ch = _stream->read();
	_packet[_index] = ch;
    switch (_index)
    {
    case 0:
      if (ch != a_ID)
      {
        return;
      }
      break;
    case 1:
      if (ch != 0x03)
      {
        _index = 0;
        return;
      }
      break;
    case 2:
      _frameLen = ch;
	  if(_frameLen != _recvLen)
      {
        _index = 0;
        return;
      }
      break;
    default:
	  if(_index != _frameLen + 3) {
 	    uint8_t payloadIndex = _index - 3;
	    if (payloadIndex < sizeof(_payload))
	    {
	      _payload[payloadIndex] = ch;
	    }	
	  }
    }
    _index++;
	if(_index > _recvLen + 4 )
	  exit;
  }  
  RTU_CRC(_packet, _recvLen + 3, &_packet[_recvLen+5], &_packet[_recvLen+6]);
  _checksum           = makeWord(_packet[_recvLen+3], _packet[_recvLen+4]);
  _calculatedChecksum = makeWord(_packet[_recvLen+5], _packet[_recvLen+6]);
  if (_calculatedChecksum == _checksum) {
	_status = STATUS_OK;
	switch (_mode)
	{
	case g_N:
	  _data->NITROGEN   = makeWord(_payload[0], _payload[1]);
	  break;
	case g_P:
	  _data->POTASSIUM  = makeWord(_payload[0], _payload[1]);
	  break;
	case g_K:
	  _data->PHOSPHORUS = makeWord(_payload[0], _payload[1]);
	  break;
	case g_NPK:
      _data->NITROGEN   = makeWord(_payload[0], _payload[1]);
      _data->POTASSIUM  = makeWord(_payload[2], _payload[3]);
      _data->PHOSPHORUS = makeWord(_payload[4], _payload[5]);
	  break;
	}
  }
}

uint8_t PR3KTR::recvPacketLen(byte buf[]) 
{
	return buf[2];
}

uint8_t PR3KTR::sentDataLen(byte buf[])
{
	return buf[5] * 2;
}

void PR3KTR::RTU_CRC(byte buf[], int len, uint8_t *LB, uint8_t *HB)
{
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
  #ifdef DEBUG
    Serial.printf("LB:%02X HB:%02X\n", *LB, *HB);
  #endif
}