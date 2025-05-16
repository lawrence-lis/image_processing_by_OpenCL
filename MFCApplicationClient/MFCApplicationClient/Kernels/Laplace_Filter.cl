__kernel void LaplaceKernel(__read_only image2d_t input_image,
                             __write_only image2d_t output_image,
                             int width,
                             int height,
                             int count_channels, // Должно быть 4
                             sampler_t sampler,
                             __constant int* laplacianKernel)
{
	int x = get_global_id(0);
    int y = get_global_id(1);
	
	float4 outputPixel = (float4)(0.0f, 0.0f, 0.0f, 1.0f);
	
	for (int c = 0; c < 3; c++)
    {
		float laplacianValue = 0.0f;
		
		for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
				int2 coord = (int2)(x + j, y + i);
                float4 pixel = read_imagef(input_image, sampler, coord);
                laplacianValue += laplacianKernel[(i + 1) * 3 + (j + 1)] * pixel[c];
			}
		}
		outputPixel[c] = laplacianValue;
	}
	int2 coord = (int2)(x, y);
    float4 inputPixel = read_imagef(input_image, sampler, coord);
    outputPixel[3] = inputPixel[3];
	
	write_imagef(output_image, (int2)(x, y), outputPixel);
}