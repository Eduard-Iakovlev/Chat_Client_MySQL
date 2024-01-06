#include "Chat_Cross_platform.h"

int main()
{
#ifdef _WIN32
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
#endif // _WIN32

	Chat chat;
	chat.chat_work(); // работа чата
}
