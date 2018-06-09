#include "Utils.h"
#include "md5.h"
#include <fstream>
//defining data
int s_width = 2560;
int s_height = 1440;
LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 d3ddev;
HWND hWnd;
HWND twnd;
MARGINS  margin = { 0,0,s_width,s_height };
LPD3DXFONT pFont;
ID3DXLine* d3dLine;

list<int> upper_part = { Bones::neck_01, Bones::Head, Bones::forehead };
list<int> right_arm = { Bones::neck_01, Bones::upperarm_r, Bones::lowerarm_r, Bones::hand_r };
list<int> left_arm = { Bones::neck_01, Bones::upperarm_l, Bones::lowerarm_l, Bones::hand_l };
list<int> spine = { Bones::neck_01, Bones::spine_01, Bones::spine_02, Bones::pelvis };

list<int> lower_right = { Bones::pelvis, Bones::thigh_r, Bones::calf_r, Bones::foot_r };
list<int> lower_left = { Bones::pelvis, Bones::thigh_l, Bones::calf_l, Bones::foot_l };

list<list<int>> skeleton = { upper_part, right_arm, left_arm, spine, lower_right, lower_left };





int ActorIds[4];
int uaz[3];
int dacia[5];
int motorbike[7];
int pickup[10];
int buggy[6];
int boat[2];
int van[3];
int itemtype[2];
int airDrop[3];
//


D3DXMATRIX Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}

Vector3 World2Screen(Vector3 WorldLocation, FCameraCacheEntry CameraCacheL)
{
	Vector3 Screenlocation = Vector3(0, 0, 0);

	auto POV = CameraCacheL.POV;
	Vector3 Rotation = POV.Rotation; // FRotator

	D3DMATRIX tempMatrix = Matrix(Rotation); // Matrix

	Vector3 vAxisX, vAxisY, vAxisZ;

	vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - POV.Location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	float FovAngle = POV.FOV;
	float ScreenCenterX = s_width / 2.0f;
	float ScreenCenterY = s_height / 2.0f;

	Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
	Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;

	return Screenlocation;
}


//
//typedef __int64(__fastcall *decryptf)(dword_ptr);
//ulonglong globalcrypttable = 0x3c34120;
//size_t calcfunclengthex(uint64_t funcaddress, byte end = 0xc3)
//{
//	size_t lenght = 0;
//	while (m_kreader->readtype<byte>(funcaddress++) != end)
//		lenght++;
//	return lenght;
//}
//template<class t> t __rol__(t value, int count)
//{
//	const uint nbits = sizeof(t) * 8;
//
//	if (count > 0)
//	{
//		count %= nbits;
//		t high = value >> (nbits - count);
//		if (t(-1) < 0) // signed value
//			high &= ~((t(-1) << count));
//		value <<= count;
//		value |= high;
//	}
//	else
//	{
//		count = -count % nbits;
//		t low = value << (nbits - count);
//		value >>= count;
//		value |= low;
//	}
//	return value;
//}
//inline uint8  __rol1__(uint8  value, int count) { return __rol__((uint8)value, count); }
//inline uint8  __ror1__(uint8  value, int count) { return __rol__((uint8)value, -count); }
//inline uint16 __rol2__(uint16 value, int count) { return __rol__((uint16)value, count); }
//inline uint16 __ror2__(uint16 value, int count) { return __rol__((uint16)value, -count); }
//inline uint64 __ror8__(uint64 value, int count) { return __rol__((uint64)value, -count); }
//
//
//pvoid codememact = null;
//dword_ptr decryptactors(dword_ptr cryptedoffset)
//{
//	if (codememact == null)
//		codememact = virtualalloc(0, 1024, mem_commit, page_execute_readwrite);
//
//	uint32 v1 = m_kreader->readtype<uint32>(cryptedoffset);
//	dword_ptr v2 = m_kreader->readtype<dword_ptr>(cryptedoffset + 8);
//
//	dword_ptr v4 = globalcrypttable + m_kreader->getpubase();
//
//	auto v5 = (unsigned __int16)__ror2__(v1 - 78, -78) ^ ((unsigned __int16)(word1(v1) + 42) + 42706);
//
//	dword_ptr targetfunc = m_kreader->readtype<dword_ptr>(v4 + (8 * (((unsigned __int8)(((v1 + 11) ^ (~((~byte2(v1) + 63) ^ 0xc1) + 69)) - 29) ^ ((unsigned __int8)__rol1__((unsigned __int16)((v1 + 11) ^ (~((~word1(v1) + 63) ^ 0xffc1) + 1861)) >> 8, 19) + 54)) % 128)));
//
//	ulong delta = m_kreader->readtype<ulong>(targetfunc + 10);
//
//	if (!m_kreader->readsize(targetfunc, codememact, 9))
//		return 0;
//	auto funcl = calcfunclengthex((uint64_t)(targetfunc + 14 + delta));
//	if (!m_kreader->readsize(targetfunc + 14 + delta, pvoid((__int64)codememact + 9), funcl))
//		return 0;
//	if (!m_kreader->readsize(targetfunc + 9 + 5, pvoid((__int64)codememact + 9 + funcl), 0x45))
//		return 0;
//	decryptf decrypt = (decryptf)((__int64)codememact);
//	auto res = __ror8__(decrypt(v2 ^ v1), 49);
//	zeromemory(codememact, 1024);
//	return res;
//}
void updateSafeZone() {
	DWORD_PTR pGameState = m_KReader->readType<DWORD_PTR>(global::pUWorld + GAME_STATE, PROTO_NORMAL_READ);
	
	global::safeZone.white = m_KReader->readType<Vector3>(pGameState + 0x0480); 
	global::safeZone.whiteR = m_KReader->readType<float>(pGameState + 0x048C); 
	global::safeZone.blue = m_KReader->readType<Vector3>(pGameState + 0x0470);
	global::safeZone.blueR = m_KReader->readType<float>(pGameState + 0x047C);
	/*
	global::safeZone.white = m_KReader->readType<Vector3>(pGameState + 0x47C);
	global::safeZone.whiteR = m_KReader->readType<float>(pGameState + 0x488);
	global::safeZone.blue = m_KReader->readType<Vector3>(pGameState + 0x46C);
	global::safeZone.blueR = m_KReader->readType<float>(pGameState + 0x478*/
}
struct tsl tsl;
void UpdateAddresses()
{
	if (!m_KReader)
		return;

	//if (!global::pUWorld ||!global::pGameInstance || !global::pULocalPlayer|| !global::pLocalPlayer|| !global::pViewportClient || 
		//	!global::pActorArray || !global::pULevel || !global::pWorld) {
		
	  
		if (!tsl_init(&tsl)) {
			// what?
		}

	   //global::pUWorld = tsl_decrypt_world(&tsl, m_KReader->getPUBase() + UWORD_BASE);
	    global::pUWorld = m_KReader->readType<DWORD_PTR>(m_KReader->readType<DWORD_PTR>(m_KReader->getPUBase() + UWORD_BASE, PROTO_NORMAL_READ));
		updateSafeZone();
		//global::pUWorld = m_KReader->readType<DWORD_PTR>(m_KReader->readType<DWORD_PTR>(m_KReader->getPUBase() + 0x3EB9580, PROTO_NORMAL_READ), PROTO_NORMAL_READ);
		global::pGameInstance = tsl_decrypt_prop(&tsl, global::pUWorld + GAMEINSTANCE);
		DWORD_PTR temp = m_KReader->readType<DWORD_PTR>(global::pGameInstance + LOCALPLAYERARRAY, PROTO_NORMAL_READ);
		global::pULocalPlayer = tsl_decrypt_prop(&tsl, m_KReader->readType<DWORD_PTR>(global::pGameInstance + LOCALPLAYERARRAY));  // local_player 
		global::pLocalPlayer = global::pULocalPlayer;//m_KReader->readType<DWORD_PTR>(global::pULocalPlayer + LOCALPLAYER, PROTO_NORMAL_READ);
		global::pLocalPlayer_Controller = tsl_decrypt_prop(&tsl, global::pULocalPlayer + 0x30);
		global::pViewportClient = tsl_decrypt_prop(&tsl, global::pLocalPlayer + VIEWPORTCLIENT);
		global::pWorld = m_KReader->readType<DWORD_PTR>(global::pViewportClient + PWORLD, PROTO_NORMAL_READ);
		global::BaseLocation2D.x = m_KReader->readType<int32_t>(global::pWorld + 0x958, PROTO_NORMAL_READ);
		global::BaseLocation2D.y = m_KReader->readType<int32_t>(global::pWorld + 0x95C, PROTO_NORMAL_READ);
		//global::BaseLocation2D.z = m_KReader->readType<int32_t>(global::pWorld + 0x0940, PROTO_NORMAL_READ);


		global::pULevel = tsl_decrypt_prop(&tsl, global::pUWorld + ULEVEL);

		DWORD_PTR DeCryptAActors = 	tsl_decrypt_actor(&tsl, global::pULevel + 0xc0);
		global::ActorCount = m_KReader->readType<int>(DeCryptAActors + 0x8);
		/*if (global::ActorCount < 0x100000)		return;

		if (global::ActorCount > 3000) 	global::ActorCount = 3000;
		if (global::ActorCount < 1)return;*/
		global::pActorArray = m_KReader->readType<DWORD_PTR>(DeCryptAActors); // ok
		global::pLocalPlayer_entity = tsl_decrypt_prop(&tsl, global::pLocalPlayer_Controller + 0x480);
	    global::cameracache = GetCameraCache();
		
}

Vector3 GetLocalPlayerPos()
{
	Vector3 pos;
	if (!m_KReader)
		return pos;
	pos = m_KReader->readType<Vector3>(global::pLocalPlayer + LOCALPLAYERPOS, PROTO_NORMAL_READ);

	return pos;
}

Vector3 GetActorPos(DWORD_PTR entity)
{
	Vector3 pos;
	if (!m_KReader)
		return pos;	
	DWORD_PTR rootcomp = tsl_decrypt_prop(&tsl, entity + ROOTCOMPONENT);
	DWORD_PTR playerState = m_KReader->readType<DWORD_PTR>(entity + PLAYERSTATE, PROTO_NORMAL_READ);
	pos = m_KReader->readType<Vector3>(rootcomp + (playerState == 0 ? ACTORPOSINVEHICLE : ACTORPOS), PROTO_NORMAL_READ);

	return pos;
}

float GetLocalPlayerHealth()
{
	if (!m_KReader)
		return 0.f;
	auto playercontroller = m_KReader->readType<DWORD_PTR>(global::pLocalPlayer + 0x30, PROTO_NORMAL_READ);
	auto pawn = m_KReader->readType<DWORD_PTR>(playercontroller + 0x418, PROTO_NORMAL_READ);
	return m_KReader->readType<float>(pawn + 0x107C, PROTO_NORMAL_READ);
}

float GetActorHealth(DWORD_PTR pAActor)
{
	if (!m_KReader)
		return 0.f;
	return m_KReader->readType<float>(pAActor + ACTORHEALTH, PROTO_NORMAL_READ);
}

DWORD_PTR GetEntityList()
{
	if (!m_KReader)
		return 0;
	return m_KReader->readType<DWORD_PTR>(m_KReader->readType<DWORD_PTR>(global::pUWorld + 0x30, PROTO_NORMAL_READ) + 0xA0, PROTO_NORMAL_READ);
}

FCameraCacheEntry GetCameraCache()
{
	FCameraCacheEntry cce;
	if (!m_KReader)
		return cce;
	auto playercontroller = global::pLocalPlayer_Controller;
	DWORD_PTR temp = m_KReader->readType<DWORD_PTR>(playercontroller + PLAYERCAMERAMANAGER, PROTO_NORMAL_READ);
	cce = m_KReader->readType<FCameraCacheEntry>(temp + CAMERACACHE, PROTO_NORMAL_READ);
	return cce;
}
string GetPlayerName(DWORD_PTR playerState)
{
	FString fstruct = m_KReader->readType<FString>(playerState + PLAYERNAME, PROTO_NORMAL_READ);
	/*
	DWORD_PTR playerNameAddress = m_KReader->readType<DWORD_PTR>(playerState + PLAYERNAME, PROTO_NORMAL_READ);
	int32_t playerNameLength = m_KReader->readType<int32_t>(playerState + PLAYERNAME + 0x8, PROTO_NORMAL_READ);
	*/
	DWORD_PTR playerNameAddress = fstruct.stringBuffer;
	int playerNameLength = fstruct.stringLength;
	std::string s("");
	if (playerNameAddress > 0 && playerNameLength > 0) {
		wchar_t * buffer = (wchar_t *)m_KReader->readSize(playerNameAddress, playerNameLength * 2, PROTO_NORMAL_READ);	
		if (buffer != NULL) {
			s = Wchar_tToString(buffer).substr(0, playerNameLength - 1);
			delete buffer;
		}
	}
	return s;
}
inline string Wchar_tToString(wchar_t *wchar)
{
	wchar_t * wText = wchar;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte的运用  
	char *psText; // psText为char*的临时数组，作为赋值给std::string的中间变量  
	psText = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);// WideCharToMultiByte的再次运用  
	std::string szDst(psText);// std::string赋值  
	delete[]psText;// psText的清除  
	return szDst;
}

std::string GetNameFromId(int w_id)
{
	/*ActorIds[0] = 92775;
	ActorIds[1] = 92778;
	ActorIds[2] = 112528;
	ActorIds[3] = 112530;
	itemtype[0] = 15268;
	itemtype[1] = 14566;*/
	
	
	//DWORD_PTR encryptGNames = m_KReader->readType<DWORD_PTR>(m_KReader->getPUBase() + GNAME_BASE, PROTO_NORMAL_READ); // 3.5  GNames
	
	//DWORD_PTR GNames = m_KReader->readType<DWORD_PTR>(encryptGNames + 0xf8, PROTO_NORMAL_READ) + 0x530; // 3.5  GNames
	/*
	encryptedGNamesPtr = BASE + 0x3F5E910;
	TslEncryptedPointer encryptedGNames;
	MEM->Read(encryptedGNamesPtr, &encryptedGNames, sizeof(encryptedGNames));
	MEM->Read(encryptedGNames.Decrypt(decryptFunctions), &encryptedGNames, sizeof(encryptedGNames));
	uint64_t GName = encryptedGNames.Decrypt(decryptFunctions);
	*/
	//auto GNamesPtr1 = DecryptData(m_KReader->getPUBase() + 0x3F5ED10, m_KReader->getPUBase() + 0x3D90770);
	//auto GNamesPtr2 = DecryptData(GNamesPtr1, m_KReader->getPUBase() + 0x3D90770);
	//DWORD_PTR GNames = DecryptData(GNamesPtr2, m_KReader->getPUBase() + 0x3D90770);
	//DWORD_PTR GNames = m_KReader->readType<DWORD_PTR>(m_KReader->readType<DWORD_PTR>(m_KReader->getPUBase() + 0x03E6FBF0, PROTO_NORMAL_READ) + 0xF8, PROTO_NORMAL_READ)+ 0x500;

	//auto GNamesPtr1 = DecryptGName(m_KReader->getPUBase() + 0x406D6B0, m_KReader->getPUBase() + 0x3C32120);
	//auto GNamesPtr2 = DecryptGName2(GNamesPtr1, m_KReader->getPUBase() + 0x3C32120);
	//DWORD_PTR GNames = GNamesPtr2;
	DWORD_PTR GNames = m_KReader->readType<DWORD_PTR>(m_KReader->getPUBase() + GNAME_BASE, PROTO_NORMAL_READ);
	DWORD_PTR singleNameChunk = m_KReader->readType<DWORD_PTR>(GNames + (w_id / 0x4000) * 8, PROTO_NORMAL_READ);
	DWORD_PTR singleNamePtr = m_KReader->readType<DWORD_PTR>(singleNameChunk + 8 * (w_id % 0x4000), PROTO_NORMAL_READ);

	char* name;
	if ((name = (char*)m_KReader->readSize(singleNamePtr + 0x18, 64, PROTO_NORMAL_READ)) != NULL)
	{
		std::string s = std::string(name);
		delete name;
		return s;
	}
	else
	{
		return std::string("FAIL");
	}
	//return  std::string("FAIL");
}

void CacheNames()
{
	/*
	string name = "";
	std::ofstream outfile;
	outfile.open("name.log");

	for (int i = 0; i < 200000; i++) {
		outfile <<i <<","<<GetNameFromId(i)<< std::endl;
	}
	outfile.close();
	*/
	double  allstart = GetTickCount();
	string name = "";
	for (int i = 100, c1 = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0, c6 = 0, airDropIndex = 0,vanIndex = 0,boatIndex = 0,pickupIndex=0; i < 135000; i++)
	{
		/*if (c1 == 4 && c2 == 3 && c3 == 4 && c4 == 5 && c5 == 3 && boat != 0 && c6 == 2)
		{
			cout << "find string success" << endl;
			return;
		}*/
		//name = MD5(GetNameFromId(i)).toStr();
		name = GetNameFromId(i);
		if (name == "PlayerFemale_A" || name == "PlayerFemale_A_C" || name == "PlayerMale_A" || name == "PlayerMale_A_C")
		{
			cout << name << " " << i << endl;
			ActorIds[c1++] = i;
		}
		else if (name == "Uaz_A_01_C" || name == "Uaz_B_01_C" || name == "Uaz_C_01_C")
		{
			uaz[c2++] = i;
			cout << name << " " << i << endl;
		}
		else if (name == "Dacia_A_01_C" || name == "Dacia_A_02_C" || name == "Dacia_A_03_C" || name == "Dacia_A_04_C" || name == "Dacia_A_01_v2_C")
		{
			dacia[c3++] = i;
			cout << name << " " << i << endl;
		}
		else if (name == "BP_PickupTruck_A_01_C" || name == "BP_PickupTruck_A_02_C" || name == "BP_PickupTruck_A_03_C" || name == "BP_PickupTruck_A_04_C" || name == "BP_PickupTruck_A_05_C" || 
				name == "BP_PickupTruck_B_01_C" || name == "BP_PickupTruck_B_02_C" || name == "BP_PickupTruck_B_03_C" || name == "BP_PickupTruck_B_04_C" || name == "BP_PickupTruck_B_05_C")
		{
			pickup[pickupIndex++] = i;
			cout << name << " " << i << endl;
		}
		else if (name == "ABP_Motorbike_03_C" || name == "ABP_Motorbike_04_C" || name == "BP_Motorbike_03_C"  || name == "BP_Motorbike_04_C"
				|| name == "ABP_Motorbike_03_Sidecart_C" || name == "BP_Motorbike_04_Desert_C"  || name == "BP_Motorbike_04_SideCar_Desert_C")
		{
			cout << name << " " << i << endl;
			motorbike[c4++] = i;
		}
		else if (name == "Buggy_A_01_C" || name == "Buggy_A_02_C" || name == "Buggy_A_03_C" || name == "Buggy_A_04_C" || name == "Buggy_A_05_C" || name == "Buggy_A_06_C")
		{
			cout << name << " " << i << endl;
			buggy[c5++] = i;
		}
		else if (name == "Boat_PG117_C" || name == "AquaRail_A_01_C")
		{
			cout << name << " " << i << endl;
			boat[boatIndex++] = i;
		}
		else if (name == "BP_Van_A_01_C" || name == "BP_Van_A_02_C" || name ==  "BP_Van_A_03_C") {
			cout << name << " " << i << endl;
			van[vanIndex++] = i;
		}
		else if (name == "DroppedItemInteractionComponent" || name == "DroppedItemGroup")
		{
			cout << name << " " << i << endl;
			itemtype[c6++] = i;
		}
		/*else if (name == "dcfdfd0275790bcf24776d1a8b49323e" || name == "7f7aadffe48af6ea5e184f8d904c079f" || name == "cd29bbb227924789495d649b0afde59b")
		{
			cout << wantItemIndex << ", wantItemIndex" << i << endl;
			wantItem[wantItemIndex++] = i;
		}*/
		else if (name == "Carapackage_RedBox_C" || name == "Carapackage_FlareGun_C" || name == "Carapackage_RedBox")
		{
			cout << name << " " << i << endl;
			airDrop[airDropIndex++] = i;
		}
	}
	double  allend = GetTickCount();
	double time = allend - allstart;
	cout << "cache cost time:" << time << endl;
	
	
}

int isUaz(int id)
{
	for (int i = 0; i < 3; i++)
		if (id == uaz[i])
			return 1;
	return 0;
}

int isDacia(int id)
{
	for (int i = 0; i < 5; i++)
		if (id == dacia[i])
			return 1;
	return 0;
}
int isPickup(int id)
{
	for (int i = 0; i < 10; i++)
		if (id == pickup[i])
			return 1;
	return 0;
}

int isBuggy(int id)
{
	for (int i = 0; i < 6; i++)
		if (id == buggy[i])
			return 1;
	return 0;
}

int isBike(int id)
{
	for (int i = 0; i < 7; i++)
		if (id == motorbike[i])
			return 1;
	return 0;
}
int isVan(int id)
{
	for (int i = 0; i < 3; i++)
		if (id == van[i])
			return 1;
	return 0;
}
int isBoat(int id)
{
	for (int i = 0; i < 2; i++)
		if (id == boat[i])
			return 1;
	return 0;
}

int isAirDrop(int id)
{
	for (int i = 0; i < 3; i++)
		if (id == airDrop[i])
			return 1;
	return 0;
}
void DrawVehicle(DWORD_PTR entity, Vector3 local, const char* txt, json &data)
{
	Vector3 pos = GetActorPos(entity);
	if (pos.x > 0 && pos.y > 0) {
		data["v"].emplace_back(json::object({ { "v", txt },{ "x", pos.x + global::BaseLocation2D.x },{ "y", pos.y + global::BaseLocation2D.y } }));
	}

	//Vector3 spos = World2Screen(pos, global::cameracache);
	//DrawString((int)spos.x, (int)spos.y, D3DCOLOR_ARGB(255, 160, 32, 240), pFont, txt, local.calcDistance(pos) / 100.f);
}

D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}


FTransform GetBoneIndex(DWORD_PTR mesh, int index)
{
	DWORD_PTR bonearray = m_KReader->readType<DWORD_PTR>(mesh + BONEARRAY, PROTO_NORMAL_READ);

	return  m_KReader->readType<FTransform>(bonearray + (index * 0x30), PROTO_NORMAL_READ);
}

Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id)
{
	FTransform bone = GetBoneIndex(mesh, id);

	FTransform ComponentToWorld = m_KReader->readType<FTransform>(mesh + COMPONENTTOWORLD, PROTO_NORMAL_READ);
	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());
	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}

void DrawSkeleton(DWORD_PTR mesh)
{
	Vector3 neckpos = GetBoneWithRotation(mesh, Bones::neck_01);
	Vector3 pelvispos = GetBoneWithRotation(mesh, Bones::pelvis);
	Vector3 previous(0, 0, 0);
	Vector3 current, p1, c1;
	for (auto a : skeleton)
	{
		previous = Vector3(0, 0, 0);
		for (int bone : a)
		{
			current = bone == Bones::neck_01 ? neckpos : (bone == Bones::pelvis ? pelvispos : GetBoneWithRotation(mesh, bone));
			if (previous.x == 0.f)
			{
				previous = current;
				continue;
			}
			p1 = World2Screen(previous, global::cameracache);
			c1 = World2Screen(current, global::cameracache);
			DrawLine(p1.x, p1.y, c1.x, c1.y, D3DCOLOR_ARGB(255, 153, 249, 9));
			previous = current;
		}
	}
}
/*
wchar_t MakecodeChar(wchar_t c, int key) {
	return c = c^key;
}

wchar_t CutcodeChar(wchar_t c, int key) {
	return c^key;
}



void Makecode(wchar_t  *pstr, int *pkey) {
	int len = wcslen(pstr); 
	for (int i = 0; i<len; i++)
		*(pstr + i) = MakecodeChar(*(pstr + i), pkey[i % 5]);
}

 
inline void Cutecode(wchar_t *pstr, int *pkey) {
	int len = wcslen(pstr);
	for (int i = 0; i<len; i++)
		*(pstr + i) = CutcodeChar(*(pstr + i), pkey[i % 5]);

}

*/



int tsl_init(struct tsl *tsl) {
	tsl->func = (decrypt_func)VirtualAlloc(NULL, 0x800, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (tsl->func == NULL) {
		return 0;
	}
	return 1;
}

void tsl_finit(struct tsl *tsl) {
	if (tsl->func != NULL) {
		VirtualFree(tsl->func, 0, MEM_RELEASE);
		tsl->func = NULL;
	}
}

// ida

#define BYTEn(x, n) (*((BYTE *)&(x) + n))
#define WORDn(x, n) (*((WORD *)&(x) + n))
#define DWORDn(x, n) (*((DWORD *)&(x) + n))

#define IDA_LOBYTE(x) BYTEn(x, 0)
#define IDA_LOWORD(x) WORDn(x, 0)
#define IDA_LODWORD(x) WORDn(x, 0)
#define IDA_HIBYTE(x) BYTEn(x, 1)
#define IDA_HIWORD(x) WORDn(x, 1)
#define IDA_HIDWORD(x) DWORDn(x, 1)

#define BYTE1(x) BYTEn(x, 1)
#define BYTE2(x) BYTEn(x, 2)
#define WORD1(x) WORDn(x, 1)
#define DWORD1(x) DWORDn(x, 1)


// rotate

static uint8_t rol1(uint8_t x, unsigned int count) {
	count %= 8;
	return (x << count) | (x >> (8 - count));
}

static uint16_t rol2(uint16_t x, unsigned int count) {
	count %= 16;
	return (x << count) | (x >> (16 - count));
}

static uint32_t rol4(uint32_t x, unsigned int count) {
	count %= 32;
	return (x << count) | (x >> (32 - count));
}

static uint64_t rol8(uint64_t x, unsigned int count) {
	count %= 64;
	return (x << count) | (x >> (64 - count));
}

static uint8_t ror1(uint8_t x, unsigned int count) {
	count %= 8;
	return (x << (8 - count)) | (x >> count);
}

static uint16_t ror2(uint16_t x, unsigned int count) {
	count %= 16;
	return (x << (16 - count)) | (x >> count);
}

static uint32_t ror4(uint32_t x, unsigned int count) {
	count %= 32;
	return (x << (32 - count)) | (x >> count);
}

static uint64_t ror8(uint64_t x, unsigned int count) {
	count %= 64;
	return (x << (64 - count)) | (x >> count);
}

// macro for uc!

// bool read_size(uint64_t src, void *dest, size_t size) {...}
#define READ(src, dest, size) m_KReader->readSize(src, dest, size)
// template<typename T> read(uint64_t addr)
#define READ32(addr)  m_KReader->readType<uint32_t>(addr)
#define READ64(addr)  m_KReader->readType<uint64_t>(addr)

#define GET_ADDR(addr) (m_KReader->getPUBase() + addr)

struct rel_addr {
	uint32_t offset;
	uint32_t addr;
};


static int find_call(const uint8_t *buf, uint32_t size, struct rel_addr *ret) {
	uint32_t offset = 0;
	while (offset < (size - 5)) {
		if (buf[offset] == 0xe8) {
			uint32_t addr = *(uint32_t *)(buf + (offset + 1));
			if (addr < 0x8000) {
				ret->offset = offset + 5;
				ret->addr = addr;
				return 1;
			}
		}
		offset++;
	}
	return 0;
}

static uint32_t get_func_len(const uint8_t *buf, uint32_t size, uint8_t start, uint32_t end) {
	if (*buf == start) {
		uint32_t offset = 0;
		while (offset < (size - sizeof(end))) {
			if (*(uint32_t *)(buf + offset) == end) {
				return offset;
			}
			offset++;
		}
	}
	return 0;
}

static uint64_t decrypt(struct tsl *tsl, uint64_t func, uint64_t arg) {
	uint8_t buf_0x100[0x100];
	if (!READ(func, buf_0x100, 0x100)) {
		return 0;
	}
	struct rel_addr rel_addr;
	if (!find_call(buf_0x100, 0x100, &rel_addr)) {
		return 0;
	}
	uint64_t abs_addr = func + (rel_addr.offset + rel_addr.addr);
	uint8_t buf_0x20[0x20];
	if (!READ(abs_addr, buf_0x20, 0x20)) {
		return 0;
	}
	uint32_t len = get_func_len(buf_0x20, 0x20, 0x48, 0xccccccc3);
	if (!len || len > 0xf) {
		return 0;
	}
	uint32_t temp = rel_addr.offset - 5;
	memcpy(tsl->func, buf_0x100, temp);
	memcpy((char *)tsl->func + temp, buf_0x20, len);
	memcpy((char *)tsl->func + (temp + len), buf_0x100 + rel_addr.offset, 0x100 - rel_addr.offset);
	uint64_t ret = tsl->func(arg);
	memset(tsl->func, 0, 0x200);
	return ret;
}

// exports

#define TABLE 0x3edf120

struct uint128_t {
	uint64_t low;
	uint64_t high;
};

uint64_t tsl_decrypt_world(struct tsl *tsl, uint64_t world) {
	return 0;
}

uint64_t tsl_decrypt_gnames(struct tsl *tsl, uint64_t gnames) {
	return 0;
}

uint64_t tsl_decrypt_actor(struct tsl *tsl, uint64_t actor) {
	struct uint128_t xmm;
	if (!READ(actor, &xmm, 16)) {
		return 0;
	}
	uint32_t key = (uint32_t)xmm.low;
	uint64_t func = READ64(GET_ADDR(TABLE) + 0x8 * (((uint8_t)(IDA_LOWORD(key) ^ (IDA_HIWORD(key) - 32 + 96)) ^ ((uint8_t)(((uint16_t)(IDA_LOWORD(key) ^ (IDA_HIWORD(key) - 32 + 8288)) >> 8) - 96) + 64)) % 128));
	return ror8(decrypt(tsl, func, xmm.high - key), -32);
}

uint64_t tsl_decrypt_prop(struct tsl *tsl, uint64_t prop) {
	struct uint128_t xmm;
	if (!READ(prop, &xmm, 16)) {
		return 0;
	}
	uint32_t key = (uint32_t)xmm.low;
	uint16_t x = (uint16_t)(IDA_LOWORD(key) + 39) ^ (rol2(IDA_HIWORD(key) + 107, 8) + 11671);
	uint64_t func = READ64(GET_ADDR(TABLE) + 0x8 * (((uint8_t)(((IDA_LOWORD(key) + 39) ^ (rol2(IDA_HIWORD(key) + 107, 8) - 105)) - 33) ^ (BYTE1(x) + 18)) % 128));
	return ror8(decrypt(tsl, func, ror8(xmm.high, 8 * (IDA_LOWORD(key) & 7u)) - key), 59);
}