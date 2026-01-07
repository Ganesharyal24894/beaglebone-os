#include <iostream>

class Console
{
    public:

    Console(void)
    {
        std::cout << "*******Console Started*******" << std::endl;
        std::cout << "*******USER DISPLAY STARTED*******" << std::endl;
    }

    ~Console()
    {

    }
};

int main(void)
{
    Console BeagleBoneConsole;
    return 0;
}