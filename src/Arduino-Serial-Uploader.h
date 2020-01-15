#ifndef Serial_Uploader_h
#define Serial_Uploader_h

#include "Arduino.h"
#include "FS.h"
#include <SD.h>

#include "HEXparser.h"
#include "UploadProtocol.h"

class SerialUploader
{
	public:
		SerialUploader(int reset);
		SerialUploader(int reset, Stream* serial);

		int uploaderFile(fs::File *file);
		int uploaderSDFile(const char *name);
		int uploaderSPIFFSFile(const char *name);

	private:
		int _reset = 0;
		Stream *_serial;
};

#endif
