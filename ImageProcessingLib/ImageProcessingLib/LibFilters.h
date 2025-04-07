#pragma once

/* Этот модуль содержит набор функций, которые направлены на саму обработку изображений. 
Перечень назначений функций:
	- медианная фильтрация;
	*/

#define PROCESSINGLIBRARY_EXPORTS

#ifdef PROCESSINGLIBRARY_EXPORTS
#define PROCESSINGLIBRARY_API __declspec(dllexport)
#else
#define PROCESSINGLIBRARY_API __declspec(dllimport)
#endif

#include "CL/cl.h"
#include "cl_initialize.h"
#include "cl_runtime.h"

extern "C" PROCESSINGLIBRARY_API bool medianFilter(BYTE * inputPixels, BYTE * outputPixels, int ImageWidth, int ImageHeight, int ImageChannels, int kernelSize, cl_command_queue commandQueue, cl_kernel kernel, cl_context context, cl_sampler sampler, cl_ulong * dur);

extern "C" PROCESSINGLIBRARY_API bool gaussianBlurFilter(BYTE * inputPixels, BYTE * outputPixels, int ImageWidth, int ImageHeight, int ImageChannels, cl_command_queue commandQueue, cl_kernel kernel, cl_context context, cl_sampler sampler, cl_ulong * dur);