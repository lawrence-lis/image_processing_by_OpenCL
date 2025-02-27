#include <stdio.h>
#include <iostream>
#include "WorkingWithImages.h"

char* CreateBufferFromImage(const char* src_file_name, unsigned int& width, unsigned int& height)
{
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(src_file_name, 0);
	FIBITMAP* image = FreeImage_Load(format, src_file_name);
	image = FreeImage_ConvertTo32Bits(image);
	width = FreeImage_GetWidth(image);
	height = FreeImage_GetHeight(image);
	char* res = new char[width * height * 4];
	memcpy(res, FreeImage_GetBits(image), width * height * 4);
	FreeImage_Unload(image);
	return res;
}

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

void addSaltAndPepperNoiseRGB(FIBITMAP* image, double saltProbability, double pepperProbability)
{
	if (image == NULL)
		return;
	int width = FreeImage_GetWidth(image);
	int height = FreeImage_GetHeight(image);
	int bpp = FreeImage_GetBPP(image);
	if (bpp != 24 && bpp != 32) {
		printf("Error: Image must be 24 or 32 bits per pixel (RGB or RGBA).\n");
		return;
	}
	BYTE* bits = FreeImage_GetBits(image);
	if (bits == NULL) 
		return;
	srand(time(NULL));
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			BYTE* cur_pixel = &bits[(y * width + x) * (bpp / 8)];

			double randVal = (double)rand() / RAND_MAX;

			if (randVal < saltProbability) {
				// Salt noise (белый пиксель)
				cur_pixel[0] = 255; // Blue
				cur_pixel[1] = 255; // Green
				cur_pixel[2] = 255; // Red
			}
			else if (randVal > (1.0 - pepperProbability)) {
				// Pepper noise (черный пиксель)
				cur_pixel[0] = 0;
				cur_pixel[1] = 0;
				cur_pixel[2] = 0;
			}
		}
	}
}

void AddPulseNoiseToImage(const char* src_file_name, double salt, double pepper)
{
	FreeImage_Initialise();
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(src_file_name, 0);
	FIBITMAP* image = FreeImage_Load(fif, src_file_name, 0);
	addSaltAndPepperNoiseRGB(image, salt, pepper);
	std::string finally_file_name = src_file_name;
	finally_file_name.resize(finally_file_name.size() - 4);
	finally_file_name += "-noisy.png";

	FREE_IMAGE_FORMAT outputFif = FIF_PNG;  // ¬ыходной формат PNG
	if (!FreeImage_Save(outputFif, image, finally_file_name.c_str(), 0)) {
		std::cerr << "Error: Could not save image to " << finally_file_name.c_str() << std::endl;
	}
	else {
		std::cout << "Image saved to " << finally_file_name.c_str() << std::endl;
	}
	FreeImage_Unload(image);
	FreeImage_DeInitialise();
}