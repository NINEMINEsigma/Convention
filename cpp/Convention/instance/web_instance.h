#ifndef __FILE_CONVENTION_WEB_INSTANCE
#define __FILE_CONVENTION_WEB_INSTANCE

#include "Convention/instance/Interface.h"


struct web_indicator
{
#if defined(_WIN64)||defined(_WIN32)
	using socket_type = unsigned long long;

#endif
	struct broadcast
	{
		struct server
		{
			socket_type serverSocket;
			std::vector<socket_type> clientSockets;
			std::mutex clientsMutex;
			bool running = false;
			int PORT = 8888;
		};
		struct client
		{
			socket_type clientSocket;
			std::atomic<bool> running = false;
			const char* SERVER_IP = "127.0.0.1";
			int PORT = 8888;
		};
	};
	using tag = void;
	constexpr static bool value = true;
};

template<>
class instance<web_indicator::broadcast::server, true> :public instance<web_indicator::broadcast::server, false>
{
public:
	using tag = web_indicator::broadcast::server;
	using _Mybase = instance<tag, false>;
private:
	void handleClient(web_indicator::socket_type clientSocket);
	void broadcastMessage(const char* message, web_indicator::socket_type sender);
public:
	instance() :_Mybase(new tag()) {}
	template<typename... Args>
	instance(Args&&... args):_Mybase(std::forward<Args>(args)...){}
	instance_move_operator(public)
	{

	}
	bool init();
	void start();
	void stop();
};

template<>
class instance<web_indicator::broadcast::client, true> :public instance<web_indicator::broadcast::client, false>
{
public:
	using tag = web_indicator::broadcast::client;
	using _Mybase = instance<tag, false>;
private:
	void receiveMessages();
public:
	instance() :_Mybase(new tag()) {}
	template<typename... Args>
	instance(Args&&... args) : _Mybase(std::forward<Args>(args)...) {}
	instance_move_operator(public)
	{

	}
	bool init();
	void start();
	void stop();
};


#endif // __FILE_CONVENTION_WEB_INSTANCE
