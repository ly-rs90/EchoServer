#include "EchoServer.h"



EchoServer::EchoServer(int port): m_port(port), BaseServer(port)
{
}


EchoServer::~EchoServer()
{
}

void EchoServer::OnRead(SOCKET s, char *buf, int len)
{
	send(s, buf, len, 0);
	BaseServer::OnRead(s, buf, len);
}
