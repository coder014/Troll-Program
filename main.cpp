#include <windows.h>
#include <stdlib.h>
#include <time.h>

//function & global variable definition 
POINT cursor;
BOOL Stop = FALSE;
BOOL CALLBACK EnumChildProc(HWND hwnd,LPARAM lParam);
void WINAPI MouseMove(void);
void WINAPI TextRev(void);

//windows main entry 
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	//create mutex in order to avoid multi-execution
	HANDLE hMutex = CreateMutex(NULL,FALSE,"FuckChunHui666");
	if(hMutex)
	{
		if(GetLastError() == ERROR_ALREADY_EXISTS)
		{
			CloseHandle(hMutex);
			return -1;
		}
	}
	else
	{
		CloseHandle(hMutex);
		return -4;
	}

	//initialise & get full screen 
	srand(time(NULL));
	HWND hwnd = GetDesktopWindow();
	HDC hdc = GetWindowDC(hwnd);
	int w = GetDeviceCaps(hdc,DESKTOPHORZRES);//get width 
	int h = GetDeviceCaps(hdc,DESKTOPVERTRES);//get height
	
	//create threads
	HANDLE hThread[2];
	DWORD dwThreadId[2];
	hThread[0] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)MouseMove,NULL,0,&dwThreadId[0]);//mouse pointer shaking
	if(hThread[0] == NULL)//check error
		return -2;
	hThread[1] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)TextRev,NULL,0,&dwThreadId[1]);//messy text
	if(hThread[1] == NULL)//check error
	{
		Stop = TRUE;
		WaitForMultipleObjects(1,hThread,TRUE,INFINITE);
		CloseHandle(hThread[0]);
		return -3;
	}
	
	//main thread
	int sum = 0,ttime;
	while(sum <= 68000)
	{
		ttime = 4000+rand()%4001;
		sum += ttime;
		Sleep(ttime);
		BitBlt(hdc,0,0,w,h,hdc,0,0,NOTSRCCOPY);//inverse full screen color
	}

	//stop threads
	Stop=TRUE;//set stop signal
	WaitForMultipleObjects(2,hThread,TRUE,INFINITE);//wait for threads to stop
	for(int i=0;i<2;i++)
		CloseHandle(hThread[i]);//close threads' handle

	CloseHandle(hMutex);//close mutex's handle
	return 0;
}

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)//enumerate text areas
{
	char str[8];
	for(int i=0;i<8;i++)
		str[i] = rand();		//make messy text
	SendMessageTimeoutW(hwnd,WM_SETTEXT,NULL,(LPARAM)str,SMTO_ABORTIFHUNG,100,NULL);//set text
	return TRUE;
}

void WINAPI MouseMove(void)//shake mouse pointer
{
	Sleep(1000);
	while(!Stop)
	{
		GetCursorPos(&cursor);
		SetCursorPos(cursor.x+(rand()%15-7),cursor.y+(rand()%15-7));
		Sleep(10);
	}
}

void WINAPI TextRev(void)
{
	while(!Stop)
	{
		EnumChildWindows(GetDesktopWindow(),&EnumChildProc,NULL);//enumerate windows
		Sleep(5000);
	}
}
