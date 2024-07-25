#include "parser.hpp"
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <filesystem>

using namespace Parser;


Parse_configuration::Parse_configuration(const std::string& conf_path) : path(conf_path), status(ERR::FAILED)
{
    headers["LEDS"] = true;
    status = parser_read();
}

ERR Parse_configuration::get_status()
{
    return status;
}


ERR Parse_configuration::parser_read()
{
    try
    {
        std::ifstream file(this->path);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open the configuration file: " + path);
        }

        std::string line;
        while (std::getline(file, line))
        {
            if (parse_line(line) != ERR::SUCCESS)
            {
                throw std::runtime_error("Failed to parse line: " + line);
            }
        }
        std::string command = "sudo insmod Led.ko GPIOS=";
        for(int i = 0 ; i < gpios.size() - 1; i++)
        {
            command += gpios[i];
            command+= ',';
        }
        command += gpios[gpios.size()-1];
        std::string command2 = "sudo rmmod Led";

        std::string filePath = "/dev/LED0";
    
        std::string permission = "sudo chmod 666 /dev/LED";
        if (std::filesystem::exists(filePath)) 
        {
            system(command2.c_str());
        } 
        system(command.c_str());
        for(int i = 0 ; i < gpios.size() ; i++)
        {
            char ch = i + '0';
            std::string temp = permission + ch;
            system(temp.c_str());
        }
        file.close();
        status = ERR::SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << "An error occurred: " << e.what() << '\n';
        status = ERR::FAILED;
    }
    return status;
}


ERR Parse_configuration::parse_line(const std::string &line)
{
    bool endofKey = false;
    for(auto &ch : line)
    {
        if(ch == '[' || ch == '{')
        {
            if(ch == '[' && key != ":" || (!stk.empty() && ch == stk.top()))
            {
                return ERR::FAILED;
            }
            key = "";
            this->stk.push(ch);
        }
        else if((ch == ']' || ch == '}' ) )
        {
            if(stk.empty()) return ERR::FAILED;
            if((this->stk.top() == '[' && ch == ']') || (this->stk.top() == '{' && ch == '}') )
            {
                this->stk.pop();
                if(ch == ']')
                {
                    if(freq[key] == false)
                    {
                        freq[key] = true;
                        gpios.push_back(key);
                    }
                    else
                    {

                        return ERR::FAILED;
                    }
                }
                continue;
            }
            else
            {
                return ERR::FAILED;
            }

        }
        else 
        {
            if(ch ==' ' || ch == '\n') continue;
      
            if(ch == '"')
            {
                if(endofKey == true)
                {
                    endofKey = false;
                }
                else
                {
                    key = "";
                    endofKey = true;
                }
                continue;
            }
            if(ch == ':' && endofKey == false)
            {
                if(stk.top() == '{')
                {
                    if(headers[key] != true || freq[key] == true)
                    {
                        return ERR::FAILED;
                    }
                    freq[key] = true;
                }
                else if(stk.top() == '[')
                {
                    if(freq[key] == false)
                    {
                        freq[key] = true;
                    }
                    else
                    {

                        return ERR::FAILED;
                    }
                }

                key = ":";
                continue;
            }
            else
            {
                if(ch == ':')
                {
                    return ERR::FAILED;
                }
            }
        }
        if(ch == ',')
        {
            if(stk.top() == '{')
            {
                flag_comma = true;
                continue;
            }
            if(freq[key] == false)
            {
                freq[key] = true;
                gpios.push_back(key);
            }
            else
            {

                return ERR::FAILED;
            }
            continue;
        }
        if(key == ":") key = "";
        key += ch;
    }
    return ERR::SUCCESS;
}
