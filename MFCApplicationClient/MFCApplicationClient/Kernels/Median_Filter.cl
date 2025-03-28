__kernel void median_filter(__read_only image2d_t input_image,
							__write_only image2d_t output_image,
							int width,
                            int height,
							int count_channels,
							sampler_t sampler,
							int filter_size)
{
	// Получаем координаты текущего пикселя
    int2 coord = (int2)(get_global_id(0), get_global_id(1));
	// Вычисляем границы фильтра
    int half_filter_size = filter_size / 2;
	// Создаем массив для хранения значений пикселей в окне фильтра
	#define MAX_FILTER_SIZE 5
    float4 pixel_values[MAX_FILTER_SIZE * MAX_FILTER_SIZE];
    int num_pixels = 0;
	// Перебираем пиксели в окне фильтра
    for (int y = -half_filter_size; y <= half_filter_size; y++) {
        for (int x = -half_filter_size; x <= half_filter_size; x++) {
            // Вычисляем координаты пикселя в окне
            int2 sample_coord = (int2)(coord.x + x, coord.y + y);
            // Используем clamp для обработки границ
            int2 clamp_coord = clamp(sample_coord, (int2)(0, 0), (int2)(width - 1, height - 1));
			// Считываем значение пикселя из входного изображения
			pixel_values[num_pixels++] = read_imagef(input_image, sampler, sample_coord);            
        }
    }
	// Вычисляем медиану для каждого канала
    float4 median_value;
	for (int channel = 0; channel < count_channels; channel++) {
		// Создаем массив для хранения значений одного канала
        float channel_values[MAX_FILTER_SIZE * MAX_FILTER_SIZE];
        for (int i = 0; i < num_pixels; i++) {
            channel_values[i] = pixel_values[i][channel];
		}
		// Сортируем значения пикселей (Пузырьковая сортировка)
		for (int i = 0; i < num_pixels - 1; i++) {
			for (int j = 0; j < num_pixels - i - 1; j++) {
				// Сравниваем по яркости
				if (channel_values[j] > channel_values[j + 1]) {
                    float temp = channel_values[j];
                    channel_values[j] = channel_values[j + 1];
                    channel_values[j + 1] = temp;
                }
			}
		}
		// Выбираем медиану для текущего канала
		if (num_pixels > 0) {
			if (num_pixels % 2 == 0) { // Четное количество пикселей
				// Усредняем два центральных значения
				median_value[channel] = (channel_values[num_pixels / 2 - 1] + channel_values[num_pixels / 2]) / 2.0f;
			}
			else {// Нечетное количество пикселей
				median_value[channel] = channel_values[num_pixels / 2];
			}
		}
		else {
			median_value[channel] = 0.0f;
		}
	}
	// Записываем медианное значение в выходное изображение
    write_imagef(output_image, coord, median_value);
}