#include <stdio.h>
#include <fstream>
#include <sstream>
// Если начнёт выёбываться на определение функций, то сотри и заново вставь следующую строку
#include "ProcessingLib.h"
#include "CL/cl.hpp"
#include "WorkingWithImages.h"

size_t RoundUp(size_t group_size, cl_int global_size)
{
	// Округление для рабочих групп
	int r = global_size % group_size;
	if (r == 0)
		return global_size;
	else
		return global_size + group_size - r;
}

int main(void)
{
	// Работа с изображением
	const char* fileName = "cat.jpg";
	unsigned int width, height;
	AddPulseNoiseToImage("cat.jpg", 0.01, 0.01);
	char* buffer = CreateBufferFromImage("cat-noisy.png", width, height);

	cl_image_format clImageFormat;
	clImageFormat.image_channel_order = CL_RGBA;
	clImageFormat.image_channel_data_type = CL_UNORM_INT8;
	/*/////////////////////////////////////////////////////////////
	/////////////////////// Работа с OpenCL ///////////////////////
	/////////////////////////////////////////////////////////////*/
	cl_int err;
	cl_uint num_platforms, num_devices;
	cl_platform_id platform;
	cl_device_id device;
	// Подсчёт платформ
	num_platforms = cl_init_get_num_platforms();
	printf("Available platform(s): %d\n", num_platforms);
	// Выделение массива платформ
	cl_platform_id* platforms = cl_init_get_array_platforms(num_platforms);
	cl_display_arr_platforms_info_default(platforms, num_platforms);
	platform = platforms[0];
	// Подсчёт устройств
	num_devices = cl_init_get_num_devices(platform);
	printf("Available device(s): %d\n", num_devices);
	// Выделение массива устройств
	cl_device_id* devices = cl_init_get_array_devices(platform, CL_DEVICE_TYPE_GPU, num_devices);
	cl_display_arr_devices_info_default(devices, num_devices);
	device = devices[0];
	// Создание контекста
	cl_context context = cl_init_create_context_by_devices(devices, num_devices);
	cl_display_context_info_minimum(context);
	//Создание очереди команд
	cl_command_queue commandQueue = cl_runtime_create_command_queue(context, device);
	// Создание объектов изображений (входного и выходного)
	cl_mem clInputImage_gaussian = cl_runtime_create_image_for_reading(context, &clImageFormat, width, height, buffer);
	cl_mem clOutputImage_gaussian = cl_runtime_create_image_for_writing(context, &clImageFormat, width, height);
	// Проверка, поддерживается ли изображения устройством, в противном случае - приложение прекратит свою работу
	cl_bool imageSupport = CL_FALSE;
	clGetDeviceInfo(device, CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &imageSupport, NULL);
	if (imageSupport != CL_TRUE)
	{
		printf("OpenCL device does not support images.\n");
		exit(1);
	}
	// Создание сэмплера для выборки объекта изображения
	cl_sampler sampler = cl_runtime_create_sampler(context);
	/*/////////////////////////////////////////////////////////////
	//////////////////////// Работа с ядрами ///////////////////////
	/////////////////////////////////////////////////////////////*/
	cl_program program_gaussian = cl_runtime_create_program_from_file(context, "Kernels/Kernel_Gaussian_Filter.cl");
	if (cl_runtime_build_program(program_gaussian, device, NULL) == -1)
		exit(-1);
	// Создание объектов ядра для каждого присутствующего в исходном коде ядра
	cl_kernel gaussian_kernel = cl_runtime_create_kernel(program_gaussian, "gaussian_filter");
	// Передача аргументов ядру
	if (cl_runtime_set_kernel_param(gaussian_kernel, 0, sizeof(cl_mem), &clInputImage_gaussian) == -1)
		exit(-1);
	if (cl_runtime_set_kernel_param(gaussian_kernel, 1, sizeof(cl_mem), &clOutputImage_gaussian) == -1)
		exit(-1);
	if (cl_runtime_set_kernel_param(gaussian_kernel, 2, sizeof(cl_sampler), &sampler) == -1)
		exit(-1);
	if (cl_runtime_set_kernel_param(gaussian_kernel, 3, sizeof(cl_int), &width) == -1)
		exit(-1);
	if (cl_runtime_set_kernel_param(gaussian_kernel, 4, sizeof(cl_int), &height) == -1)
		exit(-1);
	// Настройка рабочих групп
	size_t local_work_size[2] = { 16, 16 };
	size_t global_work_size[2] = { RoundUp(local_work_size[0], width), RoundUp(local_work_size[1], height) };
	// Запуск ядер на исполение 
	if (cl_runtime_enqueue_kernel(gaussian_kernel, commandQueue, 2, global_work_size, local_work_size) == -1)
		exit(-1);
	// Считывание выходного буфера обратно на хост
	size_t origin[3] = { 0, 0, 0 };
	size_t region[3] = { width, height, 1 };
	err = clEnqueueReadImage(commandQueue, clOutputImage_gaussian, CL_TRUE, origin, region, 0, 0, buffer, 0, NULL, NULL);
	if (err != CL_SUCCESS) {
		printf("Error reading result buffer.\n");
		exit(-1);
	}
	// Сохраняем результат
	CreateImageFromBuffer(fileName, (BYTE*)buffer, width, height, "-gaussian-filter.png");
	clReleaseMemObject(clOutputImage_gaussian);
	clReleaseProgram(program_gaussian);
	clReleaseKernel(gaussian_kernel);
	// Теперь поработаем с другим ядром. Ядром медианной фильтрации
	buffer = CreateBufferFromImage(fileName, width, height);
	cl_mem clInputImage_median = cl_runtime_create_image_for_reading(context, &clImageFormat, width, height, buffer);
	cl_mem clOutputImage_median = cl_runtime_create_image_for_writing(context, &clImageFormat, width, height);
	cl_program program_meidan = cl_runtime_create_program_from_file(context, "Kernels/Median_Filter.cl");
	if (cl_runtime_build_program(program_meidan, device, NULL) == -1)
		exit(-1);
	int filter_size = 3;
	cl_kernel median_kernel = cl_runtime_create_kernel(program_meidan, "median_filter");
	if (cl_runtime_set_kernel_param(median_kernel, 0, sizeof(cl_mem), &clInputImage_median) == -1)
		exit(-1);
	if (cl_runtime_set_kernel_param(median_kernel, 1, sizeof(cl_mem), &clOutputImage_median) == -1)
		exit(-1);
	if (cl_runtime_set_kernel_param(median_kernel, 2, sizeof(cl_sampler), &sampler) == -1)
		exit(-1);
	if (cl_runtime_set_kernel_param(median_kernel, 3, sizeof(cl_int), &filter_size) == -1)
		exit(-1);
	if (cl_runtime_enqueue_kernel(median_kernel, commandQueue, 2, global_work_size, local_work_size) == -1)
		exit(-1);
	err = clEnqueueReadImage(commandQueue, clOutputImage_median, CL_TRUE, origin, region, 0, 0, buffer, 0, NULL, NULL);
	if (err != CL_SUCCESS) {
		printf("Error reading result buffer.\n");
		exit(-1);
	}

	CreateImageFromBuffer(fileName, (BYTE*)buffer, width, height, "-median-filter.png");
	clReleaseMemObject(clOutputImage_median);
	clReleaseProgram(program_meidan);
	clReleaseKernel(median_kernel);

	clReleaseMemObject(clInputImage_gaussian);
	clReleaseSampler(sampler);
	clReleaseCommandQueue(commandQueue);
	clReleaseContext(context);
	free(devices);
	clReleaseDevice(device);
	free(platforms);
	delete[] buffer;
}