#pragma once

#include <string>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <shlwapi.h>
#include <clocale>

#pragma comment(lib, "shlwapi.lib")

#pragma warning(disable: 4996) //  This function or variable may be unsafe.

namespace Log
{
    enum LogLevel
    {
        Debug,
        Info,
        Warning,
        Error,
        Critical,
    };

    class ConsoleLogHandler
    {
    public:
        ConsoleLogHandler()
        {
            init();
        }
        
        ~ConsoleLogHandler()
        {
            // FreeConsole();
        }

    public:
        void Handle(std::string message, LogLevel)  
        {
            std::cout << message;
        }

    private:
        void init()
        {
#if !defined(_CONSOLE)
            AllocConsole();
            freopen("CONOUT$", "w+", stdout);

            SetConsoleTitle("Log Recorder");

            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            CONSOLE_SCREEN_BUFFER_INFO csbiInfo = {0};
            GetConsoleScreenBufferInfo(hConsole, &csbiInfo);

            SMALL_RECT rect = csbiInfo.srWindow;
            COORD coord = {200, 600};
            rect.Right = 599;

            SetConsoleScreenBufferSize(hConsole, coord);
            SetConsoleWindowInfo(hConsole, FALSE, &rect);
            CloseHandle(hConsole);
#endif // !defined(_CONSOLE)
        }
    };


    template<typename Handler = ConsoleLogHandler>
    class Logger {
        std::ostringstream stringstream_;
        LogLevel level_;

    public:
        Logger(std::string prefix, LogLevel level) : level_(level) {
            if(level_ >= getCurrLevel()) {
            stringstream_ << "(" << timestamp() << ") [" << prefix << "] ";
            }

        }

        ~Logger() {
            if(level_ >= getCurrLevel()) {
                stringstream_ << std::endl;
                getHandler()->Handle(stringstream_.str(), level_);
            }
        }

        template <typename T>
        Logger& operator<<(T const &value) {

            if(level_ >= getCurrLevel()) {
                stringstream_ << value;
            }
            return *this;
        }

        static void setCurrLevel(LogLevel level) {
            getCurrLevelRef() = level;
        }

        static LogLevel getCurrLevel() {
            return getCurrLevelRef();
        }

    private:
        static std::string timestamp()
        {
            time_t rawtime;
            struct tm * timeinfo;
            char buffer[80] = {0};

            time (&rawtime);
            timeinfo = localtime (&rawtime);

            size_t sz = strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
            return std::string(buffer, buffer+sz);
        }

        static LogLevel& getCurrLevelRef()
        {
            static LogLevel currLevel = (LogLevel)Debug;
            return currLevel;
        }
        static Handler*& getHandler()
        {
            static Handler handler;
            static Handler* currHandler = &handler;
            return currHandler;
        }
    };
}
#define Logger Log::Logger<>
