#include<WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
#include<thread>
#include<iostream>
#include<conio.h>
#define _WIN32_WINNT 0x0500
#include<Windows.h>

using namespace std;

HANDLE h;
PCSTR IP = "51.83.128.163";
int PORT = 9034;
string currentInput = "";

void ClearCurrentInput()
{
	cout << '\r';
	for (unsigned int i = 0; i <= currentInput.length(); i++)
	{
		cout << " ";
	}
	cout << '\r';
}

void ListenForMessages(SOCKET s)
{
	char message[512];
	while (1)
	{
		int recv_size = recv(s, message, 512, 0);
		if (recv_size == SOCKET_ERROR || message[0] == NULL)
		{
			continue;
		}
		//if (strstr(message, "admin"))
		//{
			//SetConsoleTextAttribute(h, 12);
		//}
		//else
		//{
			SetConsoleTextAttribute(h, 7);
		//}
		ClearCurrentInput();
		cout << message << endl;
		SetConsoleTextAttribute(h, 10);
		cout << currentInput;
		Sleep(1);
	}
}

void CheckForHide()
{
	bool hidden = false;
	bool isPressed = false;
	while (1)
	{
		if (GetAsyncKeyState(17) && GetAsyncKeyState(72) && isPressed)
		{
			continue;
		}
		else if (!(GetAsyncKeyState(17) && GetAsyncKeyState(72)) && isPressed)
		{
			isPressed = false;
		}
		else if (GetAsyncKeyState(17) && GetAsyncKeyState(72) && !isPressed)
		{
			if (hidden)
			{
				ShowWindow(GetConsoleWindow(), SW_SHOW);
			}
			else
			{
				ShowWindow(GetConsoleWindow(), SW_HIDE);
			}
			hidden = !hidden;
			isPressed = true;
		}
		Sleep(1);
	}
}

int main()
{
	h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, 10);

	WSADATA wsa;
	cout << "Initializing Winsock..." << endl;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "Failed. Error Code : " << WSAGetLastError();
		return 1;
	}
	cout << "Initialized." << endl;

	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		cout << "Could not create socket : " << WSAGetLastError();
		return 1;
	}
	cout << "Socket created." << endl;

	sockaddr_in server;
	server.sin_family = AF_INET;
	inet_pton(AF_INET, IP, &server.sin_addr);
	server.sin_port = htons(PORT);

	if (connect(s, (sockaddr*)& server, sizeof(server)) < 0)
	{
		cout << "Connect error";
		return 1;
	}
	cout << "Connected." << endl;

	thread t1(ListenForMessages,s);
	thread t2(CheckForHide);
	char c;
	while (1)
	{
		c = _getch();
		if (c == 13)
		{
			if (send(s, currentInput.c_str(), 512, 0) < 0)
			{
				cout << "Send failed";
				return 1;
			}
			cout << "\ryou: " << currentInput << endl;
			currentInput = "";
		}
		else if (c == 8)
		{
			if (currentInput.length() > 0) currentInput.pop_back();
			ClearCurrentInput();
			cout << currentInput;
		}
		else
		{
			currentInput += c;
			cout << '\r' << currentInput;
		}
	}
	exit(0);
}