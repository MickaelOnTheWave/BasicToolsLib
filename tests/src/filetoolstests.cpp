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

#include "filetools.h"

using namespace std;

/*******************/

TEST_CASE("FileTools - FileExists() - Existing file")
{
   bool exists = FileTools::FileExists("existingFile");
   REQUIRE( exists == true );
   exists = FileTools::FileExists(L"existingFile");
   REQUIRE( exists == true );
}

TEST_CASE("FileTools - FileExists() - Non existing file")
{
   bool exists = FileTools::FileExists("nonexistingFile");
   REQUIRE( exists == false );
   exists = FileTools::FileExists(L"nonexistingFile");
   REQUIRE( exists == false );
}

TEST_CASE("FileTools - FolderExists() - Existing file")
{
   const bool exists = FileTools::FolderExists(L"existingFile");
   REQUIRE( exists == false );
}

TEST_CASE("FileTools - FolderExists() - Non existing file")
{
   const bool exists = FileTools::FileExists(L"nonexistingFile");
   REQUIRE( exists == false );
}

TEST_CASE("FileTools - FolderExists() - Existing folder")
{
   const bool exists = FileTools::FileExists(L"existingFolder");
   REQUIRE( exists == true );
}

TEST_CASE("FileTools - RemoveFile() - Non existing file")
{
   const wstring testFilename = L"nonexistingfile";
   const bool ok = FileTools::RemoveFile(testFilename);
   REQUIRE( ok == false );

   const bool exists = FileTools::FileExists(testFilename);
   REQUIRE( exists == false );
}

TEST_CASE("FileTools - RemoveFile() - Existing file")
{
   const wstring testFilename = L"todeletefile";
   bool ok = FileTools::WriteBufferToFile(testFilename, L"lalala");
   REQUIRE(ok == true);

   ok = FileTools::RemoveFile(testFilename);
   REQUIRE(ok == true);

   const bool exists = FileTools::FileExists(testFilename);
   REQUIRE(exists == false);
}
