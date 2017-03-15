#pragma once
#include <stdint.h>
#include <iostream>
#include <vector>
#include <Windows.h>




extern "C"
{
#include "setupapi.h"
#include "hidsdi.h"				// Must link in hid.lib  
}


#include "membuffer.h"


#define TOKCHAR_CR		0xD
#define TOKCHAR_LF		0xA


#define HID_USAGE_PAGE_KEYBOARD       ((USAGE) 0x07)
#define HID_USAGE_GENERIC_MOUSE        ((USAGE) 0x02)

typedef struct tag_USBDEVICEINFO
{
	int wVID;
	int wPID;

}USBDEVICEINFO, *pUSBDEVICEINFO;
typedef std::vector<USBDEVICEINFO> USBDEVICEINFOVEC;


class CUsb
{
public:	
	explicit CUsb(void);
	~CUsb(void);

	bool FindUsbDevice(USBDEVICEINFOVEC &vec);
	bool OpenUsb();
	bool OpenUsb(int wVID = 1155, int wPID = 22352);
	bool ReadFromUsb(membuffer *prxBuf);
	bool WriteToUsb(const char *pData, unsigned int nSize);
	bool CloseUsb();

protected:
	void GetDeviceCapabilities();
	bool WaitSignalTimeOut(int nTimeOut, OVERLAPPED os);


private:
	HANDLE m_hUsb;
	std::string m_szDevicePathName;
	HIDP_CAPS m_Capabilities;

};

