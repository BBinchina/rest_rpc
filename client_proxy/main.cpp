#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <kapok/Kapok.hpp>
#include "client_proxy.hpp"
#include "base64.hpp"
#include "../common.h"

struct person
{
	int age;
	std::string name;

	META(age, name);
};

void test_client()
{
	try
	{
		boost::asio::io_service io_service;
		DeSerializer dr;
		client_proxy client(io_service);
		client.connect("192.168.2.154", "9000");
		person p = { 20, "aa" };
		//auto str = client.make_json("fun1", p, 1);
		//client.call(str);
		
		std::string result = client.call("fun1", p, 1);
		dr.Parse(result);

		response_msg<int> response = {};
		dr.Deserialize(response);
		if (response.code == result_code::OK)
		{
			std::cout << response.result << std::endl;
		}
		io_service.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

template<typename T>
void handle_result(const char* result)
{
	DeSerializer dr;
	dr.Parse(result);
	Document& doc = dr.GetDocument();
	doc.Parse(result);
	if (doc[CODE].GetInt() == result_code::OK)
	{
		response_msg<T> response = {};
		dr.Deserialize(response);
		std::cout << response.result << std::endl;
	}
	else
	{
		//maybe exception, output the exception message.
		std::cout << doc[RESULT].GetString() << std::endl;
	}
}

void test_async_client()
{
	try
	{
		boost::asio::io_service io_service;
		client_proxy client(io_service);
		client.async_connect("baidu.com", "9000", 5*1000, [&client] (boost::system::error_code& ec)
		{
			if (ec)
			{
				std::cout << "connect error." << std::endl;
				return;
			}

			client.async_call("add", [&client](boost::system::error_code ec, std::string result)
			{
				if (ec)
				{
					std::cout << "call error." << std::endl;
					return;
				}

				handle_result<int>(result.c_str());
			},1, 2);

		});

		io_service.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

void test_spawn_client()
{
	try
	{
		boost::asio::io_service io_service;
		boost::asio::spawn(io_service, [&io_service] (boost::asio::yield_context yield)
		{
			client_proxy client(io_service);
			client.async_connect("127.0.0.1", "9000", yield);
			//auto str = client.make_json("fun1", p, 1);
			//client.call(str);

			std::string result = client.async_call("add", yield, 1,2);
			handle_result<int>(result.c_str());
		});
		io_service.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

void test_performance()
{
	try
	{
		boost::asio::io_service io_service;
		
		client_proxy client(io_service);
		client.connect("192.168.2.154", "9000");

		auto str = client.make_json("add", 1, 2);
		std::thread thd([&io_service] {io_service.run(); });
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		while (true)
		{
			client.call(str);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

void test_translate()
{
	try
	{
		boost::asio::io_service io_service;
		client_proxy client(io_service);
		client.connect("127.0.0.1", "9000");
		
		std::string result = client.call("translate", "test");
		handle_result<std::string>(result.c_str());
		//std::this_thread::sleep_for(std::chrono::seconds(20));
		io_service.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

void test_upload()
{
	try
	{
		boost::asio::io_service io_service;
		client_proxy client(io_service);
		client.connect("127.0.0.1", "9000");
		//client.connect("baidu.com", "12345", 5*1000);

		std::ifstream file("client_proxy.sln", ios::binary);
		if (!file.is_open())
			return;

		std::stringstream ss;
		ss << file.rdbuf();
		auto content = base64_encode(ss.str().c_str(), ss.str().length());
		std::string result = client.call("upload", "test", content);
		handle_result<bool>(result.c_str());
		io_service.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

void test_read()
{
	try
	{
		boost::asio::io_service io_service;
		client_proxy client(io_service);
		client.connect("127.0.0.1", "9000");
		auto str = client.call("translate", "test");
		std::cout << str << std::endl;
		std::thread thd([&io_service] {io_service.run(); });

		//wait for message from server.

		bool ok = true;
		while (ok)
		{
			size_t length = client.recieve();
			
			if (length != 0)
				std::cout << client.recieve_data() << std::endl;
		}

		thd.join();
//		io_service.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

void test_sub()
{
	try
	{
		boost::asio::io_service io_service;
		client_proxy client(io_service);
		client.connect("127.0.0.1", "9000");
		auto json_str = client.sub("translate");
		std::cout << json_str << std::endl;
		std::thread thd([&io_service] {io_service.run(); });

		//wait for message from server.
		bool ok = true;
		while (ok)
		{
			size_t length = client.recieve();
			
			ok = length > 0;
			if (ok)
				std::cout << client.recieve_data() << std::endl;
		}

		thd.join();
		std::string str;
		std::cin >> str;
		/*std::cout << str << std::endl;
		io_service.run();*/
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

void test_pub()
{
	try
	{
		boost::asio::io_service io_service;
		client_proxy client(io_service);
		client.connect("127.0.0.1", "9000");
		std::thread thd([&io_service] {io_service.run(); });
		while (true)
		{
			client.pub("translate", "test");
			std::string str;
			std::cin >> str;
			if (str != "pub")
				break;
		}

		//io_service.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

int main()
{
	//test_performance();
	//test_client();
	//test_upload();
	//while (true)
	//{
	//	std::string str;
	//	std::cin >> str;
	//	if (str == "pub")
	//		test_pub();
	//	else if (str == "sub")
	//		test_sub();
	//	else
	//		break;
	//}
	
	//test_sub();
	//test_read();
	test_translate();
	//test_async_client();
	//test_spawn_client();
	return 0;
}