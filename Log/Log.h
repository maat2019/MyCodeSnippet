#pragma once

#include <string>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
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

    class Locker
    {
    public:
        virtual ~Locker() {};
        void Lock() = 0;
        void Unlock() = 0;
    };

    template<class Locker>
    class LogHandler
    {
        Locker locker_;
        class LockerHolder
        {
            Locker& locker_;
        public:
            LockerHolder(Locker& locker) : locker_(locker)
            {
                locker_.Lock();
            }

            ~LockerHolder(Locker& locker)
            {
                locker_.Unlock();
            }
        };
    public:
        void Handle(std::string message, LogLevel)
        {
           LockerHolder lock;
           lock;
           handle(message);
        }

        LogHandler()
        {
        }

        ~LogHandler()
        {
        }

    protected:
        virtual void handle(std::string message) = 0;
    };

    class ConsoleLogHandler : public LogHandler
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

    protected:
        virtual void handle(std::string message)  
        {
            std::cout << message;
            std::cout << std::flush;
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

    class FileLogHandler : public LogHandler
    {
    public:
        FileLogHandler()
        {
            setlocale(LC_ALL, "chs");

            char lpLogName[MAX_PATH] = {0};
            HMODULE hModule = NULL;
            GetModuleFileName(hModule, lpLogName, MAX_PATH);
            PathRenameExtension(lpLogName, ".log");

            out.open(lpLogName, std::ios::app | std::ios::binary);

        }

        virtual ~FileLogHandler()
        {
            out.close();
        }

    protected:
        void handle(std::string message)  
        {
            write(message);
        }


    private:
        void write(std::string message)
        {
            out.write(message.c_str(), message.length());
            out << std::flush;
        }

    private:
        std::ofstream out;
    };

    template<typename Handler = FileLogHandler>
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
};

#define ConsoleLogger Log::Logger<Log::ConsoleLogHandler>
#define FileLogger Log::Logger<Log::FileLogHandler>

