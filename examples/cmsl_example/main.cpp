#include <iostream>
#include <string>

extern std::string get_message();

int main()
{
    std::cout << get_message() << '\n';
}

