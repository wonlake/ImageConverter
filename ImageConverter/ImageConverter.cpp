// ImageConverter.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>

#include <windows.h>

#define FREEIMAGE_LIB
#include <freeimage.h>
#include <arg_parser.h>

#ifdef _DEBUG
#pragma comment( lib, "libfreeimage_d.lib" )
#else
#pragma comment( lib, "libfreeimage.lib" )
#endif

#pragma comment( lib, "libarg_parser.lib" )

void Image2Png(const std::string& strSrcFileName,
	const std::string& strDstFileName, int bits)
{
	FreeImage_Initialise();

	FIBITMAP* pFiBitmap = NULL;

	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(strSrcFileName.c_str());
	if (fif == FIF_UNKNOWN)
	{
		FreeImage_DeInitialise();
		return;
	}

	if (fif == FIF_JPEG)
		pFiBitmap = FreeImage_Load(fif, strSrcFileName.c_str(), JPEG_CMYK);
	else if (fif == FIF_GIF)
		pFiBitmap = FreeImage_Load(fif, strSrcFileName.c_str(), GIF_PLAYBACK);
	else if (fif == FIF_ICO)
		pFiBitmap = FreeImage_Load(fif, strSrcFileName.c_str(), ICO_MAKEALPHA);
	else
		pFiBitmap = FreeImage_Load(fif, strSrcFileName.c_str(), 0);

	if (bits == 24)
	{
		if (FreeImage_GetBPP(pFiBitmap) > 24)
			pFiBitmap = FreeImage_ConvertTo24Bits(pFiBitmap);
	}
	else if (bits == 32)
	{
		if (FreeImage_GetBPP(pFiBitmap) < 32)
		{
			pFiBitmap = FreeImage_ConvertTo32Bits(pFiBitmap);
		}

		BYTE* pData = FreeImage_GetBits(pFiBitmap);
		if (pData[3] == 255)
		{
			pData[3] = 254;
		}
	}

	FreeImage_Save(FIF_PNG, pFiBitmap, strDstFileName.c_str(), 0);

	FreeImage_Unload(pFiBitmap);
	FreeImage_DeInitialise();
}

void Image2Wdp(const std::string& strSrcFileName,
	const std::string& strDstFileName, int bits)
{

	FreeImage_Initialise();

	FIBITMAP* pFiBitmap = NULL;

	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(strSrcFileName.c_str());
	if (fif == FIF_UNKNOWN)
	{
		FreeImage_DeInitialise();
		return;
	}

	if (fif == FIF_JPEG)
		pFiBitmap = FreeImage_Load(fif, strSrcFileName.c_str(), JPEG_CMYK);
	else if (fif == FIF_GIF)
		pFiBitmap = FreeImage_Load(fif, strSrcFileName.c_str(), GIF_PLAYBACK);
	else if (fif == FIF_ICO)
		pFiBitmap = FreeImage_Load(fif, strSrcFileName.c_str(), ICO_MAKEALPHA);
	else
		pFiBitmap = FreeImage_Load(fif, strSrcFileName.c_str(), 0);

	if (bits == 24)
	{
		if (FreeImage_GetBPP(pFiBitmap) > 24)
			pFiBitmap = FreeImage_ConvertTo24Bits(pFiBitmap);
	}
	else if (bits == 32)
	{
		if (FreeImage_GetBPP(pFiBitmap) < 32)
		{
			pFiBitmap = FreeImage_ConvertTo32Bits(pFiBitmap);
		}

		BYTE* pData = FreeImage_GetBits(pFiBitmap);
		if (pData[3] == 255)
		{
			pData[3] = 254;
		}
	}
	FreeImage_Save(FIF_JXR, pFiBitmap, strDstFileName.c_str());

	FreeImage_Unload(pFiBitmap);
	FreeImage_DeInitialise();
}

void GifAnim2Png(const std::string& strSrcFileName,
	const std::string& pDstFileFolder)
{
	FreeImage_Initialise();
	FIBITMAP* dib = NULL;
	FIMULTIBITMAP* mul = NULL;
	DWORD             index = 0;
	BOOL              bMultiBitmap = FALSE;

	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(strSrcFileName.c_str(), 0);
	if (fif != FIF_GIF)
		return;

	mul = FreeImage_OpenMultiBitmap(fif, strSrcFileName.c_str(), FALSE, TRUE, TRUE, GIF_PLAYBACK);
	int iPageCount = FreeImage_GetPageCount(mul);

	std::stringstream ss;
	ss << std::string(pDstFileFolder) << "/frametime.txt";
	std::ofstream file(ss.str());

	for (int i = 0; i < iPageCount; ++i)
	{
		dib = FreeImage_LockPage(mul, i);
		FITAG* pTag = NULL;
		FreeImage_GetMetadata(FIMD_ANIMATION, dib, "FrameTime", &pTag);
		if (pTag)
		{
			file << *(long*)FreeImage_GetTagValue(pTag) << std::endl;
		}
		int bpp = FreeImage_GetBPP(dib);

		if (bpp < 32)
		{
			FIBITMAP* p = FreeImage_ConvertTo32Bits(dib);
			FreeImage_Unload(dib);
			dib = p;
		}

		std::stringstream is;
		is << pDstFileFolder << "/" << i + 1 << ".png";
		FreeImage_Save(FIF_PNG, dib, is.str().c_str(), 0);
		FreeImage_UnlockPage(mul, dib, FALSE);
	}
	FreeImage_CloseMultiBitmap(mul, 0);
	FreeImage_DeInitialise();
}

void show_help()
{
	std::cout << "ImageProcessor.exe [options] " << std::endl <<
		"--option arg : split, convert" << std::endl <<
		"--input arg: source file name" << std::endl <<
		"--output arg: output file name" << std::endl <<
		"--type arg: png, wdp" << std::endl <<
		"--bit arg: 32, 24" << std::endl;
}

int main(int argc, char** argv)
{
	Arg_parser::Option options[] =
	{
		{ 1, "type",   Arg_parser::yes },
		{ 2, "bit",	 Arg_parser::yes },
		{ 3, "input",	 Arg_parser::yes },
		{ 4, "output", Arg_parser::yes },
		{ 5, "option", Arg_parser::yes },
		{ 255, "help",	 Arg_parser::no },
		{ 0,  0,       Arg_parser::no },
	};

	Arg_parser parser(argc, argv, options);
	if (parser.error().size())
	{
		std::cout << parser.error() << std::endl;
		return -1;
	}

	if (parser.arguments() < 2)
	{
		show_help();
		return -1;
	}

	std::string strOp = "";
	std::string strInput = "";
	std::string strOutput = "";
	std::string strBits = "32";
	std::string strType = "png";

	for (int argind = 0; argind < parser.arguments(); ++argind)
	{
		const int code = parser.code(argind);

		if (!code)
			break;				// no more options

		switch (code)
		{
		case 1:
			strType = parser.argument(argind);
			break;				// example, do nothing
		case 2:
			strBits = parser.argument(argind);
			break;				// example, do nothing
		case 3:
			strInput = parser.argument(argind);
			break;				// example, do nothing
		case 4:
			strOutput = parser.argument(argind);
			break;
		case 5:
			strOp = parser.argument(argind);
			break;
		case 255:
			show_help();
			return 0;
		case 256:
			break;				// example, do nothing
		default:
			break;
		}
	}

	if (strOp == "" || strInput == "")
	{
		show_help();
		return -1;
	}

	if (strOp == "convert")
	{
		int bits = 32;
		if (strBits == "24")
			bits = 24;

		if (strType == "png")
			Image2Png(strInput.c_str(), strOutput.c_str(), bits);
		else if (strType == "wdp")
			Image2Wdp(strInput.c_str(), strOutput.c_str(), bits);
	}
	else
	{
		show_help();
	}
	return 0;
}
