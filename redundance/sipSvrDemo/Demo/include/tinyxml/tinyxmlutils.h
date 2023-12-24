#ifndef TINYXML_UTILS_INCLUDED
#define TINYXML_UTILS_INCLUDED

#include <string>
#include <sstream>
#include "tinyxml.h"

namespace TiXmlUtils
{

	/* �����ӽڵ� */
	void AddChild(TiXmlNode* node, const std::string& name);

	/* �����ı��ӽڵ� */
	template< class T>
	void AddChildText(TiXmlNode* node, const std::string& name, const T& val)
	{
		if (node != NULL && !name.empty())
		{
			std::stringstream ss;
			ss << val;
			node->InsertEndChild(TiXmlElement(name))->InsertEndChild(TiXmlText(ss.str()));
		}
	}
	template< class T>
	void AddChildText(TiXmlNode* node, const std::string& name, const std::string& str)
	{
		if (node != NULL && !name.empty())
		{
			node->InsertEndChild(TiXmlElement(name))->InsertEndChild(TiXmlText(str));
		}
	}

	/* ��ȡ�ӽڵ��ı� */
	std::string GetChildText(const TiXmlNode* node, const std::string& name);

    /* ����XML�ڵ㣬�ڵ�·����.�ָ� */
    TiXmlNode* FindXmlNode(TiXmlNode* rootNode, const std::string& path);

    /* ����ת�� */
    bool is_true(const char* val);
    void to_bool(const char* val, bool& b);
    void to_number(const char* val, int& num);
    void to_number(const char* val, int& num, int min);
#ifdef _WIN32
    void to_number(const char* val, __int64& num, __int64 min);
#else
    void to_number(const char* val, int64_t& num, int64_t min);
#endif
    void to_number(const char* val, int& num, int min, int max);
    void to_string(const char* val, std::string& str);

}

#endif

