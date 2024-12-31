#include <stdio.h>
// Если начнёт выёбываться на определение функций, то сотри и заново вставь следующую строку
#include "ProcessingLib.h"
#include "CL/cl.h"
#include "Testing.h"
#include <opencv2/opencv.hpp>

using namespace cv;

int main(void)
{
	printf("Testing:\t---***---\n");
	OpenCLTest_I1_Counting_Available_Platforms();
	OpenCLTest_I2_Getting_Available_Platforms_With_Recounting();
	OpenCLTest_I3_Getting_Information_About_Available_Platforms();
	OpenCLTest_I4_Getting_Informations_About_All_Available_Platforms();
	OpenCLTest_I5_Getting_Count_Of_Devices_To_Selected_Platform();
	OpenCLTest_I6_Getting_Array_Of_Devices_To_Selected_Platform();
	printf("\nEnd testing:\t---***---\n\n");

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

	// Чтение изображения из файла
	Mat image = imread("cat-animal-art.jpg");
	// Проверка на успешность чтения
	if (image.empty()) {
		printf("Unable to read image\n");
		return -1;
	}
	// Отображение изображения
	namedWindow("Image", WINDOW_NORMAL);
	imshow("Image", image);
	waitKey(0);
}