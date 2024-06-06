#ifndef OSTOOLS_H
#define OSTOOLS_H

class OsTools
{
public:
    static bool IsOnWindows();
    static bool IsOnLinux();
    static bool IsRunningAsAdministrator();
};

#endif // OSTOOLS_H
