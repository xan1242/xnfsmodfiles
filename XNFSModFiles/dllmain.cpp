// XNFSModFile - a chunk resource file adder/replacer for BBX NFS games
#include "..\includes\injector\injector.hpp"
#include "XNFSPrintTypes.h"

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
#define HAS_STAGE2
#endif

#ifdef GAME_MW
#include "MW_addresses.h"
#define HAS_STAGE2
#endif

#ifdef GAME_CARBON
#include "Carbon_addresses.h"
#define HAS_STAGE2
#endif

#ifdef GAME_PS
#include "ProStreet_addresses.h"
#define HAS_STAGE2
#endif

#ifdef GAME_UC
#include "Undercover_addresses.h"
#define HAS_STAGE2
#endif

#ifdef GAME_UGARCADE
bool bLoadAmusementChunks = 0;
#endif

#ifdef GAME_UG
unsigned int UndergroundJumpPoint = CAVEEXIT;
#endif

void*(*CreateResourceFile)(const char* FileName, int ResourceFileType, int something1, int something2, int something3) = (void*(*)(const char*, int, int, int, int))CREATERESOURCEADDRESS;
unsigned int(__thiscall *BeginLoading)(void* dis, int a1, int a2) = (unsigned int(__thiscall *)(void* dis, int a1, int a2))BEGINLOADINGADDRESS;

#ifdef GAME_UC
unsigned int(*SharedStringPoolAllocate)(const char* String) = (unsigned int(*)(const char*))STRINGPOOLALLOCATEADDRESS;
#endif

void LoadResourceFile(unsigned int ResourceFileType, const char* FileName)
{
	unsigned int* v1 = (unsigned int*)CreateResourceFile(FileName, ResourceFileType, 0, 0, 0);
#ifdef GAME_UC
	v1[10] = 0x2000;
	v1[11] = *(int*)0x00D3BDD4;
	v1[9] = SharedStringPoolAllocate(FileName);
#endif
	BeginLoading(v1, 0, 0);
}

void LoadFiles(unsigned int FileStage)
{
	if (!FileStage)
		printf("%s XNFSModFiles entering stage 1 (preinit)\n", PRINTTYPEINFO);
	else
		printf("%s XNFSModFiles entering stage 2 (postinit)\n", PRINTTYPEINFO);
	FILE *fin = fopen("XNFSModFiles.txt", "r");
	if (fin == NULL || !fin)
	{
		fin = fopen("scripts\\XNFSModFiles.txt", "r");
		if (fin == NULL || !fin)
		{
			if (!FileStage)
				printf("%s Couldn't open XNFSModFiles.txt\n", PRINTTYPEERROR);
			else
				printf("%s Couldn't open XNFSModFiles.txt at stage 2!\n", PRINTTYPEERROR);
			return;
		}
	}
	unsigned int CurrentResourceFileType = 0;
	unsigned int CurrentFileStage = 0;

	char CurrentLine[1028];
	char Part2[1024];
	char CurrentFileName[1024];

	while (!feof(fin))
	{
		CurrentResourceFileType = 0;
		if (fgets(CurrentLine, 1028, fin))
		{
			if (CurrentLine[0] != '#')
			{
				sscanf(CurrentLine, "%d,%d,%s", &CurrentFileStage, &CurrentResourceFileType, Part2);

				// inline comment
				if (strchr(Part2, '#') == NULL)
					strcpy(CurrentFileName, Part2);
				else
					strncpy(CurrentFileName, Part2, Part2 - strchr(Part2, '#'));

				if ((strcmp(CurrentFileName, "") != 0) && (CurrentFileStage == FileStage))
				{
					printf("%s Loading file %s resource type %d\n", PRINTTYPEINFO, CurrentFileName, CurrentResourceFileType);
					LoadResourceFile(CurrentResourceFileType, CurrentFileName);
				}
			}
		}
	}
	fclose(fin);
}

void LoadGlobalAChunksDetour()
{
	LoadFiles(0);
	LoadResourceFile(1, "GLOBAL\\GLOBALA.BUN");
}

#ifdef GAME_UG
void __declspec(naked) UndergroundJump()
{
	LoadGlobalAChunksDetour();
	_asm jmp UndergroundJumpPoint
}
#endif


int LoadFilesStage2(unsigned int somethingidunno)
{
	LoadFiles(1);
	return 0;
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
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);

#ifdef GAME_UG
	injector::MakeJMP(CAVEENTRY, UndergroundJump, true);
#else
	injector::MakeJMP(CAVEENTRY, LoadGlobalAChunksDetour, true);
#endif

#ifdef HAS_STAGE2
	injector::MakeCALL(STAGE2CAVEENTRY, LoadFilesStage2, true);
#ifdef GAME_UC
	injector::WriteMemory<unsigned int>(STAGE2CAVEENTRY, (unsigned int)&LoadFilesStage2, true);
#endif
#endif

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
