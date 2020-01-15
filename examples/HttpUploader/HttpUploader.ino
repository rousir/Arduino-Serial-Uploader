#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#include "FS.h"
#include "Arduino-Serial-Uploader.h"

#define DEBUG_SERIAL  Serial
#define UPLOAD_SERIAL Serial

#define UNO_RESET_PIN 4

#define APSSID "APSSID"
#define APPSK  "APPSK"


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
	Dir dir = SPIFFS.openDir("/");
	while (dir.next()) {
		String fileName = dir.fileName();
		size_t fileSize = dir.fileSize();
		DEBUG_SERIAL.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
	}
	DEBUG_SERIAL.printf("\n");
}

int httpDownloadFileToSPIFFS(const char *url)
{
	if (!SPIFFS.begin()) {
		return 0;
	}

	String surl(url);
	String fileName = surl.substring(surl.lastIndexOf('/'));

	DEBUG_SERIAL.print("fileName:");
	DEBUG_SERIAL.println(fileName);

	if (SPIFFS.exists(fileName)) {
		SPIFFS.remove(fileName);
	}

	File hexfile = SPIFFS.open(fileName, "w");

	if (!hexfile) {
		return 0;
	}

	HTTPClient http;
	WiFiClient client;

	http.begin(client, url);

	int httpCode = http.GET();

	if (httpCode != HTTP_CODE_OK) {
		return 0;
	}

	int len = http.getSize();
	uint8_t buff[128];

	WiFiClient * stream = http.getStreamPtr();

	for(int i=0; i<len; i++){
	    hexfile.write(stream->read());
	}

	delay(500);
	hexfile.close();

	DEBUG_SERIAL.print("[HTTP] connection closed or file end.\n");
	http.end();
	return 1;
}

void setup()
{
	DEBUG_SERIAL.begin(115200);
	UPLOAD_SERIAL.begin(115200);

	if (SPIFFS.begin()) {
		DEBUG_SERIAL.println("SPIFFS Card initialized.");
		printFileList();
	} else {
		DEBUG_SERIAL.println("Failed to mount file system");
	}

	WiFi.mode(WIFI_STA);

	WiFi.disconnect();
	WiFi.begin(APSSID, APPSK);

	while (WiFi.status() != WL_CONNECTED) {
		DEBUG_SERIAL.print(".");
		delay(500);
	}

	httpDownloadFileToSPIFFS("http://server/Blink2.hex");
	DEBUG_SERIAL.println("Update UNO sketch...");
	printFileList();

	delay(500);

	SerialUploader serialuploader(UNO_RESET_PIN, &UPLOAD_SERIAL);
	serialuploader.uploaderSPIFFSFile("/Blink2.hex");
}

void loop()
{

}