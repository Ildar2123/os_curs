#include "FileManager.h"
const char *HELP_FLAG = "-h";
const char *MOVE_FLAG = "-mv";
const char *DELETE_FLAG = "-dl";
const char *COPY_FLAG = "-cp";
const char *LS_FLAG = "-ls";
const char *SZ_FLAG = "-sz";
const char *PROC_FLAG = "-pinf";

int FileManager::replaceFile(const char *oldName, const char *newName)
{
    return rename(oldName, newName);
}

int FileManager::removeFile(const char *fileName)
{
    return remove(fileName);
}

int FileManager::copyFile(std::experimental::filesystem::path fileToCopy, std::experimental::filesystem::path destination)
{
    try
    {
        fs::create_directories(destination);
        fs::copy_file(fileToCopy, destination / fileToCopy.filename(), fs::copy_options::overwrite_existing);
    }
    catch (std::exception &e)
    {
        std::cout << e.what();
        return -1;
    }
    return 0;
}

int FileManager::showContent(const char *dirName)
{
    DIR *dir = opendir(dirName);
    if (dir == nullptr)
    {
        std::cout << "Неудалось открыть каталог" << dirName << std::endl;
        return -1;
    }

    dirent *contentInfo;
    std::cout << "Содержимое каталога " << dirName << ":\n";
    while ((contentInfo = readdir(dir)) != nullptr)
    {
        if (strcmp(contentInfo->d_name, ".") != 0 && strcmp(contentInfo->d_name, "..") != 0)
            std::cout << contentInfo->d_name << std::endl;
    }
    closedir(dir);
    return 0;
}

unsigned long FileManager::size(std::string fileName)
{
    unsigned long contentSize = 0;

    struct stat fileInfo;
    if (stat(fileName.c_str(), &fileInfo) != 0)
    {
        std::string error = "Cannot get info about file \"" + fileName + "\"";
        throw error;
    }
    if (S_ISDIR(fileInfo.st_mode))
    {
        DIR *dir = opendir(fileName.c_str());
        if (dir == nullptr)
        {
            std::string error = "Cannot open dir \"" + fileName + "\"";
            throw error;
        }

        dirent *contentInfo;
        while ((contentInfo = readdir(dir)) != nullptr)
        {
            if ((strcmp(contentInfo->d_name, ".") != 0) && (strcmp(contentInfo->d_name, "..") != 0))
            {
                std::string pathToNestedFile = fileName + "/" + contentInfo->d_name;
                contentSize += size(pathToNestedFile);
            }
        }
        closedir(dir);
        return contentSize;
    }
    else
    {
        return fileInfo.st_size;
    }
}

int FileManager::showProcfsProcesses()
{
    DIR *dir = opendir("/proc/");
    if (dir == nullptr)
    {
        return -1;
    }
    dirent *contentInfo;

    // std::cout << std::setiosflags(std::ios::left);
    // std::cout << std::setw(20) << "COMM";
    // std::cout << std::setw(6) << "PID";
    // std::cout << std::setw(6) << "STATE";
    // std::cout << std::setw(6) << "PPID";
    // std::cout << std::setw(6) << "NICE" << std::endl;

    while ((contentInfo = readdir(dir)) != nullptr)
    {
        char *pid = getContent(contentInfo->d_name);
        if (atoi(pid) != 0)
        {
            std::string pidPath = "/proc/";
            pidPath += pid;

            std::ifstream pidInfo(pidPath + "/stat", std::ios_base::in);
            if (!pidInfo.is_open())
            {
                std::string error = "Cannot open file " + pidPath;
                throw error;
            }

            // std::cout << std::setw(20) << getPidInfo(pidInfo, 2);
            // std::cout << std::setw(6) << pid;
            // std::cout << std::setw(6) << getPidInfo(pidInfo, 1);
            // std::cout << std::setw(6) << getPidInfo(pidInfo, 1);
            // std::cout << std::setw(6) << getPidInfo(pidInfo, 15) << std::endl;
        }
        delete pid;
    }

    closedir(dir);
    return 0;
}

char *FileManager::getContent(char *str)
{
    char *buf = new char(64);
    char ch;
    int i = 0;

    do
    {
        ch = str[i];
        buf[i] = ch;
        ++i;
    } while (ch != '\\' && ch != '\0');

    buf[i] = '\0';

    return buf;
}

std::string FileManager::getPidInfo(std::ifstream &file, const int &offset)
{
    std::string buf;
    for (int i = 0; i < offset; i++)
    {
        buf.clear();
        file >> buf;
    }
    return buf;
}

int executeRequest(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Ошибка! Слишком мало аргументов -h для помощи\n";
        return -1;
    }

    char *flag = nullptr;
    for (int i = 1; i < argc; i++)
    {
        flag = argv[i];
        if (strcmp(flag, HELP_FLAG) == 0)
        {
            std::cout << "Авторы (ИС-841): Шамбуров Михаил\n"
                      << "Аргументы:\n"
                      << "1) -mv <oldName> <newName> - перемещает файл из oldName в newName\n"
                      << "Пример: ./file_manager -mv code.cpp src/code.cpp\n"
                      << "2) -dl <fileName> - удаляет файл fileName\n"
                      << "Пример: ./file_manager -dl src/code.cpp\n"
                      << "3) -cp <fileName> <destionation> - копирует файл fileName в destionation\n"
                      << "Пример: ./file_manager -cp src/code.cpp ../code.cpp\n"
                      << "4) -ls <fileName> - показывает содержимое каталога fileName\n"
                      << "Пример: ./file_manager -ls src/\n"
                      << "3) -sz <fileName> - показывае размер файла или каталога fileName\n"
                      << "Пример: ./file_manager -sz src/\n"
                      << "3) -pinf - показывает информацию о процессах из procfs\n"
                      << "Пример: ./file_manager -pinf\n";
            continue;
        }
        if (strcmp(flag, MOVE_FLAG) == 0)
        {
            if (argc < i + 2) // 2 означает, что присутствуют стрый путь и новый
                std::cout << "Ошибка! Недостаточно аргументов для флага " << MOVE_FLAG << std::endl;
            else
            {
                if (FileManager::replaceFile(argv[i + 1], argv[i + 2]) == 0)
                {
                    std::cout << "Файл успешно перенесен!\n";
                }
                else
                {
                    std::cout << "Ошибка! Неудалось переместить файл\n";
                };
                i += 2;
            }
            continue;
        }
        if (strcmp(flag, DELETE_FLAG) == 0)
        {
            if (argc < i + 1)
                std::cout << "Ошибка! Недостаточно аргументов для флага " << DELETE_FLAG << std::endl;
            else
            {
                if (FileManager::removeFile(argv[i + 1]) == 0)
                {
                    std::cout << "Файл успешно удален!\n";
                }
                else
                {
                    std::cout << "Ошибка! Неудалось удалить файл\n";
                };
                ++i;
            }
            continue;
        }
        if (strcmp(flag, COPY_FLAG) == 0)
        {
            if (argc < i + 2)
                std::cout << "Ошибка! Недостаточно аргументов для флага " << COPY_FLAG << std::endl;
            else
            {
                if (FileManager::copyFile(argv[i + 1], argv[i + 2]) == 0)
                {
                    std::cout << "Файл успешно копирован!\n";
                }
                else
                {
                    std::cout << "Ошибка! Неудалось скопировать файл\n";
                };
                i += 2;
            }
            continue;
        }
        if (strcmp(flag, LS_FLAG) == 0)
        {
            if (argc < i + 1)
                std::cout << "Ошибка! Недостаточно аргументов для флага " << LS_FLAG << std::endl;
            else
            {
                FileManager::showContent(argv[i + 1]);
                ++i;
            }
            continue;
        }
        if (strcmp(flag, SZ_FLAG) == 0)
        {
            if (argc < i + 1)
                std::cout << "Ошибка! Недостаточно аргументов для флага " << SZ_FLAG << std::endl;
            else
            {
                try
                {
                    std::cout << FileManager::size(argv[i + 1]) << " byte\n";
                }
                catch (std::string error)
                {
                    std::cout << error << std::endl;
                }
                ++i;
            }
            continue;
        }
        if (strcmp(flag, PROC_FLAG) == 0)
        {
            try
            {
                FileManager::showProcfsProcesses();
            }
            catch (std::string error)
            {
                std::cout << error << std::endl;
            }
            continue;
        }
        std::cout << "Ошибка! Неизвестный флаг " << flag << std::endl;
    }
    return 0;
}
