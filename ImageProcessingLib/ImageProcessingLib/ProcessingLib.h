#pragma once

#define PROCESSINGLIBRARY_EXPORTS

#ifdef PROCESSINGLIBRARY_EXPORTS
#define PROCESSINGLIBRARY_API __declspec(dllexport)
#else
#define PROCESSINGLIBRARY_API __declspec(dllimport)
#endif

#include "CL/cl.h"
#include "cl_initialize.h"

////////////////////////////////////////////////////////////////////////////
/////// Блок функций, отвечающих за настройку рабочего пространства ////////
////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////// Платформы /////////////////////////////////////////////

/* Функция, отображающая минимальную информацию (имя и версию) о переданной платформе
	Параметры:
		* platform - идентификатор платформы, информацию о которой надо отобразить*/
extern "C" PROCESSINGLIBRARY_API void cl_display_platform_info_minimum(cl_platform_id platform);

/* Функция, отображающая информацию по-умолчанию (имя, версию и профиль) о переданной платформе
	Параметры:
		* platform - идентификатор платформы, информацию о которой надо отобразить*/
extern "C" PROCESSINGLIBRARY_API void cl_display_platform_info_default(cl_platform_id platform);

/* Функция, отображающая всю доступную информацию (имя, версию, производителя, профиль и расширения) о переданной платформе
	Параметры:
		* platform - идентификатор платформы, информацию о которой надо отобразить*/
extern "C" PROCESSINGLIBRARY_API void cl_display_platform_info_all(cl_platform_id platform);

/* Функция, отображающая минимальную информацию (имя и версию) о переданных платформах
	Параметры:
		* platforms - указатель на массив идентификаторов платформ, информацию о которых надо отобразить
		* platforms_count - количество элементов в переданном массиве platforms*/
extern "C" PROCESSINGLIBRARY_API void cl_display_arr_platforms_info_minimum(cl_platform_id* platforms, cl_uint platforms_count = cl_init_get_num_platforms());

/* Функция, отображающая информацию по-умолчанию (имя, версию и профиль) о переданных платформах
	Параметры:
		* platforms - указатель на массив идентификаторов платформ, информацию о которых надо отобразить
		* platforms_count - количество элементов в переданном массиве platforms*/
extern "C" PROCESSINGLIBRARY_API void cl_display_arr_platforms_info_default(cl_platform_id * platforms, cl_uint platforms_count = cl_init_get_num_platforms());

/* Функция, отображающая всю доступную информацию (имя, версию, производителя, профиль и расширения) о переданных платформах
	Параметры:
		* platforms - указатель на массив идентификаторов платформ, информацию о которых надо отобразить
		* platforms_count - количество элементов в переданном массиве platforms*/
extern "C" PROCESSINGLIBRARY_API void cl_display_arr_platforms_info_all(cl_platform_id * platforms, cl_uint platforms_count = cl_init_get_num_platforms());

///////////////////////////////////////////// Устройства /////////////////////////////////////////////

/* Функция, отображающая минимальную информацию (тип, имя, поставщик, версия драйвера и версия OpenCL устройства) о выбранном устройстве
	Параметры:
		* device - идентификатор устройства, информацию о которой надо отобразить*/
extern "C" PROCESSINGLIBRARY_API void cl_display_device_info_minimum(cl_device_id device);

/* Функция, отображающая информацию по-умолчанию (тип, имя, поставщик, версия драйвера, версия OpenCL, профиль, максимальное количество вычислительных блоков и расширения устройства) о выбранном устройстве
	Параметры:
		* device - идентификатор устройства, информацию о которой надо отобразить*/
extern "C" PROCESSINGLIBRARY_API void cl_display_device_info_default(cl_device_id device);

/* Функция, отображающая всю доступную информацию о выбранном устройстве
	Параметры:
		* device - идентификатор устройства, информацию о которой надо отобразить*/
extern "C" PROCESSINGLIBRARY_API void cl_display_device_info_all(cl_device_id device);

/* Функция, отображающая минимальную информацию о выбранных устройствах
	Параметры:
		* devices - указатель на массив идентификаторов устройств, информацию о которых надо отобразить
		* devices_count - количество элементов в переданном массиве devices*/
extern "C" PROCESSINGLIBRARY_API void cl_display_arr_devices_info_minimum(cl_device_id * devices, cl_uint devices_count);

/* Функция, отображающая информацию по-умолчанию о выбранных устройствах
	Параметры:
		* devices - указатель на массив идентификаторов устройств, информацию о которых надо отобразить
		* devices_count - количество элементов в переданном массиве devices*/
extern "C" PROCESSINGLIBRARY_API void cl_display_arr_devices_info_default(cl_device_id * devices, cl_uint platforms_count);

/* Функция, отображающая всю доступную информацию о выбранных устройствах
	Параметры:
		* devices - указатель на массив идентификаторов устройств, информацию о которых надо отобразить
		* devices_count - количество элементов в переданном массиве devices*/
extern "C" PROCESSINGLIBRARY_API void cl_display_arr_devices_info_all(cl_device_id * devices, cl_uint platforms_count);

///////////////////////////////////////////// Контекст /////////////////////////////////////////////

/* Функция, создающая контекст для переданного массива устройств
	Параметры:
	* devs - массив устройств, для которого создаётся контекст
	* count - количество устройств, для которых создаётся контекст*/
extern "C" PROCESSINGLIBRARY_API cl_context ipGetContextByArr(cl_device_id * devs, cl_uint count);

/* Функция, создающая контекст для всех устройств заданного типа
	Параметры:
	* type - тип устройств, для которых создаётся контекст*/
extern "C" PROCESSINGLIBRARY_API cl_context ipGetContextByType(cl_device_type type);

///////////////////////////////////////////// Очередь команд /////////////////////////////////////////////

/* Функция, создающая очередь команд для созданного контекста и выбранного устройства
	Параметры:
	* ctx - контекст, в рамках которого создаётся очередь команд
	* dev - устройство, для которого создаётся очередь команд*/
extern "C" PROCESSINGLIBRARY_API cl_command_queue ipGetComQueueToDevice(cl_context ctx, cl_device_id dev);

///////////////////////////////////////////// Работа с кодом ядер /////////////////////////////////////////////

// Функции, выполняющие все необходимые операции для работы с ядрами используя исходный файл (расширением .cl) / строку

/* Функция, загружающая код ядра в память используя исходный файл (расширением .cl) / строку
	Параметры:
	  * ctx - контекст
	  * filename - адрес читаемого файла или строка, которую надо перевести*/
extern "C" PROCESSINGLIBRARY_API cl_program ipGetProgram(cl_context ctx, const char filename[]);

/* Функция, компилирующая и линкующая исполняемого на устройствах кода
	Параметры:
	  * prg - код программы, исполняемый на устройствах (получен функцией ipGetProgram)
	  * devs - массив устройств, на которых будет выполняться итоговый код*/
extern "C" PROCESSINGLIBRARY_API cl_int ipBuildProgram(cl_program prg, cl_device_id * devs);

/* Функция для создания объекта ядра, готового для последующего запуска на рабочих единицах соответствующего устройства
	Параметры:
	  * prg - программный объект с успешно построенным исполняемым кодом ядра
	  * name - имя функции со спецификатором __kernel*/
extern "C" PROCESSINGLIBRARY_API cl_kernel ipGetKernel(cl_program prg, const char* name);

/* Функция для отправки ядру дополнительной информации в виде параметров кода ядра
	Параметры:
	  * krn - объект ядра, которому передаётся информация с целью последующего запуска
	  * param_idx - индекс параметра, отсчитывается с нуля, начиная с самого левого параметра функции ядра
	  * param_size - размер передаваемых данных в байтах
	  * param - указатель на данные, которые будут использованы для установки значения параметра*/
extern "C" PROCESSINGLIBRARY_API cl_int ipSetKernelParam(cl_kernel krn, cl_uint param_idx, size_t param_size, const void* param);

/* Функция для работы с ядром, а именно: запуск оного
	Параметры:
	  * krn - объект ядра, который предстоит запустить
	  * que - очередь команд
	  * dim - количество измерений рабочего размера
	  * qws - рабочий размер*/
extern "C" PROCESSINGLIBRARY_API cl_int ipWorkKernel(cl_kernel krn, cl_command_queue que, cl_uint dim, const size_t * gws);