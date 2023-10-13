#include "include/Log.h"
#include <iostream>
#include <windows.h>
#include <minwinbase.h>

int main(int argc, char **argv)
{
    std::string name = "E:\\Program Files (x86)\\Source\\Log\\log.txt";
    HANDLE handle;
    LPWIN32_FIND_DATA fd;
    handle = FindFirstFile(name.c_str(), fd);
    if (handle != INVALID_HANDLE_VALUE)
        std::cout << "ok" << std::endl;
    else
        std::cout << "no" << std::endl;
    std::cin.get();
}