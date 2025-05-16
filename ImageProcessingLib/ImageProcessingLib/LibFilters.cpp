#include "pch.h"
#include "CL/cl.h"
#include "LibFilters.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

size_t RoundUp(size_t group_size, cl_int global_size)
{
    int r = global_size % group_size;
    if (r == 0)
        return global_size;
    else
        return global_size + group_size - r;
}

// Вспомогательная функция для расчета оптимального размера локальной рабочей группы
// на основе размеров изображения и максимального размера группы.
// 32 - для моей видеокарты.
size_t GetOptimalLocalWorkGroupSize(size_t w_h, size_t max_group_size = 32)
{
    if (w_h <= max_group_size ) {
        return w_h; // Если изображение маленькое, используем все элементы
    }

    // Выбираем размер группы, который является саксимально делимым нацело от ширины или высоты.
    size_t best_size = 1;
    for (int cur_size = 1; cur_size <= max_group_size; cur_size++) {
        if (w_h % cur_size == 0)
            best_size = cur_size;
    }

    return best_size;
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
        cl_runtime_release_mem_object(inputImage);
        return false;
    }

    // 3. Устанавливаем аргументы ядра
    if (cl_runtime_set_kernel_param(kernel, 0, sizeof(cl_mem), &inputImage) != 0) {
        printf("Error setting the kernel argument to 0.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 1, sizeof(cl_mem), &outputImage) != 0) {
        printf("Error setting the kernel argument to 1.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 2, sizeof(cl_int), &ImageWidth) != 0) {
        printf("Error setting the kernel argument to 2.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 3, sizeof(cl_int), &ImageHeight) != 0) {
        printf("Error setting the kernel argument to 3.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 4, sizeof(cl_int), &ImageChannels) != 0) {
        printf("Error setting the kernel argument to 4.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 5, sizeof(cl_sampler), &sampler) != 0) {
        printf("Error setting the kernel argument to 5.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 6, sizeof(cl_int), &kernelSize) != 0) {
        printf("Error setting the kernel argument to 6.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        return false;
    }

    // 4. Запускаем ядро
    size_t localWorkSize[2] = { GetOptimalLocalWorkGroupSize(ImageWidth), GetOptimalLocalWorkGroupSize(ImageHeight) };
    size_t globalWorkSize[2] = { RoundUp(localWorkSize[0], ImageWidth), RoundUp(localWorkSize[1], ImageHeight) };

    if (cl_runtime_enqueue_kernel(kernel, commandQueue, 2, globalWorkSize, localWorkSize, dur) != 0) {
        printf("OpenCL kernel startup error.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        return false;
    }

    // 5. Читаем результаты
    size_t origin[3] = { 0, 0, 0 };
    size_t region[3] = { (size_t)ImageWidth, (size_t)ImageHeight, 1 };
    size_t elem_size;
    clGetImageInfo(outputImage, CL_IMAGE_ELEMENT_SIZE, sizeof(elem_size), &elem_size, NULL);
    size_t host_buffer_size = ImageWidth * ImageWidth * elem_size;
    if (!cl_runtime_read_image(commandQueue, outputImage, origin, region, outputPixels, host_buffer_size)) {
        printf("OpenCL output buffer could not be read.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        return false;
    }

    // 6. Освобождаем ресурсы
    cl_runtime_release_mem_object(outputImage);
    cl_runtime_release_mem_object(inputImage);

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
        cl_runtime_release_mem_object(inputImage);
        return false;
    }

    cl_mem kernelBuffer = cl_runtime_create_buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mask_size * mask_size * sizeof(float), gaussianMask);
    if (kernelBuffer == NULL)
    { 
        printf("Error: OpenCL gaussianMask buffer could not be created.\n");
        free(gaussianMask);
        return false;
    }

    // 4. Устанавливаем аргументы ядра
    if (cl_runtime_set_kernel_param(kernel, 0, sizeof(cl_mem), &inputImage) != 0) {
        printf("Error setting the kernel argument to 0.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 1, sizeof(cl_mem), &outputImage) != 0) {
        printf("Error setting the kernel argument to 1.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        return false;
    }

    if (cl_runtime_set_kernel_param(kernel, 2, sizeof(cl_mem), &kernelBuffer) != 0) {
        printf("Error setting the kernel argument to 2.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 3, sizeof(cl_int), &ImageWidth) != 0) {
        printf("Error setting the kernel argument to 3.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 4, sizeof(cl_int), &ImageHeight) != 0) {
        printf("Error setting the kernel argument to 4.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 5, sizeof(cl_int), &ImageChannels) != 0) {
        printf("Error setting the kernel argument to 5.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 6, sizeof(cl_int), &mask_size) != 0) {
        printf("Error setting the kernel argument to 6.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        return false;
    }
    if (cl_runtime_set_kernel_param(kernel, 7, sizeof(cl_sampler), &sampler) != 0) {
        printf("Error setting the kernel argument to 5.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        return false;
    }

    // 5. Запускаем ядро
    size_t localWorkSize[2] = { GetOptimalLocalWorkGroupSize(ImageWidth), GetOptimalLocalWorkGroupSize(ImageHeight) };
    size_t globalWorkSize[2] = { RoundUp(localWorkSize[0], ImageWidth), RoundUp(localWorkSize[1], ImageHeight) };

    if (cl_runtime_enqueue_kernel(kernel, commandQueue, 2, globalWorkSize, localWorkSize, dur) != 0) {
        printf("OpenCL kernel startup error.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        return false;
    }

    // 6. Читаем результаты
    size_t origin[3] = { 0, 0, 0 };
    size_t region[3] = { (size_t)ImageWidth, (size_t)ImageHeight, 1 };
    size_t elem_size;
    clGetImageInfo(outputImage, CL_IMAGE_ELEMENT_SIZE, sizeof(elem_size), &elem_size, NULL);
    size_t host_buffer_size = ImageWidth * ImageWidth * elem_size;
    if (!cl_runtime_read_image(commandQueue, outputImage, origin, region, outputPixels, host_buffer_size)) {
        printf("OpenCL output buffer could not be read.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        cl_runtime_release_mem_object(kernelBuffer);
        free(gaussianMask);
        return false;
    }

    // 7. Освобождаем ресурсы
    cl_runtime_release_mem_object(outputImage);
    cl_runtime_release_mem_object(inputImage);
    cl_runtime_release_mem_object(kernelBuffer);
    free(gaussianMask);

    return true;
}

bool detectEdges(BYTE* inputPixels, BYTE* outputPixels, int edge_detecting_mode, int ImageWidth, int ImageHeight, int ImageChannels, cl_command_queue commandQueue, cl_kernel kernel, cl_context context, cl_sampler sampler, cl_ulong* dur)
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
        cl_runtime_release_mem_object(inputImage);
        return false;
    }

    // 3. Работа с операторами
    cl_mem Buf1 = nullptr; // Сюда будут записаны все первые (в случае Лапласа единственная) маски.
    cl_mem Buf2 = nullptr; // Сюда уже пойдут оставшиеся
    switch (edge_detecting_mode)
    {
    case 0: // Собель
    {
        int sobelGx[9] = { -1, 0, 1,
                            -2, 0, 2,
                            -1, 0, 1 };
        int sobelGy[9] = { -1,-2,-1,
                            0, 0, 0,
                            1, 2, 1 };

        Buf1 = cl_runtime_create_buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 3 * 3 * sizeof(int), sobelGx);
        Buf2 = cl_runtime_create_buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 3 * 3 * sizeof(int), sobelGy);

        if (cl_runtime_set_kernel_param(kernel, 0, sizeof(cl_mem), &inputImage) != 0) {
            printf("Error setting the kernel argument to 0.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 1, sizeof(cl_mem), &outputImage) != 0) {
            printf("Error setting the kernel argument to 1.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 2, sizeof(cl_int), &ImageWidth) != 0) {
            printf("Error setting the kernel argument to 2.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 3, sizeof(cl_int), &ImageHeight) != 0) {
            printf("Error setting the kernel argument to 3.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 4, sizeof(cl_int), &ImageChannels) != 0) {
            printf("Error setting the kernel argument to 4.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 5, sizeof(cl_sampler), &sampler) != 0) {
            printf("Error setting the kernel argument to 5.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 6, sizeof(cl_mem), &Buf1) != 0) {
            printf("Error setting the kernel argument to 6.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 7, sizeof(cl_mem), &Buf2) != 0) {
            printf("Error setting the kernel argument to 7.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }

        break;
    }
    case 1: // Лаплас
    {
        int laplacianKernel[9] = {  0, 1, 0, 
                                    1, -4, 1, 
                                    0, 1, 0 };

        Buf1 = cl_runtime_create_buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 3 * 3 * sizeof(int), laplacianKernel);

        if (cl_runtime_set_kernel_param(kernel, 0, sizeof(cl_mem), &inputImage) != 0) {
            printf("Error setting the kernel argument to 0.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 1, sizeof(cl_mem), &outputImage) != 0) {
            printf("Error setting the kernel argument to 1.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 2, sizeof(cl_int), &ImageWidth) != 0) {
            printf("Error setting the kernel argument to 2.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 3, sizeof(cl_int), &ImageHeight) != 0) {
            printf("Error setting the kernel argument to 3.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 4, sizeof(cl_int), &ImageChannels) != 0) {
            printf("Error setting the kernel argument to 4.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 5, sizeof(cl_sampler), &sampler) != 0) {
            printf("Error setting the kernel argument to 5.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 6, sizeof(cl_mem), &Buf1) != 0) {
            printf("Error setting the kernel argument to 6.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }

        break;
    }
    case 2: // Превитт
    {
        int prewittGx[9] = {-1, 0, 1,
                            -1, 0, 1,
                            -1, 0, 1 };
        int prewittGy[9] = {-1,-1,-1,
                            0, 0, 0,
                            1, 1, 1 };

        Buf1 = cl_runtime_create_buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 3 * 3 * sizeof(int), prewittGx);
        Buf2 = cl_runtime_create_buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 3 * 3 * sizeof(int), prewittGy);

        if (cl_runtime_set_kernel_param(kernel, 0, sizeof(cl_mem), &inputImage) != 0) {
            printf("Error setting the kernel argument to 0.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 1, sizeof(cl_mem), &outputImage) != 0) {
            printf("Error setting the kernel argument to 1.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 2, sizeof(cl_int), &ImageWidth) != 0) {
            printf("Error setting the kernel argument to 2.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 3, sizeof(cl_int), &ImageHeight) != 0) {
            printf("Error setting the kernel argument to 3.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 4, sizeof(cl_int), &ImageChannels) != 0) {
            printf("Error setting the kernel argument to 4.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 5, sizeof(cl_sampler), &sampler) != 0) {
            printf("Error setting the kernel argument to 5.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 6, sizeof(cl_mem), &Buf1) != 0) {
            printf("Error setting the kernel argument to 6.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 7, sizeof(cl_mem), &Buf2) != 0) {
            printf("Error setting the kernel argument to 7.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }

        break;
    }
    case 3: // Робертс
    {
        int robertsGx[4] = {1,  0,
                            0, -1 };
        int robertsGy[4] = { 0, 1,
                            -1, 0 };

        Buf1 = cl_runtime_create_buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 2 * 2 * sizeof(int), robertsGx);
        Buf2 = cl_runtime_create_buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 2 * 2 * sizeof(int), robertsGy);

        if (cl_runtime_set_kernel_param(kernel, 0, sizeof(cl_mem), &inputImage) != 0) {
            printf("Error setting the kernel argument to 0.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 1, sizeof(cl_mem), &outputImage) != 0) {
            printf("Error setting the kernel argument to 1.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 2, sizeof(cl_int), &ImageWidth) != 0) {
            printf("Error setting the kernel argument to 2.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 3, sizeof(cl_int), &ImageHeight) != 0) {
            printf("Error setting the kernel argument to 3.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 4, sizeof(cl_int), &ImageChannels) != 0) {
            printf("Error setting the kernel argument to 4.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 5, sizeof(cl_sampler), &sampler) != 0) {
            printf("Error setting the kernel argument to 5.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 6, sizeof(cl_mem), &Buf1) != 0) {
            printf("Error setting the kernel argument to 6.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }
        if (cl_runtime_set_kernel_param(kernel, 7, sizeof(cl_mem), &Buf2) != 0) {
            printf("Error setting the kernel argument to 7.\n");
            cl_runtime_release_mem_object(outputImage);
            cl_runtime_release_mem_object(inputImage);
            cl_runtime_release_mem_object(Buf1);
            cl_runtime_release_mem_object(Buf2);
            return false;
        }

        break;
    }
    default: // Ошибка
        printf("Error: Invalid Edge Detecting Mode.\n");
        return false;
    }

    // 5. Запускаем ядро
    size_t localWorkSize[2] = { GetOptimalLocalWorkGroupSize(ImageWidth, 16), GetOptimalLocalWorkGroupSize(ImageHeight, 16) };
    size_t globalWorkSize[2] = { RoundUp(localWorkSize[0], ImageWidth), RoundUp(localWorkSize[1], ImageHeight) };

    if (cl_runtime_enqueue_kernel(kernel, commandQueue, 2, globalWorkSize, localWorkSize, dur) != 0) {
        printf("OpenCL kernel startup error.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        return false;
    }

    // 6. Читаем результаты
    size_t origin[3] = { 0, 0, 0 };
    size_t region[3] = { (size_t)ImageWidth, (size_t)ImageHeight, 1 };
    size_t elem_size;
    clGetImageInfo(outputImage, CL_IMAGE_ELEMENT_SIZE, sizeof(elem_size), &elem_size, NULL);
    size_t host_buffer_size = ImageWidth * ImageWidth * elem_size;
    if (!cl_runtime_read_image(commandQueue, outputImage, origin, region, outputPixels, host_buffer_size)) {
        printf("OpenCL output buffer could not be read.\n");
        cl_runtime_release_mem_object(outputImage);
        cl_runtime_release_mem_object(inputImage);
        cl_runtime_release_mem_object(Buf1);
        if (Buf2 != nullptr) cl_runtime_release_mem_object(Buf2);
        return false;
    }

    // 7. Освобождаем ресурсы
    cl_runtime_release_mem_object(outputImage);
    cl_runtime_release_mem_object(inputImage);
    cl_runtime_release_mem_object(Buf1);
    if (Buf2 != nullptr) cl_runtime_release_mem_object(Buf2);

    return true;
}
