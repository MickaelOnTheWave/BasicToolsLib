#ifndef CONFIGURATIONOBJECT_H
#define CONFIGURATIONOBJECT_H

#include <list>
#include <map>
#include <string>

class ConfigurationObject
{
public :
    ConfigurationObject(const std::wstring& _name = L"");
    ~ConfigurationObject();

    std::wstring GetName() const;
    void SetName(const std::wstring& value);

    std::wstring GetProperty(const std::wstring& name) const;
    int GetPropertyAsInt(const std::wstring& name) const;
    void SetProperty(const std::wstring& name, const std::wstring& value);
    void SetProperty(const std::wstring& name, const int value);
    void SetProperty(const std::wstring& name, const bool value);
    /**
     * @brief Returns the first non-empty property of the two parameters
     * @param prop1 First parameter. It is is not empty, this one is returned.
     * @param prop2 Second parameter. Only returned if first one is empty.
     */
    std::wstring GetFirstProperty(const std::wstring& prop1, const std::wstring& prop2) const;


    ConfigurationObject* GetObject(const std::wstring& name) const;
    void AddObject(ConfigurationObject* object);

    std::wstring GetFullDescription(const int tabCount = 0) const;

    std::wstring CreateConfigurationString(const int tabCount = 0) const;

    std::map<std::wstring, std::wstring>::iterator BeginProperties();
    std::map<std::wstring, std::wstring>::iterator EndProperties();

    std::list<ConfigurationObject*>::iterator BeginObjects();
    std::list<ConfigurationObject*>::iterator EndObjects();

private:
    std::wstring name;
    std::map<std::wstring, std::wstring>	propertyList;
    std::list<ConfigurationObject*>		objectList;
};
#endif // CONFIGURATIONOBJECT_H
