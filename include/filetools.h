#ifndef FILETOOLS_H
#define FILETOOLS_H

#include <fstream>
#include <string>
#include <vector>

class FileTools
{
public:

    static bool FileExists(const std::string& name);

    static bool FileExists(const std::wstring& name);

    static bool FolderExists(const std::wstring& folderName);

    static bool CreateDummyFile(const std::wstring& name, const size_t fileSize, const bool textOnly);

    static bool CreateFolder(const std::wstring& folderName);
  
    static bool RemoveFile(const std::wstring& name);

    /**
     * @brief Removes the folder and all its data recursively.
     * @param folderName folder to remove.
     * @param keepRootFolder If true, foldername is not removed, only its contents.
     */
    static bool RemoveFolder(const std::wstring& folderName, const bool keepRootFolder);

    static bool MoveFileTo(const std::wstring& oldname, const std::wstring& newname);

    static std::string GetFilenameFromUnixPath(const std::string& path);

    static std::wstring GetFilenameFromUnixPath(const std::wstring& path);

    static unsigned long GetFileSize(const std::string& file);

    static unsigned long GetFileSize(const std::wstring& file);

    static bool GetTextFileContent(const std::wstring& fileName, std::wstring& fileContents);

    static std::wstring GetTextFileContent(const std::wstring& fileName);

    template<class stringT>
    static char *GetBinaryFileContent(const stringT& fileName,
                                      unsigned long& fileSize);

    static bool WriteBufferToFile(const std::wstring& fileName, const std::wstring& buffer);

    static bool IsFolderEmpty(const std::wstring& name);

    static std::wstring GetAlternativeName(const std::wstring& filename);

private:
   template<class stringT>
   static stringT GetFilenameFromUnixPath(const stringT& path, const stringT& separator);

   static std::wofstream OpenFile(const std::wstring& fileName);
};

template <class stringT>
char *FileTools::GetBinaryFileContent(const stringT& fileName,
                                      unsigned long &fileSize)
{
   std::ifstream inFile(fileName.c_str());

   if (!inFile.is_open())
      return NULL;

   fileSize = GetFileSize(fileName) + 1;
   char* fileContents = new char[fileSize];
   inFile.read(fileContents, fileSize);
   fileContents[fileSize - 1] = '\0';
   inFile.close();

   return fileContents;
}

template<class stringT>
stringT FileTools::GetFilenameFromUnixPath(const stringT& path, const stringT& separator)
{
   size_t pos = path.rfind(separator);
   if (pos != stringT::npos)
      ++pos;
   else
      pos = 0;
   return path.substr(pos);
}


#endif // FILETOOLS_H
