#include "filetools.h"

#include <cstdio>
#include <malloc.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "stringtools.h"

#ifdef _WIN32
   #include <direct.h>
   #include <wchar.h>
#endif
#ifdef _MSC_VER
   #define getcwd _getcwd
   #include <windows.h>
   #include "Shobjidl.h"
   #include "pathtools.h"
#else
   #include <dirent.h>
   #include <unistd.h>
#endif

using namespace std;

#ifdef _MSC_VER
bool WindowsDeleteFileItem(IShellItem* item)
{
   bool operationOk = false;
   if (item)
   {
      IFileOperation *pfo;
      HRESULT hr = CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pfo));
      if (SUCCEEDED(hr))
      {
         hr = pfo->SetOperationFlags(FOF_SILENT | FOF_NOERRORUI | FOF_NOCONFIRMATION);
         if (SUCCEEDED(hr))
         {
            hr = pfo->DeleteItem(item, NULL);
            if (SUCCEEDED(hr))
            {
               hr = pfo->PerformOperations();
               if (SUCCEEDED(hr))
                  operationOk = true;
            }
         }
         pfo->Release();
      }
   }
   return operationOk;
}

bool WindowsRemoveFolder(const wstring& folder)
{
   IShellItem* item = NULL;
   const wstring convertedFolder = PathTools::ToWindowsPath(folder);
   const HRESULT result = SHCreateItemFromParsingName(convertedFolder.c_str(), NULL, IID_PPV_ARGS(&item));
   if (result == S_OK)
      return WindowsDeleteFileItem(item);
   else
      return false;
}

bool WindowsIsFolderEmpty(const wstring&)
{
   // TODO : implement
   return false;
}
#else
bool LinuxRemoveFolder(const string& folderName, const bool keepRootFolder)
{
   DIR *dir;
   struct dirent *ent;
   if ((dir = opendir(folderName.c_str())) != NULL)
   {
      while ((ent = readdir(dir)) != NULL)
      {
         if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
            continue;

         size_t len = strlen(folderName.c_str()) + strlen(ent->d_name) + 2;
         char *buf = static_cast<char*>(malloc(len));
         snprintf(buf, len, "%s/%s", folderName.c_str(), ent->d_name);
         struct stat statbuf;
         int returnValue = -1;
         if (stat(buf, &statbuf) == 0)
         {
            if (S_ISDIR(statbuf.st_mode))
               returnValue = LinuxRemoveFolder(buf, false) ? 0 : -1;
            else
               returnValue = unlink(buf);
         }

         free(buf);

         if (returnValue != 0)
            return false;
      }
      closedir(dir);
      if (keepRootFolder == false)
      {
         int value = remove(folderName.c_str());
         if (value != 0)
            return false;
      }

      return true;
   }
   else
      return false;
}

bool LinuxRemoveFolder(const wstring& folderName, const bool keepRootFolder)
{
   const string utf8FolderName = StringTools::UnicodeToUtf8(folderName);
   return LinuxRemoveFolder(utf8FolderName, keepRootFolder);
}

bool LinuxIsFolderEmpty(const wstring& name)
{
   const string utf8Name = StringTools::UnicodeToUtf8(name);
   DIR *dp = opendir(utf8Name.c_str());

   if (dp != NULL)
   {
      bool validItemPresent = false;
      struct dirent *ep;
      while ((ep = readdir(dp)) != NULL)
      {
         if (strcmp(ep->d_name, ".") != 0 &&
             strcmp(ep->d_name, "..") != 0)
         {
            validItemPresent = true;
            break;
         }
      }
      (void)closedir(dp);
      return !validItemPresent;
   }
   else
      return false;
}
#endif

bool FileTools::RemoveFolder(const wstring &folderName, const bool keepRootFolder)
{
#ifdef _MSC_VER
   return WindowsRemoveFolder(folderName);
#else
   return LinuxRemoveFolder(folderName, keepRootFolder);
#endif
}

bool FileTools::MoveFileTo(const wstring& oldname, const wstring& newname)
{
#ifdef _MSC_VER
   return MoveFileExW(oldname.c_str(), newname.c_str(), MOVEFILE_WRITE_THROUGH);
#else
   const string utf8Old = StringTools::UnicodeToUtf8(oldname);
   const string utf8New = StringTools::UnicodeToUtf8(newname);
   return (std::rename(utf8Old.c_str(), utf8New.c_str()) == 0);
#endif
}

string FileTools::GetFilenameFromUnixPath(const string &path)
{
   return GetFilenameFromUnixPath<string>(path, "/");
}

wstring FileTools::GetFilenameFromUnixPath(const wstring &path)
{
   return GetFilenameFromUnixPath<wstring>(path, L"/");
}

unsigned long FileTools::GetFileSize(const string& file)
{
#ifdef _MSC_VER
   FILE* filePtr;
   errno_t error = fopen_s(&filePtr, file.c_str(), "rb");
   if (error == 0)
#else
   FILE* filePtr = fopen(file.c_str(), "rb");
   if (!filePtr)
#endif
      return 0;

   fseek(filePtr, 0, SEEK_END);
   unsigned long fileSize = ftell(filePtr);
   fclose(filePtr);
   return fileSize;
}

unsigned long FileTools::GetFileSize(const wstring &file)
{
#ifdef _MSC_VER
   FILE* filePtr;
   errno_t error = _wfopen_s(&filePtr, file.c_str(), L"rb");
   if (error == 0)
#else
   const string utf8File = StringTools::UnicodeToUtf8(file);
    FILE* filePtr = fopen(utf8File.c_str(), "rb");
    if (!filePtr)
#endif
        return 0;

    fseek (filePtr, 0, SEEK_END);
    unsigned long fileSize = ftell(filePtr);
    fclose(filePtr);
    return fileSize;
}

bool FileTools::GetTextFileContent(const wstring& fileName, wstring& fileContents)
{
#ifdef _MSC_VER
   wifstream inFile(fileName.c_str());
#else
   wifstream inFile(StringTools::UnicodeToUtf8(fileName).c_str());
#endif

    if (!inFile.is_open())
        return false;

    fileContents = L"";

    wstring line;
    while (getline(inFile, line))
    {
        fileContents += line;
        if (!inFile.eof())
            fileContents += L"\n";
    }

    inFile.close();
    return true;
}

wstring FileTools::GetTextFileContent(const std::wstring& fileName)
{
    wstring contents;
    bool ok = GetTextFileContent(fileName, contents);
    return (ok) ? contents : wstring(L"");
}

bool FileTools::FolderExists(const wstring &folderName)
{
#ifdef _WIN32
   struct _stat64i32 info;
   if( _wstat( folderName.c_str(), &info ) != 0 )
#else
    const string utf8Name = StringTools::UnicodeToUtf8(folderName);
    struct stat info;
    if( stat( utf8Name.c_str(), &info ) != 0 )
#endif
        return false;
    else if( info.st_mode & S_IFDIR )
        return true;
    else
        return false;
}

bool FileTools::CreateDummyFile(const wstring &name, const size_t fileSize, const bool textOnly)
{
    if (FileExists(name))
        return false;

#ifdef _MSC_VER
    FILE* file;
    errno_t error = _wfopen_s(&file, name.c_str(), L"w");
    if (error == 0)
#else
    const string utf8Name = StringTools::UnicodeToUtf8(name);
    FILE* file = fopen(utf8Name.c_str(), "w");
    if (!file)
#endif
        return false;

    for (size_t dataPtr = 0; dataPtr < fileSize; ++dataPtr)
    {
        char val;
        if  (textOnly)
        {
            int letterNumber = (dataPtr%26);
            val = (char)(letterNumber+97);
        }
        else
            val = (char)(dataPtr%256);
        fwrite(&val, sizeof(char), 1, file);
    }
    fwrite("\n", sizeof(char), 1, file);

    fclose(file);
    return true;
}

bool FileTools::CreateFolder(const wstring& folderName)
{
#ifdef __linux__
   const string utf8Name = StringTools::UnicodeToUtf8(folderName);
   return (mkdir(utf8Name.c_str(), S_IRWXU) == 0);
#elif _WIN32
   return (_wmkdir(folderName.c_str()) == 0);
#endif
}

bool FileTools::RemoveFile(const wstring& name)
{
#ifdef _WIN32
   const int result = _wremove(name.c_str());
   return (result == 0);
#else
   const int result = remove(StringTools::UnicodeToUtf8(name).c_str());
   return (result == 0);
#endif
}

bool FileTools::FileExists(const string &name)
{
   ifstream file(name.c_str());
   return file.is_open();
}

bool FileTools::FileExists(const wstring &name)
{
#ifdef _MSC_VER
   wifstream file(name.c_str());
#else
   wifstream file(StringTools::UnicodeToUtf8(name).c_str());
#endif
   return file.is_open();
}

bool FileTools::WriteBufferToFile(const wstring &fileName, const wstring &buffer)
{
   try {
      wofstream file = OpenFile(fileName);
      if (!file.is_open() || file.fail())
          return false;

      file << buffer;
      file.close();
      return true;
   }
   catch (const ofstream::failure& e)
   {
         return false;
   }
}

bool FileTools::IsFolderEmpty(const wstring& name)
{
#ifdef _MSC_VER
   return WindowsIsFolderEmpty(name);
#else
   return LinuxIsFolderEmpty(name);
#endif
}

wstring FileTools::GetAlternativeName(const wstring& filename)
{
   int counter = 1;
   wstring newName = filename;
   do
   {
      wstringstream counterwstring;
      counterwstring << "_" <<  counter;
      newName = filename + counterwstring.str();
   }
   while (FileExists(newName));
   return newName;
}

wofstream FileTools::OpenFile(const std::wstring& fileName)
{
#ifdef _MSC_VER
   wofstream file(fileName.c_str());
#else
   std::locale::global(std::locale(""));
   const string utf8Filename = StringTools::UnicodeToUtf8(fileName);
   wofstream file(utf8Filename.c_str());
#endif

   return file;
}
