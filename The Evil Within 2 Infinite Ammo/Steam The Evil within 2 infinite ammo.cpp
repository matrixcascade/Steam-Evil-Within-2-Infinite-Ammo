#include <Windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <psapi.h>
#pragma comment(lib,"Psapi.lib")

#define EXEJ_EXENAME "TEW2.exe"

static unsigned char const bin[]={0x29,0x79,0x10,0x8b,0x51,0x18,0x8b,0x41,0x10,0x85,0xd2,0x79,0x1e};
static unsigned char const crkbin[]={0x90,0x90,0x90};
static unsigned char const crked[]={0x90,0x90,0x90,0x8b,0x51,0x18,0x8b,0x41,0x10,0x85,0xd2,0x79,0x1e};

int main()
{
	printf("The Evil Within 2(all versions) Infinite Ammo Cracker\nYou need start the game before crack it.\n\npress any key to execute crack-process.\n");
	getchar();
	fflush(stdin);
	

	int bfound=FALSE;
	HANDLE procSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(procSnap == INVALID_HANDLE_VALUE)
	{
		printf("Could not enum target process\n");
		return 0;
	}
	//
	PROCESSENTRY32 procEntry = { 0 };
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	BOOL bRet = Process32First(procSnap,&procEntry);
	while(bRet)
	{
		if (strcmp(procEntry.szExeFile,EXEJ_EXENAME)==0)
		{
			bfound=TRUE;
			break;
		}
		bRet = Process32Next(procSnap,&procEntry);
	}

	if (!bfound)
	{
		printf("The Evil Within 2 is not running\n");
		return 0;
	}
	CloseHandle(procSnap);
	

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procEntry.th32ProcessID);        

	if (!hProcess)
	{
		printf("Could not open target process\n");
		return 0;
	}
	DWORD cbNeeded = 0;
	TCHAR szModName[MAX_PATH];

	HMODULE hMods[256]={0};

	EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded,LIST_MODULES_64BIT);
	for (UINT i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
	{
		GetModuleFileNameEx(hProcess, hMods[i], szModName, _countof(szModName));
		printf("Module Scanning %llx\n",hMods[i]);

		SIZE_T fw;
		unsigned char buffer4k[4096];
		for (unsigned char *p=(unsigned char*)hMods[i];p<(unsigned char*)hMods[i]+0x7fffffff;p+=sizeof(bin))
		{
			if(ReadProcessMemory( hProcess, (LPVOID)p, buffer4k, sizeof(buffer4k), NULL))
			{
				for (int soft=0;soft<sizeof(buffer4k)-sizeof(bin);soft++)
				{
					if (memcmp(buffer4k+soft,bin,sizeof(bin))==0)
					{
						WriteProcessMemory(hProcess,(LPVOID)(p+soft),crkbin,sizeof(crkbin),&fw);
						if (fw==sizeof(crkbin))
						{
							printf("cracked succeed.\n press any key to exit.");
							CloseHandle(hProcess);
							getchar();
							return 0;
						}
					}
					if (memcmp(buffer4k+soft,crked,sizeof(crked))==0)
					{
						printf("The Game has already been cracked.\n\npress any key to exit.");
						CloseHandle(hProcess);
						getchar();
					}
				}
				p+=sizeof(buffer4k)-sizeof(bin);
			}
			else
			{
				break;
			}
			
		}
	}
	CloseHandle(hProcess);
	printf("crack failed.\n press any key to exit.");
	getchar();
}