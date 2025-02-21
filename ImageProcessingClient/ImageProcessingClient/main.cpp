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
	/*/////////////////////////////////////////////////////////////
	///////// Настройка среды для работы с изображениями //////////
	/////////////////////////////////////////////////////////////*/
	const char* fileName = "cat.jpg";
	unsigned int width, height;
	char* buffer = CreateBufferFromImage(fileName, width, height);

	cl_image_format clImageFormat;
	clImageFormat.image_channel_order = CL_RGBA;
	clImageFormat.image_channel_data_type = CL_UNORM_INT8;
	/*/////////////////////////////////////////////////////////////
	///////////// Настройка среды для работы с OpenCL /////////////
	/////////////////////////////////////////////////////////////*/
	cl_int err;
	cl_uint num_platforms, num_devices;
	cl_platform_id platform;
	cl_device_id device;
	cl_program program;
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
	cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
	if (err != CL_SUCCESS) {
		printf("Error to create context to device.\n");
		exit(-1);
	}
	//Создание очереди команд
	cl_command_queue commandQueue = clCreateCommandQueue(context, device, 0, &err);
	if (err != CL_SUCCESS) {
		printf("Error to create Command Queue.\n");
		exit(-1);
	}

	// Создание объекта входного изображения, хранящего данные из передаваемого изображения
	cl_mem clInputImage = clCreateImage2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, &clImageFormat, width, height, 0, buffer, &err);
	if (err != CL_SUCCESS) {
		printf("Error to create OpenCL input image object.\n");
		exit(-1);
	}
	// Создание объекта выходного изображения, который сохранит результат выполнения гауссовой фильтрации входного изображения
	// Этот объект создается без host_ptr, поскольку он будет заполнен данными в ядре.
	// Кроме того, для mem_flags установлено значение CL_MEM_WRITE_ONLY, потому что образ будет только записан в ядро, но не прочитан.
	cl_mem clOutputImage = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &clImageFormat, width, height, 0, NULL, &err);
	if (err != CL_SUCCESS) {
		printf("Error to create OpenCL output image object.\n");
		exit(-1);
	}
	// Проверка, поддерживается ли изображения устройством, в противном случае - приложение прекратит свою работу
	cl_bool imageSupport = CL_FALSE;
	clGetDeviceInfo(device, CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &imageSupport, NULL);
	if (imageSupport != CL_TRUE)
	{
		printf("OpenCL device does not support images.\n");
		exit(1);
	}
	// Создание сэмплера для выборки объекта изображения
	cl_sampler sampler = clCreateSampler(context, CL_FALSE, CL_ADDRESS_CLAMP_TO_EDGE, CL_FILTER_NEAREST, &err);
	if (imageSupport != CL_TRUE)
	{
		printf("Error to create sampler.\n");
		exit(1);
	}
	/*/////////////////////////////////////////////////////////////
	//////////////////////// Работа с ядром ///////////////////////
	/////////////////////////////////////////////////////////////*/
	std::ifstream kernel_file("Kernel_Gaussian_Filter.cl", std::ios::in);
	if (!kernel_file.is_open()) {
		printf("Error to open kernel file to reading.\n");
		exit(-1);
	}
	std::ostringstream oss;
	oss << kernel_file.rdbuf();
	std::string srcStdStr = oss.str();
	const char* srcStr = srcStdStr.c_str();
	program = clCreateProgramWithSource(context, 1, (const char**)&srcStr, NULL, &err);
	if (err != CL_SUCCESS) {
		printf("Error to create OpenCL program from source.\n");
		exit(-1);
	}
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err != CL_SUCCESS) {
		char buildLog[16384];
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, NULL);
		printf("Error in kernel: %s.\n", buildLog);
		exit(-1);
	}
	// Создание объектов ядра для каждого присутствующего в исходном коде ядра
	cl_kernel gaussian_kernel = clCreateKernel(program, "gaussian_filter", &err);
	if (err != CL_SUCCESS) {
		printf("Error to create kernel object.\n");
		exit(-1);
	}
	// Передача аргументов ядру
	err = clSetKernelArg(gaussian_kernel, 0, sizeof(cl_mem), &clInputImage);
	if (err != CL_SUCCESS) {
		printf("Error to set kernel 0 argument.\n");
		exit(-1);
	}
	err = clSetKernelArg(gaussian_kernel, 1, sizeof(cl_mem), &clOutputImage);
	if (err != CL_SUCCESS) {
		printf("Error to set kernel 1 argument.\n");
		exit(-1);
	}
	err = clSetKernelArg(gaussian_kernel, 2, sizeof(cl_sampler), &sampler);
	if (err != CL_SUCCESS) {
		printf("Error to set kernel 2 argument.\n");
		exit(-1);
	}
	err = clSetKernelArg(gaussian_kernel, 3, sizeof(cl_int), &width);
	if (err != CL_SUCCESS) {
		printf("Error to set kernel 3 argument.\n");
		exit(-1);
	}
	err = clSetKernelArg(gaussian_kernel, 4, sizeof(cl_int), &height);
	if (err != CL_SUCCESS) {
		printf("Error to set kernel 4 argument.\n");
		exit(-1);
	}
	// Настройка рабочих групп
	size_t local_work_size[2] = { 16, 16 };
	size_t global_work_size[2] = { RoundUp(local_work_size[0], width), RoundUp(local_work_size[1], height) };
	// Запуск ядер на исполение 
	err = clEnqueueNDRangeKernel(commandQueue, gaussian_kernel, 2, NULL, global_work_size, local_work_size, 0, NULL, NULL);
	if (err != CL_SUCCESS) {
		printf("Error queuing kernel for execution.\n");
		exit(-1);
	}
	// Считывание выходного буфера обратно на хост
	size_t origin[3] = { 0, 0, 0 };
	size_t region[3] = { width, height, 1 };
	err = clEnqueueReadImage(commandQueue, clOutputImage, CL_TRUE, origin, region, 0, 0, buffer, 0, NULL, NULL);
	if (err != CL_SUCCESS) {
		printf("Error reading result buffer.\n");
		exit(-1);
	}
	// Сохраняем результат
	CreateImageFromBuffer(fileName, (BYTE*)buffer, width, height);
	/*/////////////////////////////////////////////////////////////
	//////// Освобождение Добби (всех выделенных ресурсов) ////////
	/////////////////////////////////////////////////////////////*/
	clReleaseKernel(gaussian_kernel);
	clReleaseProgram(program);
	clReleaseSampler(sampler);
	clReleaseCommandQueue(commandQueue);
	clReleaseContext(context);
	free(devices);
	clReleaseDevice(device);
	free(platforms);
	delete[] buffer;
}