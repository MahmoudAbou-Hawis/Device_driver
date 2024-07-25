#ifndef __PARSER__
#define __PARSER__

#include <string>
#include <stack>
#include <map>
#include <vector>

namespace Parser
{
    enum class ERR
    {
        SUCCESS ,
        FAILED
    };

    class Parse_configuration
    {  
        private:
            std::string path;
            std::stack<char>stk;
            std::map<std::string,std::vector<std::string>>mp;
            std::map<std::string,bool> headers;
            std::map<std::string,bool> freq;
            std::vector<std::string>gpios;
            std::string key = "";
            bool flag_comma = false;
            ERR status;
            ERR parser_read();
            ERR parse_line(const std::string &line);
        public:
            explicit Parse_configuration(const std::string &conf_path);
            ERR get_status();
    };
};

#endif