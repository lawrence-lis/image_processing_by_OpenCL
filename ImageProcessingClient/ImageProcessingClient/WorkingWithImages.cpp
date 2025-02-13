#include <stdio.h>
#include <iostream>
#include "WorkingWithImages.h"



void CreateImageFromBuffer(const char* src_file_name, const unsigned char* buffer, unsigned width, unsigned height, const char* dst_str_name)
{
	FreeImage_Initialise();
	FIBITMAP* bitmap = FreeImage_Allocate(width, height, 32);
	if (!bitmap) {
		printf("Error creating image.\n");
		FreeImage_DeInitialise();
		return;
	}
	BYTE* bits = FreeImage_GetBits(bitmap);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int srcIdx = (y * width + x) * 4;
			int destIdx = (y)*width * 4 + x * 4;

			bits[destIdx + 0] = buffer[srcIdx + 0];
			bits[destIdx + 1] = buffer[srcIdx + 1];
			bits[destIdx + 2] = buffer[srcIdx + 2];
			bits[destIdx + 3] = buffer[srcIdx + 3];
		}
	}
	// доработка итогового имени файла
	std::string finally_file_name = src_file_name;
	finally_file_name.resize(finally_file_name.size() - 4);
	finally_file_name += dst_str_name;

	FreeImage_Save(FIF_PNG, bitmap, finally_file_name.c_str(), 0);
	FreeImage_Unload(bitmap);
	FreeImage_DeInitialise();
}

void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char* message)
{
	printf("\n***** ");
	if (fif != FIF_UNKNOWN)
		printf("%s Format\n", FreeImage_GetFormatFromFIF(fif));
	printf(message);
	printf(" *****\n");
}