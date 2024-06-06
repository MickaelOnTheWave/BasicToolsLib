#include "ostools.h"

#ifdef _WIN32
    #include <Windows.h>
    #include <ShlObj.h>
#endif

bool OsTools::IsOnWindows()
{
#ifdef _WIN32
   return true;
#else
   return false;
#endif
}

bool OsTools::IsOnLinux()
{
    return !IsOnWindows();
}

bool OsTools::IsRunningAsAdministrator()
{
#ifdef _WIN32
    return (IsUserAnAdmin() == TRUE);
#else
    return false;
#endif
}
