#include "pch.h"
#include "CL/cl.h"
#include "LibFilters.h"
#include <stdio.h>

size_t RoundUp(size_t group_size, cl_int global_size)
{
    // Округление для рабочих групп
    int r = global_size % group_size;
    if (r == 0)
        return global_size;
    else
        return global_size + group_size - r;
}

bool medianFilter(BYTE* inputPixels, BYTE* outputPixels, int ImageWidth, int ImageHeight, int ImageChannels, int kernelSize, cl_command_queue commandQueue, cl_kernel kernel, cl_context context, cl_sampler sampler)
{
    // 0. Проверка входных данных
    if (inputPixels == nullptr || outputPixels == nullptr)
    {
        printf("Error: Invalid input data (pixel pointers).\n");
        return false;
    }

    if (ImageWidth <= 0 || ImageHeight <= 0 || ImageChannels <= 0)
    {
        printf("Error: Invalid image sizes.\n");
        return false;
    }

    if (kernelSize <= 0 || kernelSize % 2 == 0)
    {
        printf("Error: Invalid core size.  Must be odd and positive.\n");
        return false;
    }

    if (commandQueue == nullptr || kernel == nullptr || context == nullptr)
    {
        printf("Error: Invalid OpenCL objects.\n");
        return false;
    }

    cl_int result;
    // 1. Определяем формат изображения OpenCL
    cl_image_format imageFormat;
    switch (ImageChannels)
    {
    case 1:
        imageFormat.image_channel_order = CL_R; // Grayscale
        imageFormat.image_channel_data_type = CL_UNORM_INT8; // 8-bit unsigned normalized integers
        break;
    case 3:
        imageFormat.image_channel_order = CL_RGB; // RGB
        imageFormat.image_channel_data_type = CL_UNORM_INT8;
        break;
    case 4:
        imageFormat.image_channel_order = CL_RGBA; // RGBA
        imageFormat.image_channel_data_type = CL_UNORM_INT8;
        break;
    default:
        printf("Error: Unsupported number of channels.\n");
        return false;
    }

    // 2. Создаем буферы памяти (объекты изображений)  OpenCL
    cl_mem inputImage = cl_runtime_create_image_for_reading(context, &imageFormat, ImageWidth, ImageHeight, inputPixels, 0);
    if (inputImage == NULL)
    {
        printf("Error: OpenCL input buffer could not be created.\n");
        return false;
    }

    cl_mem outputImage = cl_runtime_create_image_for_writing(context, &imageFormat, ImageWidth, ImageHeight, 0);
    if (outputImage == NULL)
    {
        printf("Error: OpenCL output buffer could not be created.\n");
        clReleaseMemObject(inputImage);
        return false;
    }

    // 3. Устанавливаем аргументы ядра
    if (cl_runtime_set_kernel_param(kernel, 0, sizeof(cl_mem), &inputImage) != 0) {
        printf("Error setting the kernel argument to 0.\n");
        clReleaseMemObject(outputImage);
        clReleaseMemObject(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 1, sizeof(cl_mem), &outputImage) != 0) {
        printf("Error setting the kernel argument to 1.\n");
        clReleaseMemObject(outputImage);
        clReleaseMemObject(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 2, sizeof(cl_int), &ImageWidth) != 0) {
        printf("Error setting the kernel argument to 2.\n");
        clReleaseMemObject(outputImage);
        clReleaseMemObject(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 3, sizeof(cl_int), &ImageHeight) != 0) {
        printf("Error setting the kernel argument to 3.\n");
        clReleaseMemObject(outputImage);
        clReleaseMemObject(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 4, sizeof(cl_int), &ImageChannels) != 0) {
        printf("Error setting the kernel argument to 4.\n");
        clReleaseMemObject(outputImage);
        clReleaseMemObject(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 5, sizeof(cl_sampler), &sampler) != 0) {
        printf("Error setting the kernel argument to 5.\n");
        clReleaseMemObject(outputImage);
        clReleaseMemObject(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 6, sizeof(cl_int), &kernelSize) != 0) {
        printf("Error setting the kernel argument to 6.\n");
        clReleaseMemObject(outputImage);
        clReleaseMemObject(inputImage);
        return false;
    }

    // 4. Запускаем ядро
    size_t localWorkSize[2] = { 16, 16 };
    size_t globalWorkSize[2] = { RoundUp(localWorkSize[0], ImageWidth), RoundUp(localWorkSize[1], ImageHeight) };

    if (cl_runtime_enqueue_kernel(kernel, commandQueue, 2, globalWorkSize, localWorkSize) != 0) {
        printf("OpenCL kernel startup error.\n");
        clReleaseMemObject(outputImage);
        clReleaseMemObject(inputImage);
        return false;
    }

    // 5. Читаем результаты
    size_t origin[3] = { 0, 0, 0 };
    size_t region[3] = { (size_t)ImageWidth, (size_t)ImageHeight, 1 };
    result = clEnqueueReadImage(commandQueue, outputImage, CL_TRUE, origin, region, 0, 0, outputPixels, 0, NULL, NULL);
    if (result != CL_SUCCESS) {
        printf("OpenCL output buffer could not be read.\n");
        clReleaseMemObject(outputImage);
        clReleaseMemObject(inputImage);
        return false;
    }

    // 6. Освобождаем ресурсы
    clReleaseMemObject(outputImage);
    clReleaseMemObject(inputImage);

    return true;
}