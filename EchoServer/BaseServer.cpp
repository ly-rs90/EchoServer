#include "BaseServer.h"

static DWORD WINAPI ThreadProc(LPVOID p)
{
	BaseServer *bs = (BaseServer*)p;

	while (true) {
		if (!bs->m_start) break;
		int index = WSAWaitForMultipleEvents(bs->m_total, bs->m_event, FALSE, 1000, FALSE);

		if (index == WSA_WAIT_TIMEOUT) continue;

		index = index - WSA_WAIT_EVENT_0;
		WSANETWORKEVENTS e;
		int r = WSAEnumNetworkEvents(bs->m_socket[index], bs->m_event[index], &e);
		if (r != 0) {
			closesocket(bs->m_socket[index]);
			WSACloseEvent(bs->m_event[index]);
			for (int i = index; i < bs->m_total - 1; i++) {
				bs->m_event[index] = bs->m_event[index + 1];
				bs->m_socket[index] = bs->m_socket[index + 1];
			}
			bs->m_total--;
			continue;
		}

		// 客户端连接请求
		if (e.lNetworkEvents & FD_ACCEPT && 
			e.iErrorCode[FD_ACCEPT_BIT] == 0 && 
			bs->m_total<WSA_MAXIMUM_WAIT_EVENTS) {
			SOCKET s = accept(bs->m_socket[index], NULL, NULL);
			WSAEVENT event = WSACreateEvent();
			bs->m_event[bs->m_total] = event;
			bs->m_socket[bs->m_total] = s;
			bs->m_total++;
			WSAEventSelect(s, event, FD_CLOSE | FD_READ);
		}
		// 读数据
		if ((e.lNetworkEvents & FD_READ) &&
			e.iErrorCode[FD_READ_BIT] == 0) {
			char *buf = new char[4096]();
			int len = 0;
			do {
				len = recv(bs->m_socket[index], buf, 4096, 0);
				if (len > 0) {
					bs->OnRead(bs->m_socket[index], buf, len);
				}
			} while (len > 0);
		}
		// 连接关闭
		if (e.lNetworkEvents & FD_CLOSE &&
			e.iErrorCode[FD_CLOSE_BIT] == 0) {
			closesocket(bs->m_socket[index]);
			WSACloseEvent(bs->m_event[index]);
			for (int i = index; i < bs->m_total - 1; i++) {
				bs->m_socket[index] = bs->m_socket[index + 1];
				bs->m_event[index] = bs->m_event[index + 1];
			}
			bs->m_total--;
		}
	}
	return 0;
}


BaseServer::BaseServer(int port)
{
	m_listenPort = port;
	m_total = 0;
	m_start = false;
}


BaseServer::~BaseServer()
{
	UnInit();
}


// 启动server，成功返回true，失败返回false
bool BaseServer::Start()
{
	if (!Init()) return false;

	if (m_listenPort < 0 || m_listenPort > 65535) return false;

	sockaddr_in localAddr;
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	localAddr.sin_port = htons(m_listenPort);
	m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (bind(m_listenSocket, (sockaddr*)&localAddr, sizeof(localAddr)) != 0) return false;

	if (listen(m_listenSocket, SOMAXCONN) != 0) return false;

	WSAEVENT e = WSACreateEvent();
	if (e == WSA_INVALID_EVENT) return false;

	if (WSAEventSelect(m_listenSocket, e, FD_ACCEPT) != 0) {
		WSACloseEvent(e);
		return false;
	}

	m_event[m_total] = e;
	m_socket[m_total] = m_listenSocket;
	m_total++;
	m_start = true;

	HANDLE t = CreateThread(NULL, 0, ThreadProc, (LPVOID)this, 0, NULL);

	if (t == NULL) return false;

	return true;
}


bool BaseServer::Stop()
{
	m_start = false;
	for (int i = 0; i < m_total; i++) {
		closesocket(m_socket[i]);
		WSACloseEvent(m_event[i]);
		m_total--;
	}
	return true;
}


// 初始化
bool BaseServer::Init()
{
	WSADATA wsaData;
	
	return (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0);
}


// 释放资源
bool BaseServer::UnInit()
{
	return (WSACleanup() == 0);
}

void BaseServer::OnRead(SOCKET s, char *buf, int len)
{
	delete[] buf;
}
