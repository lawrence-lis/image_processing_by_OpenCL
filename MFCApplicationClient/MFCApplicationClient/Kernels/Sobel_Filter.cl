__kernel void SobelKernel(	__read_only image2d_t input_image,
							__write_only image2d_t output_image,
							int width,
							int height,
							int count_channels,
							sampler_t sampler,
							__constant int* sobelGx,
							__constant int* sobelGy
)
{
	int x = get_global_id(0);
    int y = get_global_id(1);
	
	float4 outputPixel = (float4)(0.0f, 0.0f, 0.0f, 1.0f);
	
	for (int c = 0; c < count_channels - 1; c++)
	{
		float Gx = 0.0f;
		float Gy = 0.0f;
		
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				int2 coord = (int2)(x + j, y + i);
				float4 pixel = read_imagef(input_image, sampler, coord);
				Gx += sobelGx[(i + 1) * 3 + (j + 1)] * pixel[c];
                Gy += sobelGy[(i + 1) * 3 + (j + 1)] * pixel[c];
			}
		}		
		float magnitude = sqrt(Gx * Gx + Gy * Gy);
		
		outputPixel[c] = magnitude;
	}
	// Копируем альфа-канал из входного изображения в выходное
    int2 coord = (int2)(x, y);
	float4 inputPixel = read_imagef(input_image, sampler, coord);
	outputPixel[3] = inputPixel[3];
	
	write_imagef(output_image, (int2)(x, y), outputPixel);
}