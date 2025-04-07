#include "pch.h"
#include "CL/cl.h"
#include "LibFilters.h"
#include <stdio.h>
#include <stdlib.h>

size_t RoundUp(size_t group_size, cl_int global_size)
{
    // Округление для рабочих групп
    int r = global_size % group_size;
    if (r == 0)
        return global_size;
    else
        return global_size + group_size - r;
}

bool medianFilter(BYTE* inputPixels, BYTE* outputPixels, int ImageWidth, int ImageHeight, int ImageChannels, int kernelSize, cl_command_queue commandQueue, cl_kernel kernel, cl_context context, cl_sampler sampler, cl_ulong * dur)
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

    if (cl_runtime_enqueue_kernel(kernel, commandQueue, 2, globalWorkSize, localWorkSize, dur) != 0) {
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

bool gaussianBlurFilter(BYTE* inputPixels, BYTE* outputPixels, int ImageWidth, int ImageHeight, int ImageChannels, cl_command_queue commandQueue, cl_kernel kernel, cl_context context, cl_sampler sampler, cl_ulong* dur)
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

    if (commandQueue == nullptr || kernel == nullptr || context == nullptr)
    {
        printf("Error: Invalid OpenCL objects.\n");
        return false;
    }

    // 1. Генерируем маску
    int mask_size = 3;
    float* gaussianMask = (float*)malloc(mask_size * mask_size * sizeof(float));
    if (gaussianMask == nullptr) {
        printf("Error: Failed to allocate memory for 3x3 Gaussian mask.\n");
        return false;
    }
    gaussianMask[0] = 1.f / 16.f; gaussianMask[1] = 2.f / 16.f; gaussianMask[2] = 1.f / 16.f;
    gaussianMask[3] = 2.f / 16.f; gaussianMask[4] = 4.f / 16.f; gaussianMask[5] = 2.f / 16.f;
    gaussianMask[6] = 1.f / 16.f; gaussianMask[7] = 2.f / 16.f; gaussianMask[8] = 1.f / 16.f;

    // 2. Определяем формат изображения OpenCL
    cl_int result;
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

    // 3. Создаем буферы памяти (объекты изображений)  OpenCL
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

    cl_mem kernelBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mask_size * mask_size * sizeof(float), gaussianMask, &result);
    if (result != CL_SUCCESS)
    { 
        printf("Error: OpenCL gaussianMask buffer could not be created.\n");
        free(gaussianMask);
        return false;
    }

    // 4. Устанавливаем аргументы ядра (Пока нет кода ядра)
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

    if (cl_runtime_set_kernel_param(kernel, 2, sizeof(cl_mem), &kernelBuffer) != 0) {
        printf("Error setting the kernel argument to 2.\n");
        clReleaseMemObject(outputImage);
        clReleaseMemObject(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 3, sizeof(cl_int), &ImageWidth) != 0) {
        printf("Error setting the kernel argument to 3.\n");
        clReleaseMemObject(outputImage);
        clReleaseMemObject(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 4, sizeof(cl_int), &ImageHeight) != 0) {
        printf("Error setting the kernel argument to 4.\n");
        clReleaseMemObject(outputImage);
        clReleaseMemObject(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 5, sizeof(cl_int), &ImageChannels) != 0) {
        printf("Error setting the kernel argument to 5.\n");
        clReleaseMemObject(outputImage);
        clReleaseMemObject(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 6, sizeof(cl_int), &mask_size) != 0) {
        printf("Error setting the kernel argument to 6.\n");
        clReleaseMemObject(outputImage);
        clReleaseMemObject(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 7, sizeof(cl_sampler), &sampler) != 0) {
        printf("Error setting the kernel argument to 5.\n");
        clReleaseMemObject(outputImage);
        clReleaseMemObject(inputImage);
        return false;
    }

    // 5. Запускаем ядро
    size_t localWorkSize[2] = { 16, 16 };
    size_t globalWorkSize[2] = { RoundUp(localWorkSize[0], ImageWidth), RoundUp(localWorkSize[1], ImageHeight) };

    if (cl_runtime_enqueue_kernel(kernel, commandQueue, 2, globalWorkSize, localWorkSize, dur) != 0) {
        printf("OpenCL kernel startup error.\n");
        clReleaseMemObject(outputImage);
        clReleaseMemObject(inputImage);
        return false;
    }

    // 6. Читаем результаты
    size_t origin[3] = { 0, 0, 0 };
    size_t region[3] = { (size_t)ImageWidth, (size_t)ImageHeight, 1 };
    result = clEnqueueReadImage(commandQueue, outputImage, CL_TRUE, origin, region, 0, 0, outputPixels, 0, NULL, NULL);
    if (result != CL_SUCCESS) {
        printf("OpenCL output buffer could not be read.\n");
        clReleaseMemObject(outputImage);
        clReleaseMemObject(inputImage);
        return false;
    }

    // 7. Освобождаем ресурсы
    clReleaseMemObject(outputImage);
    clReleaseMemObject(inputImage);
    clReleaseMemObject(kernelBuffer);
    free(gaussianMask);

    return true;
}