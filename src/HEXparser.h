#ifndef HEXparser_h
#define HEXparser_h

#include "Arduino.h"

class HEXparser
{
	public:
		HEXparser();
		void  ParseRecord(byte* data); // Import data from spiffs.open() in web server.
		byte* FetchRaw();             // will be called from main server code
		byte* FetchAddress();		  // for flashing
		bool  CheckReady();            // if flash-ready

	private:
		int  recType = 0;              // Here only 0/1 is needed-data present/end of file.
		int  length = 0;  
		int  index = 0;                // to keep track of the no. of bytes in each chunk
		byte RawPage[128];            // sending 128 bytes of 'chunk' each time
		byte address[2];              // storage array
		bool firstTime = true;        // data address is to be updated only after the first chunk transfer
		bool ready = false;           // init- not ready

		/* Intel HEX file format info */
		int  RecordLength(byte* record);
		void RecordAddress(byte* record);
		int  RecordType(byte* record);
        //                             //
		void extractData(byte* record, int len);
		void FileEnd();
};

#endif


