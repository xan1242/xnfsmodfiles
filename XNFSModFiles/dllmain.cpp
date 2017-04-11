// XNFSModFile - a chunk resource file adder/replacer for BBX NFS games
#include "..\includes\injector\injector.hpp"
#include "..\includes\IniReader.h"

#ifdef GAME_UG // broken
#include "UG_addresses.h"
#define UNDERGROUNDMODE
#endif

#ifdef GAME_UGARCADE // should be the same as regular UG, needs more work as it's crashing
#include "UGArcade_addresses.h"
#define UNDERGROUNDMODE
#endif

#ifdef GAME_UG2
#include "UG2_addresses.h"
#endif

#ifdef GAME_MW
#include "MW_addresses.h"
#endif

#ifdef GAME_CARBON
#include "Carbon_addresses.h"
#endif

#ifdef GAME_PS
#include "ProStreet_addresses.h"
#endif

#ifdef GAME_UC // no idea if it even works
#include "Undercover_addresses.h"
#endif

unsigned int CreateResourceFileFunction = CREATERESOURCEADDRESS;
unsigned int BeginLoadingFunction = BEGINLOADINGADDRESS;

#ifdef GAME_UGARCADE
bool bLoadAmusementChunks = 0;
#endif

#ifndef GAME_PS
unsigned int ModFileCaveExit = CAVEEXIT;
#endif

#ifdef GAME_UC
unsigned int SharedStringPoolAllocateFunction = STRINGPOOLALLOCATEADDRESS;
#endif

unsigned int NumberOfFiles = 0;
char TempStringBuffer[15];
char* PathBuffer;

int CreateResourceFile(const char* FileName, int ResourceFileType, int something1, int something2, int something3)
{
	_asm
	{
#ifdef GAME_UGARCADE
		push 0
		push 0
#endif
		push something3
		push something2
		push something1
		push ResourceFileType
		push[FileName]
		call CreateResourceFileFunction
	}
}

#ifdef GAME_UC
unsigned int SharedStringPoolAllocate(const char* FileName)
{
	_asm
	{
		mov esi, eax
		push [FileName]
		mov dword ptr [esi+0x28], 0x2000
		mov [esi+0x2C], edi
		call SharedStringPoolAllocateFunction
	}
}
#endif

void BeginLoading(void(*something1)(void *), void *something2)
{
	_asm
	{
#ifndef UNDERGROUNDMODE
		push something1
		push something2

#ifdef GAME_UC
		mov ecx, esi
		mov [esi+0x24], eax
#else
		mov ecx, eax
#endif

#endif
		call BeginLoadingFunction
	}
}

void LoadFiles()
{
	CIniReader inireader("");
	for (unsigned int i = 1; i <= NumberOfFiles; i++)
	{
		sprintf(TempStringBuffer, "File%d", i);
		PathBuffer = inireader.ReadString("Mods", TempStringBuffer, "");
		CreateResourceFile(PathBuffer, 1, 0, 0, 0);

#ifdef GAME_UC
		SharedStringPoolAllocate(PathBuffer);
#endif

		BeginLoading(0, 0);
	}

#ifdef GAME_UGARCADE
	if (bLoadAmusementChunks)
		CreateResourceFile("GLOBAL\\AMUSEMENT\\GLOBALA.BUN", 1, 0, 0, 0);
	else
		CreateResourceFile("GLOBAL\\CAREER\\GLOBALA.BUN", 1, 0, 0, 0);
#else
	CreateResourceFile("GLOBAL\\GLOBALA.BUN", 1, 0, 0, 0);
#endif

#ifdef GAME_UC
	SharedStringPoolAllocate("GLOBAL\\GLOBALA.BUN");
#endif

	BeginLoading(0, 0);
}

void __declspec(naked) ModFileCave()
{
	LoadFiles();
	_asm
	{
#ifdef GAME_PS
		retn
#else
		jmp ModFileCaveExit
#endif
	}
}

#ifdef GAME_UGARCADE
void __declspec(naked) MarkAmusementMode()
{
	bLoadAmusementChunks = 1;
	ModFileCave();
}
#endif

int Init()
{
	CIniReader inireader("");
	NumberOfFiles = inireader.ReadInteger("ModFiles", "NumberOfFiles", 0);
	injector::MakeJMP(CAVEENTRY, ModFileCave, true);

#ifdef GAME_UGARCADE
	injector::MakeJMP(CAVEENTRY2, MarkAmusementMode, true);
#endif

	return 0;
}

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		Init();
	}
	return TRUE;
}
