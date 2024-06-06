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

#ifndef COMMANDLINEMANAGER_H
#define COMMANDLINEMANAGER_H

#include <vector>
#include <map>
#include <string>

class CommandLineParameter
{
public:
   CommandLineParameter(const std::string& _name, const std::string& _description, const bool _mandatory);

   std::string name;
   std::string description;
   bool mandatory;
};

/// TODO Manage short commands, unix-style
class CommandLineManager
{
public:
    CommandLineManager(int argc, char* argv[]);
    CommandLineManager(const std::map<std::string, std::string>& parameters);

    // TODO Put a field just for param value.
    void AddParameter(const std::string& name, const std::string& description, const bool mandatory = true);
    void AddOptionalParameter(const std::string& name, const std::string& description);

    bool HasParameter(const std::string& parameter) const;

    std::string GetParameterValue(const std::string& parameter) const;
    unsigned long GetParameterValueAsUnsignedLong(const std::string& parameter);

    bool HasUnknownParameters(void) const;

    /**
     * @brief Shows command usage information if unknown parameters are found.
     * @return false if all parameters are known.
     */
    bool HandleUnknownParameters();

    /**
     * @brief Shows version and copyright information on version command.
     * @return false if command was not version.
     */
    bool HandleVersionCommand();

    /**
     * @brief Shows command usage information on help command.
     * @return false if command was not help.
     */
    bool HandleHelpCommand();

    void ShowUsageInformation();

    void EnableHelpCommand();

    void EnableVersionCommand(const std::string& appName, const std::string& appVersion,
                              const std::string& author, const std::string& copyright);


private:
    using ParameterList = std::vector<CommandLineParameter> ;

    void BuildParametersMap(const int argc, char* argv[]);

    void BuildParametersMap(const int argc, wchar_t* argv[]);

    bool IsInKnownList(const std::string& parameter) const;

    void ShowVersionInformation();

    void UpdateParameterColumnSize(const ParameterList& parameters);

    void ShowParamUsage(const CommandLineParameter& param, const bool displayMandatoryTag);

    void ShowParamUsage(const std::string& param, const std::string& description);

    void ShowParamUsage(const std::string& param, const std::string& description, const bool displayMandatoryTag);

    bool ParamsHaveDifferentTypes() const;

    static std::string Spaces(const int spaceCount);

    std::map<std::string, std::string> options;
    ParameterList knownParameters;
    bool usingHelpCommand;
    bool usingVersionCommand;
    std::string applicationName;
    std::string applicationVersion;
    std::string authorName;
    std::string copyrightInfo;
    size_t parameterColumnSize;
};

#endif // COMMANDLINEMANAGER_H
