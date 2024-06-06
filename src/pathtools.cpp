#include "pathtools.h"

#include <vector>
#include "filetools.h"
#include "stringtools.h"
#include "tools.h"

#ifdef _WIN32
   #include <Windows.h>
   #include <direct.h>
#else
   #include <cstring>
   #include <unistd.h>
#endif

using namespace std;

const wchar_t slash = L'/';

wstring PathTools::GetCommandPath(const wstring &command, const std::vector<wstring> &searchPaths)
{
    if (FileTools::FileExists(command))
        return command;
    else
    {
#ifndef _WIN32
        wstring path = GetCommandPathUsingWhich(command);
        if (path != L"")
            return path;
        else
#endif
            return GetCommandPathUsingPathCollection(command, searchPaths);
    }
}

wstring PathTools::GetCommandPathUsingWhich(const wstring &command)
{
    wstring path(L"");
    wstring checkingCommand = wstring(L"which ") + command;
    int returnCode = Tools::RunExternalCommandToBuffer(checkingCommand, path, true);
    if (returnCode != 0)
        return wstring(L"");
    else
        return path;
}

wstring PathTools::GetCommandPathUsingPathCollection(
        const wstring &command,
        const std::vector<wstring> &searchPaths)
{
    vector<wstring>::const_iterator it=searchPaths.begin();
    for (; it!=searchPaths.end(); it++)
    {
        wstring currentName = *it + L"/" + command;
        bool commandExists = FileTools::FileExists(currentName);
        if (commandExists)
            return currentName;
    }

    return wstring(L"");
}

wstring PathTools::GetCurrentExecutablePath()
{
#ifdef _WIN32
   const DWORD bufferSize = 2018;
   wchar_t filename[bufferSize];
   if (GetModuleFileNameW(NULL, filename, bufferSize) != 0)
      return wstring(filename);
   else
      return wstring(L"");
#else
   const unsigned int bufferSize = 2018;
   char buffer[bufferSize];
   ssize_t count = readlink("/proc/self/exe", buffer, bufferSize);

   const string path = (count > 0) ? string(buffer) : string("");
   return StringTools::Utf8ToUnicode(path);
#endif
}

wstring PathTools::BuildFullPath(const wstring &name)
{
   const wstring currentPath = CrossPlatformGetCwd();
   wstring fullPath = currentPath + slash + name;
   if (name[name.size()-1] != slash)
    fullPath.push_back(slash);
   return fullPath;
}

wstring PathTools::BuildFullPathIfRelative(const wstring& name)
{
   return (IsAbsolutePath(name)) ? name : BuildFullPath(name);
}

wstring PathTools::RelativePath(const wstring& name)
{
   wstring path = name;
   if (!name.empty() && name[name.size()-1] != slash)
    path.push_back(slash);
   return path;
}

wstring PathTools::GetCurrentFullPath()
{
    return CrossPlatformGetCwd();
}

bool PathTools::IsAbsolutePath(const wstring& name)
{
   return (!name.empty() && name[0] == '/');
}

wstring PathTools::ToWindowsPath(const wstring& path)
{
   vector<wstring> components;
   StringTools::Tokenize(path, '/', components);
   return JoinStringParts(components, L"\\");
}

bool PathTools::ChangeCurrentDir(const wstring& path)
{
#ifdef _WIN32
   const int returnValue = _wchdir(path.c_str());
   return (returnValue == 0);
#else
   const string utf8Dir = StringTools::UnicodeToUtf8(path);
   return (chdir(utf8Dir.c_str()) == 0);
#endif
}

wstring PathTools::GetFilenameOnly(const wstring& fullFilename)
{
   const wstring separator = GuessSeparator(fullFilename);
   return GetFilenameOnly(fullFilename, separator);
}

wstring PathTools::GetPathOnly(const wstring& fullFilename)
{
   const wstring separator = GuessSeparator(fullFilename);
   return GetPathOnly(fullFilename, separator);
}

wstring PathTools::GuessSeparator(const wstring& fullFilename)
{
   const wstring strSlash = L"/";
   const wstring backslash = L"\\";
   const int slashCount = CountOccurrences(fullFilename, strSlash);
   const int backSlashCount = CountOccurrences(fullFilename, backslash);
   return (backSlashCount > slashCount) ? backslash : strSlash;
}

unsigned int PathTools::CountOccurrences(const wstring& target,
                                     const wstring& pattern)
{
   unsigned int count = 0;
   size_t offset = 0;
   while (offset != wstring::npos)
   {
      offset = target.find(pattern, offset);
      if (offset != wstring::npos)
      {
         ++count;
         offset += pattern.length();
      }
   }
   return count;
}

wstring PathTools::GetFilenameOnly(const wstring& fullFilename, const wstring& separator)
{
   vector<wstring> pathComponents;
   StringTools::Tokenize(fullFilename, separator, pathComponents);
   return pathComponents.back();
}

wstring PathTools::GetPathOnly(const wstring& fullFilename, const wstring& separator)
{
   const size_t pos = fullFilename.rfind(separator);
   return fullFilename.substr(0, pos);
}

wstring PathTools::CrossPlatformGetCwd()
{
#ifdef _WIN32
   wchar_t* buffer = NULL;
   const int bufferSize = 0;
   return _wgetcwd(buffer, bufferSize);
#else
   const string directory = get_current_dir_name();
   return StringTools::Utf8ToUnicode(directory);
#endif
}

wstring PathTools::JoinStringParts(const vector<wstring>& parts,
                                   const wstring& separator)
{
   wstring finalResult;
   if (parts.size() > 0)
   {
      finalResult = parts.front();
      if (parts.size() > 1)
      {
         vector<wstring>::const_iterator it = parts.begin() + 1;
         for (; it != parts.end(); ++it)
            finalResult += separator + *it;
      }
   }
   return finalResult;
}
