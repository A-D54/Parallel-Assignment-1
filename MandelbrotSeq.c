#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


#define WIDTH 600
#define HEIGHT 600
#define MAX_ITER 256

typedef struct {
    double x, y;
} complex;

complex square(complex a) {
    complex result;
    result.x = a.x * a.x - a.y * a.y;
    result.y = 2 * a.x * a.y;
    return result;
}

complex addition(complex a, complex b) {
    complex result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}



int cal_pixel(complex c) {
    complex z = {0, 0};
    int n = 0;
    while (n < MAX_ITER && sqrt(z.x * z.x + z.y * z.y) < 2) {
        z = addition(square(z), c);
        n++;
    }
    return n;
}



int main() {
    unsigned char image[WIDTH][HEIGHT];
    double x_min = -2;
    double y_min = -1.5;
    double x_max = 1;
    double y_max = 1.5;


    double x_step = (x_max - x_min) / WIDTH;
    double y_step = (y_max - y_min) / HEIGHT;
    
    clock_t start_time = clock();
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            complex c = {x_min + i * x_step, y_min + j * y_step};
            int iterations = cal_pixel(c);
            image[i][j] = (unsigned char)(255 * iterations / (double)MAX_ITER);
        }
    }

    clock_t end_time = clock();
    double computation_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    FILE *fp = fopen("mandelbrott.pgm", "wb");
    fprintf(fp, "P5\n%d %d\n255\n", WIDTH, HEIGHT);
    fwrite(image, sizeof(image), 1, fp);
    fclose(fp);

    printf("Computation time: %f seconds\n", computation_time);

    return 0;
}
