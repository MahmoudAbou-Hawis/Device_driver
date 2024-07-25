#include "parser.hpp"
#include <iostream>

using namespace Parser;
int main()
{
    Parser::Parse_configuration parserConfig("/home/pi/Desktop/parse_tool/system.conf");
    if(parserConfig.get_status() != ERR::SUCCESS)
    {
        std:: cout << "ERROR\n";
    }

    return 0;
}
