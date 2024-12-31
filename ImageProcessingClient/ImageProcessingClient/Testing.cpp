#include "CL/cl.h"
#include "Testing.h"
#include "ProcessingLib.h"
#include <cstdlib>

/////////////////////////////////// Функции, через которые я тестирую разные вещи //////////////////////////////////////////////////////

void Checking_clGetPlatformIds(cl_uint num_entries, cl_platform_id* platforms, cl_uint* num_platforms)
{
	switch (clGetPlatformIDs(num_entries, platforms, num_platforms))
	{
	case CL_SUCCESS:
		printf("Available platforms: %d\n", *num_platforms);
		return;
	case CL_INVALID_VALUE:
		printf("Error counting available platforms.\n"); 
		return;
	case CL_OUT_OF_HOST_MEMORY:
		printf("Error: Unable to allocate resources required by the OpenCL implementation on the host.\n");
		return;
	default:
		printf("WARNING: unknown error when calculating the number of available platforms\n");
		return;
	}
}

cl_platform_id* Checking_ipGetArrPlatforms(cl_uint count, bool recounting = false)
{
	// Ввожу эту функцию для того, чтобы обработать передачу значений входных параметров не больше нуля и привести соответствующую функцию библиотеки к рабочему варианту, 
	// если текущий вариант не работает
	cl_int subCount = (int)count;
	cl_platform_id* res;
	if (recounting)
		if (count <= 0) 
			subCount = ipGetCountPlatforms();
	switch (subCount) {
		// Я заменю часть исходной функции. В оригинале функция прерывает работу программы, а здесь, только для тестов, пусть функция возвращает нулевой указатель и выводит код ошибки.
	case -101:
		printf("Error counting available platforms.\n  > [problem area: the \"ipGetArrPlatforms\" function]\n\tError code: -1011\n");
		return NULL;
	case -102:
		printf("Error in calculating the number of available platforms: host.\n  > [problem area: the \"ipGetArrPlatforms\" function]\n\tError code: -1021\n");
		return NULL;
	case -103:
		printf("Error in calculating the number of available platforms: unknown.\n  > [problem area: the \"ipGetArrPlatforms\" function]\n\tError code: -1031\n");
		return NULL;
	default:
		printf("calculating/getting successfull\n");
		break;
	}
	res = new cl_platform_id[subCount];
	cl_uint status = clGetPlatformIDs(subCount, res, NULL);
	switch (status) {
	case CL_SUCCESS:
		return res;
	case CL_INVALID_VALUE:
		printf("Error in the execution of the clGetPlatformIDs function\n  > [problem area: the \"ipGetArrPlatforms\" function]\n\tError code: -1012\n"); 
		return NULL;
	case CL_OUT_OF_HOST_MEMORY:
		printf("Error allocating resources needed to implement OpenCL on the host\n  > [problem area: the \"ipGetArrPlatforms\" function]\n\tError code: -1022\n");
		return NULL;
	default:
		printf("Unknown error\n  > [problem area: the \"ipGetArrPlatforms\" function]\n\tError code: -1032\n");
		return NULL;
	}
}

cl_platform_id Checking_ipGetPlatformByIndex(cl_platform_id* platforms, cl_uint count, int idx, bool EmptyArray = false)
{
	cl_platform_id res;
	if (EmptyArray)
	{
		if (platforms == NULL)
		{
			cl_platform_id* subArrPlatforms = ipGetArrPlatforms(count);
			res = subArrPlatforms[0];
			delete[] subArrPlatforms;
		}
		else res = platforms[idx];
	}
	else res = platforms[idx];
	return res;
}

void Checking_ipGetInfoAboutAvailablePlatforms(cl_uint count_platforms = ipGetCountPlatforms())
{
	switch (count_platforms) {
	case -101:
		printf("Error counting available platforms.\n  > [problem area: the \"ipGetInfoAboutAvailablePlatforms\" function]\n");
		return;
	case -102:
		printf("Error in calculating the number of available platforms: host.\n  > [problem area: the \"ipGetInfoAboutAvailablePlatforms\" function]\n");
		return;
	case -103:
		printf("Error in calculating the number of available platforms: unknown.\n  > [problem area: the \"ipGetInfoAboutAvailablePlatforms\" function]\n");
		return;
	default:
		printf("Number of available platforms: %d\n\n", count_platforms);
		break;
	}
	cl_platform_id* platforms = ipGetArrPlatforms(count_platforms);
	size_t size;
	for (cl_uint i = 0; i < count_platforms; i++)
	{
		printf("Platform[%d]:\n", i);
		ipGetInfoAboutPlatform(platforms[i], CL_PLATFORM_NAME, size, "Name");
		ipGetInfoAboutPlatform(platforms[i], CL_PLATFORM_VERSION, size, "Version");
		ipGetInfoAboutPlatform(platforms[i], CL_PLATFORM_PROFILE, size, "Profile");
		ipGetInfoAboutPlatform(platforms[i], CL_PLATFORM_EXTENSIONS, size, "Extensions");
		printf("\n");
	}
	printf("\n");
	delete[] platforms;
}

cl_device_id* Checking_ipGetArrDevices(cl_platform_id platform, cl_device_info device_type, cl_uint count, bool prov = true)
{
	cl_device_id* res;
	cl_platform_id subPlatform = platform;
	cl_device_info subDevType = device_type;
	cl_uint status;
	cl_int subCount = (int)count;
	if (prov == true) {
		if (subCount <= 0 || platform == NULL || device_type == NULL) {
			if (platform == NULL) {
				subPlatform = ipGetPlatformByIndex(NULL, 0, NULL);
				if (device_type == NULL)
					subDevType = CL_DEVICE_TYPE_ALL;
				subCount = ipGetCountDevices(subPlatform, subDevType);
			}
			else {
				if (device_type == NULL) {
					subDevType = CL_DEVICE_TYPE_ALL;
				}
				subCount = ipGetCountDevices(subPlatform, subDevType);
			}
		}
	}
	switch (subCount) {
	case -201:
		printf("Error counting available Devices. The provided platform is not a valid platform.\n  > [problem area: the \"ipGetArrDevices\" function]\n");
		return NULL;
	case -202:
		printf("Error counting available Devices. The provided device type is not a valid value.\n  > [problem area: the \"ipGetArrDevices\" function]\n");
		return NULL;
	case -203:
		printf("Error counting available Devices. Error when passing arguments to the function for counting the number of devices.\n  > [problem area: the \"ipGetArrDevices\" function]\n");
		return NULL;
	case -204:
		printf("Error counting available Devices. No OpenCL devices that matched device_type were found.\n  > [problem area: the \"ipGetArrDevices\" function]\n");
		return NULL;
	case -205:
		printf("Error counting available Devices. There is a failure to allocate resources required by the OpenCL implementation on the device.\n  > [problem area: the \"ipGetArrDevices\" function]\n");
		return NULL;
	case -206:
		printf("Error counting available Devices. There is a failure to allocate resources required by the OpenCL implementation on the host.\n  > [problem area: the \"ipGetArrDevices\" function]\n");
		return NULL;
	case -207:
		printf("Error counting available Devices. Unknown error when calculating the number of available devices to selected platform.\n  > [problem area: the \"ipGetArrDevices\" function]\n");
		return NULL;
	default:
		break;
	}
	res = new cl_device_id[subCount];
	status = clGetDeviceIDs(subPlatform, subDevType, subCount, res, NULL);
	switch (status) {
	case CL_SUCCESS:
		printf("Getting array of devices successful!\n");
		return res;
	case CL_INVALID_PLATFORM:
		printf("Error detecting available devices. The provided platform is not a valid platform.\n  > [problem area: the \"ipGetArrDevices\" function]\n");
		return NULL;
	case CL_INVALID_DEVICE_TYPE:
		printf("Error detecting available devices. The provided device type is not a valid value.\n  > [problem area: the \"ipGetArrDevices\" function]\n");
		return NULL;
	case CL_INVALID_VALUE:
		printf("Error detecting available devices. Error when passing arguments to the function for counting the number of devices.\n  > [problem area: the \"ipGetArrDevices\" function]\n");
		return NULL;
	case CL_DEVICE_NOT_FOUND:
		printf("Error detecting available devices. No OpenCL devices that matched device_type were found.\n  > [problem area: the \"ipGetArrDevices\" function]\n");
		return NULL;
	case CL_OUT_OF_RESOURCES:
		printf("Error detecting available devices. There is a failure to allocate resources required by the OpenCL implementation on the device.\n  > [problem area: the \"ipGetArrDevices\" function]\n");
		return NULL;
	case CL_OUT_OF_HOST_MEMORY:
		printf("Error detecting available devices. There is a failure to allocate resources required by the OpenCL implementation on the host.\n  > [problem area: the \"ipGetArrDevices\" function]\n");
		return NULL;
	default:
		printf("Error detecting available devices. Unknown error.\n  > [problem area: the \"ipGetArrDevices\" function]\n");
		return NULL;
	}
}

////////////////////////////////////////////////// Дальше пойдут функции, через которые клиент вызывает тесты ////////////////////////////////////////////////////////////////

void OpenCLTest_I1_Counting_Available_Platforms()
{
	// Тесты функции ipGetCountPlatforms
	printf("/// Testing of counting of available platforms ///\nAvailable platforms: %d\n", ipGetCountPlatforms());
	// Дальше тесты функции clGetPlatformIDs
	cl_uint res;
	/* ---1--- */
	// Успешное выполнение функции, передаём num_entries = 0 и cl_platform_id = NULL. По спецификации функция вернёт лишь запись с количеством посчитанных платформ.
	Checking_clGetPlatformIds(0, NULL, &res);
	/* ---2--- */
	// 1-й исключённый случай, передаём num_entries = 0 и cl_platform_id != NULL. По спецификации функция вернёт CL_INVALID_VALUE.
	cl_platform_id* p = new cl_platform_id[3];
	Checking_clGetPlatformIds(0, p, &res);
	delete[] p;
	/* ---3--- */
	// 2-й исключённый случай, передаём num_entries = NULL и cl_platform_id = NULL. По спецификации функция вернёт CL_INVALID_VALUE.
	Checking_clGetPlatformIds(0, NULL, NULL);
	printf("\n");
}

void OpenCLTest_I2_Getting_Available_Platforms_With_Recounting()
{
	// Тесты функции ipGetArrPlatforms
	printf("/// Testing ipGetArrPlatforms function ///\n");
	cl_platform_id* p1;
	/* ---1--- */
	p1 = Checking_ipGetArrPlatforms(-101);
	/* ---2--- */
	p1 = Checking_ipGetArrPlatforms(-102);
	/* ---3--- */
	p1 = Checking_ipGetArrPlatforms(-103);
	/* ---4--- */
	p1 = Checking_ipGetArrPlatforms(0);
	/* ---5--- */
	p1 = Checking_ipGetArrPlatforms(1);
	// Включить переподсчёт доступных платформ
	printf("//->Recounting on:\n");
	/* ---6--- */
	p1 = Checking_ipGetArrPlatforms(-101, true);
	/* ---7--- */
	p1 = Checking_ipGetArrPlatforms(-102, true);
	/* ---8--- */
	p1 = Checking_ipGetArrPlatforms(-103, true);
	/* ---9--- */
	p1 = Checking_ipGetArrPlatforms(0, true);
	/* ---10--- */
	p1 = Checking_ipGetArrPlatforms(1, true);
	delete[] p1;
	printf("\n");
}

void OpenCLTest_I3_Getting_Information_About_Available_Platforms()
{
	// Тесты функций ipGetPlatformByIndex и ipGetInfoAboutPlatform
	printf("/// Testing ipGetPlatformByIndex and ipGetInfoAboutPlatform functions ///\n");
	cl_platform_id* p = ipGetArrPlatforms(ipGetCountPlatforms());
	size_t size;
	// Тесты функции Checking_ipGetPlatformByIndex
	cl_platform_id platform = Checking_ipGetPlatformByIndex(p, ipGetCountPlatforms(), 0);
	// Тесты функции ipGetInfoAboutPlatform
	/* ---1--- */
	// Ожидаемый результата: успешное выполнение, вывод информации о профиле
	ipGetInfoAboutPlatform(platform, CL_PLATFORM_PROFILE, size, "Profile");
	/* ---2--- */
	// Ожидаемый результата: неуспешное выполнение, вывод сообщения об ошибке в определении платформы.
	ipGetInfoAboutPlatform(NULL, CL_PLATFORM_PROFILE, size, "Profile");
	/* ---3--- */
	// Ожидаемый результата: неуспешное выполнение, вывод сообщения об ошибке в определении типа выводимой информации.
	ipGetInfoAboutPlatform(platform, 0x9999, size, "Profile");
	/* ---4--- */
	// Ожидаемый результата: успешное выполнение, вывод информации о версии
	ipGetInfoAboutPlatform(platform, CL_PLATFORM_VERSION, size, "Version");
	/* ---5--- */
	// Ожидаемый результата: успешное выполнение, вывод информации об имени
	ipGetInfoAboutPlatform(platform, CL_PLATFORM_NAME, size, "Name");
	/* ---6--- */
	// Ожидаемый результата: успешное выполнение, вывод информации о производителе
	ipGetInfoAboutPlatform(platform, CL_PLATFORM_VENDOR, size, "Vendor");
	/* ---7--- */
	// Ожидаемый результата: успешное выполнение, вывод информации о расширениях
	ipGetInfoAboutPlatform(platform, CL_PLATFORM_EXTENSIONS, size, "Extensions");
	delete[] p;
	printf("\n");
}

void OpenCLTest_I4_Getting_Informations_About_All_Available_Platforms()
{
	printf("/// Testing ipGetInfoAboutAvailablePlatforms function ///\n");
	// Тест 1-й. Простой вызов функции из библиотеки. Ожидаемый результат: успешное выполнение библиотечной функции и вывод на экран полученной информации.
	ipGetInfoAboutAvailablePlatforms();
	// Тест 2-й. Простой вызов переписанной функции тестирования. Ожидаемый результат: успешное выполнение тестировочной функции и вывод на экран полученной информации.
	Checking_ipGetInfoAboutAvailablePlatforms();
	// Тест 3-й. Вызов переписанной функции тестирования с параметром, отвечающим за возвращаемое значение расчётной функции доступных платформ, равным -101. Ожидаемый результат: успешное выполнение 
	// тестировочной функции и вывод на экран сообщения об ошибке.
	Checking_ipGetInfoAboutAvailablePlatforms(-101);
	// Тест 4-й. Вызов переписанной функции тестирования с параметром, отвечающим за возвращаемое значение расчётной функции доступных платформ, равным -102. Ожидаемый результат: успешное выполнение 
	// тестировочной функции и вывод на экран сообщения об ошибке.
	Checking_ipGetInfoAboutAvailablePlatforms(-102);
	// Тест 5-й. Вызов переписанной функции тестирования с параметром, отвечающим за возвращаемое значение расчётной функции доступных платформ, равным -103. Ожидаемый результат: успешное выполнение 
	// тестировочной функции и вывод на экран сообщения об ошибке.
	Checking_ipGetInfoAboutAvailablePlatforms(-103);
}

void OpenCLTest_I5_Getting_Count_Of_Devices_To_Selected_Platform()
{
	printf("\n/// Testing ipGetInfoAboutAvailablePlatforms function ///\n");
	cl_uint count_platforms = ipGetCountPlatforms();
	cl_platform_id* platforms = ipGetArrPlatforms(count_platforms);
	cl_platform_id pm = platforms[0];
	// Тест 1-й. Простой вызов функции из библиотеки для подсчёта количества доступных устройств типа GPU. Ожидаемый результат: успешное выполнение библиотечной функции и вывод на экран посчитанного 
	// количества доступных для выбранной платформы устройств выбранного типа.
	printf("Number of available GPU type devices: %d\n", ipGetCountDevices(pm, CL_DEVICE_TYPE_GPU));
	// Тест 2-й. Простой вызов функции из библиотеки для подсчёта количества доступных устройств типа CPU. Ожидаемый результат: успешное выполнение библиотечной функции и вывод на экран посчитанного 
	// количества доступных для выбранной платформы устройств выбранного типа.
	printf("Number of available CPU type devices: %d\n", ipGetCountDevices(pm, CL_DEVICE_TYPE_CPU));
	// Тест 3-й. Простой вызов функции из библиотеки для подсчёта количества всех доступных устройств. Ожидаемый результат: успешное выполнение библиотечной функции и вывод на экран посчитанного 
	// количества всех доступных для выбранной платформы устройств.
	printf("Number of available devices: %d\n", ipGetCountDevices(pm, CL_DEVICE_TYPE_ALL));
	// Тест 4-й. Передаём в качестве второго аргумента функции нулевой указатель (NULL). Ожидаемый результат: успешное выполнение библиотечной функции и вывод на экран 
	// посчитанного  количества всех доступных для выбранной платформы устройств.
	printf("Number of available devices: %d\n", ipGetCountDevices(pm, NULL));
	// Тест 5-й. Передаём в качестве первого аргумента функции нулевой указатель (NULL). Ожидаемый результат: успешное выполнение библиотечной функции с заходом на перерасчёт
	// доступных платформ и вывод на экран посчитанного  количества доступных для переопределённой платформы устройств типа GPU.
	printf("Number of available GPU type devices: %d\n", ipGetCountDevices(NULL, CL_DEVICE_TYPE_GPU));
	// Тест 6-й. Передаём в качестве первого аргумента функции нулевой указатель (NULL). Ожидаемый результат: успешное выполнение библиотечной функции с заходом на перерасчёт
	// доступных платформ и вывод на экран посчитанного  количества доступных для переопределённой платформы устройств типа CPU.
	printf("Number of available CPU type devices: %d\n", ipGetCountDevices(NULL, CL_DEVICE_TYPE_CPU));
	// Тест 7-й. Передаём в качестве первого аргумента функции нулевой указатель (NULL). Ожидаемый результат: успешное выполнение библиотечной функции с заходом на перерасчёт
	// доступных платформ и вывод на экран посчитанного  количества всех доступных для переопределённой платформы устройств.
	printf("Number of available devices: %d\n", ipGetCountDevices(NULL, CL_DEVICE_TYPE_ALL));
	// Тест 8-й. Передаём в качестве аргументов функции нулевых указателей (NULL). Ожидаемый результат: успешное выполнение библиотечной функции с заходом на перерасчёт
	// доступных платформ и вывод на экран посчитанного  количества всех доступных для переопределённой платформы устройств.
	printf("Number of available devices: %d\n", ipGetCountDevices(NULL, NULL));
	delete[] platforms;
}

void OpenCLTest_I6_Getting_Array_Of_Devices_To_Selected_Platform()
{
	printf("\n/// Testing ipGetArrDevices function ///\n");
	cl_platform_id* platforms = ipGetArrPlatforms(ipGetCountPlatforms());
	cl_platform_id pm = platforms[0];
	cl_device_id* devices;
	// Тест 1-й. Простой вызов функции из библиотеки для получения массива доступных устройств типа GPU для выбранной платформы. Ожидаемый результат: успешное выполнение библиотечной функции. На экран 
	// ничего выводиться не должно
	devices = ipGetArrDevices(pm, CL_DEVICE_TYPE_GPU, ipGetCountDevices(pm, CL_DEVICE_TYPE_GPU));
	// Тест 2-й. Простой вызов функции из библиотеки для получения массива доступных устройств всех типов для выбранной платформы. Ожидаемый результат: успешное выполнение библиотечной функции. На экран 
	// ничего выводиться не должно
	devices = ipGetArrDevices(pm, CL_DEVICE_TYPE_ALL, ipGetCountDevices(pm, CL_DEVICE_TYPE_ALL));
	// Тест 3-й. Простой вызов функции тестирования Checking_ipGetArrDevices для получения массива доступных устройств типа GPU для выбранной платформы. Ожидаемый результат: успешное выполнение тестировочной
	// функции. На экран должно вывестись сообщение об успешном выполнении функции
	devices = Checking_ipGetArrDevices(pm, CL_DEVICE_TYPE_GPU, ipGetCountDevices(pm, CL_DEVICE_TYPE_GPU));
	// Тест 4-й. Простой вызов функции тестирования Checking_ipGetArrDevices для получения массива доступных устройств типа CPU для выбранной платформы. Ожидаемый результат: успешное выполнение тестировочной
	// функции. На экран должно вывестись 2 сообщения об ошибке.
	devices = Checking_ipGetArrDevices(pm, CL_DEVICE_TYPE_CPU, ipGetCountDevices(pm, CL_DEVICE_TYPE_CPU));
	// Тест 5-й. Простой вызов функции тестирования Checking_ipGetArrDevices для получения массива доступных устройств всех типов для выбранной платформы. Ожидаемый результат: успешное выполнение 
	// тестировочной функции. На экран должно вывестись сообщение об успешном выполнении функции
	devices = Checking_ipGetArrDevices(pm, CL_DEVICE_TYPE_ALL, ipGetCountDevices(pm, CL_DEVICE_TYPE_ALL));
	// Тест 6-й. Вызов функции тестирования Checking_ipGetArrDevices с параметром count < 0. Ожидаемый результат: успешное выполнение тестировочной функции. На экран должно вывестись сообщение об успешном 
	// выполнении функции
	devices = Checking_ipGetArrDevices(pm, CL_DEVICE_TYPE_GPU, -10);
	// Тест 7-й. Вызов функции тестирования Checking_ipGetArrDevices с параметром count = 0. Ожидаемый результат: успешное выполнение тестировочной функции. На экран должно вывестись сообщение об успешном 
	// выполнении функции
	devices = Checking_ipGetArrDevices(pm, CL_DEVICE_TYPE_GPU, 0);
	// Тест 8-й. Вызов функции тестирования Checking_ipGetArrDevices с параметром count = -201. Ожидаемый результат: ошибка с прерыванием. На экран должно вывестись сообщение с описанием ошибки
	devices = Checking_ipGetArrDevices(pm, CL_DEVICE_TYPE_GPU, -201, false);
	// Тест 9-й. Вызов функции тестирования Checking_ipGetArrDevices с параметром count = -202. Ожидаемый результат: ошибка с прерыванием. На экран должно вывестись сообщение с описанием ошибки
	devices = Checking_ipGetArrDevices(pm, CL_DEVICE_TYPE_GPU, -202, false);
	// Тест 10-й. Вызов функции тестирования Checking_ipGetArrDevices с параметром count = -203. Ожидаемый результат: ошибка с прерыванием. На экран должно вывестись сообщение с описанием ошибки
	devices = Checking_ipGetArrDevices(pm, CL_DEVICE_TYPE_GPU, -203, false);
	// Тест 11-й. Вызов функции тестирования Checking_ipGetArrDevices с параметром count = -204. Ожидаемый результат: ошибка с прерыванием. На экран должно вывестись сообщение с описанием ошибки
	devices = Checking_ipGetArrDevices(pm, CL_DEVICE_TYPE_GPU, -204, false);
	// Тест 12-й. Вызов функции тестирования Checking_ipGetArrDevices с параметром count = -205. Ожидаемый результат: ошибка с прерыванием. На экран должно вывестись сообщение с описанием ошибки
	devices = Checking_ipGetArrDevices(pm, CL_DEVICE_TYPE_GPU, -205, false);
	// Тест 13-й. Вызов функции тестирования Checking_ipGetArrDevices с параметром count = -206. Ожидаемый результат: ошибка с прерыванием. На экран должно вывестись сообщение с описанием ошибки
	devices = Checking_ipGetArrDevices(pm, CL_DEVICE_TYPE_GPU, -206, false);
	// Тест 14-й. Вызов функции тестирования Checking_ipGetArrDevices с параметром count = -207. Ожидаемый результат: ошибка с прерыванием. На экран должно вывестись сообщение с описанием ошибки
	devices = Checking_ipGetArrDevices(pm, CL_DEVICE_TYPE_GPU, -207, false);
	// Тест 15-й. Отключим проверку на передаваемую платформу/число устройств/тип устройств. Ожидаемый результат: ошибка с прерыванием. На экран должно вывестись сообщение с описанием ошибки
	devices = Checking_ipGetArrDevices(NULL, CL_DEVICE_TYPE_GPU, ipGetCountDevices(pm, CL_DEVICE_TYPE_GPU), false);
	// Тест 16-й. Отключим проверку на передаваемую платформу/число устройств/тип устройств. Ожидаемый результат: ошибка с прерыванием. На экран должно вывестись сообщение с описанием ошибки
	devices = Checking_ipGetArrDevices(pm, NULL, 1, false);
	delete[] devices;
	delete[] platforms;
}