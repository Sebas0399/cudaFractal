#include <iostream>
#include <fmt/core.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <cuda_runtime.h>

static unsigned  int* device_pixels_buffer;
static  unsigned  int* host_pixels_buffer;

#define IMAGE_WIDTH 1920
#define IMAGE_HEIGTH 1080

extern "C" void setPalette(unsigned  int* h_palette);

extern "C" void invoke_mandelbrot_kernel(int block_in_grid, int threads_per_block, unsigned int* buffer, double x_start, double x_end,
                                         double y_start, double y_end, double dx, double dy
        , int max_iter);


static const double x_start=-2;
static const double x_end=1;
static const double y_start=-1;
static const double y_end=1;

static int MAXITER =1000;

std::vector<unsigned int> colors_ramp = {
        0x000003FF,
        0x000003FF,
        0x0B0726FF,
        0x240B4EFF,
        0x410967FF,
        0x5D126EFF,
        0x781C6DFF,
        0x932567FF,
        0xAE305BFF,
        0xC73E4CFF,
        0xDC5039FF,
        0xED6825FF,
        0xF7850EFF,
        0xFBA40AFF,
        0xF9C52CFF,
        0xF2E660FF,
};

void mandelbrotCuda() {

    int threads_per_block = 1024;
    int blocks_in_grid = std::ceil(float (IMAGE_WIDTH*IMAGE_HEIGTH)/threads_per_block);

    double dx = (x_end-x_start)/IMAGE_WIDTH;
    double dy = (y_end-y_start)/IMAGE_HEIGTH;


    int* d_res= new int[IMAGE_HEIGTH*IMAGE_WIDTH];

    invoke_mandelbrot_kernel(blocks_in_grid, threads_per_block,
                             device_pixels_buffer, x_start, x_end,
                             y_start, y_end, dx, dy, MAXITER );

    cudaMemcpy(host_pixels_buffer, device_pixels_buffer, IMAGE_WIDTH*IMAGE_HEIGTH*4, cudaMemcpyDeviceToHost);

}



int main() {

    host_pixels_buffer = (unsigned  int*)malloc(IMAGE_WIDTH*IMAGE_HEIGTH*4);
    cudaMalloc(&device_pixels_buffer, IMAGE_WIDTH*IMAGE_HEIGTH*4);
    setPalette(colors_ramp.data());
    sf::RenderWindow window(sf::VideoMode(IMAGE_WIDTH, IMAGE_HEIGTH), "My window");


    sf::Text text;
    sf::Font font;
    {
        font.loadFromFile("arial.ttf");
        text.setFont(font);
        text.setCharacterSize(30);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);
        text.setPosition(10, 10);
    }



    mandelbrotCuda();

    sf::Texture texture;
    texture.create(IMAGE_WIDTH,IMAGE_HEIGTH);
    texture.update((sf::Uint8 *)host_pixels_buffer);

    sf::Sprite sprite;
    sprite.setTexture(texture);

    int frames =0;
    int fps = 0;
    sf::Clock clockFrames;
    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }
        if(MAXITER<100)
            MAXITER++;
        {
            mandelbrotCuda();

            texture.update((sf::Uint8 *)host_pixels_buffer);

            auto msg = fmt::format("Mode={}, Iterations={}, FPS: {}",
                                   "GPU", MAXITER, fps);
            text.setString(msg);
        }

        if(clockFrames.getElapsedTime().asSeconds()>=1.0){
            fps=frames;
            frames = 0;
            clockFrames.restart();
        }

        frames++;

        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.draw(text);
        window.display();

    }

    return 0;
}