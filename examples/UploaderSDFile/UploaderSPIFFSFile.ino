#include <ESP8266WiFi.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include "Arduino-Serial-Uploader.h"

#define UNO_RESET_PIN 	4
#define SD_CS_PIN 		5

//format bytes
String formatBytes(size_t bytes) {
	if (bytes < 1024) {
		return String(bytes) + "B";
	} else if (bytes < (1024 * 1024)) {
		return String(bytes / 1024.0) + "KB";
	} else if (bytes < (1024 * 1024 * 1024)) {
		return String(bytes / 1024.0 / 1024.0) + "MB";
	} else {
		return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
	}
}

void printFileList()
{
	Dir dir = SD.openDir("/");
	while (dir.next()) {
		String fileName = dir.fileName();
		size_t fileSize = dir.fileSize();
		Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
	}
	Serial.printf("\n");
}

void setup()
{
	Serial.begin(115200);

	if (SD.begin(SD_CS_PIN)) {
		Serial.println("SPIFFS Card initialized.");
		printFileList();

		SerialUploader serialuploader(UNO_RESET_PIN, &Serial);
		serialuploader.uploaderSDFile("/Blink.hex");
		
	}else{
		Serial.println("Failed to mount file system");
	}

}

void loop()
{

}