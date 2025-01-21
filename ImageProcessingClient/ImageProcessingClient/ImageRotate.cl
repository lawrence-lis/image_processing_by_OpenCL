__kernel void img_rotate(
	__global float* dest_data, __global float* src_data,
	int W, int H, // Размерность изображения
	float sinTheta, float cosTheta ) // Параметры поворота
{
	// Рабочий элемент получает свой индекс в индексном пространстве
	const int ix = get_global_id(0);
	const int iy = get_global_id(1);
	// Рассчитать местоположение данных для перехода к декомпозиции выходных данных (ix,iy), как указано выше
	float xpos = ((float)ix) * cosTheta + ((float)iy) * sinTheta;
	float ypos = -1.0 * ((float)ix) * sinTheta + ((float)iy) * cosTheta;
	// Связанная проверка
	if(((int)xpos >= 0) && ((int)xpos < W) &&
		((int)ypos >= 0) && ((int)ypos <  H))
	{
		// Считывает (ix,iy) src_data и сохраняет по адресу (xpos,ypos) в dest_data
		// В этом случае, поскольку мы ориентируемся на источник и перевода нет, мы знаем, что (xpos,ypos) 
		// будут уникальными для каждого входного сигнала (ix,iy), и поэтому каждый рабочий элемент может 
		// записывать свои результаты независимо
		dest_data[(int)ypos * W + (int)xpos] = src_data[iy * W + ix];
	}
}

