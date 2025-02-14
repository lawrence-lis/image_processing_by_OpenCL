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