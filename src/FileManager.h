#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <string>
#include <iostream>
#include <experimental/filesystem>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <fstream>
#include <pwd.h>
#include <sys/types.h>
#include <iomanip>

namespace fs = std::experimental::filesystem;

class FileManager
{
private:
    static char *getContent(char *str);
    static std::string getPidInfo(std::ifstream &file, const int &offset);

public:
    static int replaceFile(const char *oldName, const char *newName);
    static int removeFile(const char *fileName);
    static int copyFile(fs::path fileToCopy, fs::path destination);
    static int showContent(const char *dirName);
    static unsigned long size(std::string fileName);
    static int showProcfsProcesses();
};

int executeRequest(int argc, char *argv[]);

#endif