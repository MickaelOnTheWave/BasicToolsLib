#ifndef BYTESTRING_H
#define BYTESTRING_H

#include <map>
#include <string>

static const unsigned long long ONE_BYTE     = 1;
static const unsigned long long ONE_KILOBYTE = 1024;
static const unsigned long long ONE_MEGABYTE = 1024 * 1024;
static const unsigned long long ONE_GIGABYTE = 1024 * ONE_MEGABYTE;


template <class stringT>
class BaseByteString
{
public:
    virtual ~BaseByteString() {}

protected:
   virtual void FillUnitMap() = 0;

   stringT InternalFormat(const unsigned long long bytes)
   {
      stringT formattedValue;

      if (bytes > ONE_GIGABYTE)
         formattedValue = FormatByteStringInUnit(bytes, ONE_GIGABYTE);
      else if (bytes > ONE_MEGABYTE)
         formattedValue = FormatByteStringInUnit(bytes, ONE_MEGABYTE);
      else if (bytes > ONE_KILOBYTE)
         formattedValue = FormatByteStringInUnit(bytes, ONE_KILOBYTE);
      else
         formattedValue = FormatByteStringInUnit(bytes, ONE_BYTE);

      return formattedValue;
   }
    
   stringT FormatByteStringInUnit(const unsigned long long value,
                                  const unsigned long long unit)
   {
      const double normalizedValue = value / static_cast<double>(unit);
      return CreateFormattedValue(normalizedValue) + units[unit];;
   }

   virtual stringT CreateFormattedValue(const double value) = 0;

   std::map<unsigned long long, stringT> units;
};

template <class stringT>
class ByteString : public BaseByteString<stringT>
{
protected:
   virtual void FillUnitMap()
   {
   }

   virtual stringT CreateFormattedValue(const double value)
   {
      return stringT();
   }
};


template <>
class ByteString<std::string> : public BaseByteString<std::string>
{
public:
   static std::string Format(const unsigned long long bytes)
   {
      ByteString<std::string> formatter;
      formatter.FillUnitMap();
      return formatter.InternalFormat(bytes);
   }

private:
   virtual void FillUnitMap()
   {
      units[ONE_BYTE] = "bytes";
      units[ONE_KILOBYTE] = "Kb";
      units[ONE_MEGABYTE] = "Mb";
      units[ONE_GIGABYTE] = "Gb";
   }

   virtual std::string CreateFormattedValue(const double value)
   {
      const size_t MAX_BUFFER_SIZE = 20;
      char buffer[MAX_BUFFER_SIZE];
#ifdef _MSC_VER
      sprintf_s(buffer, MAX_BUFFER_SIZE, "%.2f ", value);
#else
      sprintf(buffer, "%.2f ", value);
#endif

      return std::string(buffer);
   }
};

template <>
class ByteString<std::wstring> : public BaseByteString<std::wstring>
{
public:
   static std::wstring Format(const unsigned long long bytes)
   {
      ByteString<std::wstring> formatter;
      formatter.FillUnitMap();
      return formatter.InternalFormat(bytes);
   }

private:
   virtual void FillUnitMap()
   {
      units[ONE_BYTE] = L"bytes";
      units[ONE_KILOBYTE] = L"Kb";
      units[ONE_MEGABYTE] = L"Mb";
      units[ONE_GIGABYTE] = L"Gb";   }

   virtual std::wstring CreateFormattedValue(const double value)
   {
      const size_t MAX_BUFFER_SIZE = 20;
      wchar_t buffer[MAX_BUFFER_SIZE];
#ifdef _MSC_VER
      swprintf_s(buffer, MAX_BUFFER_SIZE, L"%.2f ", value);
#else
      swprintf(buffer, MAX_BUFFER_SIZE, L"%.2f ", value);
#endif

      return std::wstring(buffer);
   }
};

#endif // BYTESTRINGFORMATTER_H
