#pragma once
/*Этот файл содержит набор функций для инициализации OpenCL, которая включает в себя следующие основные разделы:
	- выбор платформы;
	- выбор устройств(а);
	- создание контекста;
	- создание очереди команд.
*/

#define PROCESSINGLIBRARY_EXPORTS

#ifdef PROCESSINGLIBRARY_EXPORTS
#define PROCESSINGLIBRARY_API __declspec(dllexport)
#else
#define PROCESSINGLIBRARY_API __declspec(dllimport)
#endif

#include "CL/cl.h"

/*   Работа с платформами   */

// Функция, возвращаяющая количество доступных платформ
extern "C" PROCESSINGLIBRARY_API cl_uint cl_init_get_num_platforms();

/* Функция, возвращающая массив доступных платформ
	Параметры:
	  * count - количество доступных платформ (предполагается, что данное число предопределено до вызова данной функции, в противном случае функция посчитает её самостоятельно)*/
extern "C" PROCESSINGLIBRARY_API cl_platform_id * cl_init_get_array_platforms(cl_uint count = cl_init_get_num_platforms());

/* Функция, отображающая информацию о переданной платформе
	Параметры:
	 * platform - идентификатор платформы, информацию о которой надо отобразить
	 * info_type - тип отображаемой информации
	 * p_s - адрес переменной размера отображаемой информации
	 * title - строка, поясняющая, что это за информация, которую выводит данна функция*/
extern "C" PROCESSINGLIBRARY_API void cl_init_platform_get_info(cl_platform_id platform, cl_platform_info info_type, const char* title);


/*   Работа с устройствами   */

/* Функция, возвращаяющая количество доступных вычислительных устройств для переданной платформы
	Параметры:
	 * platform - идентификатор платформы, количество доступных вычислительных устройств которой надо посчитать
	 * type - тип доступных вычислительных устройств, количество которых надо посчитать*/
extern "C" PROCESSINGLIBRARY_API cl_uint cl_init_get_num_devices(cl_platform_id platform, cl_device_type type = CL_DEVICE_TYPE_ALL);

/* Функция, возвращаяющая массив доступных вычислительных устройств для переданной платформы
	Параметры:
	 * platform - идентификатор платформы, массив доступных вычислительных устройств которой надо вернуть
	 * type - тип доступных вычислительных устройств, массив которых надо вернуть
	 * count - количество доступных устройств (предполагается, что данное число предопределено до вызова данной функции, в противном случае функция посчитает её самостоятельно)*/
extern "C" PROCESSINGLIBRARY_API cl_device_id * cl_init_get_array_devices(cl_platform_id platform, cl_device_type type, cl_uint count);

/* Функция для получения и отображения строки информации о переданном устройстве
	Парметры:
	 * device - идентификатор устройства, информацию о котором надо получить и отобразить 
	 * info - тип отображаемой информации
	 * title - строка, поясняющая, что это за информация, которую выводит данна функция*/
extern "C" PROCESSINGLIBRARY_API void cl_init_device_get_info(cl_device_id device, cl_device_info info, const char* title);


/*   Работа с контекстом   */

/* Функция, создающая контекст на основе списка конкретных устройств
	Параметры:
	* devices - массив устройств, для которого создаётся контекст
	* num_devices - количество устройств, для которых создаётся контекст*/
extern "C" PROCESSINGLIBRARY_API cl_context cl_init_create_context_by_devices(cl_device_id * devices, cl_uint num_devices);

/* Функция, создающая контекст для всех устройств заданного типа
	Параметры:
	* type - тип устройств, для которых создаётся контекст*/
extern "C" PROCESSINGLIBRARY_API cl_context cl_init_create_context_by_device_type(cl_device_type type);