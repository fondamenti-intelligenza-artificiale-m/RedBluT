#include "Engine.hpp"

int main(int argc, char *argv[]) {
    Engine engine;
    if (argc != 4)
    {
        perror("Uso: ./RedBluT <color> <timer> <ip_referee>");
        exit(EXIT_FAILURE);
    }

    std::string color = argv[1];
    for (char &c : color)
        c = std::toupper(static_cast<unsigned char>(c));
    printf("Color: %s\n", color.c_str());
    int seconds;

    try
    {
        seconds = std::stoi(argv[2]);
    }
    catch (const std::exception &e)
    {
        perror("Error: the timer must be an integer.");
        exit(EXIT_FAILURE);
    }
    std::string ip_referee = argv[3];

    engine.go(color, seconds, ip_referee);
    return 0;
}
