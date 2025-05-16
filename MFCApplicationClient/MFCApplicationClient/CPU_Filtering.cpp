#include "pch.h"
#include "CPU_Filtering.h"

#include <vector>
#include <algorithm>
#include <chrono>

bool CPU_Filtering::medianFilterCPU(BYTE* inputPixels, BYTE* outputPixels, int imageWidth, int imageHeight, int imageChannels, int kernelSize, unsigned long long* duration)
{ 
    // 0. Проверка переданных параметров
    if (inputPixels == nullptr || outputPixels == nullptr) return false;
    if (imageWidth <= 0 || imageHeight <= 0 || imageChannels <= 0) return false;
    if (kernelSize <= 0 || kernelSize % 2 == 0) return false;

    int halfKernelSize = kernelSize / 2;

    auto start = std::chrono::high_resolution_clock::now();
    for (int y = 0; y < imageHeight; y++) {
        for (int x = 0; x < imageWidth; x++) {
            for (int c = 0; c < imageChannels; c++) {
                std::vector<BYTE> neighborhood;
                for (int ky = -halfKernelSize; ky <= halfKernelSize; ky++) {
                    for (int kx = -halfKernelSize; kx <= halfKernelSize; kx++) {
                        int sampleX = x + kx;
                        int sampleY = y + ky;

                        sampleX = max(0, min(sampleX, imageWidth - 1));
                        sampleY = max(0, min(sampleY, imageHeight - 1));

                        int index = (sampleY * imageWidth + sampleX) * imageChannels + c;
                        neighborhood.push_back(inputPixels[index]);
                    }
                }
                std::sort(neighborhood.begin(), neighborhood.end());
                BYTE median;
                if (neighborhood.size() % 2 == 0) {
                    median = (neighborhood[neighborhood.size() / 2 - 1] + neighborhood[neighborhood.size() / 2]) / 2;
                }
                else {
                    median = neighborhood[neighborhood.size() / 2];
                }

                int outputIndex = (y * imageWidth + x) * imageChannels + c;
                outputPixels[outputIndex] = median;
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    if (duration != nullptr) {
        *duration = duration_ns;
    }

    return true;
}

bool CPU_Filtering::gaussianBlurFilterCPU(BYTE* inputPixels, BYTE* outputPixels, int imageWidth, int imageHeight, int imageChannels, unsigned long long* duration)
{
    // 0. Проверка переданных параметров
    if (inputPixels == nullptr || outputPixels == nullptr) return false;
    if (imageWidth <= 0 || imageHeight <= 0 || imageChannels <= 0) return false;

    float gaussianKernel[3][3] = {
        {1.f / 16.f, 2.f / 16.f, 1.f / 16.f},
        {2.f / 16.f, 4.f / 16.f, 2.f / 16.f,},
        {1.f / 16.f, 2.f / 16.f, 1.f / 16.f}
    };

    int halfKernelSize = 1;

    auto start = std::chrono::high_resolution_clock::now();

    for (int y = 0; y < imageHeight; y++) {
        for (int x = 0; x < imageWidth; x++) {
            for (int c = 0; c < imageChannels; c++) {
                float sum = 0.0f;
                for (int ky = -halfKernelSize; ky <= halfKernelSize; ky++) {
                    for (int kx = -halfKernelSize; kx <= halfKernelSize; kx++) {
                        int sampleX = x + kx;
                        int sampleY = y + ky;

                        sampleX = max(0, min(sampleX, imageWidth - 1));
                        sampleY = max(0, min(sampleY, imageHeight - 1));

                        int index = (sampleY * imageWidth + sampleX) * imageChannels + c;
                        float pixelValue = (float)inputPixels[index];

                        sum += pixelValue * gaussianKernel[ky + halfKernelSize][kx + halfKernelSize];
                    }
                }
                int outputIndex = (y * imageWidth + x) * imageChannels + c;
                outputPixels[outputIndex] = (BYTE)max(0.0f, min(sum, 255.0f));
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    if (duration != nullptr) {
        *duration = duration_ns;
    }

	return true;
}

bool CPU_Filtering::ApplyEdgeDetectionCPU(BYTE* inputPixels, BYTE* outputPixels, int selectedEdgeFilter, int imageWidth, int imageHeight, int imageChannels, unsigned long long* duration)
{
    if (inputPixels == nullptr || outputPixels == nullptr) return false;
    if (imageWidth <= 0 || imageHeight <= 0 || imageChannels <= 0) return false;

    switch (selectedEdgeFilter) {
    case 0: // Оператор Собеля
        ApplySobel(inputPixels, outputPixels, imageWidth, imageHeight, imageChannels, duration);
        break;
    case 1:
        ApplyLaplacian(inputPixels, outputPixels, imageWidth, imageHeight, imageChannels, duration);
        break;
    case 2:
        ApplyPrewitt(inputPixels, outputPixels, imageWidth, imageHeight, imageChannels, duration);
        break;
    case 3:
        ApplyRoberts(inputPixels, outputPixels, imageWidth, imageHeight, imageChannels, duration);
        break;
    }
    return true;
}



void CPU_Filtering::ApplySobel(BYTE* inputPixels, BYTE* outputPixels, int imageWidth, int imageHeight, int imageChannels, unsigned long long* duration)
{
    int sobelGx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    int sobelGy[3][3] = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };

    auto start = std::chrono::high_resolution_clock::now();

    for (int y = 0; y < imageHeight; y++) {
        for (int x = 0; x < imageWidth; x++) {
            for (int c = 0; c < 3; c++) {
                int Gx = 0;
                int Gy = 0;

                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        int clampedY = Clamp(y + i, 0, imageHeight - 1);
                        int clampedX = Clamp(x + j, 0, imageWidth - 1);

                        Gx += sobelGx[i + 1][j + 1] * inputPixels[(clampedY * imageWidth + clampedX) * imageChannels + c];
                        Gy += sobelGy[i + 1][j + 1] * inputPixels[(clampedY * imageWidth + clampedX) * imageChannels + c];
                    }
                }

                int magnitude = static_cast<int>(sqrt(Gx * Gx + Gy * Gy));

                outputPixels[(y * imageWidth + x) * imageChannels + c] = ClampPixel(magnitude);
            }
            outputPixels[(y * imageWidth + x) * imageChannels + 3] = inputPixels[(y * imageWidth + x) * imageChannels + 3];
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    if (duration != nullptr) {
        *duration = duration_ns;
    }
}

void CPU_Filtering::ApplyLaplacian(BYTE* inputPixels, BYTE* outputPixels, int imageWidth, int imageHeight, int imageChannels, unsigned long long* duration)
{
    int laplacianKernel[3][3] = {
        {0,  1, 0},
        {1, -4, 1},
        {0,  1, 0}
    };

    auto start = std::chrono::high_resolution_clock::now();

    for (int y = 0; y < imageHeight; y++) {
        for (int x = 0; x < imageWidth; x++) {
            for (int c = 0; c < 3; c++) {
                int laplacian = 0;

                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        int clampedY = Clamp(y + i, 0, imageHeight - 1);
                        int clampedX = Clamp(x + j, 0, imageWidth - 1);

                        laplacian += laplacianKernel[i + 1][j + 1] * inputPixels[(clampedY * imageWidth + clampedX) * imageChannels + c];
                    }
                }

                outputPixels[(y * imageWidth + x) * imageChannels + c] = ClampPixel(laplacian);
            }
            outputPixels[(y * imageWidth + x) * imageChannels + 3] = inputPixels[(y * imageWidth + x) * imageChannels + 3];
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    if (duration != nullptr) {
        *duration = duration_ns;
    }
}

void CPU_Filtering::ApplyPrewitt(BYTE* inputPixels, BYTE* outputPixels, int imageWidth, int imageHeight, int imageChannels, unsigned long long* duration)
{
    int prewittGx[3][3] = {
        {-1, 0, 1},
        {-1, 0, 1},
        {-1, 0, 1}
    };
    
    int prewittGy[3][3] = {
        {-1, -1, -1},
        {0,  0,  0},
        {1,  1,  1}
    };

    auto start = std::chrono::high_resolution_clock::now();

    for (int y = 0; y < imageHeight; y++) {
        for (int x = 0; x < imageWidth; x++) {
            for (int c = 0; c < 3; c++) {
                int Gx = 0;
                int Gy = 0;

                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        int clampedY = Clamp(y + i, 0, imageHeight - 1);
                        int clampedX = Clamp(x + j, 0, imageWidth - 1);

                        Gx += prewittGx[i + 1][j + 1] * inputPixels[(clampedY * imageWidth + clampedX) * imageChannels + c];
                        Gy += prewittGy[i + 1][j + 1] * inputPixels[(clampedY * imageWidth + clampedX) * imageChannels + c];
                    }
                }

                int magnitude = static_cast<int>(sqrt(Gx * Gx + Gy * Gy));

                outputPixels[(y * imageWidth + x) * imageChannels + c] = ClampPixel(magnitude);
            }
            outputPixels[(y * imageWidth + x) * imageChannels + 3] = inputPixels[(y * imageWidth + x) * imageChannels + 3];
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    if (duration != nullptr) {
        *duration = duration_ns;
    }
}

void CPU_Filtering::ApplyRoberts(BYTE* inputPixels, BYTE* outputPixels, int imageWidth, int imageHeight, int imageChannels, unsigned long long* duration)
{
    int robertsGx[2][2] = {
        {1, 0},
        {0, -1}
    };

    int robertsGy[2][2] = {
        {0, 1},
        {-1, 0}
    };

    auto start = std::chrono::high_resolution_clock::now();

    for (int y = 0; y < imageHeight; y++) {
        for (int x = 0; x < imageWidth; x++) {
            for (int c = 0; c < 3; c++) {
                int Gx = 0;
                int Gy = 0;

                for (int i = 0; i < 2; i++) {
                    for (int j = 0; j < 2; j++) {
                        int clampedY = Clamp(y + i, 0, imageHeight - 1);
                        int clampedX = Clamp(x + j, 0, imageWidth - 1);

                        Gx += robertsGx[i][j] * inputPixels[(clampedY * imageWidth + clampedX) * imageChannels + c];
                        Gy += robertsGy[i][j] * inputPixels[(clampedY * imageWidth + clampedX) * imageChannels + c];
                    }
                }

                int magnitude = static_cast<int>(sqrt(Gx * Gx + Gy * Gy));

                outputPixels[(y * imageWidth + x) * imageChannels + c] = ClampPixel(magnitude);
            }
            outputPixels[(y * imageWidth + x) * imageChannels + 3] = inputPixels[(y * imageWidth + x) * imageChannels + 3];
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    if (duration != nullptr) {
        *duration = duration_ns;
    }
}

BYTE CPU_Filtering::ClampPixel(int value)
{
    if (value < 0) return 0;
    if (value > 255) return 255;
    return static_cast<BYTE>(value);
}

int CPU_Filtering::Clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}