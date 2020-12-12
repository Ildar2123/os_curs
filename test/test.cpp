#include "ctest.h"
#include "../src/FileManager.h"

CTEST(FileManager, replaceFile)
{

    int expected = 0;
    int result = FileManager::replaceFile("test/test_resource/test1.txt", "test/test_resource/dest/test1.txt");
 
    FileManager::replaceFile("test/test_resource/dest/test1.txt", "test/test_resource/test1.txt");
    ASSERT_EQUAL(expected, result);
}
CTEST(FileManager, copyFile)
{
    int expected = 0;
    int result = FileManager::copyFile("test/test_resource/test2.txt", "./test/test_resource/dest");
    ASSERT_EQUAL(expected, result);
}
CTEST(FileManager, removeFile)
{
    FileManager::copyFile("test/test_resource/test3.txt", "./test/test_resource/dest");
    int expected = 0;
    int result = FileManager::removeFile("./test/test_resource/dest/test3.txt");
    ASSERT_EQUAL(expected, result);
}

CTEST(FileManager, showContent)
{
    
    int expected = 0;
    int result = FileManager::showContent("./test/test_resource/dest");
    ASSERT_EQUAL(expected, result);
}

CTEST(FileManager,size_folder){
    unsigned long  expected = 18;
    unsigned long result = FileManager::size("./test/test_resource/test_size");
    ASSERT_EQUAL(expected, result);
}
CTEST(FileManager,size_file){
    unsigned long  expected = 18;
    unsigned long result = FileManager::size("./test/test_resource/test_size/test-size.txt");
    ASSERT_EQUAL(expected, result);
}

CTEST(FileManager, showProcfsProcesses)
{
    
    int expected = 0;
    int result = FileManager::showProcfsProcesses();
    ASSERT_EQUAL(expected, result);
}
