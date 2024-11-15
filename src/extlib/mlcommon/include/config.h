#ifndef COMMON_CONFIGFILE_H_
#define COMMON_CONFIGFILE_H_

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

#define MAX_LINE_LENGTH 300

class CConfigFile
{

public:
    CConfigFile();
    CConfigFile(const char* config_file);
    ~CConfigFile();


private:
    std::ifstream file_stream_;
    bool is_open_ = false;
    enum
    {
        kIsComment,
        kIsSession,
        kIsValue,
        kNotFound
    };

private:

    short GetSession(const char* str, const char* session_name);

    short GetContent(const char* str, const char* value_name, std::string& content);

public:

    bool Open(const char* config_file);

    void Close();

    short GetValue(const char* session, const char* value_name, std::string& content);

    bool IsOpen() { return is_open_; };

};

#endif //COMMON_CONFIGFILE_H_