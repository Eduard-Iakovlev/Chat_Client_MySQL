#pragma once

#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

#include <vector>
#include <map>
#include <cstdlib>
#include <thread>
#include <chrono>

#include "User.h"
#include "Message.h"
#include "Consol_Input.h"
#include "Hash.h"

#define MESSAGE_LENGTH 1024 // Максимальный размер буфера для данных
#define PORT 7777 // Будем использовать этот номер порта


class Chat
{
public:
	void socket_file();// создаем сокет
	void close_socket(); // Закрытие сокета
	void server_address();// Настройка связи с сервером
	void exchange(const std::string mess);// Обмен данными
	char message0();// Возврат первого символа сообщения
	std::string message();// Перевод сообщения в стринг
	void receiving_server(); // Прием данных
	void transmitting(const std::string& mess); //отправка данных
	void receiving_table_users(); // приём таблицы пользователей
	void receiving_table_message(); // приём таблицы сообщений



	Chat();
	Chat(std::string, std::string, std::string);
	void greeting();
	void farewell();
	int logOrReg();
	int menu_chat();

	bool finding(std::string);
	bool check_password(std::string, std::string);
	void registration(int, bool*);

	void showListUsers();
	std::string active_user_login();

	std::string active_user_name();
	std::string active_recipient_login();


	void get_user(std::string, std::string);
	void out_user();
	void get_recipient(int);


	int sizeList();

	void send_message();
	void show_message_list();
	std::string hash_mess(std::string& pass);

	void one_user();
	void no_users();

	void clear_show_user();
	void show_activ_user();
	void clean_console();
	void system_pause(int second);

	void chat_work();
	void account_work();


private:

	int _socket_file_descriptor, _connection;
	struct sockaddr_in _server_address, _client;
	char _message[MESSAGE_LENGTH];

	std::map <std::string, User> _users;
	std::vector<Message> _messages;
	std::string _active_user_login;
	std::string _active_recipient_login;
	std::string _active_user_name;

	int _menu{ 0 };

	bool _work{ true };
	bool _check_user{ false };
	bool _discussion{ true };

	const char _fsymbolLogPass = '0';
	const char _lsymbolLogPass = '~';
	const char _esc{ 27 };

#ifdef _WIN32
	const char _enter{ 13 };
#else
	const char _enter{ 10 };
#endif // _WIN32
};
