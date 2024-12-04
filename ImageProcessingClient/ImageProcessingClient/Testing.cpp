#include "CL/cl.h"
#include "Testing.h"
#include "ProcessingLib.h"
#include <cstdlib>

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

cl_platform_id* Checking_ipGetArrPlatforms(cl_int count, bool recounting = false)
{
	// Ввожу эту функцию для того, чтобы обработать передачу значений входных параметров не больше нуля и привести соответствующую функцию библиотеки к рабочему варианту, 
	// если текущий вариант не работает
	cl_int subCount = count;
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

void OpenCLTest_I1_Counting_Available_Platforms()
{
	// Тест функции ipGetCountPlatforms
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

	/*-------------------------*/
	// Тесты функции ipGetArrPlatforms
	printf("\n************\nTesting ipGetArrPlatforms function:\n");
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
	printf("Recounting on:\n");
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
}