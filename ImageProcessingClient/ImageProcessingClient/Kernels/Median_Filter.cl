__kernel void median_filter(__read_only image2d_t input_image,
							__write_only image2d_t output_image,
							sampler_t sampler,
							int filter_size)
{
	// Получаем координаты текущего пикселя
    int2 coord = (int2)(get_global_id(0), get_global_id(1));
	 // Получаем размеры входного изображения
    int width = get_image_width(input_image);
    int height = get_image_height(input_image);
	// Проверяем, находится ли текущий пиксель в пределах изображения
    if (coord.x < 0 || coord.x >= width || coord.y < 0 || coord.y >= height) {
        return;  // Выходим, если за пределами изображения
    }
	// Вычисляем границы фильтра
    int half_filter_size = filter_size / 2;
	// Создаем массив для хранения значений пикселей в окне фильтра
    float4 pixel_values[25]; // Максимальный filter_size=5x5
    int num_pixels = 0;
	// Перебираем пиксели в окне фильтра
    for (int y = -half_filter_size; y <= half_filter_size; y++) {
        for (int x = -half_filter_size; x <= half_filter_size; x++) {
            // Вычисляем координаты пикселя в окне
            int2 sample_coord = (int2)(coord.x + x, coord.y + y);
            // Проверяем границы, чтобы не выходить за пределы изображения
            if (sample_coord.x >= 0 && sample_coord.x < width &&
                sample_coord.y >= 0 && sample_coord.y < height) {
					// Считываем значение пикселя из входного изображения
					pixel_values[num_pixels++] = read_imagef(input_image, sampler, sample_coord);
            }
        }
    }
	// Сортируем значения пикселей (Пузырьковая сортировка)
    for (int i = 0; i < num_pixels - 1; i++) {
        for (int j = 0; j < num_pixels - i - 1; j++) {
            // Сравниваем по яркости (можно изменить на другой канал, например, R или G)
            if (pixel_values[j].x > pixel_values[j + 1].x) {  // Используем x-канал (R)
                float4 temp = pixel_values[j];
                pixel_values[j] = pixel_values[j + 1];
                pixel_values[j + 1] = temp;
            }
        }
    }
	// Выбираем медиану
    float4 median_value;
    if (num_pixels > 0) {
        if (num_pixels % 2 == 0) { // Четное количество пикселей
            // Усредняем два центральных значения
            median_value = (pixel_values[num_pixels / 2 - 1] + pixel_values[num_pixels / 2]) / 2.0f;
        } else { // Нечетное количество пикселей
            median_value = pixel_values[num_pixels / 2];
        }
    } else {
        median_value = (float4)(0.0f, 0.0f, 0.0f, 1.0f); // Черный цвет, если окно пустое
    }
	// Записываем медианное значение в выходное изображение
    write_imagef(output_image, coord, median_value);
}