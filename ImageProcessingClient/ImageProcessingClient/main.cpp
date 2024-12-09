#include <stdio.h>
// Если начнёт выёбываться на определение функций, то сотри и заново вставь следующую строку
#include "ProcessingLib.h"
#include "CL/cl.h"
#include "Testing.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

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

	// Отображение видео
	namedWindow("Video", 0);
	// Создаём объект захвата видео. Этот объект умеет открывать и закрывать видеофайлы любых типов, поддерживаемых библиотекой ffmpeg
	VideoCapture cap;
	// Объекту захвата передаётся строка, содержащая полный путь к видеофайлу
	cap.open("Sber.VideoForTesting_1.mp4");
	// После открытия файла объект содержать всю информацию о считываемом видео, включая и информацию о состоянии
	// При таком создании объект захвата инициализируетсяпервым кадром видео
	// Далее создаём объект изображения, в котором будут храниться кадры видео
	Mat frame;
	for (;;)
	{
		//Внутри цикла for из видеофайла последовательно читаются кадры с помощью потокового объекта захвата
		cap >> frame;
		if (frame.empty()) break;
		imshow("Video", frame);
		// После отображения кадра мы ждем 33 мс. Если за это время пользователь нажмет какую - нибудь клавишу, то мы выходим из цикла чтения. В противном случае по прошествии 33 мс мы переходим к следующей итерации.После 
		// выхода из цикла вся выделенная память автоматически освобождается.
		if (waitKey(33) >= 0) break;
	}
}