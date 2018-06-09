#pragma once
#include "crappy.h"
#include "./tsl.h"
/*
version:
3.7.33.22
*/
#define UWORD_BASE  0x42DCF90//0x3DF2F38//
#define GNAME_BASE 0x41da808//0x3F3FB80  
#define GAMEINSTANCE		0xd0
#define GAME_STATE		0x0100
#define LOCALPLAYERARRAY		0xa0  //ULocalPlayer
#define LOCALPLAYER		0x0000
#define LOCALPLAYERPOS		0xc0
#define PLAYERCONTROLLER	0x0030
#define APawn			0x0428
#define PLAYERCAMERAMANAGER	0x4a8
#define CAMERACACHE		 0x1550
#define VIEWPORTCLIENT		0xa0
#define PWORLD			0x98
#define ULEVEL			0x140

//Acctor
#define ACTORCOUNT		0X00A8
#define ACTORARRAY		0x00A0
#define ROOTCOMPONENT		0x120 
#define ACTORPOS		0x254
#define ACTORPOSINVEHICLE		0x0290  //player in vehicle  
#define ACTORID			0x0018
#define AACTOR_TEAM		0x4cc
#define ACTORHEALTH	    0x9e0//0x11CC
#define PLYAERROTATOR	0x02DC   //0x045C
#define MESH			0x448//0x0420
#define TEAMOBJECT		0x0CF8 //teamid
#define BONEARRAY       0x970
#define COMPONENTTOWORLD     0x280
#define PLAYERSTATE		0x400   //ATslPlayerState
#define PLAYERNAME		0x3C8 
#define TEAMNUMBER		0x474//0x04CC   //ATslPlayerState
#define DROPPEDITEMGROUPARRAY 0x318
#define DROPCOUNT		 0x340
#define RELATIVEPOSITION 0x2D0
#define PUITEM			0x538

#define VEHICLERIDERCOMPONENT  0x1260 
#define LASTVEHICLEPAWN		 0x0220

typedef unsigned char uint8;      /* Unsigned 8 bit quantity  */
typedef unsigned short uint16;    /* Unsigned 16 bit quantity */
typedef unsigned int uint32;
typedef unsigned long long  uint64;
typedef unsigned int uint;

#define BYTEn(x, n)   (*((_BYTE*)&(x)+n))
#define WORDn(x, n)   (*((_WORD*)&(x)+n))
#define WORD1(x)   WORDn(x,  1)
#define BYTE1(x)   BYTEn(x,  1)   
#define BYTE2(x) BYTEn(x, 2)

#define _BYTE uint8
#define _WORD uint16





void UpdateAddresses();
void CacheNames();
DWORD_PTR GetEntityList();
Vector3 GetLocalPlayerPos();
Vector3 GetActorPos(DWORD_PTR entity);
float GetActorHealth(DWORD_PTR pAActor);
string GetPlayerName(DWORD_PTR playerState);
std::string GetNameFromId(int w_id);
inline string Wchar_tToString(wchar_t *wchar);
int isUaz(int id);
int isDacia(int id);
int isPickup(int id);
int isBuggy(int id);
int isBike(int id);
int isVan(int id);
int isBoat(int id);
int isAirDrop(int id);
void DrawSkeleton(DWORD_PTR mesh);
void DrawVehicle(DWORD_PTR entity, Vector3 local, const char* txt, json &data);
Vector3 World2Screen(Vector3 WorldLocation, FCameraCacheEntry CameraCacheL);
FCameraCacheEntry GetCameraCache();
Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id);

/*
wchar_t MakecodeChar(wchar_t c, int key);
wchar_t CutcodeChar(wchar_t c, int key);
void Makecode(wchar_t  *pstr, int *pkey);
inline void Cutecode(wchar_t *pstr, int *pkey);
*/
//KReader  *m_KReader = nullptr;
