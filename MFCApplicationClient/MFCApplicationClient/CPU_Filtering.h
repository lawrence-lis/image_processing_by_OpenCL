#pragma once


class CPU_Filtering
{
public:
	static bool medianFilterCPU(BYTE* inputPixels, BYTE* outputPixels, int imageWidth, int imageHeight, int imageChannels, int kernelSize, unsigned long long* duration);

	static bool gaussianBlurFilterCPU(BYTE* inputPixels, BYTE* outputPixels, int imageWidth, int imageHeight, int imageChannels, unsigned long long* duration);

	static bool ApplyEdgeDetectionCPU(BYTE* inputPixels, BYTE* outputPixels, int selectedEdgeFilter, int imageWidth, int imageHeight, int imageChannels, unsigned long long* duration);

private:

	// Функция применения оператора Собеля
	static void ApplySobel(BYTE* inputPixels, BYTE* outputPixels, int imageWidth, int imageHeight, int imageChannels, unsigned long long* duration);

	// Функция применения оператора Лапласа
	static void ApplyLaplacian(BYTE* inputPixels, BYTE* outputPixels, int imageWidth, int imageHeight, int imageChannels, unsigned long long* duration);

	// Функция применения оператора Превитта
	static void ApplyPrewitt(BYTE* inputPixels, BYTE* outputPixels, int imageWidth, int imageHeight, int imageChannels, unsigned long long* duration);

	// Функция применения оператора Робертса
	static void ApplyRoberts(BYTE* inputPixels, BYTE* outputPixels, int imageWidth, int imageHeight, int imageChannels, unsigned long long* duration);

	// Функция для преобразования цвета пикселя
	static BYTE ClampPixel(int value);

	static int Clamp(int value, int min, int max);
};

