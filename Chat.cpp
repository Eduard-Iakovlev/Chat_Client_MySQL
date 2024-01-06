#include "Chat.h"

Chat::Chat(){
	greeting();
}

Chat::Chat(std::string active_user_login, std::string active_recipient_login, std::string active_user_name) :
	_active_user_login(active_user_login), _active_recipient_login(active_recipient_login), _active_user_name(active_user_name) {}

//----------- Создание сокета ---------------------------
void Chat::socket_file() {
	_socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_file_descriptor == -1) {
		std::cout << "Не удалось создать сокет!" << std::endl;
		exit(1);
	}
}

//----------- Закрытие сокета ------------------------------
void Chat::close_socket() {
	close(_socket_file_descriptor);
}


//------------ Настройка связи с сервером ---------------------
void Chat::server_address() {
	// 
	_server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	// Зададим номер порта для связи
	_server_address.sin_port = htons(PORT);
	// Используем IPv4
	_server_address.sin_family = AF_INET;
	// Установка соединения с сервером
	_connection = connect(_socket_file_descriptor, (struct sockaddr*)&_server_address, sizeof(_server_address));
}


//--------------- Обмен данными------------------------------------
void Chat::exchange(const std::string mess) {
	transmitting(mess);
	receiving_server();
}

//-------------- Возврат первого символа сообщения --------------------
char Chat::message0() {
	return _message[0];
}

//--------------- Перевод сообщения в стринг -----------------------------
std::string Chat::message() {
	return std::string(_message);
}

//--------------- Прием данных ----------------------------------------------------------
void Chat::receiving_server() {
	memset(_message, 0, sizeof(_message));
	read(_socket_file_descriptor, _message, sizeof(_message));
}

//-------------- Отправка данных -----------------------------------
void Chat::transmitting(const std::string& mess) {
	memset(_message, 0, sizeof(_message));
	strcpy(_message, mess.c_str());
	ssize_t bytes = write(_socket_file_descriptor, _message, sizeof(_message));
}




//---------------- Приветствие ------------------------------------------------------------
void Chat::greeting() {
	clean_console();
	std::cout << "\n          Добро пожаловать!\n\n";
}

//----------------- Прощание --------------------------------------------------------------
void Chat::farewell() {
	std::cout << "\n\n          Всего хорошего.\n";
}

//----------------- Меню входа ------------------------------------------------------------
int Chat::logOrReg() {
	std::cout << " Меню входа в чат\n";
	Universal_Input<int> inputLOR('1', '3');
	std::cout << "\n Вход - 1\n";
	std::cout << " Регистрация - 2 \n";
	std::cout << " Выход - 3 \n -> ";

	return inputLOR.input();
}

//---------------- Меню в аккаунте -------------------------------------------------------
int Chat::menu_chat() {
	std::cout << "\n Меню аккаунта\n";
	Universal_Input<int> inputMC('1', '3');
	std::cout << "\n Выбрать беседу - 1\n";
	std::cout << " Создать сообщение всем - 2 \n";
	std::cout << " Выход из аккаунта - 3 \n ";
	std::cout << " -> ";
	return inputMC.input();
}

//--------------- Проверка логина -------------------------------------------------------
bool Chat::finding(std::string login) {
	if (_users.find(login) == _users.end()) return true;
	else return false;
}

//--------------- Проверка пароля -------------------------------------------------------
bool Chat::check_password(std::string password, std::string login) {
	if (_users.at(login).user_password() == password) return true;
	else return false;
}

//--------------- Вход и регистрация ----------------------------------------------------
void Chat::registration(int menu, bool* check_user) {
	User user;
	Universal_Input<std::string> inputPL(_fsymbolLogPass, _lsymbolLogPass);
	*check_user = false;
	// Вход в чат
	if (menu == 1) {
		receiving_server();
		std::cout << "\n" << message();
		user.get_user_login(inputPL.input());
		exchange(user.user_login());
		inputPL.clear();
		std::cout << "\n" << message();
		std::string pass = inputPL.input();
		user.get_user_password(pass);
		exchange(pass);
		inputPL.clear();
		int counter = 0;
		if (message() == "false") {
			std::cout << std::endl;
			std::cout << " Не верный логин или пароль!" << std:: endl;
			system_pause(2);
			return;
		}
		clean_console();
		get_user(user.user_login(), _users.at(user.user_login()).user_name());
		std::cout << "\n\n Вы вошли как:\n\n";
		_users.at(_active_user_login).showUser();
		*check_user = true;

	}
	// регистрация нового пользователя
	else {
		*check_user = true;
		std::cout << "\n Введите имя: ";
		std::string username;
		std::cin >> username;
		exchange(username);
		user.get_user_name(message());
		exchange(" имя пользователя активировано");
		std::cout << message();
		bool check_login;
		do {
			check_login = true;
			std::string login = inputPL.input();
			std::cout << std::endl;
			user.get_user_login(login);
			exchange(login);
			if (message() == "false") {
				user.clear_login();
				inputPL.clear();
				std::cout << "\n Данный логин занят, выберите другой: ";
				check_login = false;
			}
			inputPL.clear();
		} while (!check_login);
		exchange(" логин активирован");
		std::cout << message();
		std::string password = inputPL.input();
		exchange(password);
		if (message() == "регистрация прошла успешно") {
			user.get_user_password(password);
		}
		else {
			std::cout << " Ошибка регистрации\n";
			close_socket();
			exit(1);
		}

		_users.emplace(user.user_login(), user);
		get_user(user.user_login(), user.user_name());

		clean_console();
		std::cout << "\n\n Вы зарегистрированы как:\n\n";
		_users.at(_active_user_login).showUser();
		std::cout << std::endl;
	}
}

//----------------- Регистрация общего чата ---------------------------------------
void Chat::reg_all_user() {
	User user;
	user.get_user_login("ALL_USERS");
	user.get_user_password("admin");
	user.get_user_name("общий чат");
	_users.emplace(user.user_login(), user);
}

//---------------- Вывод списка участников чата -----------------------------------
void Chat::showListUsers() {
	int counter{ 0 };
	clear_show_user();
	std::cout << "\n Участники чата:\n\n";

	for (std::map<std::string, User>::iterator it = _users.begin(); it != _users.end(); it++) {
		counter++;
		if (it->second.user_login() == _active_user_login) continue;
		std::cout << counter << " - ";
		if (it->second.user_login() == "ALL_USERS") it->second.showUserName();
		else it->second.showUser();
		std::cout << "\n";
	}
}

//--------------- Возвращает логин активного пользователя --------------------------
std::string Chat::active_user_login() {
	return _active_user_login;
}

//--------------- Возвращает имя активного пользователя -----------------------------
std::string Chat::active_user_name() {
	return _active_user_name;
}

//--------------- Возвращает логин получателя ---------------------------------------
std::string Chat::active_recipient_login() {
	return _active_recipient_login;
}

//--------------- Активация пользователя --------------------------------------------
void Chat::get_user(std::string login, std::string name)
{
	_active_user_login = login;
	_active_user_name = name;
}

//--------------- Деактивация пользователя ------------------------------------------
void Chat::out_user() {
	_active_user_login = '\0';
	_active_user_name = '\0';

}

//--------------- Выбор получателя -------------------------------------------------
void Chat::get_recipient(int menu) {
	Universal_Input<int> inputID('0', '9');
	int counter = 0;
	std::string id_str;

	if (menu == 2) _active_recipient_login = "ALL_USERS";
	else {
		int id{ 0 };
		do {
			id = inputID.input();
			id_str = std::to_string(id);
			exchange(id_str);
			if (message() != " ID принят") {
				std::cout << "\n" << message();
				inputID.clear();
			}
			else break;

		} while (true);
		
		std::map<std::string, User>::iterator it = _users.begin();
		for (; it != _users.end(); it++) {
			counter++;
			if (counter == id) break;
		}

		_active_recipient_login = it->second.user_login();
	}
}

//------------- Определение количества пользователей --------------------------------
int Chat::sizeList() {
	return _users.size();
}

//------------- Создание и отправка сообщения ---------------------------------------
void Chat::send_message() {
	Message messages;
	std::string mess{};
	char menu{ ' ' };
	while (true) {
		receiving_server();
		std::cout << "\n Написать - \"Enter\", Выход - \"Esc\" -> ";
		menu = _getch();

		if (menu == _esc) {
			exchange("Esc");
			if(message() == "очистка")
			clear_show_user();
			break;
		}
		else if (menu == _enter) {
			exchange("enter");
			std::cout << "\n " << message() <<"\n";
			while (mess.empty()) {
				getline(std::cin, mess);
			}
			exchange(mess);
			messages.create_message(mess, _active_user_name, _active_user_login, _active_recipient_login);
			_messages.push_back(messages);
			clear_show_user();
			std::cout << "\n Сообщение ";
			if (_active_recipient_login == "ALL_USERS") std::cout << " в общий чат\n";
			else {
				std::cout << " для ";
				_users.at(_active_recipient_login).showUserName();
			}
			std::cout << message() << " \n";
			break;
		}
		else std::cout << " " << message() << " \n";
	}
	system_pause(2);
}

//----------------- Вывод беседы ----------------------------------------------------------
void Chat::show_message_list() {
	clear_show_user();
	std::cout << "\n Беседа с \n";
	_users.at(_active_recipient_login).showUser();
	for (Message i : _messages) {
		if (_active_user_login == i.login_sender() && _active_recipient_login == i.login_recipient() && _active_recipient_login != "ALL_USERS"
			|| _active_user_login == i.login_recipient() && _active_recipient_login == i.login_sender()) {

			i.show_message();
		}
		else if (_active_recipient_login == "ALL_USERS" && _active_recipient_login == i.login_recipient()) i.show_message();
		
	}
}

//----------------- Действие если пользователь один --------------------------------------
void Chat::one_user() {
	receiving_server();
	std::cout << message() << std::endl;
	out_user();
	system_pause(2);
	clean_console();
}

//----------------- Действие если пользователей нет --------------------------------------
void Chat::no_users() {
	std::cout << "\n Зарегистрированных пользователей пока нет. \n Пожалуйста сначала зарегистрируйтесь.\n ";
	system_pause(2);
	clean_console();
}

//----------------- Вывод активного пользователя с очисткой консоли ----------------------- 
void Chat::clear_show_user()
{
	clean_console();
	_users.at(_active_user_login).showUser();
}

//----------------- Очистка консоли -------------------------------------------------------
void Chat::clean_console(){
#ifdef _WIN32
	std::system("cls");  // Очистка экрана в системе Windows
#else
	std::system("clear");  // Очистка экрана в системах UNIX и Linux
#endif
}

//----------------- Пауза -----------------------------------------------------------------
void Chat::system_pause(int second){
	std::this_thread::sleep_for(std::chrono::seconds(second));

}

//----------------- Основная функция работы чата -------------------------------------------
void Chat::chat_work(){
	reg_all_user();

	socket_file();
	server_address();
	receiving_server();
	if (message() == " проверка связи") {
		transmitting(" связь установлена ");
	}
	else {
		close_socket();
		std::cout << " нет связи с сервером \n";
		exit(1);
	}

	while(_work) {

		_menu = logOrReg();
		clean_console();
		// выход из аккаунта
		if (_menu == 3) {
			exchange("3");
			std::cout << message() << std::endl;
			farewell();
			close_socket();
			_work = false;
			break;
		}
		// вход в аккаунт
		else if (_menu == 1) {
			exchange("1");
			if (message() == "error") {
				no_users();
				continue;
			}
			if (message() == "error2") {
				one_user();
				continue;
			}
			registration(_menu, &_check_user);
			if (_check_user == false) {
				system_pause(2);
				clean_console();
				continue;
			}
		}

		//регистрация нового пользователя
		else {
			exchange("2");
			std::cout << message() << std::endl;
			registration(_menu, &_check_user);
			system_pause(2);
		}
		exchange("запрос на вход");
		// проверка кол-ва зарегистрированных
		if (message() == "error2") {
			one_user();
			continue;
		}
		//работа аккаунта
		account_work();
	}
}

//--------------------- Функция работы аккаунта ------------------------------------
void Chat::account_work(){

	do {
		_menu = menu_chat();
		_discussion = true;
		switch (_menu) {
		case 1: // личная беседа
			exchange("1");
			showListUsers();
			std::cout << message();
			get_recipient(_menu);
			show_message_list();
			send_message();
			break;
		case 2: // сообщение всем
			transmitting("2");
			get_recipient(_menu);
			send_message();
			break;
		case 3: // выход
			exchange("3");
			std::cout << "\n\n      " << message() << std::endl;
			out_user();
			system_pause(2);
			clean_console();
			_discussion = false;
			break;

		default:
			break;
		}

	} while (_discussion);
}

