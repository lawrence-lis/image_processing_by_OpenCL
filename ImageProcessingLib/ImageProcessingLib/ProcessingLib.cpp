#include "pch.h"
#include <stdio.h>
#include <cstdlib>
#include "ProcessingLib.h"

///////////////////////////////////////////// Платформы /////////////////////////////////////////////

cl_uint ipGetCountPlatforms()
{
	// Функция для подсчёта количества доступных платформ
	cl_uint res;
	switch (clGetPlatformIDs(0, NULL, &res))
	{
	case CL_SUCCESS:
		// Подсчёт произошёл успешно, ничего выводить не нужно, просто вернуть посчитанное значение
		return res;
	case CL_INVALID_VALUE:
		// Выпадает если в качестве последних двух параметров функции clGetPlatformIDs передали NULL, либо если в качестве количества доступных платформ передали 0, а в качестве платформ передали 
		// ненулевой указатель. Вывести сообщение о неправильности передаваемых параметров и вернуть код ошибки, который, возможно, будет внесён и классифицирован намного позже. По идее, от пользователя
		// тут ничего не зависит, и если выпадает имеено эта ветка, то либо происходит тестирование, либо что-то не так с компьютером пользователя, либо разработчик что-то напутал и не тестировал то, 
		// что разработал.
		printf("Error counting available platforms.\n  > [problem area: the \"ipGetCountPlatforms\" function]\n");
		return -101;
	case CL_OUT_OF_HOST_MEMORY:
		// Ошибка: Не удается выделить на хосте ресурсы, необходимые для реализации OpenCL. Вывести соответствующее сообщение и вернуть код ошибки, который, возможно, будет внесён и классифицирован 
		// намного позже. По идее, эта ошибка может вылезти только из-за неисправности самого хоста.
		printf("Error: Unable to allocate resources required by the OpenCL implementation on the host.\n  > [problem area: the \"ipGetCountPlatforms\" function]\n");
		return -102;
	default:
		// Ничто из вышеперечисленного, вывестина экран сообщение о неизвестной ошибке и вернуть код ошибки, который, возможно, будет внесён и классифицирован намного позже
		printf("WARNING: unknown error when calculating the number of available platforms\n  > [problem area: the \"ipGetCountPlatforms\" function]\n");
		return -103;
	}
}

cl_platform_id* ipGetArrPlatforms(cl_uint count)
{
	// Функция для получения массива доступных платформ
	cl_uint subCount = count;		// Ввёл для обработки случая с нулём в качестве аргумента функции
	cl_platform_id* res;
	// Я хочу, чтобы в случае когда пользователь в качестве аргумента функции передал значение не больше нуля, то происходил перерасчёт доступных платформ
	if (count <= 0) subCount = ipGetCountPlatforms();
	// Проверка переданного/пересчитанного значения количества доступных платформ
	switch (subCount) {
	case -101:
		// По идее, должно сработать если в качестве аргумента функции пользователь передал конкретное значение -101 и по каким-то причинам оно успешно прошло вышестоящую проверку (чего явно не должно
		// быть, но я устал и исправлять это не буду). Либо же в процессе перерасчёта произошло что-то непредвиденное и в качестве значения количества доступных платформ вернулся код ошибки -101.
		// Связано это может быть с ошибкой при выполнении самой функции (чисто моё мнение, маловероятно). В любом случае надо вывести соответствующее сообщение об ошибке, прервать выполнение функции с
		// возвращением соответствующего кода ошибки.
		printf("Error counting available platforms.\n  > [problem area: the \"ipGetArrPlatforms\" function]\n");
		exit(-1011);
	case -102:
		// По идее, должно сработать если в качестве аргумента функции пользователь передал конкретное значение -102 и по каким-то причинам оно успешно прошло вышестоящую проверку. Либо же в процессе 
		// перерасчёта произошло что-то непредвиденное и в качестве значения количества доступных платформ вернулся код ошибки -102. Связано это может быть с проблемой работы хоста (чисто моё мнение).
		// В любом случае надо вывести соответствующее сообщение об ошибке, прервать выполнение функции с возвращением соответствующего кода ошибки.
		printf("Error in calculating the number of available platforms: host.\n  > [problem area: the \"ipGetArrPlatforms\" function]\n");
		exit(-1021);
	case -103:
		// По идее, должно сработать если в качестве аргумента функции пользователь передал конкретное значение -103 и по каким-то причинам оно успешно прошло вышестоящую проверку. Либо же в процессе 
		// перерасчёта произошло что-то непредвиденное и в качестве значения количества доступных платформ вернулся код ошибки -103. Не имею ни малейшего понятия, почему это произошло. В любом случае
		// надо вывести соответствующее сообщение об ошибке, прервать выполнение функции с возвращением соответствующего кода ошибки.
		printf("Error in calculating the number of available platforms: unknown.\n  > [problem area: the \"ipGetArrPlatforms\" function]\n");
		exit(-1031);
	default:
		// Передача значения количества доступных платформ произошла без ошибок, либо же их перерасчёт прошёл успешно. Ничего не выводить, просто продолжить работу функции.
		break;
	}
	res = new cl_platform_id[subCount];
	cl_uint status = clGetPlatformIDs(subCount, res, NULL);
	switch (status) {
	case CL_SUCCESS:
		// Выделение массива произошло успешно, ничего выводить не нужно, просто вернуть массив.
		return res;
	case CL_INVALID_VALUE:
		// Выпадает если в качестве последних двух параметров функции clGetPlatformIDs передали NULL, либо если в качестве количества доступных платформ передали 0, а в качестве платформ передали 
		// ненулевой указатель. Короче говоря, ошибка в выполнении функции clGetPlatformIDs. Вывести соответствующее сообщение об ошибке, прервать выполнение функции и вернуть соответствующий код
		// ошибки.
		printf("Error in the execution of the clGetPlatformIDs function\n  > [problem area: the \"ipGetArrPlatforms\" function]\n");
		exit(-1012);
	case CL_OUT_OF_HOST_MEMORY:
		// Выпадает если не удается выделить на хосте ресурсы, необходимые для реализации OpenCL. По идее, эта ошибка может вылезти только из-за неисправности самого хоста. Вывести соответствующее 
		// сообщение и вернуть код ошибки.
		printf("Error allocating resources needed to implement OpenCL on the host\n  > [problem area: the \"ipGetArrPlatforms\" function]\n");
		exit(-1022);
	default:
		// Ничто из вышеперечисленного. Понятия не имею, что же произошло. В любом случае вывестина экран сообщение о неизвестной ошибке и вернуть код ошибки
		printf("Unknown error\n  > [problem area: the \"ipGetArrPlatforms\" function]\n");
		exit(-1032);
	}
}

cl_platform_id ipGetPlatformByIndex(cl_platform_id* platforms, cl_uint count, int idx)
{
	cl_platform_id res;
	if (platforms == NULL)
	{
		cl_platform_id* subArrPlatforms = ipGetArrPlatforms(count);
		res = subArrPlatforms[0];
		delete[] subArrPlatforms;
	}
	else res = platforms[idx];
	return res;
}

void ipGetInfoAboutPlatform(cl_platform_id platform, cl_platform_info info_type, size_t& p_s, const char* title)
{
	// Функция написана в стиле "двойного вызова"
	cl_uint status = clGetPlatformInfo(platform, info_type, 0, NULL, &p_s);
	switch (status)
	{
	case CL_SUCCESS:
		// Функция clGetPlatformInfo выполнена успешно, количество памяти, необходимой для нужной информации, посчитано. Нужно просто вернуться к выполнению функции.
		break;
	case CL_INVALID_PLATFORM:
		// Выподает если переданная платформа (первый параметр функции) не является доступной платформой. Вывести соответствующее сообщение и вернуть управление.
		printf("The transferred platform is not a valid platform\n  > [problem area: the \"ipGetInfoAboutPlatform\" function]\n");
		return;
	case CL_INVALID_VALUE:
		// По идее на данном может выпасть если переданный тип получаемой информации не является одним из поддерживаемых значений. Вывести соответствующее сообщение и вернуть управление.
		printf("The transmitted type of information received is not one of the supported values\n  > [problem area: the \"ipGetInfoAboutPlatform\" function]\n");
		return;
	case CL_OUT_OF_HOST_MEMORY:
		// Выподает если происходит сбой в распределении ресурсов, требуемых реализацией OpenCL на хосте (так написано в спецификации). Вывести соответствующее сообщение и вернуть управление.
		printf("There was a failure in the allocation of resources required by the OpenCL implementation on the host\n  > [problem area: the \"ipGetInfoAboutPlatform\" function]\n");
		return;
	default:
		// Прроизошло что-то непонятное, но подсчёт выделяемой информации не завершился успешно. Вывести соответствующее сообщение и вернуть управление.
		printf("Unknown error\n  > [problem area: the \"ipGetInfoAboutPlatform\" function]\n");
		return;
	}
	char* str = new char[p_s];
	status = clGetPlatformInfo(platform, info_type, p_s, str, NULL);
	switch (status)
	{
	case CL_SUCCESS:
		// Функция clGetPlatformInfo выполнена успешно, информация считана в переменную. Нужно вывести на экран считанную информацию и вернуться к выполнению функции (там осталось только освобождение 
		// память, выделенной под строку с информацией).
		printf("\t%s: %s\n", title, str);
		break;
	case CL_INVALID_PLATFORM:
		// Выподает если переданная платформа (первый параметр функции) не является доступной платформой. Вывести соответствующее сообщение и вернуть управление.
		printf("The transferred platform is not a valid platform\n  > [problem area: the \"ipGetInfoAboutPlatform\" function]\n");
		break;
	case CL_INVALID_VALUE:
		// Выпадает если переданный тип информации, указанный в параметре info_type, не является одним из поддерживаемых значений или если размер в байтах, указанный в параметре p_s, меньше размера
		// возвращаемого типа, указанного в таблице запросов платформы, а значение указателя на ячейку памяти, переаднный в параметре str не равно NULL. Вывести соответствующее сообщение и вернуть 
		// управление.
		printf("The transmitted type of information specified in the info_type parameter is not one of the supported values, or if the size in bytes specified in the p_s parameter is less than the ");
		printf("size of the returned type specified in the platform query table, and the value of the pointer to the memory cell redirected in the str parameter is not NULL");
		printf("\n  >[problem area : the \"ipGetInfoAboutPlatform\" function]\n");
		break;
	case CL_OUT_OF_HOST_MEMORY:
		// Выподает если происходит сбой в распределении ресурсов, требуемых реализацией OpenCL на хосте (так написано в спецификации). Вывести соответствующее сообщение и вернуть управление.
		printf("There was a failure in the allocation of resources required by the OpenCL implementation on the host\n  > [problem area: the \"ipGetInfoAboutPlatform\" function]\n");
		break;
	default:
		// Прроизошло что-то непонятное, но подсчёт выделяемой информации не завершился успешно. Вывести соответствующее сообщение и вернуть управление.
		printf("Unknown error\n  > [problem area: the \"ipGetInfoAboutPlatform\" function]\n");
		break;
	}
	delete[] str;
}

void ipGetInfoAboutAvailablePlatforms()
{
	// 1-й вызов: для определения количества доступных платформ
	cl_uint count_platforms = ipGetCountPlatforms();
	printf("Number of available platforms: %d\n\n", count_platforms);
	// Выделение памяти под заданное число идентификаторов
	cl_platform_id* platforms = ipGetArrPlatforms(count_platforms);
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
	cl_uint count_platforms = ipGetCountPlatforms(), count_devices;
	printf("Number of available platforms: %d\n\n", count_platforms);
	cl_platform_id* platforms = ipGetArrPlatforms(count_platforms);
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