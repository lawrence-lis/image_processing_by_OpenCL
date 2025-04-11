
// MFCApplicationClientDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "MFCApplicationClient.h"
#include "MFCApplicationClientDlg.h"
#include "afxdialogex.h"

#include <ctime>
#include <exception>

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
	m_nMeanNoise(0),
	m_nStdDevNoise(30),
	m_nMedianFilterSize(3),
	countCurCells(0), 
	reName_Median_GPU(true),
	reName_Median_CPU(true),
	reName_Gaussian_GPU(true),
	reName_Gaussian_CPU(true),
	reName_Pulse_Noise(true), 
	reName_Gaussian_Noise(true)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplicationClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MEAN_GAUSSIAN_NOISE, m_nMeanNoise);
	DDX_Text(pDX, IDC_EDIT_STDDEV_GAUSSIAN_NOISE, m_nStdDevNoise);
	DDX_Text(pDX, IDC_EDIT_MEDIAN_FILTER_SIZE, m_nMedianFilterSize);
}

BEGIN_MESSAGE_MAP(CMFCApplicationClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOAD_IMAGE, &CMFCApplicationClientDlg::OnBnClickedButtonLoadImage)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_ADD_NOISE, &CMFCApplicationClientDlg::OnBnClickedButtonAddNoise)
	ON_BN_CLICKED(IDC_BUTTON_MEDIAN_FILTERING_GPU, &CMFCApplicationClientDlg::OnBnClickedButtonMedianFiltering)
	ON_EN_CHANGE(IDC_EDIT_MEADIAN_FILTER_SIZE, &CMFCApplicationClientDlg::OnEnChangeEditMeadianFilterSize)
	ON_LBN_SELCHANGE(IDC_PLATFORMS_LIST, &CMFCApplicationClientDlg::OnLbnSelchangePlatformsList)
	ON_LBN_SELCHANGE(IDC_DEVICES_LIST, &CMFCApplicationClientDlg::OnLbnSelchangeDevicesList)
	ON_BN_CLICKED(IDC_BUTTON_APPLY_GAUSSIAN_NOISE, &CMFCApplicationClientDlg::OnBnClickedButtonApplyGaussianNoise)
	ON_EN_CHANGE(IDC_EDIT_MEAN_GAUSSIAN_NOISE, &CMFCApplicationClientDlg::OnEnChangeEditMeanGaussianNoise)
	ON_EN_CHANGE(IDC_EDIT_STDDEV_GAUSSIAN_NOISE, &CMFCApplicationClientDlg::OnEnChangeEditStddevGaussianNoise)
	ON_BN_CLICKED(IDC_BUTTON_GAUSSIAN_BLUR_FILTER_GPU, &CMFCApplicationClientDlg::OnBnClickedButtonGaussianBlurFilter)
	ON_BN_CLICKED(IDC_BUTTON_MEDIAN_FILTERING_CPU, &CMFCApplicationClientDlg::OnBnClickedButtonMedianFilteringCpu)
	ON_BN_CLICKED(IDC_BUTTON_GAUSSIAN_BLUR_FILTER_CPU, &CMFCApplicationClientDlg::OnBnClickedButtonGaussianBlurFilterCpu)
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
	delete m_pInputImage;
	delete m_pOutputImage;

}

void CMFCApplicationClientDlg::OnBnClickedButtonLoadImage()
{
	countCurCells = 0;
	CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, L"Image Files (*.bmp;*.jpg;*.jpeg;*.png;*.gif)|*.bmp;*.jpg;*.jpeg;*.png;*.gif|BMP Files (*.bmp)|*.bmp|JPEG Files (*.jpg;*.jpeg)|*.jpg;*.jpeg|PNG Files (*.png)|*.png|GIF Files (*.gif)|*.gif|All Files (*.*)|*.*||");
	if (dlg.DoModal() == IDOK) {
		CString filePath = dlg.GetPathName();

		// Сохраняем путь исходного файла
		m_originalImageFilePath = filePath;

		// Загружаем изображение в m_pInputImage
		delete m_pInputImage;	// Освобождение предыдущего изображения
		if(m_pOutputImage != nullptr) m_pOutputImage = nullptr;;
		m_pInputImage = Bitmap::FromFile(filePath);

		// Проверка на ошибки после создания Bitmap
		if (m_pInputImage != nullptr && m_pInputImage->GetLastStatus() != Ok) {
			delete m_pInputImage;	// Освобождение памяти при ошибке
			m_pInputImage = nullptr;	// Устанавливаем в nullptr для безопасности
			MessageBox(L"Error loading image.", L"Error", MB_ICONERROR);
			return;
		}

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


void CMFCApplicationClientDlg::OnBnClickedButtonAddNoise()
{
	if (m_pInputImage == nullptr) {
		MessageBox(L"Upload the image first.", L"Warning", MB_ICONWARNING);
		return;
	}

	// Создаем новое изображение с шумом
	Bitmap* clonedBitmap = m_pInputImage->Clone(0, 0, m_pInputImage->GetWidth(), m_pInputImage->GetHeight(), PixelFormat32bppARGB);
	if (clonedBitmap == nullptr) {
		MessageBox(L"Failed to clone the image.", L"Error", MB_ICONERROR);
		return;
	}

	m_pOutputImage = nullptr;
	m_pOutputImage = clonedBitmap;

	// Добавить импульсный шум
	ApplyImpulseNoise(*m_pInputImage, *m_pOutputImage, 0.05);

	// Отобразит изображение с шумом
	DisplayImageInPictureControl(m_pOutputImage, IDC_OUTPUT_IMAGE);

	if (reName_Pulse_Noise) reName_Pulse_Noise = false;
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

void CMFCApplicationClientDlg::ApplyImpulseNoise(Bitmap& source, Bitmap& destination, double noiseProbability)
{
	// Получаем размеры изображения
	int width = source.GetWidth();
	int height = source.GetHeight();
	
	// Подготавливаем данные для доступа к пикселям (блокировка)
	BitmapData sourceData;
	BitmapData destinationData;
	Rect rect(0, 0, width, height);

	source.LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &sourceData);
	destination.LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, &destinationData);

	// Получаем указатели на данные пикселей
	BYTE* sourcePixels = (BYTE*)sourceData.Scan0;
	BYTE* destinationPixels = (BYTE*)destinationData.Scan0;

	// Вычисляем количество байт на строку
	int stride = sourceData.Stride;

	// Проходим по каждому пикселю
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			// Вычисляем индекс пикселя
			int index = y * stride + x * 4; // 4 байта на пиксель (ARGB)

			// Генерируем случайное число
			double randomNumber = static_cast<double>(rand()) / RAND_MAX;

			// Если случайное число меньше вероятности шума
			if (randomNumber < noiseProbability)
			{
				// Создаем импульсный шум (черный или белый пиксель)
				if (rand() % 2 == 0)
				{
					// Белый пиксель
					destinationPixels[index + 0] = 255; // B
					destinationPixels[index + 1] = 255; // G
					destinationPixels[index + 2] = 255; // R
				}
				else
				{
					// Черный пиксель
					destinationPixels[index + 0] = 0;   // B
					destinationPixels[index + 1] = 0;   // G
					destinationPixels[index + 2] = 0;   // R
				}
			}
			else
			{
				// Копируем исходный пиксель
				destinationPixels[index + 0] = sourcePixels[index + 0];
				destinationPixels[index + 1] = sourcePixels[index + 1];
				destinationPixels[index + 2] = sourcePixels[index + 2];
			}
			// Альфа канал (прозрачность) всегда копируется
			destinationPixels[index + 3] = sourcePixels[index + 3];
		}
	}
	// Разблокируем данные пикселей
	source.UnlockBits(&sourceData);
	destination.UnlockBits(&destinationData);
}


bool CMFCApplicationClientDlg::SaveBitmapToFile(Bitmap& bitmap, CString& sourceFilePath, const CString& appendedPartName, bool reName)
{
	// 1. Получаем информацию об исходном файле
	CString folderPath, fileName, fileExt;
	SplitPath(sourceFilePath, folderPath, fileName, fileExt);

	if (reName)sourceFilePath = folderPath + L"\\" + fileName + appendedPartName + fileExt;

	// 2. Создаем новое имя файла
	CString newFileName = sourceFilePath;

	// 3. Определяем формат файла (CLSID энкодера)
	CLSID encoderClsid;
	CString mimeType;
	if (fileExt.CompareNoCase(L".bmp") == 0)
		mimeType = L"image/bmp";
	else if (fileExt.CompareNoCase(L".jpg") == 0 || fileExt.CompareNoCase(L".jpeg") == 0)
		mimeType = L"image/jpeg";
	else if (fileExt.CompareNoCase(L".png") == 0)
		mimeType = L"image/png";
	else if (fileExt.CompareNoCase(L".gif") == 0)
		mimeType = L"image/gif";
	else
	{
		MessageBox(L"Unsupported file format.", L"Error", MB_ICONERROR);
		return false;
	}

	if (GetEncoderClsid(mimeType, &encoderClsid) != Ok)
	{
		MessageBox(L"Couldn't find the encoder for this format.", L"Error", MB_ICONERROR);
		return false;
	}
	// 4. Сохраняем bitmap в файл
	if (bitmap.Save(newFileName, &encoderClsid, NULL) != Ok)
	{
		MessageBox(L"Couldn't save image.", L"Error", MB_ICONERROR);
		return false;
	}
	return true;
}

Status CMFCApplicationClientDlg::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  numEncoders = 0;          // количество кодеров изображения
	UINT  size = 0;					// размер массива кодировщиков изображений в байтах

	ImageCodecInfo* pImageCodecInfo = NULL;
	Status  status;

	status = GetImageEncodersSize(&numEncoders, &size);
	if (status != Ok)
	{
		TRACE(L"GetImageEncodersSize failed with status: %d\n", status);
		return status;
	}

	if (size == 0) {
		TRACE(L"No image encoders found.\n");
		return GenericError;
	}

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
	{
		TRACE(L"Memory allocation failed.\n");
		return OutOfMemory;
	}

	status = GetImageEncoders(numEncoders, size, pImageCodecInfo);
	if (status != Ok)
	{
		TRACE(L"GetImageEncoders failed with status: %d\n", status);
		free(pImageCodecInfo);
		return status;
	}

	for (UINT j = 0; j < numEncoders; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return Ok;
		}
	}
	free(pImageCodecInfo);
	TRACE(L"Encoder not found for format: %s\n", format);
	return GenericError;
}

void CMFCApplicationClientDlg::SplitPath(CString& filePath, CString& folderPath, CString& fileName, CString& fileExt)
{
	int lastSlash = filePath.ReverseFind(L'\\');
	if (lastSlash != -1)
	{
		folderPath = filePath.Left(lastSlash);
		fileName = filePath.Mid(lastSlash + 1);
	}
	else
	{
		folderPath = L"";
		fileName = filePath;
	}

	int lastDot = fileName.ReverseFind(L'.');
	if (lastDot != -1)
	{
		fileExt = fileName.Mid(lastDot);
		fileName = fileName.Left(lastDot);
	}
	else fileExt = L"";
}

void CMFCApplicationClientDlg::OnBnClickedButtonMedianFiltering()
{
	countCurCells++;
	if (m_pInputImage == nullptr) {
		MessageBox(L"Please load an image first. (Buttons \"Load Image\" -> \"Add Noise\")", L"Error", MB_ICONERROR);
		return;
	}

	if ((m_pOutputImage != nullptr) && toRewrite) {
		// Если в m_pOutputImage уже занесёно изображение и флаг о перезаписи включен, то перезаписываем, иначе мы работаем с незашумлённым изображением либо изначально зашумленным но без перезаписи
		m_pInputImage = m_pOutputImage->Clone(0, 0, m_pOutputImage->GetWidth(), m_pOutputImage->GetHeight(), m_pOutputImage->GetPixelFormat());
		DisplayImageInPictureControl(m_pInputImage, IDC_INPUT_IMAGE);
		m_pOutputImage = nullptr;
		toRewrite = false;
	}

	// Массив платформ инициализируется при запуске приложения. Тут обрабатыается только тот случай, когда клиент не выбрал платформу
	if (platformId == NULL) platformId = platforms[0];

	if (devices == NULL)
	{
		numDevices = cl_init_get_num_devices(platformId);
		devices = cl_init_get_array_devices(platformId, CL_DEVICE_TYPE_ALL, numDevices);
	}

	if (deviceId == NULL) deviceId = devices[0];

	context = cl_init_create_context_by_devices(devices, numDevices);

	commandQueue = cl_runtime_create_command_queue(context, deviceId);

	// Проверка, поддерживается ли изображения устройством, в противном случае - приложение прекратит свою работу
	cl_bool imageSupport = CL_FALSE;
	clGetDeviceInfo(deviceId, CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &imageSupport, NULL);
	if (imageSupport != CL_TRUE)
	{
		printf("OpenCL device does not support images.\n");
		exit(1);
	}

	sampler = cl_runtime_create_sampler(context);

	cl_program program_meidan = cl_runtime_create_program_from_file(context, "Kernels/Median_Filter.cl");
	if (cl_runtime_build_program(program_meidan, deviceId, NULL) == -1)
		exit(-1);

	kernel = cl_runtime_create_kernel(program_meidan, "median_filter");

	// Получаем данные изображения из Bitmap
	int width = m_pInputImage->GetWidth();
	int height = m_pInputImage->GetHeight();

	BitmapData bitmapData;
	Rect rect(0, 0, width, height);
	PixelFormat pixelFormat = m_pInputImage->GetPixelFormat();

	m_pInputImage->LockBits(&rect, ImageLockModeRead, pixelFormat, &bitmapData);

	BYTE* inputPixels = (BYTE*)bitmapData.Scan0;

	int count_channels = 4;
	switch (bitmapData.PixelFormat) 
	{
	case PixelFormat24bppRGB: count_channels = 3; break;
	case PixelFormat32bppARGB: count_channels = 4; break;
	case PixelFormat8bppIndexed: count_channels = 1; break;
	default:
		count_channels = 0; 
		MessageBox(L"Unsupported PixelFormat", L"Error", MB_ICONERROR);
		return;
	}
	
	// Создаем Bitmap для вывода
	Bitmap* filteredBitmap = m_pInputImage->Clone(0, 0, width, height, pixelFormat);
	BitmapData filteredBitmapData;
	Rect filteredRect(0, 0, width, height);
	filteredBitmap->LockBits(&filteredRect, ImageLockModeWrite, pixelFormat, &filteredBitmapData);

	BYTE* outputPixels = (BYTE*)filteredBitmapData.Scan0;

	// Медианная фильтрация
	cl_ulong duration;
	bool success = medianFilter(inputPixels, outputPixels, width, height, count_channels, m_nMedianFilterSize, commandQueue, kernel, context, sampler, &duration);

	m_pInputImage->UnlockBits(&bitmapData);
	filteredBitmap->UnlockBits(&filteredBitmapData);

	if (success)
	{
		m_pOutputImage = nullptr;
		m_pOutputImage = filteredBitmap->Clone(0, 0, width, height, pixelFormat);

		DisplayImageInPictureControl(m_pOutputImage, IDC_OUTPUT_IMAGE);

		if (reName_Median_GPU) reName_Median_GPU = false;
	}
	else {
		delete filteredBitmap;
		MessageBox(L"Не удалось применить медианный фильтр.", L"Ошибка", MB_ICONERROR);
	}

	delete filteredBitmap;
	m_pProcessingDuration.Format(_T("Время обработки: %.4f мс"), duration / 1000000.f);
	GetDlgItem(IDC_STATIC_PROCESSING_DURATION)->SetWindowText(m_pProcessingDuration);
	writeDataToCSV("MedianFilterGPU_Data.csv", width, height, countCurCells, duration / 1000000.f);

	clReleaseContext(context);
	clReleaseCommandQueue(commandQueue);
	clReleaseKernel(kernel);
	clReleaseSampler(sampler);
	clReleaseProgram(program_meidan);
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
		devices = cl_init_get_array_devices(platformId, CL_DEVICE_TYPE_ALL, numDevices);
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

void CMFCApplicationClientDlg::ApplyGaussianNoise(Bitmap& source, Bitmap& destination, double mean, double stddev)
{
	int width = source.GetWidth();
	int height = source.GetHeight();

	BitmapData sourceData;
	BitmapData destinationData;
	Rect rect(0, 0, width, height);

	source.LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &sourceData);
	destination.LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, &destinationData);

	BYTE* sourcePixels = (BYTE*)sourceData.Scan0;
	BYTE* destinationPixels = (BYTE*)destinationData.Scan0;

	int stride = sourceData.Stride;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int idx = y * stride + x * 4;
			for (int i = 0; i < 3; i++)
			{
				int pixelValue = sourcePixels[idx + i];
				
				double sum = 0.0;
				for (int i = 0; i < 12; i++)
					sum += (double)rand() / RAND_MAX;

				double z0 = sum - 6.0;

				double noise = z0 * stddev + mean;

				int newPixelValue = (int)(pixelValue + noise);
								
				newPixelValue = max(0, min(255, newPixelValue));

				destinationPixels[idx + i] = (BYTE)newPixelValue;
			}
			destinationPixels[idx + 3] = sourcePixels[idx + 3];
		}
	}
	source.UnlockBits(&sourceData);
	destination.UnlockBits(&destinationData);
}

void CMFCApplicationClientDlg::OnBnClickedButtonApplyGaussianNoise()
{
	if (m_pInputImage == nullptr) {
		MessageBox(L"Upload the image first.", L"Warning", MB_ICONWARNING);
		return;
	}

	// Создаем новое изображение с шумом
	Bitmap* clonedBitmap = m_pInputImage->Clone(0, 0, m_pInputImage->GetWidth(), m_pInputImage->GetHeight(), PixelFormat32bppARGB);
	if (clonedBitmap == nullptr) {
		MessageBox(L"Failed to clone the image.", L"Error", MB_ICONERROR);
		return;
	}

	m_pOutputImage = nullptr;
	m_pOutputImage = clonedBitmap;

	ApplyGaussianNoise(*m_pInputImage, *m_pOutputImage, m_nMeanNoise, m_nStdDevNoise);

	DisplayImageInPictureControl(m_pOutputImage, IDC_OUTPUT_IMAGE);

	if (reName_Gaussian_Noise) reName_Gaussian_Noise = false;
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
	countCurCells++;
	if (m_pInputImage == nullptr) {
		MessageBox(L"Please load an image first. (Buttons \"Load Image\" -> \"Add Noise\")", L"Error", MB_ICONERROR);
		return;
	}

	if ((m_pOutputImage != nullptr) && toRewrite) {
		// Если в m_pOutputImage уже занесёно изображение и флаг о перезаписи включен, то перезаписываем, иначе мы работаем с незашумлённым изображением либо изначально зашумленным но без перезаписи
		m_pInputImage = m_pOutputImage->Clone(0, 0, m_pOutputImage->GetWidth(), m_pOutputImage->GetHeight(), m_pOutputImage->GetPixelFormat());
		DisplayImageInPictureControl(m_pInputImage, IDC_INPUT_IMAGE);
		m_pOutputImage = nullptr;
		toRewrite = false;
	}

	// Массив платформ инициализируется при запуске приложения. Тут обрабатыается только тот случай, когда клиент не выбрал платформу
	if (platformId == NULL) platformId = platforms[0];

	if (devices == NULL)
	{
		numDevices = cl_init_get_num_devices(platformId);
		devices = cl_init_get_array_devices(platformId, CL_DEVICE_TYPE_ALL, numDevices);
	}

	if (deviceId == NULL) deviceId = devices[0];

	context = cl_init_create_context_by_devices(devices, numDevices);

	commandQueue = cl_runtime_create_command_queue(context, deviceId);

	// Проверка, поддерживается ли изображения устройством, в противном случае - приложение прекратит свою работу
	cl_bool imageSupport = CL_FALSE;
	clGetDeviceInfo(deviceId, CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &imageSupport, NULL);
	if (imageSupport != CL_TRUE)
	{
		printf("OpenCL device does not support images.\n");
		exit(1);
	}

	sampler = cl_runtime_create_sampler(context);

	cl_program program = cl_runtime_create_program_from_file(context, "Kernels/Kernel_Gaussian_Filter.cl");
	if (cl_runtime_build_program(program, deviceId, NULL) == -1)
		exit(-1);

	kernel = cl_runtime_create_kernel(program, "gaussian_filter");

	// Получаем данные изображения из Bitmap
	int width = m_pInputImage->GetWidth();
	int height = m_pInputImage->GetHeight();

	BitmapData bitmapData;
	Rect rect(0, 0, width, height);
	PixelFormat pixelFormat = m_pInputImage->GetPixelFormat();

	m_pInputImage->LockBits(&rect, ImageLockModeRead, pixelFormat, &bitmapData);

	BYTE* inputPixels = (BYTE*)bitmapData.Scan0;

	int count_channels = 4;
	switch (bitmapData.PixelFormat)
	{
	case PixelFormat24bppRGB: count_channels = 3; break;
	case PixelFormat32bppARGB: count_channels = 4; break;
	case PixelFormat8bppIndexed: count_channels = 1; break;
	default:
		count_channels = 0;
		MessageBox(L"Unsupported PixelFormat", L"Error", MB_ICONERROR);
		return;
	}

	// Создаем Bitmap для вывода
	Bitmap* filteredBitmap = m_pInputImage->Clone(0, 0, width, height, pixelFormat);
	BitmapData filteredBitmapData;
	Rect filteredRect(0, 0, width, height);
	filteredBitmap->LockBits(&filteredRect, ImageLockModeWrite, pixelFormat, &filteredBitmapData);

	BYTE* outputPixels = (BYTE*)filteredBitmapData.Scan0;

	// Фильтрация Гауссовым размытием
	cl_ulong duration;
	bool success = gaussianBlurFilter(inputPixels, outputPixels, width, height, count_channels, commandQueue, kernel, context, sampler, &duration);

	m_pInputImage->UnlockBits(&bitmapData);
	filteredBitmap->UnlockBits(&filteredBitmapData);

	if (success)
	{
		m_pOutputImage = nullptr;
		m_pOutputImage = filteredBitmap->Clone(0, 0, width, height, pixelFormat);

		DisplayImageInPictureControl(m_pOutputImage, IDC_OUTPUT_IMAGE);

		if (reName_Gaussian_GPU)reName_Gaussian_GPU = false;
	}
	else {
		MessageBox(L"Не удалось применить медианный фильтр.", L"Ошибка", MB_ICONERROR);
		filteredBitmap = nullptr;
	}

	delete filteredBitmap;
	m_pProcessingDuration.Format(_T("Время обработки: %.4f мс"), duration / 1000000.f);
	GetDlgItem(IDC_STATIC_PROCESSING_DURATION)->SetWindowText(m_pProcessingDuration);
	writeDataToCSV("GaussianBlurFilterGPU_Data.csv", width, height, countCurCells, duration / 1000000.f);

	clReleaseContext(context);
	clReleaseCommandQueue(commandQueue);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseSampler(sampler);
}


void CMFCApplicationClientDlg::OnBnClickedButtonMedianFilteringCpu()
{
	countCurCells++;
	if (m_pInputImage == nullptr) {
		MessageBox(L"Please load an image first. (Buttons \"Load Image\" -> \"Add Noise\")", L"Error", MB_ICONERROR);
		return;
	}

	if ((m_pOutputImage != nullptr) && toRewrite) {
		// Если в m_pOutputImage уже занесёно изображение и флаг о перезаписи включен, то перезаписываем, иначе мы работаем с незашумлённым изображением либо изначально зашумленным но без перезаписи
		m_pInputImage = m_pOutputImage->Clone(0, 0, m_pOutputImage->GetWidth(), m_pOutputImage->GetHeight(), m_pOutputImage->GetPixelFormat());
		DisplayImageInPictureControl(m_pInputImage, IDC_INPUT_IMAGE);
		m_pOutputImage = nullptr;
		toRewrite = false;
	}

	// Получаем данные изображения из Bitmap
	int width = m_pInputImage->GetWidth();
	int height = m_pInputImage->GetHeight();

	BitmapData bitmapData;
	Rect rect(0, 0, width, height);
	PixelFormat pixelFormat = m_pInputImage->GetPixelFormat();

	m_pInputImage->LockBits(&rect, ImageLockModeRead, pixelFormat, &bitmapData);

	BYTE* inputPixels = (BYTE*)bitmapData.Scan0;

	int count_channels = 4;
	switch (bitmapData.PixelFormat)
	{
	case PixelFormat24bppRGB: count_channels = 3; break;
	case PixelFormat32bppARGB: count_channels = 4; break;
	case PixelFormat8bppIndexed: count_channels = 1; break;
	default:
		count_channels = 0;
		MessageBox(L"Unsupported PixelFormat", L"Error", MB_ICONERROR);
		return;
	}

	// Создаем Bitmap для вывода
	Bitmap* filteredBitmap = m_pInputImage->Clone(0, 0, width, height, pixelFormat);
	BitmapData filteredBitmapData;
	Rect filteredRect(0, 0, width, height);
	filteredBitmap->LockBits(&filteredRect, ImageLockModeWrite, pixelFormat, &filteredBitmapData);

	BYTE* outputPixels = (BYTE*)filteredBitmapData.Scan0;

	unsigned long long duration;
	bool success = CPU_Filtering::medianFilterCPU(inputPixels, outputPixels, width, height, count_channels, m_nMedianFilterSize, &duration);

	m_pInputImage->UnlockBits(&bitmapData);
	filteredBitmap->UnlockBits(&filteredBitmapData);

	if (success)
	{
		m_pOutputImage = nullptr;
		m_pOutputImage = filteredBitmap->Clone(0, 0, width, height, pixelFormat);;

		DisplayImageInPictureControl(m_pOutputImage, IDC_OUTPUT_IMAGE);

		if (reName_Median_CPU) reName_Median_CPU = false;
	}
	else {
		MessageBox(L"Не удалось применить медианный фильтр.", L"Ошибка", MB_ICONERROR);
		filteredBitmap = nullptr;
	}

	delete filteredBitmap;
	m_pProcessingDuration.Format(_T("Время обработки: %.4f мс"), duration / 1000000.f);
	GetDlgItem(IDC_STATIC_PROCESSING_DURATION)->SetWindowText(m_pProcessingDuration);
	writeDataToCSV("MedianFilterCPU_Data.csv", width, height, countCurCells, duration / 1000000.f);
}

void CMFCApplicationClientDlg::OnBnClickedButtonGaussianBlurFilterCpu()
{
	countCurCells++;
	if (m_pInputImage == nullptr) {
		MessageBox(L"Please load an image first. (Buttons \"Load Image\" -> \"Add Noise\")", L"Error", MB_ICONERROR);
		return;
	}

	if ((m_pOutputImage != nullptr) && toRewrite) {
		// Если в m_pOutputImage уже занесёно изображение и флаг о перезаписи включен, то перезаписываем, иначе мы работаем с незашумлённым изображением либо изначально зашумленным но без перезаписи
		m_pInputImage = m_pOutputImage->Clone(0, 0, m_pOutputImage->GetWidth(), m_pOutputImage->GetHeight(), m_pOutputImage->GetPixelFormat());
		DisplayImageInPictureControl(m_pInputImage, IDC_INPUT_IMAGE);
		m_pOutputImage = nullptr;
		toRewrite = false;
	}

	// Получаем данные изображения из Bitmap
	int width = m_pInputImage->GetWidth();
	int height = m_pInputImage->GetHeight();

	BitmapData bitmapData;
	Rect rect(0, 0, width, height);
	PixelFormat pixelFormat = m_pInputImage->GetPixelFormat();

	m_pInputImage->LockBits(&rect, ImageLockModeRead, pixelFormat, &bitmapData);

	BYTE* inputPixels = (BYTE*)bitmapData.Scan0;

	int count_channels = 4;
	switch (bitmapData.PixelFormat)
	{
	case PixelFormat24bppRGB: count_channels = 3; break;
	case PixelFormat32bppARGB: count_channels = 4; break;
	case PixelFormat8bppIndexed: count_channels = 1; break;
	default:
		count_channels = 0;
		MessageBox(L"Unsupported PixelFormat", L"Error", MB_ICONERROR);
		return;
	}

	// Создаем Bitmap для вывода
	Bitmap* filteredBitmap = m_pInputImage->Clone(0, 0, width, height, pixelFormat);
	BitmapData filteredBitmapData;
	Rect filteredRect(0, 0, width, height);
	filteredBitmap->LockBits(&filteredRect, ImageLockModeWrite, pixelFormat, &filteredBitmapData);

	BYTE* outputPixels = (BYTE*)filteredBitmapData.Scan0;

	unsigned long long duration;
	bool success = CPU_Filtering::gaussianBlurFilterCPU(inputPixels, outputPixels, width, height, count_channels, &duration);

	m_pInputImage->UnlockBits(&bitmapData);
	filteredBitmap->UnlockBits(&filteredBitmapData);

	if (success)
	{
		m_pOutputImage = nullptr;
		m_pOutputImage = filteredBitmap->Clone(0, 0, width, height, pixelFormat);

		DisplayImageInPictureControl(m_pOutputImage, IDC_OUTPUT_IMAGE);

		if (reName_Gaussian_CPU) reName_Gaussian_CPU = false;
	}
	else {
		MessageBox(L"Не удалось применить медианный фильтр.", L"Ошибка", MB_ICONERROR);
		filteredBitmap = nullptr;
	}

	delete filteredBitmap;
	m_pProcessingDuration.Format(_T("Время обработки: %.4f мс"), duration / 1000000.f);
	GetDlgItem(IDC_STATIC_PROCESSING_DURATION)->SetWindowText(m_pProcessingDuration);
	writeDataToCSV("GaussianBlurFilterCPU_Data.csv", width, height, countCurCells, duration / 1000000.f);
}

void CMFCApplicationClientDlg::writeDataToCSV(const std::string& filename, int imageWidth, int imageHeight, int data2, float data3)
{
	std::ofstream outputFile(filename, std::ios::app);
	if (outputFile.is_open()) {
		std::string imageSizeString = std::to_string(imageWidth) + "*" + std::to_string(imageHeight);
		long long imageSize = (long long)imageWidth * (long long)imageHeight;
		outputFile << imageSizeString << "," << imageSize << "," << data2 << "," << data3 << std::endl;
		outputFile.close();
	}
	else MessageBox(L"Не получилось записать данные!", L"Error", MB_ICONERROR);
}