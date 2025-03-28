
// MFCApplicationClientDlg.h: файл заголовка
//

#pragma once
#include "CL/cl.h"


// Диалоговое окно CMFCApplicationClientDlg
class CMFCApplicationClientDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMFCApplicationClientDlg)			// Макрос, необходимый длядинамического создания объектов диалога

private:
	Bitmap* m_pInputImage;
	Bitmap* m_pOutputImage;

	int m_nFilterSize;

	CString m_originalImageFilePath;

	bool toRewrite;										// Переписывать ли Picture Controle

	cl_platform_id platformId;
	cl_device_id deviceId;
	cl_context context;
	cl_command_queue commandQueue;
	cl_kernel kernel;
	cl_sampler sampler;
	bool openCLInitialized;

	void DisplayImageInPictureControl(Bitmap* image, int pictureControlID);			// Для отображения нового изображения в объекте Picture Control
	void ApplyImpulseNoise(Bitmap& source, Bitmap& destination, double noiseProbability);		// Для наложения импульсного шума
	bool SaveBitmapToFile(Bitmap& bitmap, const CString& sourceFilePath, const CString& appendedPartName);		// Для сохранения изображения в файл
	Status GetEncoderClsid(const WCHAR* format, CLSID* pClsid);		// Вспомогательная функция для получения расширения файла изображения (Там немного мороки, но наверное это можно как-то заменить)
	void SplitPath(const CString& filePath, CString& folderPath, CString& fileName, CString& fileExt);		// Вспомогательная функция для разделения пути файла на части

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
};
