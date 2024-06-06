#include "tools.h"

#include <algorithm>
#include <locale>
#include <time.h>
#include <stdio.h>
#include <sstream>

#include "base64.h"
#include "filetools.h"
#include "stringtools.h"

#ifdef _MSC_VER
   #include <Windows.h>
#else
   #include <unistd.h>
#endif

using namespace std;

int Tools::RunExternalCommandToFile(const wstring &commandName, const wstring &outputFileName,
                                    bool redirectStdErr)
{
    FILE* pipe = RunExternalCommand(commandName, redirectStdErr);
    if (pipe == NULL)
        return -1;

    return WriteStreamToFile(pipe, outputFileName);
}

int Tools::RunExternalCommandToBuffer(const wstring &commandName, wstring &output,
                                      bool redirectStdErr)
{
    FILE* pipe = RunExternalCommand(commandName, redirectStdErr);
    if (pipe == NULL)
        return -1;

    return WriteStreamToBuffer(pipe, output);
}

bool Tools::IsComputerAlive(const wstring& address)
{
    wstring pingCommand = L"ping -q -c 1 ";
    pingCommand += address + L" 2>&1 >/dev/null";
    wstring unusedBuffer;
    int returnCode = RunExternalCommandToBuffer(pingCommand, unusedBuffer);
    return (returnCode == 0);
}

FILE* Tools::RunExternalCommand(const wstring& commandName, bool redirectStdErr)
{
    wstring redirectedCommand = commandName;
#ifndef _WIN32
    if (redirectStdErr)
        redirectedCommand += L" 2>&1";
#endif
    return OsOpenPipe(redirectedCommand);
}

int Tools::WriteStreamToBuffer(FILE *stream, wstring &buffer)
{
    buffer = L"";
    const int bufferSize = 65536;

#ifdef _WIN32
    wchar_t readingBuffer[bufferSize];
    while (!feof(stream) && fgetws(readingBuffer, bufferSize, stream) != NULL)
        buffer += readingBuffer;
#else
    char readingBuffer[bufferSize];
    while (!feof(stream))
    {
       if (fgets(readingBuffer, bufferSize, stream) != NULL)
          buffer += StringTools::Utf8ToUnicode(string(readingBuffer));
    }
#endif

    if (buffer.size() > 1)
       RemoveLastLineBreak(buffer);

    return OsClosePipe(stream);
}

int Tools::WriteStreamToFile(FILE *stream, const wstring &filename)
{
   FILE* outputFile;
#ifdef _MSC_VER
   errno_t error = _wfopen_s(&outputFile, filename.c_str(), L"w");
   if (error == 0)
#else
   const string utf8Filename = StringTools::UnicodeToUtf8(filename);
   outputFile = fopen(utf8Filename.c_str(), "w");
   if (outputFile)
#endif
   {
      const int BUF_SIZE = 8192;
      char buffer[BUF_SIZE];
      while(!feof(stream))
      {
         if(fgets(buffer, BUF_SIZE, stream) != NULL)
            fputs(buffer, outputFile);
      }
      fclose(outputFile);
   }
   return OsClosePipe(stream);
}

bool Tools::GetFolderContentRecursively(const wstring &foldername, vector<wstring> &fileList)
{
    if (!FileTools::FolderExists(foldername))
        return false;

    const wstring command = wstring(L"ls -R ") + foldername;
    wstring output;
    int returnValue = RunExternalCommandToBuffer(command, output, true);
    if (returnValue != 0)
        return false;

    ParseRecursiveLsOutput(output, fileList);
    return true;
}

void Tools::Wait(const int seconds)
{
#ifdef _MSC_VER
   const int millisecondsFactor = 1000;
   Sleep(seconds * millisecondsFactor);
#else
   sleep(seconds);
#endif
}


void Tools::ParseRecursiveLsOutput(const wstring &output,
                                   vector<wstring> &fileList)
{
    vector<wstring> perFolderContent;
    StringTools::Tokenize(output, L"\n\n", perFolderContent);

    vector<wstring>::const_iterator itFolder = perFolderContent.begin();
    for (; itFolder != perFolderContent.end(); ++itFolder)
    {
        vector<wstring> fileContent;
        StringTools::Tokenize(*itFolder, L'\n', fileContent);

        const wstring rawFolderName = fileContent.front();
        const wstring folderName = rawFolderName.substr(0, rawFolderName.size()-1);

        vector<wstring>::iterator it = find(fileList.begin(), fileList.end(), folderName);
        if (it != fileList.end())
            fileList.erase(it);

        const wstring folderNameInPath =  folderName + L"/";
        vector<wstring>::const_iterator itFile = fileContent.begin()+1;
        for (; itFile != fileContent.end(); ++itFile)
            fileList.push_back(folderNameInPath + *itFile);
    }
}

void Tools::RemoveLastLineBreak(std::wstring& buffer)
{
   if (buffer[buffer.size() - 1] == '\n')
      buffer.erase(buffer.end() - 1);
}

FILE* Tools::OsOpenPipe(const wstring& command)
{
#ifdef _WIN32
    return _wpopen(command.c_str(), L"r");
#else
   const string utf8Command = StringTools::UnicodeToUtf8(command);
   return popen(utf8Command.c_str(), "r");
#endif
}

int Tools::OsClosePipe(FILE* openPipe)
{
#ifdef _WIN32
    return _pclose(openPipe);
#else
    return pclose(openPipe);
#endif
}
