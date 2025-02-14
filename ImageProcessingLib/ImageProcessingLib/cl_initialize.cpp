#include "pch.h"
#include <stdio.h>
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
		printf("The resources needed to implement OpenCL could not be allocated on the host.\n\tProblem area: the cl_init_get_num_platforms function.\n\n");
		return 0;
	default:
		// Неизвестная ошибка. Вывести на экран соотвествующее сообщение и вернуть 0
		printf("Unknown error when calculating the number of available platforms.\n\tProblem area: the cl_init_get_num_platforms function.\n\n");
		return 0;
	}
}

