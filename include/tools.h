#ifndef TOOLS_H
#define TOOLS_H

#include <map>
#include <string>
#include <vector>

class Tools
{
public:

    static const int COMMAND_NOT_FOUND = 32512;

	/**
		@param[in] outputFileName File name with command output.
		@return The return code returned by command.
	  */
    static int RunExternalCommandToFile(
			const std::wstring& commandName,
            const std::wstring& outputFileName,
            bool redirectStdErr = false
	);

	/**
		@param[out] output Command output
		@return The return code returned by command.
	  */
    static int RunExternalCommandToBuffer(
			const std::wstring& commandName,
            std::wstring &output,
            bool redirectStdErr = false
	);

    // TODO : once ping is implemented here, move that to NetworkTools
    static bool IsComputerAlive(const std::wstring& address);

    static bool GetFolderContentRecursively(const std::wstring& foldername,
                                            std::vector<std::wstring>& fileList);

    static void Wait(const int seconds);

   using WStringVec = std::vector<std::wstring>;
   static bool AreEqual(const WStringVec& list1, const WStringVec& list2);


protected:
    static FILE* RunExternalCommand(const std::wstring& commandName, bool redirectStdErr);

    static int WriteStreamToBuffer(FILE* stream, std::wstring& buffer);

    static int WriteStreamToFile(FILE* stream, const std::wstring& filename);

    static void ParseRecursiveLsOutput(const std::wstring& output,
                                       std::vector<std::wstring>& fileList);

    static void RemoveLastLineBreak(std::wstring& buffer);

    static FILE* OsOpenPipe(const std::wstring &command);
    static int OsClosePipe(FILE* openPipe);
};

#endif // TOOLS_H
