#include <iostream>
#include <thread>
#include "CommonObject.h"
#include "DrawThread.h"
#include "DownloadThread.h"

int main()
{
    CommonObjects common; // Create a common object to share data between threads
    DrawThread draw;
    auto draw_th = std::jthread([&] { draw(common); }); // Launch draw thread

    DownloadThread down;
    auto down_th = std::jthread([&] { down(common); }); // Launch download thread

    down.SetUrl("http://...."); // Set URL for downloading movie data
    std::cout << "running...\n"; // Indicate the application is running

    // Wait for the download and draw thread to finish
    down_th.join();
    draw_th.join();
}
