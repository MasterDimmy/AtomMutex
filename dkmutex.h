#pragma once
/*
	DK Mutex atom functions v1.5
	(c) DK, April 2009 - July 2010
	
	��������������� ������� - ������� ��������. 
	��� ���������������� �� ������� ���������.
	
	bool lockmutex (int mutexnumber)
	bool unlockmutex (int mutexnumber)
	
	���������� true ���� �������
	false � �������:
		1) ������ ��� ������������ �� ������� � ������ �� ��� ������
		2) ��� ��������� ������
*/

#include <Windows.h>
#include <cstdio>

#ifndef lockmutex
#define lockmutex(a) my_lockmutex(a,__FILE__,__LINE__)
#endif

bool my_lockmutex (int mutexnumber,const char *file, int line);
bool unlockmutex (int mutexnumber);

/*
�������, ������������ � �������:
*/
enum Mutexes {
	CONTROL_SERVICE, // - ���������� ���������
	CONTROL_PROGRAM, // - �������� ���������� ����������. ��� ������ ���������� ����� ������� �� �������� unlockmutex(CONTROL_PROGRAM);
	EVENT_QUEUE, // - �� ������ � ������� �������, ������������ � EventManager
	INNER_SERVER,		// - ���������� ������ Server.cpp
	INNER_COMMUNICATOR, // - ���������� ������ �������������
	INNER_CLIENT_MANAGER, // - ��������� ��������� ��������
	INNER_TASK_MANAGER,	 // - ���������� ��������� �������
	INNER_DICT_MANAGER,	// - ���������� ��������� ��������
	INNER_BD,		// - ���������� ��� ������
	LOG_MUTEX,	// - ��������� � LOG_GRAPH_MUTEX
	INNER_WMI,	// - ���������� � WMI

	// ���������������� �������
	INNER_CLIENT_DLL,	// � ������ ��� ������ ��������� ������ ���� ������
	INNER_CLIENT_EXE	// ������ ������ ����� ���� �������� ������
};
