#include "Log.h"

#include <unistd.h>

FILE* data_log;
FILE* run_log;

int open_logs()
{
    data_log = fopen("data_log.csv", "a");
    if (data_log == NULL)
    {
        perror("fopen");
        return -1;
    }

    run_log = fopen("run_log.txt", "a");
    if (run_log == NULL)
    {
        perror("fopen");
        return -1;
    }

    return 0;
}

int redirect_stderr()
{
    if (dup2(fileno(run_log), fileno(stderr)) == -1)
    {
        perror("dup2");
        return -1;
    }

    return 0;
}

int log_data(const char* name, const char* data)
{
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    if (fprintf(data_log, "%s, %s, %s\n", asctime(timeinfo), name, data) < 0)
    {
        perror("fprintf");
        return -1;
    }
    
    return 0;
}

int log_msg(const char* msg)
{
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    if (fprintf(run_log, "%s, %s, %s\n", asctime(timeinfo), msg) < 0)
    {
        perror("fprintf");
        return -1;
    }

    return 0;
}

int close_logs()
{
    if (fclose(run_log) == -1)
    {
        perror("fclose");
        fclose(data_log);
        return -1;
    }

    if (fclose(data_log) == -1)
    {
        perror("fclose");
        return -1;
    }

    return 0;
}