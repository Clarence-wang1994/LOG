#pragma one
#include <string>
#include <mutex>
#include <windows.h>
#include <tchar.h>
#include <stdarg.h>

namespace LOG
{
    enum LOGLEVEL
    {
        LOG_LEVEL_NONE,
        LOG_LEVEL_ERROR,
        LOG_LEVEL_WARNING,
        LOG_LEVEL_DEBUG,
        LOG_LEVEL_INFO
    };

    enum LOGTARGET
    {
        LOG_TARGET_NONE = 0x00,
        LOG_TARGET_CONSOLE = 0x01,
        LOG_TARGET_FILE = 0x10
    };

    class LOG
    {
    public:
        /**
         * @brief Init
         *
         * @param log_level
         * @param log_target
         */
        void Init(LOGLEVEL log_level, LOGTARGET log_target);

        void UnInit();

        /**
         * @brief Create a File object
         *
         * @return int
         */
        int CreateFile();

        static LOG *GetInstance();

        LOGLEVEL GetLogLevel();

        void SetLogLevel(LOGLEVEL log_level);

        LOGTARGET GetLogTarget();

        void SetLogTarget(LOGTARGET log_target);

        static int WriteLog(LOGLEVEL log_level,
                            std::string file_name,
                            std::string function,
                            int line_number,
                            std::string format,
                            ...);

        static void OutputTarget();

    private:
        LOG();
        ~LOG();

        static LOG *Log;

        // mutux
        static std::mutex log_mutex;

        static std::string log_buffer;

        LOGLEVEL log_level;

        LOGTARGET log_target;

        static HANDLE mFileHandle;
    };

    LOG *LOG::Log = null;
    std::string LOG::log_buffer = "";
    HANDLE LOG::mFileHandle = INVALID_HANDLE_VALUE;
    std::mutex log_mutex;

    LOG::LOG()
    {
        Init(LOG_LEVEL_NONE, LOG_TARGET_FILE);
    }
    void Init(LOGLEVEL log_level, LOGTARGET log_target)
    {
        setLogLevel(log_level);
        setLogTarget(log_target);
        createFile();
    }

    void LOG::UnInit()
    {
        if (mFileHandle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(mFileHandle);
        }
    }

    LOG *LOG::GetInstance()
    {
        if (Log == NULL)
        {
            log_mutex.lock();
            if (Log == NULL)
            {
                Log = new LOG();
            }
            log_mutex.unlock();
        }
        return Log;
    }

    LOGLEVEL LOG::GetLogLevel()
    {
        return this->log_level;
    }

    void LOG::SetLogLevel(LOGLEVEL log_level_value)
    {
        this->log_level = log_level_value;
    }

    LOGTARGET LOG::GetLogTarget()
    {
        return this->log_target;
    }

    void LOG::SetLogTarget(LOGTARGET log_target_value)
    {
        this->log_target = log_target_value;
    }

    int LOG::CreateFile()
    {
        TCHAR file_directory[256];
        GetCurrentDirectory(256, file_directory);

        // 创建log文件路径
        TCHAR log_file_directory[256];
        _stprintf(log_file_directory, _T("%s\\Test\\"), file_directory);

        if (_taccess(log_file_directory, 0) == -1)
        {
            _tmkdir(log_file_directory);
        }

        TCHAR c_tmp_path[MAX_PATH] = {0};
        TCHAR *lpPos = NULL;
        TCHAR c_tmp = _T('\0');

        WCHAR psz_log_file_name[256];
        wcscat(log_file_directory, _T("test.log"));
        _stprintf(psz_log_file_name, _T("%s"), log_file_directory);
        mFileHandle = CreateFile(
            psz_log_file_name,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        if (mFileHandle == INVALID_HANDLE_VALUE)
        {
            return -1;
        }

        reture 0;
    }

    static int WriteLog(LOGLEVEL log_level,
                        std::string file_name,
                        std::string function,
                        int line_number,
                        std::string format,
                        ...)
    {
        int ret = 0;

        // 获取日期和时间
        char timeBuffer[100];
        ret = getSystemTime(timeBuffer);
        log_buffer += string(timeBuffer);

        // LOG级别
        char *log_level;
        if (log_level == LOG_LEVEL_DEBUG)
        {
            log_level = "Debug";
        }
        else if (log_level == LOG_LEVEL_INFO)
        {
            log_level = "INFO";
        }
        else if (log_level == LOG_LEVEL_WARNING)
        {
            log_level = "WARNING";
        }
        else if (log_level == LOG_LEVEL_ERROR)
        {
            log_level = "ERROR";
        }

        // [进程号][线程号][Log级别][文件名][函数名:行号]
        char locInfo[100];
        char *format2 = "[PID:%4d][TID:%4d][%s][%-s][%s:%4d]";
        ret = printfToBuffer(locInfo, 100, format2,
                             GetCurrentProcessId(),
                             GetCurrentThreadId(),
                             log_level,
                             file_name,
                             function,
                             line_number);
        log_buffer += string(locInfo);

        // 日志正文
        char logInfo2[256];
        va_list ap;
        va_start(ap, format);
        ret = vsnprintf(logInfo2, 256, format, ap);
        va_end(ap);

        log_buffer += string(logInfo2);
        log_buffer += string("\n");

        outputToTarget();

        return 0;
    }

    static void LOG::outputToTarget()
    {
        if (LOG::getInstance()->getLogTarget() & LOG_TARGET_FILE)
        {
            SetFilePointer(mFileHandle, 0, NULL, FILE_END);
            DWORD dwBytesWritten = 0;
            WriteFile(mFileHandle, log_buffer.c_str(), log_buffer.length(), &dwBytesWritten, NULL);
            FlushFileBuffers(mFileHandle);
        }
        if (LOG::getInstance()->getLogTarget() & LOG_TARGET_CONSOLE)
        {
            printf("%s", log_buffer.c_str());
        }

        // 清除buffer
        log_buffer.clear();
    }

} // namespace LOG
