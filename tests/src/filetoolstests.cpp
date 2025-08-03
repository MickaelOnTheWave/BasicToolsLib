/*
 * BasicToolsLib
 * Copyright (C) 2025 Guimarães Tecnologia Ltda
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

namespace  {
   void CreatePopulatedFolder(const wstring& folderName)
   {
      if (FileTools::FolderExists(folderName))
         FileTools::RemoveFolder(folderName, false);

      bool ok = FileTools::CreateFolder(folderName);
      REQUIRE(ok == true);

      const wstring dummyFile = folderName + L"/dummyFile.txt";
      const wstring dummyContent = L"Lalalala";
      ok = FileTools::WriteBufferToFile(dummyFile, dummyContent);
      REQUIRE(ok == true);
   }
}

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

TEST_CASE("FileTools - RemoveFolder() - Inexistent folder")
{
   const wstring inexistentFolder = L"hahaha";
   bool ok = FileTools::RemoveFolder(inexistentFolder, true);
   REQUIRE(ok == false);

   ok = FileTools::RemoveFolder(inexistentFolder, false);
   REQUIRE(ok == false);
}

TEST_CASE("FileTools - RemoveFolder() - Invalid folder")
{
   const wstring testFile = L"testFile.txt";
   FileTools::WriteBufferToFile(testFile, L"My testing content\n\nOn Multiple Lines");

   bool ok = FileTools::RemoveFolder(testFile, true);
   REQUIRE(ok == false);

   ok = FileTools::RemoveFolder(testFile, false);
   REQUIRE(ok == false);
}

TEST_CASE("FileTools - RemoveFolder() - With root")
{
   const wstring testFolder = L"testFolder";
   CreatePopulatedFolder(testFolder);

   const bool ok = FileTools::RemoveFolder(testFolder, false);
   REQUIRE(ok == true);

   const bool exists = FileTools::FolderExists(testFolder);
   REQUIRE(exists == false);
}

TEST_CASE("FileTools - RemoveFolder() - Without root")
{
   const wstring testFolder = L"testFolder";
   CreatePopulatedFolder(testFolder);

   const bool ok = FileTools::RemoveFolder(testFolder, true);
   REQUIRE(ok == true);

   bool exists = FileTools::FolderExists(testFolder);
   REQUIRE(exists == true);

   const wstring dummyFile = testFolder + L"/dummyFile.txt";
   exists = FileTools::FolderExists(dummyFile);
   REQUIRE(exists == false);
}
