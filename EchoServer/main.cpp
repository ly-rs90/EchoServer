#include"EchoServer.h"
#include <iostream>
using namespace std;


int main(int argc, char *argv[])
{
	EchoServer server(9999);
	if (server.Start()) {
		cout << "�����������������˿�9999��" << endl;
	}
	else {
		cout << "��������ʧ�ܣ�" << endl;
	}
	cout << "���롰C���رշ�������" << endl;
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