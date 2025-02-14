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