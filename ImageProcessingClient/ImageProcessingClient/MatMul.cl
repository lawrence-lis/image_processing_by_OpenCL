// Ширина матрицы А равна высоте матрицы B для корректного умножения матриц
__kernel void simpleMultiply(
	__global float* outputC,
	int widthA,
	int heightA,
	int widthB,
	int heightB,
	__global float* inputA,
	__global float* inputB) {
	// Получить глобальную позицию в направлении Y
	int row = get_global_id(1);
	// Получить глобальную позицию в направлении X
	int col = get_global_id(0);
	
	float sum = 0.0f;
	
	// Вычислить результат для одного элемента матрицы C
	for (int i = 0; i < widthA; i++) {
		sum += inputA[row * widthA + i] * inputB[i * widthB + col];
	}
	outputC[row * widthB + col] = sum;
}