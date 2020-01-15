#include "Arduino.h"
#include "FS.h"
#include <SD.h>

#include "HEXparser.h"
#include "UploadProtocol.h"
#include "Arduino-Serial-Uploader.h"

SerialUploader::SerialUploader(int reset)
{
  _reset = reset;
  _serial = &Serial;
  Serial.begin(115200);
}

SerialUploader::SerialUploader(int reset, Stream *serial)
{
  _reset = reset;
  _serial = serial;
}

int SerialUploader::uploaderFile(fs::File *file)
{
  UploadProtocol avrdude(_reset, _serial);

  if (file)
  {
    avrdude.DeviceSetup();
    HEXparser parse = HEXparser();

    while (file->available())
    {
      byte buffer[50];
      String data = file->readStringUntil('\n');
      data.getBytes(buffer, data.length());
      parse.ParseRecord(buffer);

      if (parse.CheckReady())
      {
        byte* page = parse.FetchRaw();
        byte* address = parse.FetchAddress();
        avrdude.ProgramPage(address, page);
      }
    }
  }

  avrdude.closeProgMode();
  file->close();
  return 1;
}

int SerialUploader::uploaderSDFile(const char *name)
{
  File file = SD.open(name, FILE_READ);
  uploaderFile(&file);
  return 1;
}

int SerialUploader::uploaderSPIFFSFile(const char *name)
{
  File file = SPIFFS.open(name, "r");
  uploaderFile(&file);
  return 1;
}