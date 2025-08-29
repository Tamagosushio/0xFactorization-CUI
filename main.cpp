#include <windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <io.h>
#include <conio.h>
#include <stdio.h>

#include <chrono>
#include <random>
#include <thread>
#include "Factorization.hpp"
#include <fcntl.h>
#include <iostream>

int main() {
    SetConsoleOutputCP(65001);
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdOut, &mode);
    SetConsoleMode(hStdOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    Factorization::Game game{};
    constexpr size_t fps = 60;
    printf("\033[?25l");
    while (game.Update()) {
        game.Draw();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps));
    }
    printf("\x1b[?25h");
}

