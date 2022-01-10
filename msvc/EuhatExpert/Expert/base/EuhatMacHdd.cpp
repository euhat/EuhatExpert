#include "stdafx.h"
#include <common/OpCommon.h>
#include "EuhatMacHdd.h"
#include <WinIoCtl.h>
#include "EuhatBase.h"
#include <dbop/DbOpIni.h>
#include <EuhatPostDefMfc.h>

// https://www.winsim.com/diskid32/diskid32.html
string GetHDSerial(int nDriveID)
{
	HANDLE hPhysicalDrive = INVALID_HANDLE_VALUE;
	ULONG ulSerialLen = 0;
	string outStr;
	
	do {
		TCHAR szDriveName[32];
		wsprintf(szDriveName, TEXT("\\\\.\\PhysicalDrive%d"), nDriveID);

		hPhysicalDrive = CreateFile(szDriveName, 0,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);
		if (hPhysicalDrive == INVALID_HANDLE_VALUE)
		{
			break;
		}
		STORAGE_PROPERTY_QUERY query = { 0 };
		DWORD cbBytesReturned = 0;
		char local_buffer[5000] = { 0 };

		query.PropertyId = StorageDeviceProperty;
		query.QueryType = PropertyStandardQuery;

		if (DeviceIoControl(hPhysicalDrive, IOCTL_STORAGE_QUERY_PROPERTY,
			&query,
			sizeof(query),
			local_buffer,
			sizeof(local_buffer),
			&cbBytesReturned, NULL))
		{
			STORAGE_DEVICE_DESCRIPTOR *descrip = (STORAGE_DEVICE_DESCRIPTOR *)local_buffer;

			outStr = local_buffer + descrip->SerialNumberOffset;
			outStr.erase(0, outStr.find_first_not_of(' '));
			outStr.erase(outStr.find_last_not_of(' ') + 1);
		}
	} while (0);

	if (hPhysicalDrive != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hPhysicalDrive);
	}

	return outStr;
}

string GetAllHDSerial(void)
{
	const int MAX_IDE_DRIVES = 16;
	string strOut;
	for (int nDriveNum = 0; nDriveNum < MAX_IDE_DRIVES; nDriveNum++)
	{
		string strDev = GetHDSerial(nDriveNum);
		if (!strDev.empty())
		{
			strOut += strDev;
			strOut += "|";
		}
	}
	return strOut;
}

#include "iphlpapi.h"
#pragma comment (lib, "Iphlpapi.lib" )

int getLocalMacs(vector<string> &out, int type)
{
	bool bRtn = false;
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS)
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
	}
	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
		pAdapter = pAdapterInfo;
		char szMac[20] = { 0 };
		char szAddress[4] = { 0 };
		while (pAdapter)
		{
			if (strcmp(pAdapter->IpAddressList.IpAddress.String, "0.0.0.0") != 0)
			{
				for (UINT i = 0; i < pAdapter->AddressLength; i++)
				{
					sprintf_s(szAddress, "%02X", pAdapter->Address[i]);
					strcat_s(szMac, szAddress);
					if (i != pAdapter->AddressLength - 1)
					{
						if (type == 1)
							strcat_s(szMac, ":");
					}
				}
			}
			out.push_back(szMac);
			szMac[0] = 0;
			pAdapter = pAdapter->Next;
		}
	}
	free(pAdapterInfo);

	return (int)out.size();
}

#define INI_SECTION_LOCAL "local"

int isMacHddChanged(EuhatBase *eu)
{
	string hddsCode = GetAllHDSerial();
	vector<string> macs;
	getLocalMacs(macs, 0);
	if (macs.size() < 1)
		macs.push_back("");

	string hddsCodeOld = eu->ini_->readStr(INI_SECTION_LOCAL, "hdd");
	string macsOld = eu->ini_->readStr(INI_SECTION_LOCAL, "mac");

	if (hddsCodeOld.empty() || macsOld.empty())
	{
		eu->ini_->write(INI_SECTION_LOCAL, "hdd", hddsCode.c_str());
		eu->ini_->write(INI_SECTION_LOCAL, "mac", macs[0].c_str());
		return 0;
	}

	if (hddsCode != hddsCodeOld || macs[0] != macsOld)
	{
		eu->ini_->write(INI_SECTION_LOCAL, "hdd", hddsCode.c_str());
		eu->ini_->write(INI_SECTION_LOCAL, "mac", macs[0].c_str());
		return 1;
	}
	return 0;
}