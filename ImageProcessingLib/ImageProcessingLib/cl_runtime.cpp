#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include "cl_runtime.h"

/*   Работа с очередями команд   */

// Функция, создающая очередь команд для контекста и связанного с ним устройства
cl_command_queue cl_runtime_create_command_queue(cl_context context, cl_device_id device)
{
    cl_command_queue queue;
    cl_int err;
    queue = clCreateCommandQueue(context, device, NULL, &err);
    switch (err)
    {
    case CL_SUCCESS:
        // Очередь команд успешно создана. Просто вернуть значение (Во всех остальных случаях функция вернёт NULL)
        return queue;
    case CL_INVALID_CONTEXT:
        // Переданный контекст - это недопустимый контекст.
        printf("\"context\" is not a valid context.\n\tProblem area: the \"cl_runtime_create_command_queue\" function\n\n");
        return NULL;
    case CL_INVALID_DEVICE:
        // Переданное устройство - это недопустимое устройство.
        printf("\"device\" is not a valid context.\n\tProblem area: the \"cl_runtime_create_command_queue\" function\n\n");
        return NULL;
    case CL_OUT_OF_RESOURCES:
        // Не удается выделить ресурсы, требуемые реализацией OpenCL на устройстве.
        printf("There is a failure to allocate resources required by the OpenCL implementation on the device.\n\tProblem area: the \"cl_runtime_create_command_queue\" function\n\n");
        return NULL;
    case CL_OUT_OF_HOST_MEMORY:
        // Не удается выделить ресурсы, требуемые реализацией OpenCL, на хосте.
        printf("there is a failure to allocate resources required by the OpenCL implementation on the host.\n\tProblem area: the \"cl_runtime_create_command_queue\" function\n\n");
        return NULL;
    default:
        // Произошло что-то непонятное Вывести соответствующее сообщение и вернуть управление.
        printf("Unknown error\n\tProblem area: the \"cl_runtime_create_command_queue\" function\n\n");
        return NULL;
    }
}


/*   Работа изображениями   */

// Функция, создающая объект изображения из исходных данных.для внутреннех использования
cl_mem cl_runtime_create_image(cl_context context, cl_mem_flags flags, cl_image_format* format, size_t w, size_t h, void* hp, size_t rp )
{
    cl_int err;
    cl_mem image = clCreateImage2D(context, flags, format, w, h, rp, hp, &err); 
    switch (err)
    {
    case CL_SUCCESS:
        // Очередь команд успешно создана. Просто вернуть значение (Во всех остальных случаях функция вернёт NULL)
        return image;
    case CL_INVALID_CONTEXT:
        // Переданный контекст - это недопустимый контекст.
        printf("\"context\" is not a valid context.\n\tProblem area: the \"cl_runtime_create_image\" function\n\n");
        return NULL;
    case CL_INVALID_VALUE:
        // Значения, указанные во флагах, недопустимы.
        printf("Values specified in \"flags\" are not valid.\n\tProblem area: the \"cl_runtime_create_image\" function\n\n");
        return NULL;
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
        // Значения, указанные в image_format, недопустимы, или если image_format имеет значение NULL.
        printf("values specified in \"format\" are not valid or if \"format\" is NULL.\n\tProblem area: the \"cl_runtime_create_image\" function\n\n");
        return NULL;
    case CL_INVALID_IMAGE_SIZE:
        // w или h равны 0 или если они превышают максимальные значения, указанные в CL_DEVICE_IMAGE2D_MAX_WIDTH или CL_DEVICE_IMAGE2D_MAX_HEIGHT соответственно для всех устройств в контексте, или если значения, указанные в 
        // rp, не соответствуют правилам.
        printf("Incorrect image dimensions.\n\tProblem area: the \"cl_runtime_create_image\" function\n\n");
        return NULL;
    case CL_INVALID_HOST_PTR:
        // hp равен NULL, а CL_MEM_USE_HOST_PTR или CL_MEM_COPY_HOST_PTR заданы во флагах, или если hp не равен NULL, но CL_MEM_COPY_HOST_PTR или CL_MEM_USE_HOST_PTR не заданы во флагах.
        printf("Incorrect host pointer.\n\tProblem area: the \"cl_runtime_create_image\" function\n\n");
        return NULL;
    case CL_IMAGE_FORMAT_NOT_SUPPORTED:
        // В контексте нет устройств, поддерживающих image_format.
        printf("There are no devices in \"context\" that support \"format\".\n\tProblem area: the \"cl_runtime_create_image\" function\n\n");
        return NULL;
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        // Произошел сбой при выделении памяти для объекта изображения.
        printf("There is a failure to allocate memory for image object.\n\tProblem area: the \"cl_runtime_create_image\" function\n\n");
        return NULL;
    case CL_INVALID_OPERATION:
        // В контексте нет устройств, поддерживающих изображения (т. е. CL_DEVICE_IMAGE_SUPPORT, указанный в таблице запросов устройств, равен CL_FALSE).
        printf("There are no devices in \"context\" that support images (i.e. CL_DEVICE_IMAGE_SUPPORT specified in the Device Queries table is CL_FALSE).\n");
        printf("\tProblem area : the \"cl_runtime_create_image\" function\n\n");
        return NULL;
    case CL_OUT_OF_RESOURCES:
        // Не удается выделить ресурсы, требуемые реализацией OpenCL на устройстве.
        printf("There is a failure to allocate resources required by the OpenCL implementation on the device.\n\tProblem area: the \"cl_runtime_create_image\" function\n\n");
        return NULL;
    case CL_OUT_OF_HOST_MEMORY:
        // Не удается выделить ресурсы, требуемые реализацией OpenCL, на хосте.
        printf("there is a failure to allocate resources required by the OpenCL implementation on the host.\n\tProblem area: the \"cl_runtime_create_image\" function\n\n");
        return NULL;
    default:
        // Произошло что-то непонятное Вывести соответствующее сообщение и вернуть управление.
        printf("Unknown error\n\tProblem area: the \"cl_runtime_create_image\" function\n\n");
        return NULL;
    }
}

// Функция, создающая объект изображения из исходных данных, только для чтения
cl_mem cl_runtime_create_image_for_reading(cl_context context, cl_image_format* format, size_t w, size_t h, void* hp, size_t rp)
{
    return cl_runtime_create_image(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format, w, h, hp, rp);
}

//Функция для создания объекта выходного изображения, который сохранит результат выполнения обработки входного изображения.
cl_mem cl_runtime_create_image_for_writing(cl_context context, cl_image_format* format, size_t w, size_t h, size_t rp)
{
    return cl_runtime_create_image(context, CL_MEM_WRITE_ONLY, format, w, h);
}


/*   Работа с сэмплерами   */

// Функция для создания сэмплера
cl_sampler cl_runtime_create_sampler(cl_context context, cl_bool nmz_coords, cl_addressing_mode am, cl_filter_mode fm)
{
    cl_int err;
    cl_sampler sampler = clCreateSampler(context, nmz_coords, am, fm, &err);
    switch (err)
    {
    case CL_SUCCESS:
        // Очередь команд успешно создана. Просто вернуть значение (Во всех остальных случаях функция вернёт NULL)
        return sampler;
    case CL_INVALID_CONTEXT: 
        // Контекст - это недопустимый контекст.
        printf("\"context\" is not a valid context.\n\tProblem area: the \"cl_runtime_create_sampler\" function\n\n");
        return NULL;
    case CL_INVALID_VALUE: 
        // am, fm, nmz_coords или комбинация этих аргументов недопустимы.
        printf("\"am\", \"fm\", \"nmz_coords\" or a combination of these arguements are not valid.\n\tProblem area: the \"cl_runtime_create_sampler\" function\n\n");
        return NULL;
    case CL_INVALID_OPERATION: 
        // Изображения не поддерживаются ни одним устройством, связанным с context (т.е. CL_DEVICE_IMAGE_SUPPORT, указанный в таблице запросов устройств, равен CL_FALSE).
        printf("Images are not supported by any device associated with context(i.e.CL_DEVICE_IMAGE_SUPPORT specified in the Device Queries table is CL_FALSE).\n\tProblem area: the \"cl_runtime_create_sampler\" function\n\n");
        return NULL;
    case CL_OUT_OF_RESOURCES: 
        // Не удается выделить ресурсы, требуемые реализацией OpenCL на устройстве.
        printf("There is a failure to allocate resources required by the OpenCL implementation on the device.\n\tProblem area: the \"cl_runtime_create_sampler\" function\n\n");
        return NULL;
    case CL_OUT_OF_HOST_MEMORY: 
        // Не удается выделить ресурсы, требуемые реализацией OpenCL, на хосте.
        printf("There is a failure to allocate resources required by the OpenCL implementation on the host.\n\tProblem area: the \"cl_runtime_create_sampler\" function\n\n");
        return NULL;
    default:
        // Произошло что-то непонятное Вывести соответствующее сообщение и вернуть управление.
        printf("Unknown error.\n\tProblem area: the \"cl_runtime_create_sampler\" function\n\n");
        return NULL;
    }
}


/*   Работа с объектами программ   */

// Функция для чтения содержимого файла в строку
char* read_kernel_source(const char* filename, size_t* source_size)
{
    FILE* fp;
    char* source_str;
    size_t current_size = 0;
    // Открываем файл в режиме чтения
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Couldn't open the file: %s\n\tProblem area: the \"read_kernel_source\" function\n\n", filename);
        return NULL;
    }
    // Выделяем начальный буфер для строки
    current_size = 256; // Начальный размер буфера
    source_str = (char*)malloc(current_size);
    if (source_str == NULL) {
        printf("Failed to allocate memory for the kernel code.\n\tProblem area: the \"read_kernel_source\" function\n\n");
        fclose(fp);
        return NULL;
    }
    source_str[0] = '\0'; // Инициализируем как пустую строку
    size_t i = 0;
    char c;
    // Читаем содержимое файла в строку
    // Читаем файл посимвольно
    while ((c = fgetc(fp)) != EOF) {
        // Увеличиваем размер буфера, если необходимо
        if (i + 1 >= current_size) {
            current_size *= 2; // Удваиваем размер
            source_str = (char*)realloc(source_str, current_size);
            if (source_str == NULL) {
                printf("Couldn't reallocate the memory for the kernel code.\n\tProblem area: the \"read_kernel_source\" function\n\n");
                fclose(fp);
                return NULL;
            }
        }
        // Добавляем символ в строку
        source_str[i++] = c;
        source_str[i] = '\0';
    }
    fclose(fp);
    *source_size = strlen(source_str);
    return source_str;
}

// Функция для создания объекта cl_program из файла
cl_program cl_runtime_create_program_from_file(cl_context context, const char* filename)
{
    cl_program program;
    cl_int err;
    size_t source_size;
    // Читаем код ядра из файла
    char* source_str = read_kernel_source(filename, &source_size);
    if (source_str == NULL) {
        printf("Reading the kernel text from the file: %s failed.\n\tProblem area: the \"cl_runtime_create_program_from_file\" function\n\n", filename);
        return NULL;
    }
    // Создаем объект cl_program из кода ядра
    const char* source_ptr = (const char*)source_str; // Преобразуем char* в const char*
    program = clCreateProgramWithSource(context, 1, &source_ptr, &source_size, &err);
    switch (err)
    {
    case CL_SUCCESS:
        // Освобождаем память, выделенную для кода ядра
        free(source_str);
        return program;
    case CL_INVALID_CONTEXT:
        // Контекст - это недопустимый контекст.
        printf("\"context\" is not a valid context.\n\tProblem area: the \"cl_runtime_create_program_from_file\" function\n\n");
        free(source_str);
        return NULL;
    case CL_INVALID_VALUE:
        // Количество (второй параметр функции clCreateProgramWithSource) равно нулю, или если строки или любая запись в строках равна NULL.
        printf("Count is zero or if strings or any entry in strings is NULL.\n\tProblem area: the \"cl_runtime_create_program_from_file\" function\n\n");
        free(source_str);
        return NULL;
    case CL_OUT_OF_RESOURCES:
        // Не удается выделить ресурсы, требуемые реализацией OpenCL на устройстве.
        printf("There is a failure to allocate resources required by the OpenCL implementation on the device.\n\tProblem area: the \"cl_runtime_create_program_from_file\" function\n\n");
        free(source_str);
        return NULL;
    case CL_OUT_OF_HOST_MEMORY:
        // Не удается выделить ресурсы, требуемые реализацией OpenCL, на хосте.
        printf("There is a failure to allocate resources required by the OpenCL implementation on the host.\n\tProblem area: the \"cl_runtime_create_program_from_file\" function\n\n");
        free(source_str);
        return NULL;
    default:
        // Произошло что-то непонятное. Вывести соответствующее сообщение и вернуть управление.
        printf("Unknown error.\n\tProblem area: the \"cl_runtime_create_program_from_file\" function\n\n");
        free(source_str);
        return NULL;
    }
}

// Функция для создания (компиляция и компановка) исполняемого файла программы ядра
cl_int cl_runtime_build_program(cl_program program, cl_device_id device, const char* options)
{
    switch (clBuildProgram(program, 1, &device, options, NULL, NULL))
    {
    case CL_SUCCESS:
        // Программа успешно создана.
        return 0;
    case CL_INVALID_PROGRAM: 
        // program не является допустимым программным объектом.
        printf("\"program\" is not a valid program object.\n\tProblem area: the \"cl_runtime_build_program\" function\n\n");
        return -1;
    case CL_INVALID_VALUE:
        // device равен NULL.
        printf("\"device\" is NULLand num_devices is greater than zero, or if device_list is not NULL and num_devices is zero.\n\tProblem area: the \"cl_runtime_build_program\" function\n\n");
        return -1;
    case CL_INVALID_DEVICE:
        // device отсутствует в списке устройств, связанных с объектом program.
        printf("Any \"device\" in device_list is not in the list of devices associated with \"program\".\n\tProblem area: the \"cl_runtime_build_program\" function\n\n");
        return -1;
    case CL_INVALID_BINARY:
        // program создается с помощью clCreateProgramWithBinary, и на устройстве device не загружен действительный (доступный) двоичный файл программы.
        printf("\"program\" is created with clCreateProgramWithBinaryand devices listed in device_list do not have a valid program binary loaded.\n\tProblem area: the \"cl_runtime_build_program\" function\n\n");
        return -1;
    case CL_INVALID_BUILD_OPTIONS:
        // Параметры сборки, указанные в параметре options, недопустимы.
        printf("The build options specified by options are invalid.\n\tProblem area: the \"cl_runtime_build_program\" function\n\n");
        return -1;
    case CL_COMPILER_NOT_AVAILABLE:
        // Программа создается с помощью clCreateProgramWithILKHR, clCreateProgramWithSource или clCreateProgramWithIL, и компилятор недоступен, т.е. CL_DEVICE_COMPILER_AVAILABLE, указанный в таблице Запросов устройств, 
        // имеет значение CL_FALSE.
        printf("Program is created with clCreateProgramWithILKHR, clCreateProgramWithSource or clCreateProgramWithIL and a compiler is not available, i.e.CL_DEVICE_COMPILER_AVAILABLE specified in the Device Queries table is ");
        printf("set to CL_FALSE.\n\tProblem area: the \"cl_runtime_build_program\" function\n\n");
        return -1;
    case CL_BUILD_PROGRAM_FAILURE:
        // Произошел сбой при сборке исполняемого файла программы. Эта ошибка будет возвращена, если clBuildProgram не вернется до завершения сборки.
        printf("There is a failure to build the program executable.This error will be returned if clBuildProgram does not return until the build has completed.\n\tProblem area: the \"cl_runtime_build_program\" function\n\n");
        return -1;
    case CL_INVALID_OPERATION:
        // Сборка исполняемой программы для устройства с помощью предыдущего вызова clBuildProgram для program не завершена.
        // Или к программе прикреплены объекты ядра.
        // Или программа не была создана с помощью clCreateProgramWithSource, clCreateProgramWithIL или clCreateProgramWithBinary.
        printf("The build of a program executable for any of the devices listed in device_list by a previous call to clBuildProgram for \"program\" has not completed.\n");
        printf("Or there are kernel objects attached to program.\n");
        printf("Or \"program\" was not created with clCreateProgramWithSource, clCreateProgramWithIL or clCreateProgramWithBinary.\n\tProblem area: the \"cl_runtime_build_program\" function\n\n");
        return -1;
    case CL_OUT_OF_RESOURCES:
        // Не удается выделить ресурсы, требуемые реализацией OpenCL на устройстве.
        printf("There is a failure to allocate resources required by the OpenCL implementation on the device.\n\tProblem area: the \"cl_runtime_build_program\" function\n\n");
        return -1;
    case CL_OUT_OF_HOST_MEMORY:
        // Не удается выделить ресурсы, требуемые реализацией OpenCL, на хосте.
        printf("There is a failure to allocate resources required by the OpenCL implementation on the host.\n\tProblem area: the \"cl_runtime_build_program\" function\n\n");
        return -1;
    default:
        // Произошло что-то непонятное. Вывести соответствующее сообщение и вернуть управление.
        printf("Unknown error.\n\tProblem area: the \"cl_runtime_build_program\" function\n\n");
        return -1;
    }
}


/*   Работа с объектами ядер   */

// Функция для создания объекта ядра, готового для последующего запуска на рабочих единицах соответствующего устройства
cl_kernel cl_runtime_create_kernel(cl_program program, const char* fun_name)
{
    cl_int err;
    cl_kernel res = clCreateKernel(program, fun_name, &err);
    switch (err) {
    case CL_SUCCESS:
        // Объект ядра успешно создан.
        return res;
    case CL_INVALID_PROGRAM:
        // program не является допустимым программным объектом.
        printf("\"program\" is not a valid program object.\n\tProblem area: the \"cl_runtime_create_kernel\" function\n\n");
        return NULL;
    case CL_INVALID_PROGRAM_EXECUTABLE:
        // Для программы нет успешно созданного исполняемого файла.
        printf("There is no successfully built executable for \"program\".\n\tProblem area: the \"cl_runtime_create_kernel\" function\n\n");
        return NULL;
    case CL_INVALID_KERNEL_NAME:
        // fun_name не найдено в программе.
        printf("\"fun_name\" is not found in \"program\".\n\tProblem area: the \"cl_runtime_create_kernel\" function\n\n");
        return NULL;
    case  CL_INVALID_KERNEL_DEFINITION:
        // Если определение функции для __kernel function задано параметром fun_name, таким как количество аргументов, типы аргументов не одинаковы для всех устройств, для которых был создан исполняемый файл программы.
        printf("The function definition for __kernel function given by \"fun_name\" such as the number of arguments, the argument types are not the same for all devices for which the \"program\" executable has been built.\n");
        printf("\tProblem area : the \"cl_runtime_create_kernel\" function\n\n");
        return NULL;
    case CL_INVALID_VALUE:
        // fun_name is NULL.
        printf("\"fun_name\" is NULL.\n\tProblem area: the \"cl_runtime_create_kernel\" function\n\n");
        return NULL;
    case CL_OUT_OF_RESOURCES:
        // Не удается выделить ресурсы, требуемые реализацией OpenCL на устройстве.
        printf("There is a failure to allocate resources required by the OpenCL implementation on the device.\n\tProblem area: the \"cl_runtime_create_kernel\" function\n\n");
        return NULL;
    case CL_OUT_OF_HOST_MEMORY:
        // Не удается выделить ресурсы, требуемые реализацией OpenCL, на хосте.
        printf("There is a failure to allocate resources required by the OpenCL implementation on the host.\n\tProblem area: the \"cl_runtime_create_kernel\" function\n\n");
        return NULL;
    default:
        // Произошло что-то непонятное. Вывести соответствующее сообщение и вернуть управление.
        printf("Unknown error.\n\tProblem area: the \"cl_runtime_create_kernel\" function\n\n");
        return NULL;
    }
}

// Функция для отправки ядру дополнительной информации в виде параметров кода ядра
cl_int cl_runtime_set_kernel_param(cl_kernel kernel, cl_uint param_idx, size_t param_size, const void* param)
{
    switch (clSetKernelArg(kernel, param_idx, param_size, param)) {
    case CL_SUCCESS:
        // Программа успешно создана.
        return 0;
    case CL_INVALID_KERNEL:
        // ядро не является допустимым объектом ядра.
        printf("\"kernel\" is not a valid kernel object.\n\tProblem area: the \"cl_runtime_create_kernel\" function\n\n");
        return -1;
    case CL_INVALID_ARG_INDEX:
        // arg_index не является допустимым индексом аргумента.
        printf("\"param_idx\" is not a valid argument index.\n\tProblem area: the \"cl_runtime_create_kernel\" function\n\n");
        return -1;
    case CL_INVALID_ARG_VALUE:
        // Указанное значение arg_value не является допустимым значением.
        // Или аргумент представляет собой изображение, объявленное с помощью квалификатора read_only, а arg_value ссылается на объект image, созданный с помощью cl_mem_flags из CL_MEM_WRITE_ONLY, или если аргумент 
        // image объявлен с помощью квалификатора write_only, а arg_value ссылается на объект image, созданный с помощью cl_mem_flags из CL_MEM_READ_ONLY.
        printf("\"param\" specified is not a valid value.\nOr the argument is an image declared with the read_only qualifier and arg_value refers to an image object created with cl_mem_flags of CL_MEM_WRITE_ONLY or if the");
        printf(" image argument is declared with the write_only qualifierand arg_value refers to an image object created with cl_mem_flags of CL_MEM_READ_ONLY.\n\tProblem area : the \"cl_runtime_create_kernel\" function\n\n");
        return -1;
    case CL_INVALID_MEM_OBJECT:
        // Аргумент, объявленный как объект памяти, когда указанное значение arg_value не является допустимым объектом памяти.
        // Или аргумент, объявленный как изображение глубины, массив изображений глубины, изображение с несколькими выборками, массив изображений с несколькими выборками, изображение глубины с несколькими выборками или 
        // массив изображений глубины с несколькими выборками, когда указанное значение arg_value не соответствует правилам, описанным выше для объекта памяти глубины или аргумента объекта массива памяти.
        printf("An argument declared to be a memory object when the specified arg_value is not a valid memory object.\nOr an argument declared to be a depth image, depth image array, multi-sample image, multi-sample image");
        printf(" array, multi - sample depth image, or a multi - sample depth image array when the specified arg_value does not follow the rules described above for a depth memory object or memory array object argument.");
        printf("\n\tProblem area : the \"cl_runtime_create_kernel\" function\n\n");
        return -1;
    case CL_INVALID_SAMPLER:
        // Аргумент, объявленный как имеющий тип sampler_t, когда указанное arg_value не является допустимым объектом sampler.
        printf("An argument declared to be of type sampler_t when the specified arg_value is not a valid sampler object.\n\tProblem area: the \"cl_runtime_create_kernel\" function\n\n");
        return -1;
    case CL_INVALID_ARG_SIZE:
        // arg_size не соответствует размеру типа данных для аргумента, который не является объектом памяти, или если аргумент является объектом памяти и arg_size != sizeof(cl_mem), или если arg_size равен нулю, 
        // а аргумент объявлен с локальным квалификатором, или если аргумент является sampler и arg_size != sizeof(cl_sampler).
        printf("\"param_size\" does not match the size of the data type for an argument that is not a memory object or if the argument is a memory object and arg_size != sizeof(cl_mem) or if arg_size is zero and the");
        printf(" argument is declared with the local qualifier or if the argument is a samplerand arg_size != sizeof(cl_sampler).\n\tProblem area : the \"cl_runtime_create_kernel\" function\n\n");
        return -1;
    case CL_OUT_OF_RESOURCES:
        // Не удается выделить ресурсы, требуемые реализацией OpenCL на устройстве.
        printf("There is a failure to allocate resources required by the OpenCL implementation on the device.\n\tProblem area: the \"cl_runtime_set_kernel_param\" function\n\n");
        return -1;
    case CL_OUT_OF_HOST_MEMORY:
        // Не удается выделить ресурсы, требуемые реализацией OpenCL, на хосте.
        printf("There is a failure to allocate resources required by the OpenCL implementation on the host.\n\tProblem area: the \"cl_runtime_set_kernel_param\" function\n\n");
        return -1;
    default:
        // Произошло что-то непонятное. Вывести соответствующее сообщение и вернуть управление.
        printf("Unknown error.\n\tProblem area: the \"cl_runtime_set_kernel_param\" function\n\n");
        return -1;
    }
}

// Функция для работы с ядром, а именно: запуск оного
cl_int cl_runtime_enqueue_kernel(cl_kernel kernel, cl_command_queue queue, cl_uint dim, const size_t* gws, const size_t* lws)
{
    switch (clEnqueueNDRangeKernel(queue, kernel, dim, NULL, gws, lws, 0, NULL, NULL))
    {
    case CL_SUCCESS:
        // Ядро успешно запущено.
        return 0;
    case CL_INVALID_PROGRAM_EXECUTABLE:
        // Нет ни одного успешно созданного исполняемого файла программы, доступного для устройства, связанного с command_queue.
        printf("There is no successfully built program executable available for device associated with \"queue\".\n\tProblem area: the \"cl_runtime_enqueue_kernel\" function\n\n");
        return -1;
    case CL_INVALID_COMMAND_QUEUE:
        // command_queue не является допустимой командной очередью хоста.
        printf("\"queue\" is not a valid host command-queue.\n\tProblem area: the \"cl_runtime_enqueue_kernel\" function\n\n");
        return -1;
    case CL_INVALID_KERNEL:
        // ядро не является допустимым объектом ядра.
        printf("\"kernel\" is not a valid kernel object.\n\tProblem area: the \"cl_runtime_enqueue_kernel\" function\n\n");
        return -1;
    case CL_INVALID_CONTEXT:
        // Контекст, связанный с command_queue, и ядро не совпадают, или если контекст, связанный с command_queue, и события в event_wait_list не совпадают.
        printf("Context associated with command_queue and kernel are not the same or if the context associated with command_queue and events in event_wait_list are not the same.");
        printf("\n\tProblem area : the \"cl_runtime_enqueue_kernel\" function\n\n");
        return -1;
    case CL_INVALID_KERNEL_ARGS:
        // значения аргументов ядра не были указаны.
        printf("The kernel argument values have not been specified.\n\tProblem area: the \"cl_runtime_enqueue_kernel\" function\n\n");
        return -1;
    case CL_INVALID_WORK_DIMENSION:
        // dim не является допустимым значением (т. Е. Значением между 1 и CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS).
        printf("\"dim\" is not a valid value (i.e. a value between 1 and CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS).\n\tProblem area: the \"cl_runtime_enqueue_kernel\" function\n\n");
        return -1;
    case CL_INVALID_GLOBAL_WORK_SIZE:
        // gws равен NULL или если какое-либо из значений, указанных в global_work_size[0], ...global_work_size[work_dim - 1] равно 0.
        // Или любое из значений, указанных в global_work_size[0], ... global_work_size[work_dim - 1], превышает максимальное значение, которое может быть представлено параметром size_t на устройстве, на котором экземпляр 
        // ядра будет помещен в очередь.
        printf("\"gws\" is NULL or if any of the values specified in gws[0], ... ​gws[dim - 1] are 0. Or any of the values specified in gws[0], ... gws[dim - 1] exceed the maximum value representable by size_t on the device\n");
        printf(" on which the kernel-instance will be enqueued.");
        printf("\n\tProblem area : the \"cl_runtime_enqueue_kernel\" function\n\n");
        return -1;
    case CL_INVALID_GLOBAL_OFFSET:
        // Значение, указанное в gws + соответствующие значения в global_work_offset для любых измерений, больше максимального значения, представимого размером t на устройстве, на котором экземпляр ядра будет 
        // помещен в очередь, или если значение global_work_offset отличалось от NULL до версии 1.1.
        printf("The value specified in \"gws\" + the corresponding values in global_work_offset for any dimensions is greater than the maximum value representable by size t on the device on which the kernel-instance will");
        printf(" be enqueued, or if global_work_offset is non-NULL before version 1.1.\n\tProblem area: the \"cl_runtime_enqueue_kernel\" function\n\n");
        return -1;
    case CL_INVALID_WORK_GROUP_SIZE:
        // указан lws, который не соответствует требуемому размеру рабочей группы для ядра в исходном коде программы. Или указан lws, который не соответствует требуемому количеству подгрупп для ядра в исходном коде программы.
        // Или указан lws, и общее количество рабочих элементов в рабочей группе, вычисляемое как lws[0] × ... lws[dim - 1], больше значения, указанного CL_KERNEL_WORK_GROUP_SIZE в таблице запросов к объектному устройству 
        // ядра. Или размер рабочей группы должен быть равномерным, а local_work_size не равен NULL, не равен требуемому размеру рабочей группы, указанному в исходном коде ядра, или gws неравномерно делится на lws.
        printf("\"lws\" is specified and does not match the required work-group size for kernel in the program source. Or \"lws\" is specified and is not consistent with the required number of sub-groups for kernel in the ");
        printf("program source. Or \"lws\" is specified and the total number of work-items in the work-group computed as lws[0] × …​ lws[dim - 1] is greater than the value specified by CL_KERNEL_WORK_GROUP_SIZE in the Kernel ");
        printf("Object Device Queries table. Or the work-group size must be uniform and the \"lws\" is not NULL, is not equal to the required work-group size specified in the kernel source, or the \"gws\" is not evenly ");
        printf("divisible by the local_work_size.\n\tProblem area : the \"cl_runtime_enqueue_kernel\" function\n\n");
        return -1;
    case CL_INVALID_WORK_ITEM_SIZE:
        // количество рабочих элементов, указанное в любом из lws[0], ... lws[dim - 1], больше соответствующих значений, указанных в CL_DEVICE_MAX_WORK_ITEM_SIZES[0], ..., CL_DEVICE_MAX_WORK_ITEM_SIZES[dim - 1].
        printf("The number of work-items specified in any of lws[0], …​ lws[dim - 1] is greater than the corresponding values specified by CL_DEVICE_MAX_WORK_ITEM_SIZES[0], …​, CL_DEVICE_MAX_WORK_ITEM_SIZES[dim - 1].");
        printf("\n\tProblem area : the \"cl_runtime_enqueue_kernel\" function\n\n");
        return -1;
    case CL_MISALIGNED_SUB_BUFFER_OFFSET:
        // объект суббуфера указывается в качестве значения для аргумента, который является объектом буфера, и смещение, указанное при создании объекта суббуфера, не выровнено по значению CL_DEVICE_MEM_BASE_ADDR_ALIGN для 
        // устройства, связанного с очередью.
        printf("A sub-buffer object is specified as the value for an argument that is a buffer object and the offset specified when the sub-buffer object is created is not aligned to CL_DEVICE_MEM_BASE_ADDR_ALIGN value for");
        printf(" device associated with queue.\n\tProblem area : the \"cl_runtime_enqueue_kernel\" function\n\n");
        return -1;
    case CL_INVALID_IMAGE_SIZE:
        // объект изображения указывается в качестве значения аргумента, а размеры изображения (ширина изображения, высота, указанная или вычисляемая строка и / или шаг среза) не поддерживаются устройством, связанным с queue.
        printf("An image object is specified as an argument value and the image dimensions (image width, height, specified or compute row and/or slice pitch) are not supported by device associated with \"queue\".");
        printf("\n\tProblem area : the \"cl_runtime_enqueue_kernel\" function\n\n");
        return -1;
    case CL_IMAGE_FORMAT_NOT_SUPPORTED:
        // Объект изображения указан в качестве значения аргумента, а формат изображения (порядок каналов изображения и тип данных) не поддерживается устройством, связанным с очередью.
        printf("An image object is specified as an argument value and the image format (image channel order and data type) is not supported by device associated with \"queue\"."); 
        printf("\n\tProblem area : the \"cl_runtime_enqueue_kernel\" function\n\n");
        return -1;
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        // Не удается выделить память для хранилища данных, связанного с объектами изображения или буфера, указанными в качестве аргументов ядра.
        printf("There is a failure to allocate memory for data store associated with image or buffer objects specified as arguments to \"kernel\".\n\tProblem area: the \"cl_runtime_enqueue_kernel\" function\n\n");
        return -1;
    case CL_OUT_OF_RESOURCES:
        // Не удается поставить экземпляр выполнения kernel в очередь команд из-за нехватки ресурсов, необходимых для выполнения ядра. Например, явно указанный lws вызывает сбой при выполнении ядра из-за нехватки ресурсов, 
        // таких как регистры или локальная память. Другим примером может быть количество аргументов изображения только для чтения, используемых в ядре, превышающее значение CL_DEVICE_MAX_READ_IMAGE_ARGS для устройства, или
        // количество аргументов изображения только для записи и чтения-записи, используемых в ядре, превышает значение CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS для устройства, или количество сэмплеров, используемых в ядре, 
        // превышает значение CL_DEVICE_MAX_SAMPLERS для устройства. Или не удается выделить ресурсы, требуемые реализацией OpenCL на устройстве.
        printf("There is a failure to queue the execution instance of kernel on the command-queue because of insufficient resources needed to execute the kernel. For example, the explicitly specified \"lws\" causes a ");
        printf("failure to execute the kernel because of insufficient resources such as registers or local memory.Another example would be the number of read - only image args used in kernel exceed the ");
        printf("CL_DEVICE_MAX_READ_IMAGE_ARGS value for device or the number of write - only and read - write image args used in kernel exceed the CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS value for device or the number of ");
        printf("samplers used in kernel exceed CL_DEVICE_MAX_SAMPLERS for device.There is a failure to allocate resources required by the OpenCL implementation on the device.");
        printf("\n\tProblem area : the \"cl_runtime_set_kernel_param\" function\n\n");
        return -1;
    case CL_OUT_OF_HOST_MEMORY:
        // Не удается выделить ресурсы, требуемые реализацией OpenCL, на хосте.
        printf("There is a failure to allocate resources required by the OpenCL implementation on the host.\n\tProblem area: the \"cl_runtime_set_kernel_param\" function\n\n");
        return -1;
    default:
        // Произошло что-то непонятное. Вывести соответствующее сообщение и вернуть управление.
        printf("Unknown error.\n\tProblem area: the \"cl_runtime_set_kernel_param\" function\n\n");
        return -1;
        // Есть ещё варианты для обработки. Но в данной реализации они не учитываются
    }
}