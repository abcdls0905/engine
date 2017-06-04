
#ifndef _UTILS_POD_TRAITS_H
#define _UTILS_POD_TRAITS_H

#include "../public/macros.h"
#include "../public/persistid.h"
#include "../public/inlines.h"

// TPodTraits

template<typename TYPE>
class TPodTraits
{
public:
	static size_t GetHash(const TYPE& value);
	static bool Equal(const TYPE& v1, const TYPE& v2);
};

template<>
class TPodTraits<int>
{
public:
	static size_t GetHash(const int& value)
	{
		return size_t(value);
	}

	static bool Equal(const int& v1, const int& v2)
	{
		return (v1 == v2);
	}
};

template<>
class TPodTraits<unsigned int>
{
public:
	static size_t GetHash(const unsigned int& value)
	{
		return size_t(value);
	}

	static bool Equal(const unsigned int& v1, const unsigned int& v2)
	{
		return (v1 == v2);
	}
};

template<>
class TPodTraits<int64_t>
{
public:
	static size_t GetHash(const int64_t& value)
	{
		return size_t(value);
	}

	static bool Equal(const int64_t& v1, const int64_t& v2)
	{
		return (v1 == v2);
	}
};

template<>
class TPodTraits<uint64_t>
{
public:
	static size_t GetHash(const uint64_t& value)
	{
		return size_t(value);
	}

	static bool Equal(const uint64_t& v1, const uint64_t& v2)
	{
		return (v1 == v2);
	}
};

template<>
class TPodTraits<short>
{
public:
	static size_t GetHash(const short& value)
	{
		return size_t(value);
	}

	static bool Equal(const short& v1, const short& v2)
	{
		return (v1 == v2);
	}
};

template<>
class TPodTraits<unsigned short>
{
public:
	static size_t GetHash(const unsigned short& value)
	{
		return size_t(value);
	}

	static bool Equal(const unsigned short& v1, const unsigned short& v2)
	{
		return (v1 == v2);
	}
};

template<>
class TPodTraits<char>
{
public:
	static size_t GetHash(const char& value)
	{
		return size_t(value);
	}

	static bool Equal(const char& s1, const char& s2)
	{
		return (s1 == s2);
	}
};

template<>
class TPodTraits<unsigned char>
{
public:
	static size_t GetHash(const unsigned char& value)
	{
		return size_t(value);
	}

	static bool Equal(const unsigned char& s1, const unsigned char& s2)
	{
		return (s1 == s2);
	}
};

template<>
class TPodTraits<wchar_t>
{
public:
	static size_t GetHash(const wchar_t& value)
	{
		return size_t(value);
	}

	static bool Equal(const wchar_t& s1, const wchar_t& s2)
	{
		return (s1 == s2);
	}
};

template<>
class TPodTraits<void*>
{
public:
	static size_t GetHash(const void* value)
	{
		return size_t(value);
	}

	static bool Equal(const void* s1, const void* s2)
	{
		return (s1 == s2);
	}
};

template<>
class TPodTraits<PERSISTID>
{
public:
	static size_t GetHash(const PERSISTID& value)
	{
		return size_t(value.nIdent + value.nSerial);
	}

	static bool Equal(const PERSISTID& s1, const PERSISTID& s2)
	{
		return (s1 == s2);
	}
};

template<>
class TPodTraits<const char*>
{
public:
	static size_t GetHash(const char* value)
	{
		return GetHashValueCase(value);
	}

	static bool Equal(const char* s1, const char* s2)
	{
		return strcmp(s1, s2) == 0;
	}
};

template<>
class TPodTraits<const wchar_t*>
{
public:
	static size_t GetHash(const wchar_t* value)
	{
		return GetHashValueCaseW(value);
	}

	static bool Equal(const wchar_t* s1, const wchar_t* s2)
	{
		return wcscmp(s1, s2) == 0;
	}
};

template<typename TYPE>
class TPodTraitsCI: public TPodTraits<TYPE>
{
};

template<>
class TPodTraitsCI<const char*>
{
public:
	static size_t GetHash(const char* value)
	{
		return GetHashValue(value);
	}

	static bool Equal(const char* s1, const char* s2)
	{
		return stricmp(s1, s2) == 0;
	}
};

template<>
class TPodTraitsCI<const wchar_t*>
{
public:
	static size_t GetHash(const wchar_t* value)
	{
		return GetHashValueW(value);
	}

	static bool Equal(const wchar_t* s1, const wchar_t* s2)
	{
		return wcsicmp(s1, s2) == 0;
	}
};

#endif // _UTILS_POD_TRAITS_H
