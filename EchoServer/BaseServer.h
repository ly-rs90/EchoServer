#pragma once
#include<WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
static DWORD WINAPI ThreadProc(LPVOID);
class BaseServer
{
public:
	BaseServer(int port=8000);
	virtual ~BaseServer();
private:
	int m_listenPort;
	SOCKET m_listenSocket;
	WSAEVENT m_event[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKET m_socket[WSA_MAXIMUM_WAIT_EVENTS];
	int m_total;
	bool m_start;
public:
	bool Start();
	bool Stop();
	virtual void OnRead(SOCKET s, char *buf, int len);
	friend DWORD WINAPI ThreadProc(LPVOID);
private:
	// 初始化
	bool Init();
	// 释放资源
	bool UnInit();
};

