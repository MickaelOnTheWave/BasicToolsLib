#ifndef PATHTOOLS_H
#define PATHTOOLS_H

#include <string>
#include <vector>

class PathTools
{
public:
   static std::wstring GetCommandPath(
           const std::wstring& command,
           const std::vector<std::wstring>& searchPaths
           );

   static std::wstring GetCommandPathUsingWhich(const std::wstring& command);

   static std::wstring GetCommandPathUsingPathCollection(
           const std::wstring& command,
           const std::vector<std::wstring>& searchPaths
           );

    static std::wstring GetFilenameOnly(const std::wstring& fullFilename);

    static std::wstring GetPathOnly(const std::wstring& fullFilename);

    static std::wstring GetCurrentExecutablePath();

    /**
     * @brief Creates a folder with all missing intermediate folders
     * @param folder name to create
     * @return Absolute path of created folder
     */
    static std::wstring BuildFullPath(const std::wstring& name);

    static std::wstring BuildFullPathIfRelative(const std::wstring& name);

    static std::wstring RelativePath(const std::wstring& name);

    static std::wstring GetCurrentFullPath();

   static bool IsAbsolutePath(const std::wstring& name);

   static std::wstring ToWindowsPath(const std::wstring& path);

   static bool ChangeCurrentDir(const std::wstring& path);

private:
    static unsigned int CountOccurrences(const std::wstring& target,
                                         const std::wstring& pattern);

    static std::wstring GuessSeparator(const std::wstring& fullFilename);

    static std::wstring GetFilenameOnly(const std::wstring& fullFilename, const std::wstring& separator);

    static std::wstring GetPathOnly(const std::wstring& fullFilename, const std::wstring& separator);

    static std::wstring CrossPlatformGetCwd();

    static std::wstring JoinStringParts(const std::vector<std::wstring>& parts, const std::wstring& separator);

};

#endif // PATHTOOLS_H
