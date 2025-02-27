#pragma once
#include "FreeImage.h"

// Функция для создания изображения из буфера. Выполняется после всех процедур.
void CreateImageFromBuffer(const char* src_file_name, const unsigned char* buffer, unsigned width, unsigned height, const char* dst_str_name = "-output.png");

/* Функция для вывода сообщения о сбое, возникшем при работе с растровым изображением.
* Параметр fif, передаваемый в первом аргументе обратного вызова, может быть равен FIF_UNKNOWN,
* когда генерируется ошибка, не связанная с плагином. В этом случае вызов FreeImage_GetFormatFromFIF(FIF_UNKNOWN)
* вернёт значение NULL. Присвоение нулевого значения таким функциям, как “printf(...)”, может привести к сбою 
* вашего приложения, поэтому просто будь осторожен … */
void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char* message);

/* Функция для создания буфера из изображения. */
char* CreateBufferFromImage(const char* src_file_name, unsigned int& width, unsigned int& height);

// Функция для добавления импульсного шума на RGB изображение
void addSaltAndPepperNoiseRGB(FIBITMAP* image, double saltProbability, double pepperProbability);

// Функция для добавления импульсного шума на RGB изображение (работа непосредственно с файлом)
void AddPulseNoiseToImage(const char* src_file_name, double salt, double pepper);