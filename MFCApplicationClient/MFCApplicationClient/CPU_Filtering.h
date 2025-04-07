#pragma once


class CPU_Filtering
{
public:
	static bool medianFilterCPU(BYTE* inputPixels, BYTE* outputPixels, int imageWidth, int imageHeight, int imageChannels, int kernelSize, unsigned long long* duration);

	static bool gaussianBlurFilterCPU(BYTE* inputPixels, BYTE* outputPixels, int imageWidth, int imageHeight, int imageChannels, unsigned long long* duration);
};

