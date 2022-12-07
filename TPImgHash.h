#pragma once
#include "CImg/CImg.h"
#include "ximage.h"
using namespace cimg_library;

#if defined(__cplusplus)
extern "C" {
#endif

	__declspec(dllexport) BOOL __stdcall TPImageHash(BYTE* pBuf, UINT uCount, ULONG64& uFeature);

#if defined(__cplusplus)
}
#endif