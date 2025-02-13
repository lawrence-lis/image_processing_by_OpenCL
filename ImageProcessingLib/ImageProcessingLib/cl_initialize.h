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