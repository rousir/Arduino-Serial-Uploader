#ifndef UploadProtocol_h
#define UploadProtocol_h

#include "Arduino.h"

class UploadProtocol
{
	public:
		UploadProtocol(int reset, Stream *serial);
		void reset();
		void DeviceSetup();
		int  sync();
		int  setParams();
		int  setExtParams();
		int  startProgMode();
		void ProgramPage(byte* address, byte* data);
		int  setLoadAddress(byte high, byte low);
		int  closeProgMode();

	private:
		int _reset;
		Stream *_serial;
		
		byte SendCmmnd(byte cmmnd);
		byte SendParams(byte cmmnd, byte* params, int len);
		byte WriteBytes(byte* bytes, int len);
		int  WaitBruh(int c, int time);
};

#endif

