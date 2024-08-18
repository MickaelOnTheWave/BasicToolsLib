/*
 * BasicToolsLib
 * Copyright (C) 2023 Guimarães Tecnologia Ltda
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

#include <catch2/catch.hpp>

#include "commandlinemanager.h"
#include "stringtools.h"

using std::map, std::string;

/*******************/

TEST_CASE("CommandLineManager - Simple - Retrieve parameters")
{
    const map<string, string> params = {{"mandatoryParam1", "value1"},
                   {"optionalParam1", "value2"},
                   {"param3", "value3"}
                  };
    CommandLineManager manager(params);

    REQUIRE( manager.GetParameterValue("mandatoryParam1") == "value1");
    REQUIRE( manager.GetParameterValue("optionalParam1") == "value2");
    REQUIRE( manager.GetParameterValue("param3") == "value3");
}

TEST_CASE("CommandLineManager - Detects unknown parameters")
{
    const map<string, string> params = {{"param3", "value3"}};
    CommandLineManager manager(params);

    SECTION("Parameter is unknown")
    {
        REQUIRE( manager.HasUnknownParameters() == true);
    }

    SECTION("Parameter is known")
    {
        manager.AddParameter("param3", "");
        REQUIRE( manager.HasUnknownParameters() == false);
    }
}

TEST_CASE("CommandLineManager - Detects version command")
{
    SECTION("Parameter is version")
    {
        const map<string, string> params = {{"version", ""}};
        CommandLineManager manager(params);
        REQUIRE( manager.HandleVersionCommand() == true);
    }

    SECTION("Parameter isn't version")
    {
        const map<string, string> params = {{"something", "else"}};
        CommandLineManager manager(params);
        REQUIRE( manager.HandleVersionCommand() == false);
    }
}

TEST_CASE("CommandLineManager - Detects help command")
{
    SECTION("Parameter is help")
    {
        const map<string, string> params = {{"help", ""}};
        CommandLineManager manager(params);
        REQUIRE( manager.HandleHelpCommand() == true);
    }

    SECTION("Parameter isn't help")
    {
        const map<string, string> params = {{"someother", "command"}};
        CommandLineManager manager(params);
        REQUIRE( manager.HandleHelpCommand() == false);
    }
}

TEST_CASE("StringTools - Utf8ToUnicode")
{
    SECTION("Simple string")
    {
        auto converted = StringTools::Utf8ToUnicode("my string is simple");
        REQUIRE( converted == L"my string is simple");
    }

    SECTION("Using accents")
    {
        auto converted = StringTools::Utf8ToUnicode("Oune Strïngue avé dé acceñtè por moí");
        REQUIRE( converted == L"Oune Strïngue avé dé acceñtè por moí");
    }
}
