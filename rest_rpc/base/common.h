#pragma once

namespace timax{ namespace rpc 
{
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

	struct head_t
	{
		int16_t code;
		int16_t	framework_type;
		int32_t len;
	};

	enum class result_code : int16_t
	{
		OK = 0,
		FAIL = 1,
		EXCEPTION = 2,
		ARGUMENT_EXCEPTION = 3
	};

	//
	enum class framework_type
	{
		DEFAULT = 0,
		ROUNDTRIP = 1,
		PUB = 2,
		SUB = 3,
	};

	enum class data_type
	{
		JSON = 0,
		BINARY = 1,
	};

	static std::atomic<std::uint64_t> g_succeed_count(0); //for test qps
} }

