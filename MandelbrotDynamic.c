

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
    
    int n, local_height;
    int pixel[WIDTH][HEIGHT];
    double scale_real = 3.0 / WIDTH;
    double scale_imag = 3.0 / HEIGHT;

    double end, comm_start, comm_end, comm_time;

    // Initialize MPI
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Determine the range of rows to be processed by this process
    local_height = HEIGHT / size;
    int start_row = rank * local_height;
    int end_row = (rank + 1) * local_height;
    if (rank == size - 1) {
        end_row = HEIGHT;
    }

    // Allocate memory for the local pixel array
    int local_pixel[local_height][WIDTH];

    // Perform the Mandelbrot set calculation for each pixel in the range of rows
    int i, j, k;
    double x0, y0, x, y, x_temp;
    double start;
    // Get the start timer
    start = MPI_Wtime();
    for (i = start_row; i < end_row; i++) {
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
                local_pixel[i-start_row][j] = 0;
            } else {
                local_pixel[i-start_row][j] = k;
            }
        }
    }
    
    // Get the end time
    end = MPI_Wtime();
    
    //start time of the communication
    comm_start = MPI_Wtime();
    MPI_Gather(local_pixel, local_height * WIDTH, MPI_INT, pixel, local_height * WIDTH, MPI_INT, 0, MPI_COMM_WORLD);
    
    //end time of the communication
    comm_end = MPI_Wtime();
    
    // Calculate the communication time
    comm_time = comm_end - comm_start;

    

    // Output the result to a PPM file if rank = 0(master)
    if (rank == 0) {
        FILE* fp = fopen("mandelbrot2.ppm", "wb");
        fprintf(fp, "P6\n%d %d\n255\n", WIDTH, HEIGHT);
        for (i = 0; i < WIDTH; i++) {
            for (j = 0; j < HEIGHT; j++) {
                n = pixel[i][j];
                fputc((n % 256), fp);
                fputc((n / 256 % 256), fp);
                fputc((n / 65536 % 256), fp);
            }
        }
        fclose(fp);

        // Output the calculation time
        printf("Calculation Time taken: %f seconds\n", end - start);
        
        // output the communication time
        printf("Communication time taken: %f seconds\n", comm_time);
        
        // output the total time
        printf("Total time taken: %f seconds\n", (end-start) + comm_time);
    }

    // Finalize MPI
    MPI_Finalize();

    return 0;
}
