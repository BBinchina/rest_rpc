#pragma once
#include <atomic>
#include <cstdint>
#include <kapok/Kapok.hpp>
#include "log.hpp"
#include "consts.h"

//resultҪô�ǻ������ͣ�Ҫô�ǽṹ�壻������ɹ�ʱ��codeΪ0, ����������޷������͵ģ���resultΪ��; 
//������з���ֵ�ģ���resultΪ����ֵ��response_msg�����л�Ϊһ����׼��json�����ط����ͻ��ˡ� 
//������Ϣ�ĸ�ʽ��length+body����4���ֽڵĳ�����Ϣ������ָʾ����ĳ��ȣ��Ӱ�����ɡ� 
template<typename T, typename Tag = void>
struct response_msg
{
	int		code;
	T		result;
	Tag		tag;
	META(code, result, tag);
};

template <typename T>
struct response_msg<T, void>
{
	int code;
	T result; //json��ʽ�ַ������������ͻ����ǽṹ��.
	META(code, result);
};

enum class result_code
{
	OK = 0,
	FAIL = 1,
	EXCEPTION = 2,
	ARGUMENT_EXCEPTION = 3
};

//
enum class framework_type : int16_t
{
	DEFAULT = 0,
	ROUNDTRIP = 1,
};

enum class data_type : int16_t
{
	JSON = 0,
	BINARY = 1,
};

static std::atomic<std::uint64_t> g_succeed_count(0); //for test qps
