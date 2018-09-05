#include"EchoServer.h"
#include <iostream>
using namespace std;


int main(int argc, char *argv[])
{
	EchoServer server(9999);
	if (server.Start()) {
		cout << "服务器启动，监听端口9999！" << endl;
	}
	else {
		cout << "服务启动失败！" << endl;
	}
	cout << "输入“C”关闭服务器。" << endl;
	char c = 0;
	while (c != 'C') {
		cin >> c;
		if (c == 'C') {
			server.Stop();
			break;
		}
	}
	return 0;
}