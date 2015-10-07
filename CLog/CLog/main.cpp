
#include <stdio.h>
#include <Windows.h>
#include "PLog.h"
#include <string.h>


int main()
{
	PLog* log = new PLog();
	log->InitLogFile(L"D:\\", L"testfile.txt", 5000);
	log->PrintBanner(L" STARTED ");
	for (int i = 0; i < 50000;i++)
	log->Log(0, L" test stmt");
}


