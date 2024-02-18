


/* 
	Author Adam Dandan
	Done on: February 18th, 2024
	Mandelbrot Dynamic task assignment
*/





#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define WIDTH 600
#define HEIGHT 600
#define MAX_ITERATIONS 256

int main(int argc, char** argv) {

    double x0, y0, x, y, x_temp;
    int i, j, k, n, rank, size;
    int color[WIDTH][HEIGHT];
    double scale_real = 3.0 / WIDTH;
    double scale_imag = 3.0 / HEIGHT;

    double start, end;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Get the start timer
    start = MPI_Wtime();

    // Perform the Mandelbrot set calculation for each pixel
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            x0 = i * scale_real - 1.5;
            y0 = j * scale_imag - 1.5;
            x = 0.0;
            y = 0.0;
            k = 0;

            while (k < MAX_ITERATIONS && (x*x + y*y) < 4.0) {
                x_temp = x*x - y*y + x0;
                y = 2*x*y + y0;
                x = x_temp;
                k++;
            }

            if (k == MAX_ITERATIONS) {
                color[i][j] = 0;
            } else {
                color[i][j] = k;
            }
        }
    }
    
    // Get the end time
    end = MPI_Wtime();

    // Output the result to a PPM file
    if (rank == 0) {
        FILE* fp = fopen("mandelbrot1.ppm", "wb");
        fprintf(fp, "P6\n%d %d\n255\n", WIDTH, HEIGHT);
        for (i = 0; i < WIDTH; i++) {
            for (j = 0; j < HEIGHT; j++) {
                n = color[i][j];
                fputc((n % 256), fp);
                fputc((n / 256 % 256), fp);
                fputc((n / 65536 % 256), fp);
            }
        }
        fclose(fp);

        

        // Output the time taken
        printf("Time taken: %f seconds\n", end - start);
    }

    // Finalize MPI
    MPI_Finalize();

    return 0;
}
