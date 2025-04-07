__kernel void gaussian_filter(	__read_only image2d_t srcImg,
								__write_only image2d_t dstImg,
								__global const float *mask,
								int width, int height, int count_channels,
								int filter_size,
								sampler_t sampler)
{	
	// Получаем координаты текущего пикселя
	int x = get_global_id(0);
    int y = get_global_id(1);
    
	if (x < 0 || x >= width || y < 0 || y >= height)
        return;
		
	// Вычисляем границы фильтра
    int half_filter_size = filter_size / 2;
	float sum[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	int2 coord;
	
	for (int ky = -half_filter_size; ky <= half_filter_size; ++ky) {
		for (int kx = -half_filter_size; kx <= half_filter_size; ++kx) {
			coord.x = x + kx;
            coord.y = y + ky;
			
			int kernelIndex = (ky + half_filter_size) * filter_size + (kx + half_filter_size);
            float kernelValue = mask[kernelIndex];
			
			float4 pixelValue = read_imagef(srcImg, sampler, coord);
			
			sum[0] += pixelValue.x * kernelValue;
			if (count_channels > 1) {
				sum[1] += pixelValue.y * kernelValue;
				sum[2] += pixelValue.z * kernelValue;
				if (count_channels == 4) sum[3] += pixelValue.w * kernelValue;
			}
		}
	}
	
	int2 outputCoord;
    outputCoord.x = x;
    outputCoord.y = y;
	
	float4 outputPixel;
    outputPixel.x = sum[0];
	if (count_channels > 1) {
		outputPixel.y = sum[1];
        outputPixel.z = sum[2];
		if (count_channels == 4) outputPixel.w = sum[3];
		else outputPixel.w = 1.0f;
	}
	else {
		outputPixel.y = outputPixel.z = 0.0f;
		outputPixel.w = 1.0f;
	}
	
	write_imagef(dstImg, outputCoord, outputPixel);
}