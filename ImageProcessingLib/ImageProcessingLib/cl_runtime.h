#pragma once
/* Этот модуль содержит набор функций, которые управляют объектами OpenCL, такими как:
	- очереди команд;
	- объекты памяти;
	- программные объекты;
	- объекты ядра для функций ядра в программе;
  А также функции, которые позволяют помещать команды в очередь команд, например, выполнение ядра, чтение или запись объекта памяти и т.д.*/

#define PROCESSINGLIBRARY_EXPORTS

#ifdef PROCESSINGLIBRARY_EXPORTS
#define PROCESSINGLIBRARY_API __declspec(dllexport)
#else
#define PROCESSINGLIBRARY_API __declspec(dllimport)
#endif

#include "CL/cl.h"

  /*   Работа с очередями команд   */

/* Функция, создающая очередь команд для контекста и связанного с ним устройства
	Параметры:
	* context - контекст, для которого создаётся очередь команд
	* device - устройство, для которого создаётся очередь команд*/
extern "C" PROCESSINGLIBRARY_API cl_command_queue cl_runtime_create_command_queue(cl_context context, cl_device_id device);

/*   Работа с объектами памяти   */

/* Функция, создающая объект изображения из исходных данных. для внутреннех использования
	Параметры:
	* context - Контекст, в рамках которого создаваемый объект и будет существовать
	* flags - флаги
	* format - формат создаваемого изображения
	* w и h - ширина и высота изображения соответственно
	* rp - количество байт между началом одной строки изображения и началом следующей строки. Если установить rp в 0, OpenCL рассчитает значение автоматически
	* hp - указатель на данные изображения*/
extern "C" PROCESSINGLIBRARY_API cl_mem cl_runtime_create_image(cl_context context, cl_mem_flags flags, cl_image_format * format, size_t w, size_t h, void* hp = NULL, size_t rp = 0);

/* Функция, создающая объект изображения из исходных данных, только для чтения
	Параметры:
	* context - Контекст, в рамках которого создаваемый объект и будет существовать
	* format - формат создаваемого изображения
	* w и h - ширина и высота изображения соответственно
	* rp - количество байт между началом одной строки изображения и началом следующей строки. Если установить rp в 0, OpenCL рассчитает значение автоматически
	* hp - указатель на данные изображения*/
extern "C" PROCESSINGLIBRARY_API cl_mem cl_runtime_create_image_for_reading(cl_context context, cl_image_format* format, size_t w, size_t h, void* hp, size_t rp = 0);

/* Функция для создания объекта выходного изображения, который сохранит результат выполнения обработки входного изображения. Этот объект создается без host_ptr, поскольку он будет заполнен данными в ядре.
	Кроме того, для mem_flags установлено значение CL_MEM_WRITE_ONLY, потому что образ будет только записан в ядро, но не прочитан.
	Параметры:
	* context - Контекст, в рамках которого создаваемый объект и будет существовать
	* format - формат создаваемого изображения
	* w и h - ширина и высота изображения соответственно
	* rp - количество байт между началом одной строки изображения и началом следующей строки. Если установить rp в 0, OpenCL рассчитает значение автоматически*/
extern "C" PROCESSINGLIBRARY_API cl_mem cl_runtime_create_image_for_writing(cl_context context, cl_image_format * format, size_t w, size_t h, size_t rp = 0);

/* Функция, которая освобождает объект памяти OpenCL
	Параметры:
	* mem_obj - Объект памяти, который нужно освободить*/
extern "C" PROCESSINGLIBRARY_API void cl_runtime_release_mem_object(cl_mem mem_obj);

/* Функция, которая создает объект буфера OpenCL
	Параметры:
	* context - Контекст OpenCL
	* flags - Флаги для буфера (например, CL_MEM_READ_ONLY, CL_MEM_WRITE_ONLY, CL_MEM_COPY_HOST_PTR).
	* size - Размер буфера в байтах.
	* host_ptr - (Необязательный) Указатель на данные, которые нужно скопировать в буфер.*/
extern "C" PROCESSINGLIBRARY_API cl_mem cl_runtime_create_buffer(cl_context context, cl_mem_flags flags, size_t size, void* host_ptr);

/* Функция, которая безопасно и удобно считывает данные из cl_mem (изображения) обратно на хост (CPU)
	Параметры:
	* command_queue - очередь команд, в которую будет поставлена операция чтения изображения
	* image - объект памяти OpenCL, содержащий изображение, которое нужно прочитать
	* origin - смещение в изображении, с которого начинается чтение. Он определяет начальную координату (x, y, z) области изображения, которую нужно прочитать
	* region - размеры области изображения, которую нужно прочитать. Он определяет ширину, высоту и глубину области изображения, которую нужно скопировать.
	* host_ptr - адрес в памяти хоста (CPU), куда будут скопированы данные изображения. Это должен быть указатель на буфер, который вы выделили на хосте. 
	Размер этого буфера должен быть достаточным для хранения данных, которые вы читаете из изображения (определяется параметрами origin и region, а также форматом изображения).*/
extern "C" PROCESSINGLIBRARY_API bool cl_runtime_read_image(cl_command_queue command_queue, cl_mem image, size_t origin[3], size_t region[3], void* host_ptr, size_t host_buffer_size);

/*   Работа с сэмплерами   */

/* Функция для создания сэмплера
	Параметры:
	* context - Контекст, в рамках которого создаваемый объект и будет существовать
	* nmz_coords - определяет, нормализованные ли координаты
	* am - режим адресации
	* fm - режим фильтрации*/
extern "C" PROCESSINGLIBRARY_API cl_sampler cl_runtime_create_sampler(cl_context context, cl_bool nmz_coords = CL_FALSE, cl_addressing_mode am = CL_ADDRESS_CLAMP_TO_EDGE, cl_filter_mode fm = CL_FILTER_NEAREST);


/*   Работа с объектами программ   */

/* Функция для чтения содержимого файла в строку
	Параметры:
	* filename - имя файла
	* source_size - указатель на переменную для возврата размера файла*/
extern "C" PROCESSINGLIBRARY_API char* read_kernel_source(const char* filename, size_t * source_size);

/* Функция для создания объекта cl_program из файла
	Параметры:
	* context - Контекст, в рамках которого и создается программа
	* filename - имя файла, из которого считывается текст*/
extern "C" PROCESSINGLIBRARY_API cl_program cl_runtime_create_program_from_file(cl_context context, const char* filename);

/* Функция для создания (компиляция и компановка) исполняемого файла программы ядра для одного устройства
	Параметры:
	* program - объект программы
	* device - устройство, длякоторого создается программа
	* options - параметры сборки*/
extern "C" PROCESSINGLIBRARY_API cl_int cl_runtime_build_program(cl_program program, cl_device_id device, const char* options);


/*   Работа с объектами ядер   */

/* Функция для создания объекта ядра, готового для последующего запуска на рабочих единицах соответствующего устройства
	Параметры:
	  * program - программный объект с успешно построенным исполняемым кодом ядра
	  * fun_name - имя функции со спецификатором __kernel*/
extern "C" PROCESSINGLIBRARY_API cl_kernel cl_runtime_create_kernel(cl_program program, const char* fun_name);

/* Функция для отправки ядру дополнительной информации в виде параметров кода ядра
	Параметры:
	  * kernel - объект ядра, которому передаётся информация с целью последующего запуска
	  * param_idx - индекс параметра, отсчитывается с нуля, начиная с самого левого параметра функции ядра
	  * param_size - размер передаваемых данных в байтах
	  * param - указатель на данные, которые будут использованы для установки значения параметра*/
extern "C" PROCESSINGLIBRARY_API cl_int cl_runtime_set_kernel_param(cl_kernel kernel, cl_uint param_idx, size_t param_size, const void* param);

/* Функция для работы с ядром, а именно: запуск оного
	Параметры:
	  * kernel - объект ядра, который предстоит запустить
	  * queue - очередь команд
	  * dim - количество измерений рабочего размера
	  * qws - глобальный рабочий размер
	  * lws - локальный рабочий размер
	  * dur - указатель на переменную, куда будет записано время, затраченное на выполнение функции*/
extern "C" PROCESSINGLIBRARY_API cl_int cl_runtime_enqueue_kernel(cl_kernel kernel, cl_command_queue queue, cl_uint dim, const size_t * gws, const size_t * lws, cl_ulong * dur);