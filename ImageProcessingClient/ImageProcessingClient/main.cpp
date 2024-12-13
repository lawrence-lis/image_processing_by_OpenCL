#include <stdio.h>
// Если начнёт выёбываться на определение функций, то сотри и заново вставь следующую строку
#include "ProcessingLib.h"
#include "CL/cl.h"
#include "Testing.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>

using namespace cv;
using namespace std;

int g_slider_position = 0;
int g_run = 1, g_dontset = 0; // начинаем в режиме покадрового просмотра
cv::VideoCapture g_cap;

void onTrackbarSlide(int pos, void*)
{
	g_cap.set(cv::CAP_PROP_POS_FRAMES, pos);
	if (!g_dontset)
		g_run = 1;
	g_dontset = 0;
}

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
	g_cap.open("Sber.VideoForTesting_1.mp4");
	int frames = (int)g_cap.get(CAP_PROP_FRAME_COUNT);
	int tmtw = (int)g_cap.get(CAP_PROP_FRAME_WIDTH);
	int tmth = (int)g_cap.get(CAP_PROP_FRAME_HEIGHT);
	printf("Frame count: %d, size: [%d x %d]\n", frames, tmtw, tmth);
	cv::createTrackbar("Position", "Video", &g_slider_position, frames, onTrackbarSlide);
	cv::Mat frame;
	for (;;)
	{
		if (g_run != 0)
		{
			g_cap >> frame;
			if (frame.empty()) 
				break;
			int current_pos = (int)g_cap.get(CAP_PROP_POS_FRAMES);
			g_dontset = 1;
			cv::setTrackbarPos("Position", "Video", current_pos);
			imshow("Video", frame);
			g_run -= 1;
		}
		char c = (char)cv::waitKey(10);
		if (c == 's') // покадровый режим
		{
			g_run = 1;
			printf("Frame-by-frame mode, run = %d\n", g_run);
		}
		if (c == 'r') // непрерывный режим
		{
			g_run = -1;
			printf("Continuous mode, run = %d\n", g_run);
		}
		if (c == 27)
			break;
	}
}