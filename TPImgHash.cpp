#include "TPImgHash.h"
#include <bitset>

CImg<float>* Dct(const int N) {
	CImg<float>* ptr_matrix = new CImg<float>(N, N, 1, 1, 1 / sqrt((float)N));
	float c1 = (float)sqrt(2.0 / N);
	for (int x = 0; x < N; x++) {
		for (int y = 1; y < N; y++) {
			*ptr_matrix->data(x, y) =
				c1 * (float)cos((PI / 2 / N) * y * (2 * x + 1));
		}
	}
	return ptr_matrix;
}

int CaclImageHash(CxMemFile& image, ULONG64& uFeature) {
	CImg<uint8_t> src;
	if (-1 == src.load_png(&image)) {
		return -1;
	}

	CImg<float> meanfilter(7, 7, 1, 1, 1);
	CImg<float> img;
	if (src.spectrum() == 3) {
		img = src.RGBtoYCbCr().channel(0).get_convolve(meanfilter);
	}
	else if (src.spectrum() == 4) {
		int width = img.width();
		int height = img.height();
		int depth = img.depth();
		img =
			src.crop(0, 0, 0, 0, width - 1, height - 1, depth - 1, 2).RGBtoYCbCr().channel(0).get_convolve(meanfilter);
	}
	else {
		img = src.channel(0).get_convolve(meanfilter);
	}

	img.resize(32, 32);

	const int nSize = 32;
	CImg<float>* C = Dct(nSize);

	CImg<float> Ctransp = C->get_transpose();
	CImg<float> dctImage = (*C) * img * Ctransp;
	CImg<float> subsec = dctImage.crop(1, 1, 8, 8).unroll('x');
	float median = subsec.median();

	int k = 0;
	std::bitset<64> pHash;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			pHash[k] = subsec(k) > median ? 1 : 0;
			++k;
		}
	}

	uFeature = pHash.to_ullong();

	delete C;

	return 0;
}

int TryCaclImageHash(CxMemFile& memFile, ULONG64& uFeature) {
	int nRet = -1;
	cimg::exception_mode(0);
	__try {
		nRet = CaclImageHash(memFile, uFeature);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		nRet = -1;
	}

	return nRet;
}

__declspec(dllexport) BOOL __stdcall TPImageHash(BYTE* pBuf, UINT uCount, ULONG64& uFeature) {
	if (!pBuf || uCount == 0) {
		return FALSE;
	}

	CxMemFile memFile(pBuf, uCount);
	memFile.Seek(0, SEEK_SET);

	int nRet = TryCaclImageHash(memFile, uFeature);

	return (nRet == -1) ? FALSE : TRUE;
}