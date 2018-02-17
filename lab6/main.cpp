#include <iostream>
#include "VirtualFilesystem.h"

int main(int argc, char *argv[]) {
    std::string user_input;

    std::cout << "Welcome to" << std::endl;
    std::cout << " _______  _____" << std::endl;
    std::cout << "/____  / |  ___|" << std::endl;
    std::cout << "    / /  | |___" << std::endl;
    std::cout << "   / /   |  ___| ____" << std::endl;
    std::cout << "  / /    | |    / __/" << std::endl;
    std::cout << " / /___  | |   _\\ \\ " << std::endl;
    std::cout << "|______| |_|  /___/  version 1.0" << std::endl;
    print_options();

    std::getline(std::cin, user_input);
    std::transform(user_input.begin(), user_input.end(), user_input.begin(), ::toupper);
    handle_input(user_input);
    return 0;

    }


