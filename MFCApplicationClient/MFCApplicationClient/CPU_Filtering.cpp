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