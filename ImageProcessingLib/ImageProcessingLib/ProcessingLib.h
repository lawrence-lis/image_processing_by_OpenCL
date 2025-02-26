#pragma once

#define PROCESSINGLIBRARY_EXPORTS

#ifdef PROCESSINGLIBRARY_EXPORTS
#define PROCESSINGLIBRARY_API __declspec(dllexport)
#else
#define PROCESSINGLIBRARY_API __declspec(dllimport)
#endif

#include "CL/cl.h"
#include "cl_initialize.h"
#include "cl_runtime.h"

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
extern "C" PROCESSINGLIBRARY_API void cl_display_arr_devices_info_default(cl_device_id * devices, cl_uint devices_count);

/* Функция, отображающая всю доступную информацию о выбранных устройствах
	Параметры:
		* devices - указатель на массив идентификаторов устройств, информацию о которых надо отобразить
		* devices_count - количество элементов в переданном массиве devices*/
extern "C" PROCESSINGLIBRARY_API void cl_display_arr_devices_info_all(cl_device_id * devices, cl_uint devices_count);


///////////////////////////////////////////// Контекст /////////////////////////////////////////////

/* Функция, отображающая минимальную информацию (количество и массив устройств) о выбранном контексте
	Параметры:
		* context - контекст, информацию о которой надо отобразить*/
extern "C" PROCESSINGLIBRARY_API void cl_display_context_info_minimum(cl_context context);

/* Функция, отображающая минимальную информацию о выбранных контекстах
	Параметры:
		* contexts - указатель на массив контекстов, информацию о которых надо отобразить
		* contexts_count - количество элементов в переданном массиве contexts*/
extern "C" PROCESSINGLIBRARY_API void cl_display_arr_contexts_info_minimum(cl_context * contexts, cl_uint contexts_count);