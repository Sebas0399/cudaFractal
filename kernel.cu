#include <iostream>
#define WIDTH 1920

__constant__ unsigned int c_palette[16];

__device__ int diverge(double cx,double cy, int max_iters){
    int iter=0;
    double vx=cx;
    double vy=cy;
    double tx,ty;

    while (iter<max_iters && (vx*vx+vy*vy)<4){
        tx=vx*vx-vy*vy+cx;
        ty=2*vx*vy+cy;

        vx=tx;
        vy=ty;

        iter++;
    }
    if(iter>0 && iter<max_iters){
        return c_palette[iter%16];
    }

    return 0x000000;
}

__global__ void mandelbrot_kernel(unsigned  int* buffer, double x_start, double x_end,
                                  double y_start, double y_end,
                                  double dx, double dy,
                                  int max_iters){

    int id= blockDim.x * blockIdx.x + threadIdx.x;

    int i=id % WIDTH;
    int j=id / WIDTH;

    //cx+cxi numero complejo
    double cx=x_start+i*dx;
    double cy=y_end-j*dy;
    int color=diverge(cx,cy, max_iters);

    buffer[id]=color;
}

//Exportar
extern "C" void setPalette (unsigned int* h_palette){
    cudaMemcpyToSymbol(c_palette, h_palette, 16*sizeof(unsigned  int));
}

extern "C" void invoke_mandelbrot_kernel(
        int block_in_grid, int threads_per_block,
        unsigned  int* buffer, double x_start, double x_end,
        double y_start, double y_end,
        double dx, double dy,
        int max_iter){

    mandelbrot_kernel<<<block_in_grid, threads_per_block>>>(
            buffer, x_start, x_end, y_start, y_end,
            dx,dy,
            max_iter
    );
}

