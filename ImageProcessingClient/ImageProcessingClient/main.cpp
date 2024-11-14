#include <stdio.h>
// Если начнёт выёбываться на определение функций, то сотри и заново вставь следующую строку
#include "ProcessingLib.h"
#include "CL/cl.h"

int main(void)
{
	ipGetInfoAboutAvailableDevices();

	cl_device_id device = ipGetDeviceByIndex(NULL, 0, 0);
	printf("Selected Device:\n");
	ipGetAllInfoAboutSelectedDevices(device);

	cl_context context1 = ipGetContextByArr(&device, 1);
	cl_context context2 = ipGetContextByType(CL_DEVICE_TYPE_GPU);

	cl_command_queue queue1 = ipGetComQueueToDevice(context1, device);

	cl_program program = ipGetProgram(context1, "heis.cl");

	cl_int prg_build_log = ipBuildProgram(program, &device);

	cl_kernel kernel = ipGetKernel(program, "test");

	int a = 30;
	cl_int work = ipSetKernelParam(kernel, 0, sizeof(a), &a);

	size_t gl_size[1] = { 100000 };
	work = ipWorkKernel(kernel, queue1, 1, gl_size);

	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue1);
	clReleaseContext(context1);
	clReleaseContext(context2);
}