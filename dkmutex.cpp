/*
DK Mutex atom functions v1.6
(c) DK, April 2009

Мультипотоковые функции - аналоги мютексов. 
Для функционирования не требуют библиотек.

bool lockmutex (int mutexnumber)
bool unlockmutex (int mutexnumber)

возвращают true если успешно
false в случаях:
1) мютекс для освобождения не лочился а значит не был создан
2) нет свободной памяти

12.09.2009:
	+ не загружает процессор
07.10.2009:
	+ поиск дедлоков - последовательный вызов lock() и lock() в одном потоке
06.05.2010:
	+ ~tmutex bugfix
06.07.2010:
	+ позиция в файле где был deadlock
*/

#include "dkmutex.h"

#pragma optimize( "", off )
struct tmutex {
	volatile DWORD state;
	int num;
	tmutex *next;
	DWORD tid; // Идентификатор потока, который заблокировал мютекс. 
	// Для проверки

	tmutex() {
		tid = 0;
		num = 0;
		state = 0;
		next = NULL;
	}
	~tmutex() {
		try {
			//здесь автоматически будет вызвана рекурсия так для next сработает ~tmutex
			if (next!=0) {
				delete next;
				next = 0;
			}
		} catch (...) {
		}
	}
};

volatile tmutex mutex;
volatile tmutex systemmutex;

//__declspec (naked) 
void lock(DWORD *a) {
	DWORD a1,a2;
	__asm {
			pusha
			mov eax, dword ptr a
			mov ebx, 1
		k:	mov a1, eax
			mov a2, ebx
	}
	Sleep (1);
	__asm {
			mov eax, dword ptr a1
			mov ebx, dword ptr a2
			lock xchg ebx, [eax]
			test ebx,ebx
			jnz k
			popa 
	}
}

//__declspec (naked) 
void unlock(DWORD *a) {
	__asm  {
		pusha
			mov eax, dword ptr a
			mov ebx, 0
			lock xchg ebx, [eax]
		popa
	}
}

bool my_lockmutex (int mutexnumber,const char *file, int line) {
	lock((DWORD*)&(systemmutex.state));
	tmutex *cur = (tmutex*)&mutex;
	while (cur->num != mutexnumber) {
		if (cur->next == NULL) {
			cur->next = new tmutex;
			if (cur->next==NULL) {
				unlock((DWORD*)&(systemmutex.state));
				return false;
			}				
			cur->next->num = mutexnumber;
			cur = cur->next;
		} else
			cur = cur->next;
	}
	// проверка на дедлок
	DWORD curtid = GetCurrentThreadId();
	if (cur->tid == curtid) {
		char temp[1024] = {0};
		sprintf(temp,"ОШИБКА! Мютекс уже был заблокирован! Возможен deadlock! файл:%s строка:%d",file,line);
		OutputDebugStringA(temp);
		char err[10]= {0};
		int i = 0;
		while (curtid>0) {
			#pragma warning(push)
			#pragma warning(disable: 4244)
			err[8-i] = '0' + (curtid % 10);
			#pragma warning(pop)
			i++;
			curtid = curtid / 10;
		}
		err[9] = 0;
		for (i=0;i<8;i++)
			if (err[i]==0)
				err[i] = ' ';
		// выводим THREAD ID на экран
		OutputDebugStringA(err);
	}
	cur->tid = GetCurrentThreadId();
	unlock((DWORD*)&(systemmutex.state));
	lock((DWORD*)&(cur->state));
	return true;
};

bool unlockmutex (int mutexnumber) {
	lock((DWORD*)&(systemmutex.state));
	tmutex *cur = (tmutex*)&mutex;
	while (cur->num != mutexnumber) {
		if (cur->next == NULL) {
			unlock((DWORD*)&(systemmutex.state));	
			return false;
		} else
			cur = cur->next;
	}
	cur->tid = 0;
	unlock((DWORD*)&(systemmutex.state));	
	unlock((DWORD*)&(cur->state));
	return true;
};
#pragma optimize( "", on )


/*
//ниже находится PoC код для созданных функций (c) DK

#include <Windows.h>
#include <cstdio>
#include <process.h>  

bool lockmutex (int mutexnumber);
bool unlockmutex (int mutexnumber);

int perem = 0;

void ThreadFunct1(void *a) {
lockmutex(CONTROL_PROGRAM);

printf("thread 1: go\n");
lockmutex(EVENT_QUEUE);
for (int i=0;i<10;i++) {
perem++;
Sleep(100);
printf("thread 1: %d\n",perem);
}
printf("thread 1: finish\n");
unlockmutex(EVENT_QUEUE);
}

void ThreadFunct2(void *b) {
printf("thread 2: waiting\n");
lockmutex(EVENT_QUEUE);
perem++;
printf("thread 2: %d\n",perem);
printf("thread 2: finish\n");
unlockmutex(EVENT_QUEUE);

unlockmutex(CONTROL_PROGRAM);
}

int main(int argc, char* argv[])
{
_beginthread( ThreadFunct1, 0, NULL );
Sleep(100);
_beginthread( ThreadFunct2, 0, NULL );

printf("Main thread: waiting\n");
lockmutex(CONTROL_PROGRAM);
unlockmutex(CONTROL_PROGRAM);
printf("Main thread: All done\n");

return 0;
}
*/