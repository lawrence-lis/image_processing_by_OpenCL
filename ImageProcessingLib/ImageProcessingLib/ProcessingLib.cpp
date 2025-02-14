#include "pch.h"
#include <stdio.h>
#include <cstdlib>
#include "ProcessingLib.h"

///////////////////////////////////////////// Платформы /////////////////////////////////////////////

cl_platform_id ipGetPlatformByIndex(cl_platform_id* platforms, cl_uint count, int idx)
{
	cl_platform_id res;
	if (platforms == NULL)
	{
		cl_platform_id* subArrPlatforms = cl_init_get_array_platforms(count);
		res = subArrPlatforms[0];
		delete[] subArrPlatforms;
	}
	else res = platforms[idx];
	return res;
}

void ipGetInfoAboutPlatform(cl_platform_id platform, cl_platform_info info_type, size_t& p_s, const char* title)
{
	cl_uint status = clGetPlatformInfo(platform, info_type, 0, NULL, &p_s);
	char* str = new char[p_s];
	status = clGetPlatformInfo(platform, info_type, p_s, str, NULL);
	printf("\t%s: %s\n", title, str);
	delete[] str;
}

void ipGetInfoAboutAvailablePlatforms()
{
	// 1-й вызов: для определения количества доступных платформ
	cl_uint count_platforms = cl_init_get_num_platforms();
	printf("Number of available platforms: %d\n\n", count_platforms);
	// Выделение памяти под заданное число идентификаторов
	cl_platform_id* platforms = cl_init_get_array_platforms(count_platforms);
	size_t size;
	for (cl_uint i = 0; i < count_platforms; i++)
	{
		printf("Platform[%d]:\n", i);
		// Определение и вывод профиля платформы (что бы это ни значило)
		ipGetInfoAboutPlatform(platforms[i], CL_PLATFORM_PROFILE, size, "Profile");
		// Определение и вывод имени платформы
		ipGetInfoAboutPlatform(platforms[i], CL_PLATFORM_NAME, size, "Name");
		// Определение и вывод номер версии платформы
		ipGetInfoAboutPlatform(platforms[i], CL_PLATFORM_VERSION, size, "Version");
		// Определение и вывод доступные расширения платформы
		ipGetInfoAboutPlatform(platforms[i], CL_PLATFORM_EXTENSIONS, size, "Extensions");
		printf("\n");
	}
	printf("\n");
	delete[] platforms;
}

///////////////////////////////////////////// Устройства /////////////////////////////////////////////

cl_uint ipGetCountDevices(cl_platform_id platform, cl_device_info device_type)
{
	cl_uint res, status;
	if (platform == NULL)
	{
		cl_platform_id subPlatform = ipGetPlatformByIndex(NULL, 0, NULL);
		if (device_type == NULL)
			status = clGetDeviceIDs(subPlatform, CL_DEVICE_TYPE_ALL, 0, NULL, &res);
		else status = clGetDeviceIDs(subPlatform, device_type, 0, NULL, &res);
	}
	else if (device_type == NULL) status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &res);
	else status = clGetDeviceIDs(platform, device_type, 0, NULL, &res);
	return res;
}

cl_device_id* ipGetArrDevices(cl_platform_id platform, cl_device_info device_type, cl_uint count)
{
	cl_device_id* res;
	cl_platform_id subPlatform = platform;
	cl_device_info subDevType = device_type;
	cl_uint status, subCount = count;
	if (count == 0 || platform == NULL || device_type == NULL)
	{
		// Если было передано количество устройств, равное 0, или , то идёт перерасчёт доступных устройств и на выход подаётся массив всех доступных устроствя для данной/доступной платформы
		if (platform == NULL)
		{
			// Если не была передана платформа, то происходит поиск доступных платформ и выбор первой попавшейся
			subPlatform = ipGetPlatformByIndex(NULL, 0, NULL);
			if (device_type == NULL)
			{
				// Если не был передан тип устройств, то идёт перерасчёт всех доступных устройств всех доступных типов для перевыбранной платформой
				subDevType = CL_DEVICE_TYPE_ALL;
				subCount = ipGetCountDevices(subPlatform, subDevType);
			}
			else subCount = ipGetCountDevices(subPlatform, subDevType);
		}
		else if (device_type == NULL)
		{
			// Если не был передан тип устройств, то идёт перерасчёт всех доступных устройств всех доступных типов для изначально выбранной платформой
			subDevType = CL_DEVICE_TYPE_ALL;
			subCount = ipGetCountDevices(subPlatform, subDevType);
		}
	}
	res = new cl_device_id[subCount];
	status = clGetDeviceIDs(subPlatform, subDevType, subCount, res, NULL);
	return res;
}

cl_device_id ipGetDeviceByIndex(cl_device_id* devices, cl_uint count, int idx)
{
	cl_device_id res;
	if (devices == NULL)
	{
		cl_device_id* subDevices = ipGetArrDevices(NULL, 0, count);
		if (count == 0) res = subDevices[0];
		else res = subDevices[idx];
		delete[] subDevices;
	}
	else res = devices[idx];
	return res;
}

void ipGetInfoAboutSelectedDevice(cl_device_id device, cl_device_info info_type, size_t& p_s, const char* title)
{
	cl_uint status = clGetDeviceInfo(device, info_type, 0, NULL, &p_s);
	char* str = new char[p_s];
	status = clGetDeviceInfo(device, info_type, p_s, str, NULL);
	printf("\t%s: %s\n", title, str);
	delete[] str;
}

void ipGetAllInfoAboutSelectedDevices(cl_device_id device)
{
	size_t size;
	// Определение и вывод профиля устройства (что бы это не значило)
	ipGetInfoAboutSelectedDevice(device, CL_DEVICE_PROFILE, size, "Profile");
	// Определение и вывод типа устройства
	ipGetInfoAboutSelectedDevice(device, CL_DEVICE_TYPE, size, "Type");
	// Определение и вывод имени устройства
	ipGetInfoAboutSelectedDevice(device, CL_DEVICE_NAME, size, "Name");
	// Определение и вывод имени производителя устройства
	ipGetInfoAboutSelectedDevice(device, CL_DEVICE_VENDOR, size, "Vendor");
	// Определение и вывод максимального количества параллельных вычислительных единиц
	ipGetInfoAboutSelectedDevice(device, CL_DEVICE_MAX_COMPUTE_UNITS, size, "Max compute units");
	// Определение и вывод максимальной частоты устройства в мегагерцах (MHz)
	ipGetInfoAboutSelectedDevice(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, size, "Max frequency");
	// Определение и вывод доступных устройству расширений
	ipGetInfoAboutSelectedDevice(device, CL_DEVICE_EXTENSIONS, size, "Extensions");
	printf("\n");
}

void ipGetInfoAboutAvailableDevices()
{
	cl_uint count_platforms = cl_init_get_num_platforms(), count_devices;
	printf("Number of available platforms: %d\n\n", count_platforms);
	cl_platform_id* platforms = cl_init_get_array_platforms(count_platforms);
	size_t size;
	for (cl_uint i = 0; i < count_platforms; i++)
	{
		printf("Platform[%d]:\n", i);
		// Определение и вывод профиля платформы
		ipGetInfoAboutPlatform(platforms[i], CL_PLATFORM_PROFILE, size, "Profile");
		// Определение и вывод имени платформы
		ipGetInfoAboutPlatform(platforms[i], CL_PLATFORM_NAME, size, "Name");
		// Определение и вывод номер версии платформы
		ipGetInfoAboutPlatform(platforms[i], CL_PLATFORM_VERSION, size, "Version");
		// Определение и вывод доступные расширения платформы
		ipGetInfoAboutPlatform(platforms[i], CL_PLATFORM_EXTENSIONS, size, "Extensions");
		printf("---------------------------------------------------------------------------\n");
		count_devices = ipGetCountDevices(platforms[i], CL_DEVICE_TYPE_ALL);
		printf("Number of available devices: %d\n\n", count_devices);
		cl_device_id* devices = ipGetArrDevices(platforms[i], CL_DEVICE_TYPE_ALL, count_devices);
		for (cl_uint j = 0; j < count_devices; j++)
		{
			printf("Device[%d]:\n", i);
			ipGetAllInfoAboutSelectedDevices(devices[j]);
			printf("-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n");
		}
		printf("---------------------------------------------------------------------------\n");
		delete[] devices;
	}
	printf("\n");
	delete[] platforms;
}

///////////////////////////////////////////// Контекст /////////////////////////////////////////////

cl_context ipGetContextByArr(cl_device_id* devs, cl_uint count)
{
	cl_context res = clCreateContext(NULL, count, devs, NULL, NULL, NULL);
	return res;
}

cl_context ipGetContextByType(cl_device_type type)
{
	cl_context res = clCreateContextFromType(NULL, type, NULL, NULL, NULL);
	return res;
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
		printf("Device: ");
		ipGetInfoAboutSelectedDevice(devs[i], CL_DEVICE_NAME, dev_size, "Name");
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