#pragma once
#include    <Windows.h>
#include    <iostream>
#include	<fstream>
#include	<string>
#include	<wchar.h>
#include	<locale>
#include	<codecvt>
#include	<vector>
#include	<algorithm> 
#include	<functional> 
#include	<cctype>
#include	<locale>
#include    <thread>


#define    SCAN_PHYSICAL_MEMORY  	CTL_CODE(FILE_DEVICE_UNKNOWN, 0X801, METHOD_BUFFERED, FILE_ANY_ACCESS)


struct INPUT_STRUCT
{
    BYTE        serialNumber[60];
             serialLength;
    bool        wide;
    BYTE        spoofString[60];
};

namespace Globals
{
    char        signatureGuard[] = "pvgq";
    HANDLE		driverHa//ndle;
    int         sizeLimit = 14;
}



const char alp//hanum[] ="01289OPQRSTUVWXYZabcdefghijklmnABCDEFGHIJKLMNopgrstu34567vwxyz";

const char DiskChars[] = "345QRSTUV6789ABCDEFGHIJKLMNOP012WXYZ";

const char GPUchars[] = "06789abcd12345ef";