#ifndef out_h
#define out_h
#include <fstream>
#include <ctime>
#include <chrono>
#include <iostream>
std::ofstream fout ("log.txt");
//std::ostream fout; 
//#define fout std::cout
std::string time(){
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    
    std::tm* local_time = std::localtime(&now_c);
    
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S] ", local_time);
    
    return std::string(buffer);
}
#endif
