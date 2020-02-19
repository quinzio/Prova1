#pragma once
#include <string>
#include <iostream>

inline std::string getCurrentDateTime(std::string s);
inline void Logger(std::string logMsg);


inline std::string getCurrentDateTime(std::string s) {
    time_t now = time(0);
    struct tm  tstruct;
    char  buf[80] = { 0 };
    localtime_s(&tstruct, &now);
    if (s == "now")
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    else if (s == "date")
        strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return std::string(buf);
};
inline void Logger(std::string logMsg) {

    std::string filePath = "log/log_" + getCurrentDateTime("date") + ".txt";
    std::string now = getCurrentDateTime("now");
    std::ofstream ofs(filePath.c_str(), std::ios_base::out | std::ios_base::app);
    //ofs << now << '\t' << logMsg << " " << __FILE__ << " " << std::to_string(__LINE__) << '\n';
    ofs << now << '\t' << logMsg << '\n';
    ofs.close();
}