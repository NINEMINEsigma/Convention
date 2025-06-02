#ifndef CONVENTION_KIT_WEB_INSTANCE_H
#define CONVENTION_KIT_WEB_INSTANCE_H

#include "Convention/instance/Interface.h"

namespace Convention
{

	struct WebIndicator
	{
#if defined(_WIN64)||defined(_WIN32)
		using SocketType = unsigned long long;

#endif
		struct Broadcast
		{
			struct Server
			{
				SocketType serverSocket;
				std::vector<SocketType> clientSockets;
				std::mutex clientsMutex;
				bool running = false;
				int PORT = 8888;
			};
			struct Client
			{
				SocketType clientSocket;
				std::atomic<bool> running = false;
				const char* SERVER_IP = "127.0.0.1";
				int PORT = 8888;
			};
		};
		using tag = void;
		constexpr static bool value = true;
	};

	template<template<typename...> class Allocator>
	class instance<WebIndicator::Broadcast::Server, true, Allocator, false>
		: public instance<WebIndicator::Broadcast::Server, false, Allocator, false>
	{
	public:
		using tag = WebIndicator::Broadcast::Server;
	private:
		using _Mybase = instance<WebIndicator::Broadcast::Server, false, Allocator, false>;
		void HandleClient(WebIndicator::SocketType clientSocket);
		void BroadcastMessage(const char* message, WebIndicator::SocketType sender);
	public:
		instance() :_Mybase(BuildMyPtr()) {}
		bool Init();
		void Start();
		void Stop();
	};

	template<template<typename...> class Allocator>
	class instance<WebIndicator::Broadcast::Client, true, Allocator, false>
		: public instance<WebIndicator::Broadcast::Client, false, Allocator, false>
	{
	public:
		using tag = WebIndicator::Broadcast::Client;
	private:
		using _Mybase = instance<WebIndicator::Broadcast::Client, false, Allocator, false>;
		void ReceiveMessages();
	public:
		instance() :_Mybase(BuildMyPtr()) {}
		bool Init();
		void Start();
		void Stop();
	};

}

#endif // !CONVENTION_KIT_WEB_INSTANCE_H
