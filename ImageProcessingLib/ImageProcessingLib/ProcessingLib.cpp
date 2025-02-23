#include "pch.h"
#include <stdio.h>
#include <cstdlib>
#include "ProcessingLib.h"

///////////////////////////////////////////// Платформы /////////////////////////////////////////////

// Функция, отображающая минимальную информацию (имя и версию) о переданной платформе
void cl_display_platform_info_minimum(cl_platform_id platform)
{
	// Проверка платформы
	if (platform == NULL) {
		printf("The wrong platform was transmitted\n\tProblem area: the \"cl_display_platform_info_minimum\" function.\n\n");
		return;
	}
	cl_init_platform_get_info(platform, CL_PLATFORM_NAME, "Name");
	cl_init_platform_get_info(platform, CL_PLATFORM_VERSION, "Version");
}

// Функция, отображающая информацию по-умолчанию (имя, версию и профиль) о переданной платформе
void cl_display_platform_info_default(cl_platform_id platform)
{
	// Проверка платформы
	if (platform == NULL) {
		printf("The wrong platform was transmitted\n\tProblem area: the \"cl_display_platform_info_default\" function.\n\n");
		return;
	}
	cl_init_platform_get_info(platform, CL_PLATFORM_NAME, "Name");
	cl_init_platform_get_info(platform, CL_PLATFORM_VERSION, "Version");
	cl_init_platform_get_info(platform, CL_PLATFORM_PROFILE, "Profile");
}

// Функция, отображающая всю доступную информацию (имя, версию, производителя, профиль и расширения) о переданной платформе
void cl_display_platform_info_all(cl_platform_id platform)
{
	// Проверка платформы
	if (platform == NULL) {
		printf("The wrong platform was transmitted\n\tProblem area: the \"cl_display_platform_info_all\" function.\n\n");
		return;
	}
	cl_init_platform_get_info(platform, CL_PLATFORM_NAME, "Name");
	cl_init_platform_get_info(platform, CL_PLATFORM_VERSION, "Version");
	cl_init_platform_get_info(platform, CL_PLATFORM_VENDOR, "Vendor");
	cl_init_platform_get_info(platform, CL_PLATFORM_PROFILE, "Profile");
	cl_init_platform_get_info(platform, CL_PLATFORM_EXTENSIONS, "Extensions");
}

// Функция, отображающая минимальную информацию (имя и версию) о переданных платформах
void cl_display_arr_platforms_info_minimum(cl_platform_id* platforms, cl_uint platforms_count)
{
	// Проверка платформы
	if (platforms == NULL) {
		printf("The wrong platforms was transmitted\n\tProblem area: the \"cl_display_arr_platforms_info_minimum\" function.\n\n");
		return;
	}
	printf("Number of transferred platforms: %d\n", platforms_count);
	for (int i = 0; i < platforms_count; i++) {
		printf("[%d]:", i);
		cl_init_platform_get_info(platforms[i], CL_PLATFORM_NAME, "Name");
		cl_init_platform_get_info(platforms[i], CL_PLATFORM_VERSION, "Version");
		printf("\n");
	}
}

// Функция, отображающая информацию по-умолчанию (имя, версию и профиль) о переданных платформах
void cl_display_arr_platforms_info_default(cl_platform_id* platforms, cl_uint platforms_count)
{
	// Проверка платформы
	if (platforms == NULL) {
		printf("The wrong platforms was transmitted\n\tProblem area: the \"cl_display_arr_platforms_info_default\" function.\n\n");
		return;
	}
	printf("Number of transferred platforms: %d\n", platforms_count);
	for (int i = 0; i < platforms_count; i++) {
		printf("[%d]:", i);
		cl_init_platform_get_info(platforms[i], CL_PLATFORM_NAME, "Name");
		cl_init_platform_get_info(platforms[i], CL_PLATFORM_VERSION, "Version");
		cl_init_platform_get_info(platforms[i], CL_PLATFORM_PROFILE, "Profile");
		printf("\n");
	}
}

// Функция, отображающая всю доступную информацию (имя, версию, производителя, профиль и расширения) о переданных платформах
void cl_display_arr_platforms_info_all(cl_platform_id* platforms, cl_uint platforms_count)
{
	// Проверка платформы
	if (platforms == NULL) {
		printf("The wrong platforms was transmitted\n\tProblem area: the \"cl_display_arr_platforms_info_all\" function.\n\n");
		return;
	}
	printf("Number of transferred platforms: %d\n", platforms_count);
	for (int i = 0; i < platforms_count; i++) {
		printf("[%d]:", i);
		cl_init_platform_get_info(platforms[i], CL_PLATFORM_NAME, "Name");
		cl_init_platform_get_info(platforms[i], CL_PLATFORM_VERSION, "Version");
		cl_init_platform_get_info(platforms[i], CL_PLATFORM_VENDOR, "Vendor");
		cl_init_platform_get_info(platforms[i], CL_PLATFORM_PROFILE, "Profile");
		cl_init_platform_get_info(platforms[i], CL_PLATFORM_EXTENSIONS, "Extensions");
		printf("\n");
	}
}

///////////////////////////////////////////// Устройства /////////////////////////////////////////////

// Функция, отображающая минимальную информацию (тип, имя, поставщик, версия драйвера и версия OpenCL устройства) о выбранном устройстве
void cl_display_device_info_minimum(cl_device_id device)
{
	// Проверка платформы
	if (device == NULL) {
		printf("The wrong device was transmitted\n\tProblem area: the \"cl_display_device_info_minimum\" function.\n\n");
		return;
	}
	cl_init_device_get_info(device, CL_DEVICE_TYPE, "Device type");
	cl_init_device_get_info(device, CL_DEVICE_NAME, "Device name");
	cl_init_device_get_info(device, CL_DEVICE_VENDOR, "Device vendor");
	cl_init_device_get_info(device, CL_DRIVER_VERSION, "Device driver version");
	cl_init_device_get_info(device, CL_DEVICE_VERSION, "Device OpenCL version");
}

// Функция, отображающая информацию по-умолчанию (тип, имя, поставщик, версия драйвера, версия OpenCL, профиль, максимальное количество вычислительных блоков и расширения устройства) о выбранном устройстве
void cl_display_device_info_default(cl_device_id device)
{
	// Проверка платформы
	if (device == NULL) {
		printf("The wrong device was transmitted\n\tProblem area: the \"cl_display_device_info_default\" function.\n\n");
		return;
	}
	cl_init_device_get_info(device, CL_DEVICE_TYPE, "Device type");
	cl_init_device_get_info(device, CL_DEVICE_NAME, "Device name");
	cl_init_device_get_info(device, CL_DEVICE_VENDOR, "Device vendor");
	cl_init_device_get_info(device, CL_DEVICE_VENDOR_ID, "Device vendor id");
	cl_init_device_get_info(device, CL_DRIVER_VERSION, "Device driver version");
	cl_init_device_get_info(device, CL_DEVICE_VERSION, "Device OpenCL version");
	cl_init_device_get_info(device, CL_DEVICE_PROFILE, "Device profile");
	cl_init_device_get_info(device, CL_DEVICE_MAX_COMPUTE_UNITS, "Max compute Units");

	cl_init_device_get_info(device, CL_DEVICE_EXTENSIONS, "Device extensions");
}

// Функция, отображающая всю доступную информацию о выбранном устройстве
void cl_display_device_info_all(cl_device_id device)
{
	// Проверка платформы
	if (device == NULL) {
		printf("The wrong device was transmitted\n\tProblem area: the \"cl_display_device_info_all\" function.\n\n");
		return;
	}
	cl_init_device_get_info(device, CL_DEVICE_TYPE, "Device type");
	cl_init_device_get_info(device, CL_DEVICE_NAME, "Device name");
	cl_init_device_get_info(device, CL_DEVICE_VENDOR, "Device vendor");
	cl_init_device_get_info(device, CL_DRIVER_VERSION, "Device driver version");
	cl_init_device_get_info(device, CL_DEVICE_VERSION, "Device OpenCL version");
	cl_init_device_get_info(device, CL_DEVICE_PROFILE, "Device profile");
	cl_init_device_get_info(device, CL_DEVICE_MAX_COMPUTE_UNITS, "Max compute Units");
	cl_init_device_get_info(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, "Max work item dimensions");
	cl_init_device_get_info(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, "Max clock frequency");
	cl_init_device_get_info(device, CL_DEVICE_ADDRESS_BITS, "Device address bits");
	cl_init_device_get_info(device, CL_DEVICE_MAX_READ_IMAGE_ARGS, "Device max read image args");
	cl_init_device_get_info(device, CL_DEVICE_MAX_WRITE_IMAGE_ARGS, "Device max write image args");
	cl_init_device_get_info(device, CL_DEVICE_MAX_SAMPLERS, "Device max samplers");
	cl_init_device_get_info(device, CL_DEVICE_MEM_BASE_ADDR_ALIGN, "Device mem base addr align");
	cl_init_device_get_info(device, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, "Device min data type align size");
	cl_init_device_get_info(device, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, "Device global mem cacheline size");
	cl_init_device_get_info(device, CL_DEVICE_MAX_CONSTANT_ARGS, "Device max constant args");
	cl_init_device_get_info(device, CL_DEVICE_EXTENSIONS, "Device extensions");
}

// Функция, отображающая минимальную информацию (имя и версию) о выбранных устройствах
void cl_display_arr_devices_info_minimum(cl_device_id* devices, cl_uint devices_count)
{
	// Проверка платформы
	if (devices == NULL) {
		printf("The wrong devices was transmitted\n\tProblem area: the \"cl_display_arr_devices_info_minimum\" function.\n\n");
		return;
	}
	printf("Number of transferred devices: %d\n", devices_count);
	for (int i = 0; i < devices_count; i++) {
		printf("[%d]:", i);
		cl_display_device_info_minimum(devices[i]);
		printf("\n");
	}
}

// Функция, отображающая информацию по-умолчанию о выбранных устройствах
void cl_display_arr_devices_info_default(cl_device_id* devices, cl_uint devices_count)
{
	// Проверка платформы
	if (devices == NULL) {
		printf("The wrong devices was transmitted\n\tProblem area: the \"cl_display_arr_devices_info_default\" function.\n\n");
		return;
	}
	printf("Number of transferred devices: %d\n", devices_count);
	for (int i = 0; i < devices_count; i++) {
		printf("[%d]:", i);
		cl_display_device_info_default(devices[i]);
		printf("\n");
	}
}

// Функция, отображающая всю доступную информацию о выбранных устройствах
void cl_display_arr_devices_info_all(cl_device_id* devices, cl_uint devices_count)
{
	// Проверка платформы
	if (devices == NULL) {
		printf("The wrong devices was transmitted\n\tProblem area: the \"cl_display_arr_devices_info_all\" function.\n\n");
		return;
	}
	printf("Number of transferred devices: %d\n", devices_count);
	for (int i = 0; i < devices_count; i++) {
		printf("[%d]:", i);
		cl_display_device_info_all(devices[i]);
		printf("\n");
	}
}


///////////////////////////////////////////// Контекст /////////////////////////////////////////////

// Функция, отображающая минимальную информацию (количество и массив устройств) о выбранном контексте
void cl_display_context_info_minimum(cl_context context)
{
	// Проверка контекста
	if (context == NULL) {
		printf("The wrong context was transmitted\n\tProblem area: the \"cl_display_context_info_minimum\" function.\n\n");
		return;
	}
	cl_init_context_get_info(context, CL_CONTEXT_NUM_DEVICES, "Number of context's devices");
	cl_init_context_get_info(context, CL_CONTEXT_DEVICES, "Сontext's devices");
}

// Функция, отображающая минимальную информацию о выбранных контекстах
void cl_display_arr_contexts_info_minimum(cl_context* contexts, cl_uint contexts_count)
{
	// Проверка контекста
	if (contexts == NULL) {
		printf("The wrong context was transmitted\n\tProblem area: the \"cl_display_arr_contexts_info_minimum\" function.\n\n");
		return;
	}
	printf("Number of transferred contexts: %d\n", contexts_count);
	for (int i = 0; i < contexts_count; i++) {
		printf("[%d]:", i);
		cl_display_context_info_minimum(contexts[i]);
		printf("\n");
	}
}

///////////////////////////////////////////// Очередь команд /////////////////////////////////////////////

cl_command_queue ipGetComQueueToDevice(cl_context ctx, cl_device_id dev)
{
	cl_command_queue res = clCreateCommandQueue(ctx, dev, NULL, NULL);
	return res;
}

///////////////////////////////////////////// Работа с кодом ядер /////////////////////////////////////////////

cl_program ipGetProgram(cl_context ctx, const char filename[])
{
	FILE* fp;
	size_t source_size;
	char* source_str;
	try {
		fopen_s(&fp, filename, "r");
		if (!fp) {
			fprintf(stderr, "Failed to load kernel.\n");
			exit(1);
		}
		source_str = (char*)malloc(512);
		source_size = fread(source_str, 1, 512, fp);
		fclose(fp);
	}
	catch (int a) {
		printf("%d", a);
	}
	cl_program res = clCreateProgramWithSource(ctx, 1, (const char**)&source_str, (const size_t*)&source_size, NULL);
	return res;
}

cl_int ipBuildProgram(cl_program prg, cl_device_id* devs)
{
	cl_int res;
	cl_uint count_devs = sizeof(devs) / sizeof(cl_device_id);
	res = clBuildProgram(prg, count_devs, devs, NULL, NULL, NULL);
	for (int i = 0; i < count_devs; i++)
	{
		size_t log_size, dev_size;
		char* log;
		clGetProgramBuildInfo(prg, devs[i], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		log = (char*)malloc(log_size);
		clGetProgramBuildInfo(prg, devs[i], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
		printf("\n  > Program Build Log: %s\n", log);
	}
	printf("\n");
	return res;
}

cl_kernel ipGetKernel(cl_program prg, const char* name)
{
	cl_kernel res = clCreateKernel(prg, name, NULL);
	return res;
}

cl_int ipSetKernelParam(cl_kernel krn, cl_uint param_idx, size_t param_size, const void* param)
{
	cl_int res = clSetKernelArg(krn, param_idx, param_size, param);
	return res;
}

cl_int ipWorkKernel(cl_kernel krn, cl_command_queue que, cl_uint dim, const size_t* gws)
{
	cl_int res = clEnqueueNDRangeKernel(que, krn, dim, NULL, gws, NULL, 0, NULL, NULL);
	return res;
}