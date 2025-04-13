
// MFCApplicationClientDlg.h: файл заголовка
//

#pragma once
#include "CL/cl.h"
#include <vector>


// Диалоговое окно CMFCApplicationClientDlg
class CMFCApplicationClientDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMFCApplicationClientDlg)			// Макрос, необходимый длядинамического создания объектов диалога

private:
	Bitmap* m_pInputImage;
	Bitmap* m_pOutputImage;

	CListBox m_pPlatformListBox; // Для платформ
	CListBox m_pDeviceListBox; // Для устройств

	CComboBox m_pNoiseType;

	float m_nMeanNoise;
	float m_nStdDevNoise;
	int m_nMedianFilterSize;
	int m_nStatisticCountCalculations;

	CString m_originalImageFilePath;
	CString m_pOpenCLInitStatus;
	CString m_pProcessingDuration;
	CString platformInfo, deviceInfo;

	bool toRewrite;										// Переписывать ли Picture Controle

	BOOL m_pStatisticModeCheckBox;

	cl_platform_id* platforms;							// Массив всех доступных платформ
	cl_platform_id platformId;							// Идентификатор выбранной пользователем платформы OpenCL
	cl_device_id* devices;								// Массив всех доступных устройств
	cl_uint numDevices;									// Количество выбранных пользователем устройств
	cl_device_id deviceId;								// Идентификатор выбранного пользователем устройства OpenCL
	cl_context context;
	cl_command_queue commandQueue;
	cl_kernel kernel;
	cl_sampler sampler;
	cl_program program;

	void DisplayImageInPictureControl(Bitmap* image, int pictureControlID);			// Для отображения нового изображения в объекте Picture Control
	void ApplyImpulseNoise(Bitmap& source, Bitmap& destination, double noiseProbability);		// Для наложения импульсного шума
	void ApplyGaussianNoise(Bitmap& source, Bitmap& destination, double mean, double stddev);		// Для наложения гауссового шума
	bool SaveBitmapToFile(Bitmap& bitmap, CString& sourceFilePath, const CString& appendedPartName);		// Для сохранения изображения в файл
	Status GetEncoderClsid(const WCHAR* format, CLSID* pClsid);		// Вспомогательная функция для получения расширения файла изображения (Там немного мороки, но наверное это можно как-то заменить)
	void SplitPath(CString& filePath, CString& folderPath, CString& fileName, CString& fileExt);		// Вспомогательная функция для разделения пути файла на части
	/// <summary>
	/// Вспомогательная функция для обработки изображений.
	/// Параметры:
	///		* only_cpu - флаг использования исключительно центрального процессора. В этом случае не производятся инициализация OpenCL и не используются никакие типы данных свойственных данному API.
	///		* kernel_file_name - имя файла, содержащего код ядра для выполнения на устройстве OpenCL. Имеет расширение .cl. Либо строка, ч помощью которой можно использовать тот или иной метод обработки на CPU.
	///		* kernel_function_name - имя функции, реализованной в коде ядра, которую будет выполнять устройство.
	/// </summary>
	void DefiningConditions(bool only_cpu, const char* kernel_file_name, const char* kernel_function_name);

	// Создание
public:
	CMFCApplicationClientDlg(CWnd* pParent = nullptr);	// стандартный конструктор
	virtual ~CMFCApplicationClientDlg();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATIONCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonLoadImage();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonAddNoise();
	afx_msg void OnBnClickedButtonMedianFiltering();
	afx_msg void OnEnChangeEditMeadianFilterSize();
	afx_msg void OnLbnSelchangePlatformsList();
	afx_msg void OnLbnSelchangeDevicesList();
	afx_msg void OnBnClickedButtonApplyGaussianNoise();
	afx_msg void OnEnChangeEditMeanGaussianNoise();
	afx_msg void OnEnChangeEditStddevGaussianNoise();
	afx_msg void OnBnClickedButtonGaussianBlurFilter();
	afx_msg void OnBnClickedButtonMedianFilteringCpu();
	afx_msg void OnBnClickedButtonGaussianBlurFilterCpu();
	afx_msg void OnBnClickedCheckStatisticMode();
	afx_msg void OnEnChangeEditSizeStatisticCountCalculations();
	afx_msg void OnBnClickedButtonApplyNoise();
	afx_msg void OnBnClickedButtonCalculatingStatistic();
};
