/*
 * BasicTools
 * Copyright (C) 2024 Mickaël da Cruz Guimarães
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "commandlinemanager.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdlib.h>

#include "stringtools.h"

using namespace std;

CommandLineParameter::CommandLineParameter(const std::string& _name, const std::string& _description, const bool _mandatory)
   : name(_name), description(_description), mandatory(_mandatory)
{
}


CommandLineManager::CommandLineManager(int argc,char* argv[]) :
    usingHelpCommand(false), usingVersionCommand(false),
    applicationName(""), applicationVersion(""), authorName(""), copyrightInfo(""),
    parameterColumnSize(0)
{
   BuildParametersMap(argc, argv);
}

CommandLineManager::CommandLineManager(int argc, wchar_t *argv[]) :
  usingHelpCommand(false), usingVersionCommand(false),
  applicationName(""), applicationVersion(""), authorName(""), copyrightInfo(""),
  parameterColumnSize(0)
{
   char **convertedArgv = new char*[argc];
   for (int i=0; i<argc; ++i)
   {
      const string currentArg = StringTools::UnicodeToUtf8(argv[i]);
      convertedArgv[i] = new char[currentArg.size()];
      strcpy(convertedArgv[i], currentArg.c_str());
   }

   BuildParametersMap(argc, convertedArgv);

   for (int i=0; i<argc; ++i)
   {
      delete[] convertedArgv[i];
   }
   delete[] convertedArgv;

}

CommandLineManager::CommandLineManager(const std::map<string, string> &parameters)
    : options(parameters)
{
}

void CommandLineManager::AddParameter(const string &name, const string &description, const bool mandatory)
{
   CommandLineParameter parameter(name, description, mandatory);
   knownParameters.push_back(parameter);
}

void CommandLineManager::AddOptionalParameter(const std::string& name, const std::string& description)
{
   AddParameter(name, description, false);
}

bool CommandLineManager::HasParameter(const string &parameter) const
{
    return options.count(parameter) > 0;
}

string CommandLineManager::GetParameterValue(const string &parameter) const
{
    map<string, string>::const_iterator it =  options.find(parameter);
    return (it != options.end()) ? it->second : string("");
}

unsigned long CommandLineManager::GetParameterValueAsUnsignedLong(const string &parameter)
{
#ifdef _WIN32
    return _wtol(GetParameterValue(parameter).c_str());
#else
   char* end;
   const int base = 10;
   return strtoul(GetParameterValue(parameter).c_str(), &end, base);
#endif
}

bool CommandLineManager::HasUnknownParameters() const
{
    map<string, string>::const_iterator it = options.begin();
    map<string, string>::const_iterator end = options.end();
    for (; it!=end; it++)
    {
        bool isParamInList = IsInKnownList(it->first);
        bool isStandardParam = ((usingHelpCommand && it->first == "help") ||
                                (usingVersionCommand && it->first == "version"));

        if (!isParamInList && !isStandardParam)
            return true;
    }

    return false;
}

bool CommandLineManager::HandleUnknownParameters()
{
    if (HasUnknownParameters())
    {
        cout << "Unhandled parameters." << endl;
        ShowUsageInformation();
        return true;
    }

    return false;
}

bool CommandLineManager::HandleVersionCommand()
{
    if (HasParameter("version"))
    {
        ShowVersionInformation();
        return true;
    }

    return false;
}

bool CommandLineManager::HandleHelpCommand()
{
    if (HasParameter("help"))
    {
        ShowUsageInformation();
        return true;
    }

    return false;
}

void CommandLineManager::BuildParametersMap(const int argc, char* argv[])
{
    if (argc == 1)
        return;

    for (int i=1; i<argc; i++)
    {
        string currentArg(argv[i]);
        if (currentArg.find("--") == 0)
        {
            string currentOption(currentArg.substr(2));
            string optionValue("");
            if (i+1<argc)
            {
                currentArg = argv[i+1];
                if (currentArg.find("--") == string::npos)
                {
                    optionValue = currentArg;
                    i++;
                }
            }

            options[currentOption] = optionValue;
        }
    }
}

bool CommandLineManager::IsInKnownList(const string &parameter) const
{
    ParameterList::const_iterator it=knownParameters.begin();
    ParameterList::const_iterator end=knownParameters.end();
    for(; it!=end; ++it)
    {
        if (it->name == parameter)
            return true;
    }

    return false;
}

void CommandLineManager::ShowUsageInformation()
{
   const bool showMandatoryTag = ParamsHaveDifferentTypes();
   cout << "Available options :" << endl;

   UpdateParameterColumnSize(knownParameters);

   ParameterList::const_iterator it=knownParameters.begin();
   ParameterList::const_iterator end=knownParameters.end();
   for(; it!=end; ++it)
      ShowParamUsage(*it, showMandatoryTag);

   if (usingHelpCommand)
      ShowParamUsage("help", "Shows this usage information");

   if (usingVersionCommand)
      ShowParamUsage("version", "Shows program version information");
}

void CommandLineManager::EnableHelpCommand()
{
    usingHelpCommand = true;
}

void CommandLineManager::EnableVersionCommand(const string &appName, const string &appVersion,
                                              const string &author, const string &copyright)
{
    usingVersionCommand = true;
    applicationName = appName;
    applicationVersion = appVersion;
    authorName = author;
    copyrightInfo = copyright;
}

void CommandLineManager::ShowVersionInformation()
{
    cout << applicationName << " v" << applicationVersion << endl;
    cout << "Copyright (C) " << copyrightInfo << " " << authorName << endl;
    cout << "All rights reserved." << endl;
}

void CommandLineManager::UpdateParameterColumnSize(const CommandLineManager::ParameterList &parameters)
{
    parameterColumnSize = 0;
    for (ParameterList::const_iterator it = parameters.begin(); it!=parameters.end(); ++it)
    {
        const size_t currentLength = it->name.length();
        if (currentLength > parameterColumnSize)
            parameterColumnSize = currentLength;
    }

    const int minimumSpacing = 10;
    parameterColumnSize += minimumSpacing;
}

void CommandLineManager::ShowParamUsage(const CommandLineParameter& param, const bool displayMandatoryTag)
{
   const bool displayParamAsMandatory = displayMandatoryTag && param.mandatory;
   ShowParamUsage(param.name, param.description, displayParamAsMandatory);
}

void CommandLineManager::ShowParamUsage(const string &param, const string &description)
{
   ShowParamUsage(param, description, false);
}

void CommandLineManager::ShowParamUsage(const std::string& param, const std::string& description, const bool displayMandatoryTag)
{
   const size_t spacesToInsert = parameterColumnSize - param.length();
   cout << "\t--" << param << Spaces(static_cast<int>(spacesToInsert));
   if (displayMandatoryTag)
      cout << "[MANDATORY] ";
   cout << description << endl;
}

bool CommandLineManager::ParamsHaveDifferentTypes() const
{
   auto isMandatory = [](const CommandLineParameter& parameter)
   {
      return parameter.mandatory;

   };
   const bool allOptional = std::none_of(knownParameters.begin(), knownParameters.end(), isMandatory);
   const bool allMandatory = std::all_of(knownParameters.begin(), knownParameters.end(), isMandatory);
   return !allOptional && !allMandatory;
}

string CommandLineManager::Spaces(const int spaceCount)
{
   string spaceStr;
   for (int i=0; i<spaceCount; ++i)
      spaceStr += " ";
   return spaceStr;
}

