// Hack.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "stdafx.h"
#include <vector>
#include <Windows.h>
#include "proc.h"
#include "mem.h"

int main()
{
	HANDLE hProcess = 0;

	uintptr_t moduleBase = 0, localPlayerPtr = 0, healthAddr = 0, fireRateAddr = 0;
	bool bHealth = false, bAmmo = false, bRecoil = false, bFireRate = false;
	const int newValue = 1337;
	const int shootSpeed = 0;
	DWORD procId = GetProcId(L"ac_client.exe");

	


	std::cout << "Culprit's Cheat Menu\n"
		<< "---------------------------------------------\n"
		<< "Press H for Infinite Health\n"
		<< "Press J for Infinite Ammo\n"
		<< "Press K for No-recoil\n"
		<< "Press L for Rapid Fire!\n";

	if (procId)
	{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
		moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");
		localPlayerPtr = moduleBase + 0x10F4F4;
		healthAddr = FindDMAAddy(hProcess, localPlayerPtr, { 0xF8 });
		fireRateAddr = FindDMAAddy(hProcess, localPlayerPtr, { 0x374, 0xC, 0x10A });

	}
	else
	{
		std::cout << "Process not found! Press [ENTER] to exit\n";
		getchar();
		return 0;
	}

	DWORD dwExit = 0;
	while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE)
	{
		if (GetAsyncKeyState(0x48) & 1)
		{
			bHealth = !bHealth;
		}
		if (GetAsyncKeyState(0x4A) & 1)
		{
			bAmmo = !bAmmo;
			if (bAmmo)
			{
				//ff 06 = inc [esi];
				mem::PatchEx((BYTE*)(moduleBase + 0x637E9), (BYTE*)"\xFF\x06", 2, hProcess);
			}
			else
			{
				//ff 06 = dec [esi];
				mem::PatchEx((BYTE*)(moduleBase + 0x637E9), (BYTE*)"\xFF\x0E", 2, hProcess);
			}
		}
		if (GetAsyncKeyState(0x4B) & 1)
		{
			bRecoil = !bRecoil;
			if (bRecoil)
			{
				mem::NopEx((BYTE*)(moduleBase + 0x63786), 10, hProcess);
			}
			else
			{
				mem::PatchEx((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10, hProcess);
			}
		}
		if (GetAsyncKeyState(0x4C) & 1)
		{
			bFireRate = !bFireRate;
		}
		if (bFireRate)
		{
			mem::PatchEx((BYTE*)fireRateAddr, (BYTE*)&shootSpeed, sizeof(shootSpeed), hProcess);
		}
		if (GetAsyncKeyState(VK_INSERT) & 1)
		{
			return 0;
		}
		if (bHealth)
		{
			mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newValue, sizeof(newValue), hProcess);
		}

		Sleep(10);
	}

	std::cout << "Process not found! Press ENTER to exit\n";
	getchar();
	return 0;
}


