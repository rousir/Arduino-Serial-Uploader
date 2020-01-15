#include "Arduino.h"
#include "HEXparser.h"

//initiate address
HEXparser::HEXparser()
{
	address[0] = 0x00;
	address[1] = 0x00;
}

// length = number of data bytes
int HEXparser::RecordLength(byte* record)
{
	char holder[3];

	holder[0] = record[1];
	holder[1] = record[2];
	holder[2] = '\0';

	return strtol(holder, 0, 16);
}

// the address where the corresponding record data are to be allocated in memory.
void HEXparser::RecordAddress(byte* record)
{
	char holder[3];
	
	holder[2] = '\0';

	holder[0] = record[3];
	holder[1] = record[4];
	address[0] = strtol(holder, 0, 16);

	holder[0] = record[5];
	holder[1] = record[6];
	address[1] = strtol(holder, 0, 16); 
}

// data or end of file
int HEXparser::RecordType(byte* record)
{
	char holder[3];

	holder[0] = record[7];
	holder[1] = record[8];
	holder[2] = '\0';

	return strtol(holder, 0, 16);	
}

// each record is passed
void HEXparser::extractData(byte* record, int len)
{
	int begin = 9;             // start position of data in record
	int end = (len * 2) + begin; //data is in sets of 2
	char holder[3];
	holder[2] = '\0';

	for(int i = begin; i < end; i = i + 2)
	{
		holder[0] = record[i];
		holder[1] = record[i+1];

		RawPage[index] = strtol(holder, 0, 16);
		index++;                  // keeping track of chunk size
	}
}

// check record type||chunk filled or not
void HEXparser::FileEnd()
{
	address[1] += 0x40;
	if(address[1] == 0)
	{
		address[0] += 0x1;
	}

	while(index < 128)
	{
		RawPage[index] = 0xFF;
		index++ ;
	}
}

void HEXparser::ParseRecord(byte* record)
{
	recType = RecordType(record);

	if(recType == 0)//  data present
	{
		length = RecordLength(record);
		extractData(record, length);

		if(index == 128) // chunk full
		{
			if(!firstTime)
			{
				address[1] += 0x40;   //update data address 

				if(address[1] == 0)
				{
						address[0] += 0x1;
				}
			}

			index = 0;
			firstTime = false;
			ready = true;
		}
	}

	if(recType == 1)// end of file
	{
		FileEnd();
		ready = true;
	}
}
/* Return calls to spiff/web server */
byte* HEXparser::FetchRaw()
{
	return RawPage;
}

byte* HEXparser::FetchAddress()
{
	ready = false; // ready for next loop
	return address;
}

bool HEXparser::CheckReady()  // check if flash-ready
{
	return ready;
}

