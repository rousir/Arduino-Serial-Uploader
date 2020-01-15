#include "Arduino.h"
#include "UploadProtocol.h"

UploadProtocol::UploadProtocol(int reset, Stream *serial)
{
	_serial = serial;
	// _serial->begin(115200);
	
	// while (!_serial) // wait for serial port to connect
	// {
	// 	;
	// }

	pinMode(reset, OUTPUT);
	_reset = reset;
}

// initiation functions to be flash-ready
void UploadProtocol::DeviceSetup()
{
	reset();
	sync();
	setParams();
	setExtParams();
	startProgMode();
}

// just toggle it
void UploadProtocol::reset()
{
	digitalWrite(_reset, LOW);
	delay(1);
	digitalWrite(_reset, HIGH);
	delay(100);
	digitalWrite(_reset, LOW);
	delay(1);
	digitalWrite(_reset, HIGH);
	delay(100);
}

// send command to sync with the chip
int UploadProtocol::sync()
{
	return SendCmmnd(0x30);
}

// set chip specific 'parameters'/configs
// Right now only atmega328p with OptiBoot is supported.
// *refer app note*
int UploadProtocol::setParams()
{
	byte params[] = { 0x86, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x03, 0xff, 0xff, 0xff, 0xff, 0x00, 0x80, 0x04, 0x00, 0x00, 0x00, 0x80, 0x00 }; //current support for optiboot/atmega328p
	return SendParams(0x42, params, sizeof(params));
}

int UploadProtocol::setExtParams()
{
	byte extparams[] = { 0x05, 0x04, 0xd7, 0xc2, 0x00};
	return SendParams(0x45, extparams, sizeof(extparams));
}
// send command to go into programmable mode
int UploadProtocol::startProgMode()
{
	return SendCmmnd(0x50);
}

// get data input from HEX parser and flash it, chunk by chunk.
void UploadProtocol::ProgramPage(byte* address, byte* data)
{
	byte init[] = { 0x64, 0x00, 0x80, 0x46 };

	setLoadAddress(address[1], address[0]);
	_serial->write(init, 4);

	for (int i = 0; i < 128 ; i++)
	{
		_serial->write(data[i]);
	}

	_serial->write(0x20);      // SYNC_CRC_EOP
	WaitBruh(2, 1000);
	_serial->read();
	_serial->read();
}

// where to write?
int UploadProtocol::setLoadAddress(byte high, byte low)
{
	byte buffer[] = {high, low};
	return SendParams(0x55, buffer, sizeof(buffer));
}

// Function to send commands
byte UploadProtocol::SendCmmnd(byte cmmnd)
{

	byte bytes[] = { cmmnd, 0x20 };
	return WriteBytes(bytes, 2);
}

// Function to send parameters
byte UploadProtocol::SendParams(byte cmmnd, byte* params, int len)
{

	byte bytes[32];
	bytes[0] = cmmnd;

	int i = 0;
	while (i < len)
	{
		bytes[i + 1] = params[i];
		i++;
	}

	bytes[i + 1] = 0x20;

	return WriteBytes(bytes, i + 2);
}

// To actually send bytes for command/parameter sending functions
byte UploadProtocol::WriteBytes(byte* bytes, int len)
{
	_serial->write(bytes, len);

	WaitBruh(2, 1000);

	byte inSync = _serial->read();
	byte checkOK = _serial->read();
	if (inSync == 0x14 && checkOK == 0x10)
	{
		return 1;
	}
	return 0;
}

// After flashing is done, exit programming mode!
int UploadProtocol::closeProgMode()
{
	return SendCmmnd(0x51);
}

int UploadProtocol::WaitBruh(int count, int timeout)
{
	int timer = 0;
	while (timer < timeout)
	{
		if (_serial->available() >= count)
		{
			return 1;
		}

		delay(1);
		timer++;
	}
}