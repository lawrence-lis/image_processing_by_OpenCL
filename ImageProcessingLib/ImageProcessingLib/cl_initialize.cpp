#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include "cl_initialize.h"

/*   Работа с платформами   */

/*   Функция, возвращаяющая количество доступных платформ   */
cl_uint cl_init_get_num_platforms()
{
	cl_uint res;
	switch (clGetPlatformIDs(0, NULL, &res))
	{
	case CL_SUCCESS:
		// Подсчёт доступных платформ прошел успешно, вернуть посчитанное значение
		return res;
	case CL_OUT_OF_HOST_MEMORY:
		// Не удалось выделить на хосте ресурсы, необходимые для реализации OpenCL. Вывести соответствующее сообщение и вернуть 0.
		printf("The resources needed to implement OpenCL could not be allocated on the host.\n\t[Problem area: the \"cl_init_get_num_platforms\" function.]\n\n");
		return 0;
	default:
		// Неизвестная ошибка. Вывести на экран соотвествующее сообщение и вернуть 0
		printf("Unknown error when calculating the number of available platforms.\n\t[Problem area: the \"cl_init_get_num_platforms\" function.]\n\n");
		return 0;
	}
}

/*   Функция, возвращающая массив доступных платформ   */
cl_platform_id* cl_init_get_array_platforms(cl_uint count)
{
	if (count == 0) {
		// Если клиент намеренно передал 0, то надо провести переподсчёт доступных платформ
		count = cl_init_get_num_platforms();
		if (count == 0) {
			/* То есть, если клиент передал 0, или если он ничего не передал, и функция cl_init_get_num_platforms() всё равно вернула 0, то значит, что что-то случилось и дальше функция выполняться не будет.
			Вывести соответствующее сообщение об ошибке и прервать выполнение функции, вернув указатель на NULL.*/
			printf("No OpenCL platforms were found.\n\t[Problem area: the \"cl_init_get_array_platforms\" function.]\n\n");
			return NULL;
		}
	}
	// Выделяем память для хранения идентификаторов платформ
	cl_platform_id* res = (cl_platform_id *)malloc(sizeof(cl_platform_id) * count);
	if (res == NULL) {
		printf("Memory allocation error for platform IDs\n\t[Problem area: the \"cl_init_get_array_platforms\" function.]\n\n");
		return res;
	}
	// Получаем идентификаторы платформ
	switch (clGetPlatformIDs(count, res, 0))
	{
	case CL_SUCCESS:
		// Получение доступных платформ прошел успешно, вернуть посчитанное значение
		return res; 
	case CL_INVALID_VALUE:
		/* Согласно спецификации и текущему коду млжет вылетить если каким-то волшебным образом count оказалось равныой 0 и при этом res указывает на NULL. 
		(Как при этом выполнение дошло аж до этой точки, не ясно, но на всякий случай введу). Вывести соответствующее сообщение об ошибке, прервать выполнение функции и вернуть указатель на NULL.*/
		printf("Error when getting platform IDs:\n  > Error in the execution of the clGetPlatformIDs function.\n\t[problem area: the \"cl_init_get_array_platforms\" function]\n\n");
		return NULL;
	case CL_OUT_OF_HOST_MEMORY:
		// Не удалось выделить на хосте ресурсы, необходимые для реализации OpenCL. Вывести соответствующее сообщение и вернуть указатель на NULL.
		printf("Error when getting platform IDs:\n  > Error allocating resources needed to implement OpenCL on the host.\n\tProblem area: the \"cl_init_get_array_platforms\" function.\n\n");
		return NULL;
	default:
		// Неизвестная ошибка. Вывести на экран соотвествующее сообщение и вернуть 0
		printf("Error when getting platform IDs:\n  > Unknown error.\n\tProblem area: the \"cl_init_get_array_platforms\" function.\n\n");
		return NULL;
	}
}

// Функция, отображающая информацию о переданной платформе
void cl_init_platform_get_info(cl_platform_id platform, cl_platform_info info_type, const char* title)
{
	size_t p_s;
	// Сперва посчитаем количество символов, необходимых для записи нужной информации
	switch (clGetPlatformInfo(platform, info_type, 0, NULL, &p_s))
	{
	case CL_SUCCESS:
		// Функция clGetPlatformInfo выполнена успешно, количество памяти, необходимой для нужной информации, посчитано. Нужно просто вернуться к выполнению функции.
		break;
	case CL_INVALID_PLATFORM:
		// Выподает если переданная платформа не является доступной платформой. Вывести соответствующее сообщение и вернуть управление.
		printf("The transferred platform is not a valid platform\n\tProblem area: the \"cl_init_platform_get_info\" function\n\n");
		return;
	case CL_INVALID_VALUE:
		// По идее на данном может выпасть если переданный тип получаемой информации не является одним из поддерживаемых значений. Вывести соответствующее сообщение и вернуть управление.
		printf("The transmitted type of information received is not one of the supported values\n\tProblem area: the \"cl_init_platform_get_info\" function\n\n");
		return;
	case CL_OUT_OF_HOST_MEMORY:
		// Выподает если происходит сбой в распределении ресурсов, требуемых реализацией OpenCL на хосте. Вывести соответствующее сообщение и вернуть управление.
		printf("There was a failure in the allocation of resources required by the OpenCL implementation on the host\n\tProblem area: the \"cl_init_platform_get_info\" function\n\n");
		return;
	default:
		// Прроизошло что-то непонятное, но подсчёт выделяемой информации не завершился успешно. Вывести соответствующее сообщение и вернуть управление.
		printf("Unknown error\n\tProblem area: the \"cl_init_platform_get_info\" function\n\n");
		return;
	}
	// Выделяем память для хранения запрашиваемой информации
	char* str = (char*)malloc(sizeof(char) * p_s);
	if (str == NULL) {
		printf("Memory allocation error for platform IDs\n\tProblem area: the \"cl_init_get_array_platforms\" function.\n\n");
		return;
	}
	// Получаем и выводим запрашиваемую информацию
	switch (clGetPlatformInfo(platform, info_type, p_s, str, NULL))
	{
	case CL_SUCCESS:
		// Функция clGetPlatformInfo выполнена успешно, информация считана в переменную. Нужно вывести на экран считанную информацию и вернуться к выполнению функции (там осталось только освобождение 
		// память, выделенной под строку с информацией).
		printf("\t%s: %s\n", title, str);
		break;
	case CL_INVALID_PLATFORM:
		// Выподает если переданная платформа (первый параметр функции) не является доступной платформой. Вывести соответствующее сообщение и вернуть управление.
		printf("The transferred platform is not a valid platform\n\tProblem area: the \"cl_init_platform_get_info\" function\n\n");
		break;
	case CL_INVALID_VALUE:
		// Выпадает если переданный тип информации, указанный в параметре info_type, не является одним из поддерживаемых значений или если размер в байтах, указанный в параметре p_s, меньше размера
		// возвращаемого типа, указанного в таблице запросов платформы, а значение указателя на ячейку памяти, переаднный в параметре str не равно NULL. Вывести соответствующее сообщение и вернуть 
		// управление.
		printf("The transmitted type of information specified in the info_type parameter is not one of the supported values, or if the size in bytes specified in the p_s parameter is less than the ");
		printf("size of the returned type specified in the platform query table, and the value of the pointer to the memory cell redirected in the str parameter is not NULL");
		printf("\n\tProblem area : the \"cl_init_platform_get_info\" function\n\n");
		break;
	case CL_OUT_OF_HOST_MEMORY:
		// Выподает если происходит сбой в распределении ресурсов, требуемых реализацией OpenCL на хосте (так написано в спецификации). Вывести соответствующее сообщение и вернуть управление.
		printf("There was a failure in the allocation of resources required by the OpenCL implementation on the host\n\tProblem area: the \"cl_init_platform_get_info\" function\n\n");
		break;
	default:
		// Прроизошло что-то непонятное, но подсчёт выделяемой информации не завершился успешно. Вывести соответствующее сообщение и вернуть управление.
		printf("Unknown error\n\tProblem area: the \"cl_init_platform_get_info\" function\n\n");
		break;
	}
	free(str);
}

/*   Работа с устройствами   */

// Функция, возвращаяющая количество доступных вычислительных устройств для переданной платформы
cl_uint cl_init_get_num_devices(cl_platform_id platform, cl_device_type type)
{
	cl_uint res;
	switch (clGetDeviceIDs(platform, type, 0, NULL, &res))
	{
	case CL_SUCCESS:
		// Подсчёт доступных платформ прошел успешно, вернуть посчитанное значение
		return res;
	case CL_INVALID_PLATFORM:
		// Переданная платформа не является допустимой платформой. Вывести соответствующее сообщение и вернуть 0.
		printf("The transferred platform is not a valid platform.\n\t[Problem area: the \"cl_init_get_num_devices\" function.]\n\n");
		return 0;
	case CL_INVALID_DEVICE_TYPE:
		// Переданная тип вычислительных устройств не является допустимым. Вывести соответствующее сообщение и вернуть 0.
		printf("The transmitted type of devices is not valid value.\n\t[Problem area: the \"cl_init_get_num_devices\" function.]\n\n");
		return 0;
	case CL_DEVICE_NOT_FOUND:
		// Не было найдено ни одного устройства OpenCL, соответствующего переданному типу. Вывести соответствующее сообщение и вернуть 0.
		printf("No OpenCL device matching the transmitted type was found.\n\t[Problem area: the \"cl_init_get_num_devices\" function.]\n\n");
		return 0;
	case CL_OUT_OF_RESOURCES:
		// Не удалось выделить ресурсы, необходимые для реализации OpenCL на устройстве. Вывести соответствующее сообщение и вернуть 0.
		printf("The resources needed to implement OpenCL on the device could not be allocated.\n\t[Problem area: the \"cl_init_get_num_devices\" function.]\n\n");
		return 0;
	case CL_OUT_OF_HOST_MEMORY:
		// Не удалось выделить на хосте ресурсы, необходимые для реализации OpenCL. Вывести соответствующее сообщение и вернуть 0.
		printf("The resources needed to implement OpenCL could not be allocated on the host.\n\t[Problem area: the \"cl_init_get_num_devices\" function.]\n\n");
		return 0;
	default:
		// Неизвестная ошибка. Вывести на экран соотвествующее сообщение и вернуть 0
		printf("Unknown error when calculating the number of available devices.\n\t[Problem area: the \"cl_init_get_num_devices\" function.]\n\n");
		return 0;
	}
}

// Функция, возвращаяющая массив доступных вычислительных устройств для переданной платформы
cl_device_id* cl_init_get_array_devices(cl_platform_id platform, cl_device_type type, cl_uint count)
{
	if (count == 0) {
		// Если клиент намеренно передал 0, то надо провести переподсчёт доступных платформ
		count = cl_init_get_num_devices(platform, type);
		if (count == 0) {
			/* То есть, если клиент передал 0, или если он ничего не передал, и функция cl_init_get_num_devices() всё равно вернула 0, то значит, что что-то случилось и дальше функция выполняться не будет.
			Вывести соответствующее сообщение об ошибке и прервать выполнение функции, вернув указатель на NULL.*/
			printf("No available devices were found.\n\t[Problem area: the \"cl_init_get_array_devices\" function.]\n\n");
			return NULL;
		}
	}
	// Выделяем память для хранения идентификаторов устройств
	cl_device_id* res = (cl_device_id*)malloc(sizeof(cl_device_id) * count);
	if (res == NULL) {
		printf("Memory allocation error for device IDs\n\t[Problem area: the \"cl_init_get_array_devices\" function.]\n\n");
		return res;
	}
	// Получаем идентификаторы платформ
	switch (clGetDeviceIDs(platform, type, count, res, NULL))
	{
	case CL_SUCCESS:
		// Подсчёт доступных платформ прошел успешно, вернуть посчитанное значение
		return res;
	case CL_INVALID_PLATFORM:
		// Переданная платформа не является допустимой платформой. Вывести соответствующее сообщение и вернуть 0.
		printf("The transferred platform is not a valid platform.\n\t[Problem area: the \"cl_init_get_array_devices\" function.]\n\n");
		return NULL;
	case CL_INVALID_DEVICE_TYPE:
		// Переданная тип вычислительных устройств не является допустимым. Вывести соответствующее сообщение и вернуть 0.
		printf("The transmitted type of devices is not valid value.\n\t[Problem area: the \"cl_init_get_array_devices\" function.]\n\n");
		return NULL;
	case CL_INVALID_VALUE:
		// Присутствует несоответствие между переданными данными и их количеством. Вывести соответствующее сообщение и вернуть 0.
		printf("There is a discrepancy between the transmitted data and their quantity.\n\t[Problem area: the \"cl_init_get_array_devices\" function.]\n\n");
		return NULL;
	case CL_DEVICE_NOT_FOUND:
		// Не было найдено ни одного устройства OpenCL, соответствующего переданному типу. Вывести соответствующее сообщение и вернуть 0.
		printf("No OpenCL device matching the transmitted type was found.\n\t[Problem area: the \"cl_init_get_array_devices\" function.]\n\n");
		return NULL;
	case CL_OUT_OF_RESOURCES:
		// Не удалось выделить ресурсы, необходимые для реализации OpenCL на устройстве. Вывести соответствующее сообщение и вернуть 0.
		printf("The resources needed to implement OpenCL on the device could not be allocated.\n\t[Problem area: the \"cl_init_get_array_devices\" function.]\n\n");
		return NULL;
	case CL_OUT_OF_HOST_MEMORY:
		// Не удалось выделить на хосте ресурсы, необходимые для реализации OpenCL. Вывести соответствующее сообщение и вернуть 0.
		printf("The resources needed to implement OpenCL could not be allocated on the host.\n\t[Problem area: the \"cl_init_get_array_devices\" function.]\n\n");
		return NULL;
	default:
		// Неизвестная ошибка. Вывести на экран соотвествующее сообщение и вернуть 0
		printf("Unknown error when calculating the number of available devices.\n\t[Problem area: the \"cl_init_get_array_devices\" function.]\n\n");
		return NULL;
	}
}