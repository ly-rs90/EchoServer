#pragma once
#include "BaseServer.h"
class EchoServer :
	public BaseServer
{
	int m_port;
public:
	EchoServer(int m_port);
	virtual ~EchoServer();
	void OnRead(SOCKET s, char *buf, int len) override;
};

