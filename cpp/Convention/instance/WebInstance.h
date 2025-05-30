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

	template<template<typename...> class Allocator, typename _Dx>
	class instance<
		WebIndicator::Broadcast::Server,
		true,
		Allocator,
		std::unique_ptr,
		_Dx> :public instance<
		WebIndicator::Broadcast::Server,
		false,
		Allocator,
		std::unique_ptr,
		_Dx>
	{
	public:
		using tag = WebIndicator::Broadcast::Server;
		using TMybase = instance<tag, false>;
	private:
		void HandleClient(WebIndicator::SocketType clientSocket);
		void BroadcastMessage(const char* message, WebIndicator::SocketType sender);
	public:
		instance() :TMybase(new tag()) {}
		template<typename... TArgs>
		instance(TArgs&&... args) : TMybase(std::forward<TArgs>(args)...) {}
		bool Init();
		void Start();
		void Stop();
	};

	template<>
	class instance<WebIndicator::Broadcast::Client, true> :public instance<WebIndicator::Broadcast::Client, false>
	{
	public:
		using tag = WebIndicator::Broadcast::Client;
		using TMybase = instance<tag, false>;
	private:
		void ReceiveMessages();
	public:
		instance() :TMybase(new tag()) {}
		template<typename... TArgs>
		instance(TArgs&&... args) : TMybase(std::forward<TArgs>(args)...) {}
		instance_move_operator(public)
		{

		}
		bool Init();
		void Start();
		void Stop();
	};

}

#endif // !CONVENTION_KIT_WEB_INSTANCE_H
