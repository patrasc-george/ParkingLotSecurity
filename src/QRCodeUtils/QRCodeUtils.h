#pragma once

#ifdef QRCODEUTILS_EXPORTS
#define QRCODEUTILS_API __declspec(dllexport)
#else
#define QRCODEUTILS_API __declspec(dllimport)
#endif

class QRCODEUTILS_API QRCode
{
};