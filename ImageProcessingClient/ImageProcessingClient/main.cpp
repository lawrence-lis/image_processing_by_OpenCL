#include <stdio.h>
// Если начнёт выёбываться на определение функций, то сотри и заново вставь следующую строку
#include "ProcessingLib.h"
#include "CL/cl.h"
#include "Testing.h"
#include <opencv2/opencv.hpp>

int main(void)
{
	printf("Testing:\t---***---\n");
	OpenCLTest_I1_Counting_Available_Platforms();
	printf("End testing:\t---***---\n\n");

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

	// Загрузить изображение.
	cv::Mat image = cv::imread("cat-animal-art.jpg");
	// Создаём несколько окон для показа входного и выходного изображений.
	cv::namedWindow("Example_in", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Example_out", cv::WINDOW_AUTOSIZE);
	// Показываем входное изображение.
	cv::imshow("Example_in", image);
	// Создаём объект для хранения сглаженного изображения
	cv::Mat out;
	// Сглаживаем (можно использовать GaussianBlur(), blur(), medianBlur() или bilateralFilter())
	cv::GaussianBlur(image, out, cv::Size(5, 5), 3, 3);
	cv::GaussianBlur(out, out, cv::Size(5, 5), 3, 3);
	// Показываем сглаженное изображение в окне вывода
	cv::imshow("Example_out", out);
	// Ждём нажатия клавиши, окна уничтожаются автоматически
	cv::waitKey(0);
}