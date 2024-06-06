#include "configurationobject.h"

#include <sstream>
#include "stringtools.h"

using namespace std;

ConfigurationObject::ConfigurationObject(const wstring &_name)
    : name(_name)
{
}

ConfigurationObject::~ConfigurationObject()
{
    propertyList.clear();

    list<ConfigurationObject*>::iterator it=objectList.begin();
    for (; it!=objectList.end(); ++it)
        delete *it;
    objectList.clear();
}

wstring ConfigurationObject::GetName() const
{
   return name;
}

void ConfigurationObject::SetName(const std::wstring& value)
{
   name = value;
}

ConfigurationObject *ConfigurationObject::GetObject(const wstring &objectName) const
{
    list<ConfigurationObject*>::const_iterator it=objectList.begin();
    list<ConfigurationObject*>::const_iterator end=objectList.end();
    for (; it!=end; it++)
    {
        if ((*it)->name == objectName)
            return *it;
    }
    return NULL;
}

void ConfigurationObject::AddObject(ConfigurationObject *object)
{
    objectList.push_back(object);
}

wstring ConfigurationObject::GetFullDescription(const int tabCount) const
{
    wstring tabChar = StringTools::Tabs(tabCount);
    wstringstream buffer;
    buffer << tabChar << "Name : " << name << endl;

    map<wstring, wstring>::const_iterator itProp = propertyList.begin();
    for (; itProp!=propertyList.end(); itProp++)
        buffer << tabChar << "[Property] " << itProp->first << " : " << itProp->second << endl;

    list<ConfigurationObject*>::const_iterator itObj = objectList.begin();
    for (; itObj!=objectList.end(); itObj++)
    {
        buffer << tabChar << "Object" << endl;
        buffer << (*itObj)->GetFullDescription(tabCount+1);
    }
    return buffer.str();
}

wstring ConfigurationObject::CreateConfigurationString(const int tabCount) const
{
   wstring tabChar = StringTools::Tabs(tabCount);
   wstringstream buffer;

   buffer << tabChar << name << endl;
   buffer << tabChar << "{" << endl;

   tabChar = StringTools::Tabs(tabCount+1);

   map<wstring, wstring>::const_iterator itProp = propertyList.begin();
   for (; itProp!=propertyList.end(); itProp++)
       buffer << tabChar << itProp->first << " = \"" << itProp->second << "\";" << endl;

   list<ConfigurationObject*>::const_iterator itObj = objectList.begin();
   for (; itObj!=objectList.end(); itObj++)
       buffer << (*itObj)->CreateConfigurationString(tabCount+1);

   buffer << StringTools::Tabs(tabCount) << "}" << endl;
   return buffer.str();
}

map<wstring,wstring>::iterator ConfigurationObject::BeginProperties()
{
   return propertyList.begin();
}

map<wstring,wstring>::iterator ConfigurationObject::EndProperties()
{
   return propertyList.end();
}

list<ConfigurationObject*>::iterator ConfigurationObject::BeginObjects()
{
   return objectList.begin();
}

list<ConfigurationObject*>::iterator ConfigurationObject::EndObjects()
{
   return objectList.end();
}

wstring ConfigurationObject::GetFirstProperty(const wstring &prop1, const wstring &prop2) const
{
    wstring property =  GetProperty(prop1);
    if (property == L"")
        property = GetProperty(prop2);
    return property;
}

wstring ConfigurationObject::GetProperty(const wstring &propertyName) const
{
    map<wstring, wstring>::const_iterator it = propertyList.find(propertyName);
    return (it != propertyList.end()) ? it->second : wstring(L"");
}

int ConfigurationObject::GetPropertyAsInt(const wstring& name) const
{
   int value = -1;
   wstring rawValue = GetProperty(name);
   if (rawValue != L"")
   {
      wstringstream stream(rawValue);
      stream >> value;
   }
   return value;
}

void ConfigurationObject::SetProperty(const wstring &propertyName, const wstring &value)
{
   propertyList[propertyName] = value;
}

void ConfigurationObject::SetProperty(const wstring& propertyName, const int value)
{
   wstringstream valueStr;
   valueStr << value;
   SetProperty(propertyName, valueStr.str());
}

void ConfigurationObject::SetProperty(const wstring& propertyName, const bool value)
{
   const wstring stringValue = value ? L"true" : L"false";
   SetProperty(propertyName, stringValue);
}


