
// MFCApplicationClientDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "MFCApplicationClient.h"
#include "MFCApplicationClientDlg.h"
#include "afxdialogex.h"

#include <ctime>
#include <exception>
#include <iostream>
#include <fstream>

#include "ProcessingLib.h"
#include "cl_initialize.h"
#include "cl_runtime.h"
#include "LibFilters.h"

#include "CPU_Filtering.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CMFCApplicationClientDlg, CDialogEx)

// Диалоговое окно CMFCApplicationClientDlg
#define M_PI 3.14159265358979323846


CMFCApplicationClientDlg::CMFCApplicationClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATIONCLIENT_DIALOG, pParent),
	m_pStartImage(nullptr),
	m_pInputImage(nullptr),
	m_pOutputImage(nullptr),
	toRewrite(true),
	platforms(NULL),
	platformId(NULL),
	devices(NULL),
	numDevices(0),
	deviceId(NULL),
	context(NULL),
	commandQueue(NULL),
	kernel(NULL),
	sampler(NULL),
	program(NULL),
	m_nMeanNoise(0),
	m_nStdDevNoise(30),
	m_nMedianFilterSize(3),
	m_nStatisticCountCalculations(10),
	m_pStatisticModeCheckBox(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplicationClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MEAN_GAUSSIAN_NOISE, m_nMeanNoise);
	DDX_Text(pDX, IDC_EDIT_STDDEV_GAUSSIAN_NOISE, m_nStdDevNoise);
	DDX_Text(pDX, IDC_EDIT_MEDIAN_FILTER_SIZE, m_nMedianFilterSize);
	DDX_Text(pDX, IDC_EDIT_SIZE_STATISTIC_COUNT_CALCULATIONS, m_nStatisticCountCalculations);
}

BEGIN_MESSAGE_MAP(CMFCApplicationClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOAD_IMAGE, &CMFCApplicationClientDlg::OnBnClickedButtonLoadImage)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_MEDIAN_FILTERING_GPU, &CMFCApplicationClientDlg::OnBnClickedButtonMedianFiltering)
	ON_EN_CHANGE(IDC_EDIT_MEDIAN_FILTER_SIZE, &CMFCApplicationClientDlg::OnEnChangeEditMeadianFilterSize)
	ON_LBN_SELCHANGE(IDC_PLATFORMS_LIST, &CMFCApplicationClientDlg::OnLbnSelchangePlatformsList)
	ON_LBN_SELCHANGE(IDC_DEVICES_LIST, &CMFCApplicationClientDlg::OnLbnSelchangeDevicesList)
	ON_EN_CHANGE(IDC_EDIT_MEAN_GAUSSIAN_NOISE, &CMFCApplicationClientDlg::OnEnChangeEditMeanGaussianNoise)
	ON_EN_CHANGE(IDC_EDIT_STDDEV_GAUSSIAN_NOISE, &CMFCApplicationClientDlg::OnEnChangeEditStddevGaussianNoise)
	ON_BN_CLICKED(IDC_BUTTON_GAUSSIAN_BLUR_FILTER_GPU, &CMFCApplicationClientDlg::OnBnClickedButtonGaussianBlurFilter)
	ON_BN_CLICKED(IDC_BUTTON_MEDIAN_FILTERING_CPU, &CMFCApplicationClientDlg::OnBnClickedButtonMedianFilteringCpu)
	ON_BN_CLICKED(IDC_BUTTON_GAUSSIAN_BLUR_FILTER_CPU, &CMFCApplicationClientDlg::OnBnClickedButtonGaussianBlurFilterCpu)
	ON_BN_CLICKED(IDC_CHECK_STATISTIC_MODE, &CMFCApplicationClientDlg::OnBnClickedCheckStatisticMode)
	ON_EN_CHANGE(IDC_EDIT_SIZE_STATISTIC_COUNT_CALCULATIONS, &CMFCApplicationClientDlg::OnEnChangeEditSizeStatisticCountCalculations)
	ON_BN_CLICKED(IDC_BUTTON_APPLY_NOISE, &CMFCApplicationClientDlg::OnBnClickedButtonApplyNoise)
	ON_BN_CLICKED(IDC_BUTTON_CALCULATING_STATISTIC, &CMFCApplicationClientDlg::OnBnClickedButtonCalculatingStatistic)
END_MESSAGE_MAP()


// Обработчики сообщений CMFCApplicationClientDlg

BOOL CMFCApplicationClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию
	srand(time(0));
	m_pStartImage = nullptr;
	m_pInputImage = nullptr;
	m_pOutputImage = nullptr;
	m_pPlatformListBox.SubclassDlgItem(IDC_PLATFORMS_LIST, this);
	m_pDeviceListBox.SubclassDlgItem(IDC_DEVICES_LIST, this);

	// Первичная инициализация OpenCL
	// Получение платформ
	cl_uint numPlatforms = cl_init_get_num_platforms();
	platforms = cl_init_get_array_platforms(numPlatforms);
	// Отображение платформ (Тут бы, по идее, использовать функцию библиотеки, но на данный момент такая идея не реализована, а времени её разрабатывать уже нет)
	for (int i = 0; i < numPlatforms; i++)
	{
		size_t size;
		clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 0, nullptr, &size);
		std::vector<char> buffer(size);
		clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, size, buffer.data(), nullptr);
		CString platformName(buffer.data());
		m_pPlatformListBox.AddString(platformName);
	}

	CEdit* pEditStatistic = (CEdit*)GetDlgItem(IDC_EDIT_SIZE_STATISTIC_COUNT_CALCULATIONS);
	if (pEditStatistic != nullptr) pEditStatistic->EnableWindow(m_pStatisticModeCheckBox);

	CProgressCtrl* pProgress = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS_STATISTIC);
	if (pProgress != nullptr) pProgress->ShowWindow(m_pStatisticModeCheckBox ? SW_SHOW : SW_HIDE);

	m_pNoiseType.SubclassDlgItem(IDC_COMBO_NOISE_TYPE, this);
	m_pNoiseType.AddString(_T("Импульсный шум"));
	m_pNoiseType.AddString(_T("Гауссов шум"));
	m_pNoiseType.SetCurSel(0);

	m_pStatisticFilteringType.SubclassDlgItem(IDC_COMBO_STATISTIC_FILTRATION_TYPE, this);
	m_pStatisticFilteringType.AddString(_T("Медианная фильтрация на GPU"));
	m_pStatisticFilteringType.AddString(_T("Медианная фильтрация на CPU"));
	m_pStatisticFilteringType.AddString(_T("Фильтрация Гауссовым размытием на GPU"));
	m_pStatisticFilteringType.AddString(_T("Фильтрация Гауссовым размытием на СPU"));
	m_pStatisticFilteringType.ShowWindow(m_pStatisticModeCheckBox ? SW_SHOW : SW_HIDE);
	m_pStatisticFilteringType.SetCurSel(0);

	strStatisticCountCalculations.SubclassDlgItem(IDC_STATIC_STATISTIC_FILTRATION_TYPE, this);
	strStatisticCountCalculations.ShowWindow(m_pStatisticModeCheckBox ? SW_SHOW : SW_HIDE);

	CButton* b1 = (CButton*)GetDlgItem(IDC_BUTTON_MEDIAN_FILTERING_GPU);
	b1->EnableWindow(!m_pStatisticModeCheckBox);

	CButton* b2 = (CButton*)GetDlgItem(IDC_BUTTON_MEDIAN_FILTERING_CPU);
	b2->EnableWindow(!m_pStatisticModeCheckBox);

	CButton* b3 = (CButton*)GetDlgItem(IDC_BUTTON_GAUSSIAN_BLUR_FILTER_GPU);
	b3->EnableWindow(!m_pStatisticModeCheckBox);

	CButton* b4 = (CButton*)GetDlgItem(IDC_BUTTON_GAUSSIAN_BLUR_FILTER_CPU);
	b4->EnableWindow(!m_pStatisticModeCheckBox);

	CButton* b5 = (CButton*)GetDlgItem(IDC_BUTTON_CALCULATING_STATISTIC);
	b5->EnableWindow(m_pStatisticModeCheckBox);

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CMFCApplicationClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);

		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CMFCApplicationClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CMFCApplicationClientDlg::~CMFCApplicationClientDlg()
{
	free(platforms);
	free(devices);
	delete m_pStartImage;
	delete m_pInputImage;
	delete m_pOutputImage;
}

void CMFCApplicationClientDlg::OnBnClickedButtonLoadImage()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, L"Image Files (*.bmp;*.jpg;*.jpeg;*.png;*.gif)|*.bmp;*.jpg;*.jpeg;*.png;*.gif|BMP Files (*.bmp)|*.bmp|JPEG Files (*.jpg;*.jpeg)|*.jpg;*.jpeg|PNG Files (*.png)|*.png|GIF Files (*.gif)|*.gif|All Files (*.*)|*.*||");
	if (dlg.DoModal() == IDOK) {
		CString filePath = dlg.GetPathName();

		// Сохраняем путь исходного файла
		m_originalImageFilePath = filePath;

		// Загружаем изображение в m_pInputImage
		delete m_pStartImage;	// Освобождение предыдущего изображения
		if(m_pOutputImage != nullptr) m_pOutputImage = nullptr;;
		m_pStartImage = Bitmap::FromFile(filePath);
		// Проверка на ошибки после создания Bitmap
		if (m_pStartImage != nullptr && m_pStartImage->GetLastStatus() != Ok) {
			delete m_pStartImage;	// Освобождение памяти при ошибке
			m_pStartImage = nullptr;	// Устанавливаем в nullptr для безопасности
			MessageBox(L"Error loading image.", L"Error", MB_ICONERROR);
			return;
		}

		m_pInputImage = m_pStartImage->Clone(0, 0, m_pStartImage->GetWidth(), m_pStartImage->GetHeight(), m_pStartImage->GetPixelFormat());

		DisplayImageInPictureControl(m_pInputImage, IDC_INPUT_IMAGE);
	}
	toRewrite = true;
}


void CMFCApplicationClientDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// При изменении размера окна перерисовываем изображения в Picture Control'ах
	if (m_pInputImage)
	{
		DisplayImageInPictureControl(m_pInputImage, IDC_INPUT_IMAGE);
	}
	if (m_pOutputImage)
	{
		DisplayImageInPictureControl(m_pOutputImage, IDC_OUTPUT_IMAGE);
	}

	// TODO: добавьте свой код обработчика сообщений
}

void CMFCApplicationClientDlg::DisplayImageInPictureControl(Bitmap* image, int pictureControlID)
{
	if (image == nullptr || image->GetLastStatus() != Ok) return;

	CRect rect;
	CWnd* pPictureCtrl = GetDlgItem(pictureControlID);

	if (pPictureCtrl == nullptr) return;

	pPictureCtrl->GetClientRect(&rect);
	int width = rect.Width();
	int height = rect.Height();

	Bitmap сImage(width, height, PixelFormat32bppARGB); // Создаем Bitmap для масштабирования
	Graphics graphics(&сImage);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	graphics.Clear(Color(0, 0, 0, 0)); // Прозрачный фон

	graphics.DrawImage(image, 0, 0, width, height);

	HDC hdc = ::GetDC(pPictureCtrl->m_hWnd);
	Graphics pictureGraphics(hdc);
	pictureGraphics.DrawImage(&сImage, 0, 0);
	::ReleaseDC(pPictureCtrl->m_hWnd, hdc);
}

void CMFCApplicationClientDlg::OnBnClickedButtonMedianFiltering()
{
	DefiningConditions(false, "Kernels/Median_Filter.cl", "median_filter", false, nullptr);
}


void CMFCApplicationClientDlg::OnEnChangeEditMeadianFilterSize()
{
	UpdateData(TRUE);

	if (m_nMedianFilterSize < 1) {
		m_nMedianFilterSize = 1;
		MessageBox(L"Размер фильтра должен быть не меньше 1.", L"Ошибка", MB_ICONERROR);
		UpdateData(FALSE); // Обновляем Edit Control из m_nFilterSize
	}
	else if (m_nMedianFilterSize > 5) {
		m_nMedianFilterSize = 5;
		MessageBox(L"Размер фильтра не должен превышать 5.", L"Ошибка", MB_ICONERROR);
		UpdateData(FALSE); // Обновляем Edit Control из m_nFilterSize
	}
}


void CMFCApplicationClientDlg::OnLbnSelchangePlatformsList()
{
	m_pDeviceListBox.ResetContent();
	int selectIdx = m_pPlatformListBox.GetCurSel();

	if (selectIdx != LB_ERR) 
	{
		platformId = platforms[selectIdx]; 
		// Вывод информации о выбранной платформе в строку состояния
		size_t size;
		clGetPlatformInfo(platformId, CL_PLATFORM_NAME, 0, nullptr, &size);
		std::vector<char> buffer(size);
		clGetPlatformInfo(platformId, CL_PLATFORM_NAME, size, buffer.data(), nullptr);
		CString platformName(buffer.data());
		platformInfo.Format(_T("Платформа: %s"), platformName.GetBuffer());
		// Определение массива доступных устройств для выбранной платформы
		numDevices = cl_init_get_num_devices(platformId);
		if (devices != NULL) free(devices);
		devices = cl_init_get_array_devices(platformId, CL_DEVICE_TYPE_GPU, numDevices);
		for (int i = 0; i < numDevices; i++)
		{
			clGetDeviceInfo(devices[i], CL_DEVICE_NAME, 0, nullptr, &size);
			std::vector<char> buffer(size);
			clGetDeviceInfo(devices[i], CL_DEVICE_NAME, size, buffer.data(), nullptr);
			CString deviceName(buffer.data());
			m_pDeviceListBox.AddString(deviceName);
		}
	}

	m_pOpenCLInitStatus.Format(_T("Статус инициализации OpenCL:\n\n %s"), platformInfo.GetBuffer());
	GetDlgItem(IDC_STATIC_OPENCL_STATUS)->SetWindowText(m_pOpenCLInitStatus);
}


void CMFCApplicationClientDlg::OnLbnSelchangeDevicesList()
{
	int selectIdx = m_pDeviceListBox.GetCurSel();
	if (selectIdx != LB_ERR)
	{
		deviceId = devices[selectIdx];
		// Вывод информации о выбранном устройстве в строку состояния
		size_t size;
		clGetDeviceInfo(deviceId, CL_DEVICE_NAME, 0, nullptr, &size);
		std::vector<char> buffer(size);
		clGetDeviceInfo(deviceId, CL_DEVICE_NAME, size, buffer.data(), nullptr);
		CString deviceName(buffer.data());
		deviceInfo.Format(_T("Устройство: %s"), deviceName.GetBuffer());
	}

	m_pOpenCLInitStatus.Format(_T("Статус инициализации OpenCL:\n\n %s\n\n %s"), platformInfo.GetBuffer(), deviceInfo.GetBuffer());
	GetDlgItem(IDC_STATIC_OPENCL_STATUS)->SetWindowText(m_pOpenCLInitStatus);
}

void CMFCApplicationClientDlg::OnEnChangeEditMeanGaussianNoise()
{
	UpdateData(TRUE);
}


void CMFCApplicationClientDlg::OnEnChangeEditStddevGaussianNoise()
{
	UpdateData(TRUE);
}

void CMFCApplicationClientDlg::OnBnClickedButtonGaussianBlurFilter()
{
	DefiningConditions(false, "Kernels/Kernel_Gaussian_Filter.cl", "gaussian_filter", false, nullptr);
}


void CMFCApplicationClientDlg::OnBnClickedButtonMedianFilteringCpu()
{
	DefiningConditions(true, "Median Filtering on CPU", nullptr, false, nullptr);
}

void CMFCApplicationClientDlg::OnBnClickedButtonGaussianBlurFilterCpu()
{
	DefiningConditions(true, "Gaussian Filtering on CPU", nullptr, false, nullptr);
}

void CMFCApplicationClientDlg::OnBnClickedCheckStatisticMode()
{
	m_pStatisticModeCheckBox = (IsDlgButtonChecked(IDC_CHECK_STATISTIC_MODE) == BST_CHECKED);

	CEdit* pEditStatistic = (CEdit*)GetDlgItem(IDC_EDIT_SIZE_STATISTIC_COUNT_CALCULATIONS);
	pEditStatistic->EnableWindow(m_pStatisticModeCheckBox);

	CProgressCtrl* pProgress = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS_STATISTIC);
	if (pProgress != nullptr) pProgress->ShowWindow(m_pStatisticModeCheckBox ? SW_SHOW : SW_HIDE);

	CButton* b1 = (CButton*)GetDlgItem(IDC_BUTTON_MEDIAN_FILTERING_GPU);
	b1->EnableWindow(!m_pStatisticModeCheckBox);

	CButton* b2 = (CButton*)GetDlgItem(IDC_BUTTON_MEDIAN_FILTERING_CPU);
	b2->EnableWindow(!m_pStatisticModeCheckBox);

	CButton* b3 = (CButton*)GetDlgItem(IDC_BUTTON_GAUSSIAN_BLUR_FILTER_GPU);
	b3->EnableWindow(!m_pStatisticModeCheckBox);

	CButton* b4 = (CButton*)GetDlgItem(IDC_BUTTON_GAUSSIAN_BLUR_FILTER_CPU);
	b4->EnableWindow(!m_pStatisticModeCheckBox);

	CButton* b5 = (CButton*)GetDlgItem(IDC_BUTTON_CALCULATING_STATISTIC);
	b5->EnableWindow(m_pStatisticModeCheckBox);

	m_pStatisticFilteringType.ShowWindow(m_pStatisticModeCheckBox ? SW_SHOW : SW_HIDE);
	strStatisticCountCalculations.ShowWindow(m_pStatisticModeCheckBox ? SW_SHOW : SW_HIDE);
}

void CMFCApplicationClientDlg::OnEnChangeEditSizeStatisticCountCalculations()
{
	UpdateData(TRUE);

	if (m_nStatisticCountCalculations < 1) {
		m_nStatisticCountCalculations = 1;
		MessageBox(L"Количество вычислений должно быть не меньше 1.", L"Ошибка", MB_ICONERROR);
		UpdateData(FALSE);
	}
}

void CMFCApplicationClientDlg::DefiningConditions(bool only_cpu, const char* kernel_file_name, const char* kernel_function_name, bool is_statistic, float* stat_array)
{
	//	1. Проверка начальных условий.
	//		1.1. Загружено ли обрабатываемое изображение?
	if (m_pInputImage == nullptr) {
		MessageBox(L"Please load an image first. (Buttons \"Load Image\" -> \"Add Noise\")", L"Error", MB_ICONERROR);
		return;
	}
	//		1.2. Перезапись входного изображения. Позволяет повторно применить операции обработки к результату предыдущей обработки.
	if ((m_pOutputImage != nullptr) && toRewrite) {
		//		1.2.1. Освобождаем предыдущий m_pInputImage, если он существует.
		if (m_pInputImage != nullptr) {
			delete m_pInputImage;
			m_pInputImage = nullptr;
		}
		//		1.2.2. Если в m_pOutputImage уже занесёно изображение и флаг о перезаписи включен, то перезаписываем, иначе мы работаем с незашумлённым изображением либо изначально зашумленным но без перезаписи.
		m_pInputImage = m_pOutputImage->Clone(0, 0, m_pOutputImage->GetWidth(), m_pOutputImage->GetHeight(), m_pOutputImage->GetPixelFormat());
		DisplayImageInPictureControl(m_pInputImage, IDC_INPUT_IMAGE);
		m_pOutputImage = nullptr;
		toRewrite = false;
	}
	//		1.3. Используется ли OpenCL?
	if (!only_cpu)
	{
		//		1.3.1. Используется не только CPU. В данном случае испольуются необходимые типы данных, инициализация OpenCL и т.д.
		//			1.3.1.1. Обрабатыается случай, когда клиент не выбрал платформу. Выбирается первая доступная платформа OpenCL.
		if (platformId == NULL) platformId = platforms[0];
		//			1.3.1.2. На случай если на момент вызова топ-функции массив устройств OpenCL не инициализирован. Устройства пересчитываются и переопределяются.
		if (devices == NULL)
		{
			numDevices = cl_init_get_num_devices(platformId);
			devices = cl_init_get_array_devices(platformId, CL_DEVICE_TYPE_ALL, numDevices);
		}
		//			1.3.1.3. Обрабатыается случай, когда клиент не выбрал устройство. Выбирается первое доступное устройство OpenCL.
		if (deviceId == NULL) deviceId = devices[0];
		//			1.3.1.4. Создаётся объект контекста работы OpenCL на основе доступных устройств.
		context = cl_init_create_context_by_devices(platformId, devices, numDevices);
		//			1.3.1.5. Создвётся очередь комманд для текущего контекста OpenCL и выбранного устройства.
		commandQueue = cl_runtime_create_command_queue(context, deviceId);
		//			1.3.1.6. Проверка, поддерживается ли изображения устройством, в противном случае - приложение прекратит свою работу.
		cl_bool imageSupport = CL_FALSE;
		clGetDeviceInfo(deviceId, CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &imageSupport, NULL);
		if (imageSupport != CL_TRUE)
		{
			printf("OpenCL device does not support images.\n");
			exit(1);
		}
		//			1.3.1.7. Создание сэмпленра, определяющмй особенности работы с изображением в OpenCL.
		sampler = cl_runtime_create_sampler(context);
		//			1.3.1.8. Создание объекта программы для запуска на устройстве.
		program = cl_runtime_create_program_from_file(context, kernel_file_name);
		//			1.3.1.9. Компиляция и компановка программы для запуска на устройстве.
		if (cl_runtime_build_program(program, deviceId, NULL) == -1)
			exit(-1);
		//			1.3.1.10. Создание объекта ядра. 
		kernel = cl_runtime_create_kernel(program, kernel_function_name);
	}
	//	2. Получаем данные изображения из Bitmap и готовимся к работе с ними. 
	//		2.1. Ширина и высота изображения.
	int width = m_pInputImage->GetWidth();
	int height = m_pInputImage->GetHeight();
	//		2.2. Подготавливаем данные, необходимые для прямого доступа к пикселям изображения.
	BitmapData sourceData;
	Rect rect(0, 0, width, height);
	PixelFormat pixelFormat = m_pInputImage->GetPixelFormat();
	//		2.3. Блокировка памяти для прямого доступа к пикселям изображения.
	m_pInputImage->LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &sourceData);
	//		2.4. Создание переменной для представления пиксельных данных.
	BYTE* inputPixels = (BYTE*)sourceData.Scan0;
	// 		2.5. Определяем количество каналов цвета в каждом пикселе, основываясь на формате пикселей исходного изображения.
	int count_channels;
	switch (sourceData.PixelFormat)
	{
	case PixelFormat24bppRGB: count_channels = 3; break;
	case PixelFormat32bppARGB: count_channels = 4; break;
	case PixelFormat8bppIndexed: count_channels = 1; break;
	default:
		count_channels = 0;
		MessageBox(L"Unsupported PixelFormat", L"Error", MB_ICONERROR);
		exit(-1);
	}
	// 		2.6. Создаём копию исходного изображения и подготавливаем данные для работы с новым изображением.
	Bitmap* filteredBitmap = m_pInputImage->Clone(0, 0, width, height, PixelFormat32bppARGB);
	BitmapData filteredBitmapData;
	Rect filteredRect(0, 0, width, height);
	// 		2.7. Блокируем память для записи для прямого доступа в режиме записи, предоставляя возможность изменять пиксельные данные.
	filteredBitmap->LockBits(&filteredRect, ImageLockModeWrite, PixelFormat32bppARGB, &filteredBitmapData);
	//		2.8. Создание переменной для представления пиксельных данных.
	BYTE* outputPixels = (BYTE*)filteredBitmapData.Scan0;
	//	3. Обработка. Непосредственно фильтрация
	//		3.1. В зависимости от имени файла, с которого считался код ядра OpenCL, либо переданной строке, указывающей какой именно должен быть метод фильтрации в случае если обработка ведётся на CPU, 
	//вызывается соответствующая функция или метод.
	cl_ulong duration;
	bool success;
	if (kernel_file_name == "Kernels/Median_Filter.cl") success = medianFilter(inputPixels, outputPixels, width, height, count_channels, m_nMedianFilterSize, commandQueue, kernel, context, sampler, &duration);
	else if (kernel_file_name == "Kernels/Kernel_Gaussian_Filter.cl") success = gaussianBlurFilter(inputPixels, outputPixels, width, height, count_channels, commandQueue, kernel, context, sampler, &duration);
	else if (kernel_file_name == "Median Filtering on CPU") success = CPU_Filtering::medianFilterCPU(inputPixels, outputPixels, width, height, count_channels, m_nMedianFilterSize, &duration);
	else if (kernel_file_name == "Gaussian Filtering on CPU")success = CPU_Filtering::gaussianBlurFilterCPU(inputPixels, outputPixels, width, height, count_channels, &duration);
	else {
		//		3.1.1. Осучществляем очистку выделенных ресурсов в случае ошибки, сообщаем об ошибке пользователю и завершаем работу приложения.
		delete filteredBitmap;
		m_pInputImage->UnlockBits(&sourceData);
		filteredBitmap->UnlockBits(&filteredBitmapData);
		MessageBox(L"Не получилось выполнить обработку!", L"Error", MB_ICONERROR);
		exit(-1);
	}
	//		3.2. Разблокировка памяти, которая была заблокирована для доступа к пиксельным данным. Предотвращает утечки памяти, даёт доступ к изображениям и является обязательной операцией после завершения работы
	// с заблокированными пиксельными данными в GDI+
	m_pInputImage->UnlockBits(&sourceData);
	filteredBitmap->UnlockBits(&filteredBitmapData);
	//		3.3. Освобождаем старое выходное изображение и заменяем его обработанным. И отображаем
	if (m_pOutputImage != nullptr) delete m_pOutputImage;
	m_pOutputImage = filteredBitmap->Clone(0, 0, width, height, PixelFormat32bppARGB);
	DisplayImageInPictureControl(m_pOutputImage, IDC_OUTPUT_IMAGE);
	//		3.4. Удаляем копию исходного изображения. Освобождаем память.
	delete filteredBitmap;
	//		3.5. Выводим получившееся время обработки.
	if (is_statistic)
			*stat_array = duration / 1000000.f;
	m_pProcessingDuration.Format(_T("Время обработки: %.4f мс"), duration / 1000000.f);
	GetDlgItem(IDC_STATIC_PROCESSING_DURATION)->SetWindowText(m_pProcessingDuration);
	//	4. Освобождаем ресурсы, выделенные на реализацию функционала OpenCL, если не был передан флаг об использовании только CPU.
	if (!only_cpu)
	{
		clReleaseContext(context);
		clReleaseCommandQueue(commandQueue);
		clReleaseKernel(kernel);
		clReleaseProgram(program);
		clReleaseSampler(sampler);
	}
}

void CMFCApplicationClientDlg::OnBnClickedButtonApplyNoise()
{
	ApplyNoise();
}


void CMFCApplicationClientDlg::OnBnClickedButtonCalculatingStatistic()
{
	float* stats = new float[m_nStatisticCountCalculations];
	int selectedIndex = m_pStatisticFilteringType.GetCurSel();
	CString selectedText;
	m_pStatisticFilteringType.GetLBText(selectedIndex, selectedText);

	CProgressCtrl* pProgress = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS_STATISTIC);
	for (int i = 0; i < m_nStatisticCountCalculations; i++)
	{
		ApplyNoise();
		if (selectedText == _T("Медианная фильтрация на GPU"))
		{
			DefiningConditions(false, "Kernels/Median_Filter.cl", "median_filter", true, &stats[i]);
		}
		if (selectedText == _T("Медианная фильтрация на CPU"))
		{
			DefiningConditions(true, "Median Filtering on CPU", nullptr, true, &stats[i]);
		}
		if (selectedText == _T("Фильтрация Гауссовым размытием на GPU"))
		{
			DefiningConditions(false, "Kernels/Kernel_Gaussian_Filter.cl", "gaussian_filter", true, &stats[i]);
		}
		if (selectedText == _T("Фильтрация Гауссовым размытием на СPU"))
		{
			DefiningConditions(true, "Gaussian Filtering on CPU", nullptr, true, &stats[i]);
		}
		pProgress->SetPos((i + 1) * (m_nStatisticCountCalculations));
	}
	// Сортировка
	for (int i = 0; i < m_nStatisticCountCalculations - 1; i++)
	{
		for (int j = 0; j < m_nStatisticCountCalculations - i - 1; j++)
		{
			if (stats[j] > stats[j + 1])
			{
				std::swap(stats[j], stats[j + 1]);
			}
		}
	}
	// Нахождение медианы
	float median;
	if (m_nStatisticCountCalculations % 2 == 0) 
	{
		median = (stats[m_nStatisticCountCalculations / 2 - 1] + stats[m_nStatisticCountCalculations / 2]) / 2.f;
	}
	else
	{
		median = stats[m_nStatisticCountCalculations / 2];
	}
	m_pProcessingDuration.Format(_T("Медианное время обработки: %.4f мс"), median);
	GetDlgItem(IDC_STATIC_PROCESSING_DURATION)->SetWindowText(m_pProcessingDuration);
	delete[] stats;
}

void CMFCApplicationClientDlg::ApplyNoise()
{

	if (m_pStartImage == nullptr) {
		MessageBox(L"Upload the image first.", L"Warning", MB_ICONWARNING);
		return;
	}

	DisplayImageInPictureControl(m_pStartImage, IDC_INPUT_IMAGE);

	int width = m_pStartImage->GetWidth();
	int height = m_pStartImage->GetHeight();

	BitmapData sourceData;
	BitmapData destinationData;
	Rect rect(0, 0, width, height);
	PixelFormat pixelFormat = m_pInputImage->GetPixelFormat();

	Bitmap* clonedBitmap = m_pStartImage->Clone(0, 0, width, height, pixelFormat);
	if (clonedBitmap == nullptr) {
		MessageBox(L"Failed to clone the image.", L"Error", MB_ICONERROR);
		return;
	}

	m_pStartImage->LockBits(&rect, ImageLockModeRead, pixelFormat, &sourceData);
	clonedBitmap->LockBits(&rect, ImageLockModeWrite, pixelFormat, &destinationData);

	BYTE* sourcePixels = (BYTE*)sourceData.Scan0;
	BYTE* destinationPixels = (BYTE*)destinationData.Scan0;

	int stride = sourceData.Stride;

	int count_channels;
	switch (sourceData.PixelFormat)
	{
	case PixelFormat24bppRGB: count_channels = 3; break;
	case PixelFormat32bppARGB: count_channels = 4; break;
	case PixelFormat8bppIndexed: count_channels = 1; break;
	default:
		count_channels = 0;
		MessageBox(L"Unsupported PixelFormat", L"Error", MB_ICONERROR);
		exit(-1);
	}

	int selectedIndex = m_pNoiseType.GetCurSel();
	if (selectedIndex != CB_ERR)
	{
		CString selectedText;
		m_pNoiseType.GetLBText(selectedIndex, selectedText);
		if (selectedText == _T("Импульсный шум"))
		{
			double Noise_Prob = 0.05;
			// Проходим по каждому пикселю
			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					// Вычисляем индекс пикселя
					int index = y * stride + x * count_channels;
					// Генерируем случайное число
					double randomNumber = static_cast<double>(rand()) / RAND_MAX;

					// Если случайное число меньше вероятности шума
					if (randomNumber < Noise_Prob)
					{
						// Создаем импульсный шум (черный или белый пиксель)
						if (rand() % 2 == 0)
							for (int c = 0; c < count_channels; c++)
								destinationPixels[index + c] = 255;
						else
							for (int c = 0; c < count_channels; c++)
								destinationPixels[index + c] = 0;
					}
					else
					{
						for (int c = 0; c < count_channels; c++)
							destinationPixels[index + c] = sourcePixels[index + c];
					}
				}
			}
		}
		if (selectedText == _T("Гауссов шум"))
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int idx = y * stride + x * count_channels;
					for (int c = 0; c < count_channels; c++)
					{
						int pixelValue = sourcePixels[idx + c];

						double sum = 0.0;
						for (int j = 0; j < 12; j++)
							sum += (double)rand() / RAND_MAX;

						double z0 = sum - 6.0;

						double noise = z0 * m_nStdDevNoise + m_nMeanNoise;

						int newPixelValue = (int)(pixelValue + noise);

						newPixelValue = max(0, min(255, newPixelValue));

						destinationPixels[idx + c] = (BYTE)newPixelValue;
					}
				}
			}
		}
		m_pStartImage->UnlockBits(&sourceData);
		clonedBitmap->UnlockBits(&destinationData);
	}

	if (m_pOutputImage != nullptr) delete m_pOutputImage;
	m_pOutputImage = clonedBitmap->Clone(0, 0, width, height, pixelFormat);
	delete clonedBitmap;

	toRewrite = true;
	DisplayImageInPictureControl(m_pOutputImage, IDC_OUTPUT_IMAGE);
}