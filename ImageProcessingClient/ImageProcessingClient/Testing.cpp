#include "CL/cl.h"
#include "Testing.h"
#include "ProcessingLib.h"

void Checking_clGetPlatformIds(cl_uint num_entries, cl_platform_id* platforms, cl_uint* num_platforms)
{
	switch (clGetPlatformIDs(num_entries, platforms, num_platforms))
	{
	case CL_SUCCESS: { 
		printf("Available platforms: %d\n", *num_platforms);
		return; 
	}
	case CL_INVALID_VALUE: { 
		printf("Error counting available platforms.\n"); 
		return;
	}
	case CL_OUT_OF_HOST_MEMORY: {
		printf("Error: Unable to allocate resources required by the OpenCL implementation on the host.\n");
		return;
	}
	default: {
		printf("WARNING: unknown error when calculating the number of available platforms\n");
		return;
	}
	}
}

void OpenCLTest_I1_Counting_Available_Platforms()
{
	printf("Available platforms: %d\n", ipGetCountPlatforms());
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
}