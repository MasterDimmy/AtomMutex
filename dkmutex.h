#pragma once
/*
	DK Mutex atom functions v1.5
	(c) DK, April 2009 - July 2010
	
	Мультипотоковые функции - аналоги мютексов. 
	Для функционирования не требуют библиотек.
	
	bool lockmutex (int mutexnumber)
	bool unlockmutex (int mutexnumber)
	
	возвращают true если успешно
	false в случаях:
		1) мютекс для освобождения не лочился а значит не был создан
		2) нет свободной памяти
*/

#include <Windows.h>
#include <cstdio>

#ifndef lockmutex
#define lockmutex(a) my_lockmutex(a,__FILE__,__LINE__)
#endif

bool my_lockmutex (int mutexnumber,const char *file, int line);
bool unlockmutex (int mutexnumber);

/*
мютексы, используемые в проекте:
*/
enum Mutexes {
	CONTROL_SERVICE, // - управление сервисами
	CONTROL_PROGRAM, // - контроль завершения приложения. как только приложение можно закрыть то делается unlockmutex(CONTROL_PROGRAM);
	EVENT_QUEUE, // - на доступ к очереди событий, используется в EventManager
	INNER_SERVER,		// - внутренний модуля Server.cpp
	INNER_COMMUNICATOR, // - внутренний мютекс коммуникатора
	INNER_CLIENT_MANAGER, // - внутрений менеждера клиентов
	INNER_TASK_MANAGER,	 // - внутренний менеждера заданий
	INNER_DICT_MANAGER,	// - внутренний менеждера словарей
	INNER_BD,		// - внутренний баз данных
	LOG_MUTEX,	// - внутрений у LOG_GRAPH_MUTEX
	INNER_WMI,	// - внутренний у WMI

	// внутриклиентские мютексы
	INNER_CLIENT_DLL,	// у каждой ДЛЛ модуля апробации внутри свой мютекс
	INNER_CLIENT_EXE	// каждый клиент имеет свой основной мютекс
};
