#include "Utils.h"
//definitions

KReader  *m_KReader = nullptr;
namespace global
{
	int key[] = { 1,2,3,4,5 };//¼ÓÃÜ×Ö·û  
	
	DWORD_PTR pUWorld = 0;
	DWORD_PTR pULevel = 0;
	DWORD_PTR pGameInstance = 0;
	DWORD_PTR pULocalPlayer = 0;
	DWORD_PTR pLocalPlayer = 0;
	DWORD_PTR pLocalPlayer_Controller = 0;
	DWORD_PTR pLocalPlayer_entity = 0;
	DWORD_PTR pActorArray = 0;
	int ActorCount = 0;
	DWORD_PTR ActorArray = 0;
	DWORD_PTR pViewportClient = 0;
	DWORD_PTR pWorld = 0;


	SafeZone safeZone;
	
	Vector3 BaseLocation2D(0.f,0.f,0.f);

	bool bPlayer = true;
	bool bVehicle = true;
	bool bLoot = false;
	bool bPostHttp = true;
	FCameraCacheEntry cameracache = { NULL };
	bool bStartLoop = false;
	bool bDrawD3D = false;
	bool bAimOn = true;
	float aimRange = 150.f;  //Ãé×¼·¶Î§
	int64_t temptarget = 0;

	int32_t fps = 0;

}
/*
extern "C"
{
	uint64 decptr(dummy *dummy, void *x);
	void decinit(dummy *dummy);
}
*/
extern  struct tsl tsl;
void redraw()
{
	if (GetAsyncKeyState(VK_NUMPAD5) & 1)
		global::bAimOn = global::bAimOn ? false : true; // bAim *wink* *wink*
	if (GetAsyncKeyState(VK_NUMPAD4) & 1)
		global::bPostHttp = global::bPostHttp ? false : true;
	if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		global::bVehicle = global::bVehicle ? false : true;
	if(GetAsyncKeyState(VK_NUMPAD2) & 1)
		global::bLoot = global::bLoot ? false : true;
	if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		global::bPlayer = global::bPlayer ? false : true;
	UpdateAddresses();
	wchar_t entityname[64] = { NULL };
	//DWORD_PTR enlist = GetEntityList();
	//int entitycount = m_KReader->readType<int>(global::pActorArray + 0x8, PROTO_NORMAL_READ);
	float health = 0.f;
	float distance = 0.f;
	DWORD color = 0;
	//DWORD_PTR localPlayerController = m_KReader->readType<DWORD_PTR>(global::pLocalPlayer + PLAYERCONTROLLER, PROTO_NORMAL_READ);

	//DWORD_PTR localEntity = m_KReader->readType<DWORD_PTR>(global::pLocalPlayer_Controller + APawn, PROTO_NORMAL_READ);

	DWORD_PTR localTeam = m_KReader->readType<int32_t>(tsl_decrypt_prop(&tsl, global::pLocalPlayer_entity + PLAYERSTATE) + TEAMNUMBER, PROTO_NORMAL_READ);
	Vector3 LocalEye = World2Screen(global::cameracache.POV.Location, global::cameracache);
	Vector3 local = GetLocalPlayerPos();
	json data;
	/*if (entitycount > 5000) {
		cout << "entity count:" << entitycount << endl;
	}*/
	
	for (int i = 0; i < global::ActorCount; i++)
	{
		ZeroMemory(entityname, sizeof(entityname));
	
		DWORD_PTR entity = m_KReader->readType<DWORD_PTR>(global::pActorArray + i * 0x8, PROTO_NORMAL_READ);
		if (!entity)
			continue;
	
		
		int id = m_KReader->readType<int>(entity + ACTORID, PROTO_NORMAL_READ);

		if (global::bPlayer && (id ==  ActorIds[0] || id == ActorIds[1] || id == ActorIds[2] || id == ActorIds[3]))
		{
	
			health = GetActorHealth(entity); 
				//health = 100.f;
			if (true)
			{
				Vector3 pos = GetActorPos(entity);

				DWORD_PTR rootCmpPtr = tsl_decrypt_prop(&tsl, entity + ROOTCOMPONENT);

		
				
				Vector3 relativeRotation = m_KReader->readType<Vector3>(rootCmpPtr + PLYAERROTATOR, PROTO_NORMAL_READ); 
				DWORD_PTR playerState = tsl_decrypt_prop(&tsl,entity + PLAYERSTATE);
				//DWORD_PTR playerState = m_KReader->readType<DWORD_PTR>(entity + PLAYERSTATE, PROTO_NORMAL_READ);
				/*if (!playerState) {
					//player in vehicle
					DWORD_PTR VehicleRiderComponent = m_KReader->readType<DWORD_PTR>(entity + VEHICLERIDERCOMPONENT, PROTO_NORMAL_READ);
					DWORD_PTR VehiclePawn = m_KReader->readType<DWORD_PTR>(VehicleRiderComponent + LASTVEHICLEPAWN, PROTO_NORMAL_READ);
					DWORD_PTR playerState = m_KReader->readType<DWORD_PTR>(VehiclePawn + PLAYERSTATE, PROTO_NORMAL_READ);
				}*/
				int32_t actorTeam = m_KReader->readType<int32_t>(playerState + TEAMNUMBER, PROTO_NORMAL_READ); //FIXME 
				//int32_t actorTeam = m_KReader->readType<int32_t>(entity + TEAMNUMBER, PROTO_NORMAL_READ); //FIXED
				bool isOwner = false;
			
				if (entity == global::pLocalPlayer_entity)
					isOwner = true;
				if (global::bPostHttp && (pos.x != 0 && pos.y !=0 && pos.z!=0 ) ) 
				{		
					//double  allstart = GetTickCount();								
					data["p"].emplace_back(json::object({ { "t", actorTeam },{ "hp", health },{ "x", pos.x + global::BaseLocation2D.x },{ "y", pos.y + global::BaseLocation2D.y },{ "r", relativeRotation.y  },{ "z", pos.z + global::BaseLocation2D.z },{ "name",GetPlayerName(playerState) },{ "local",isOwner } }));
					//double  allend = GetTickCount();
					//cout << "make json cost:" << allend - allstart << endl;
				}
					
				if (isOwner || health <= 0.f) {
					continue;
				}
			   if (localTeam == actorTeam) {
					continue;
				}
				Vector3 spos = World2Screen(pos, global::cameracache);
				if (spos.x < 0 || spos.x >  s_width || spos.y < 0 || spos.y > s_height) continue;
				/*distance = local.calcDistance(pos) / 100.f;
				if (distance > 500.f)
					continue;
				if (distance <= 150.f)
					color = D3DCOLOR_ARGB(100, 255, 0, 0); //color red, if less than 150m
				else if (distance > 150.f && distance <= 300.f)
					color = D3DCOLOR_ARGB(255, 255, 255, 0); //color yellow, if less than 300m and greater than 150m
				else
					color = D3DCOLOR_ARGB(255, 0, 255, 0); //color green, if greater than 300m*/
				//DrawString((int)spos.x, (int)spos.y, color, pFont, "[HP: %0.2f]", health);
				auto mesh = m_KReader->readType<DWORD_PTR>(entity + MESH, PROTO_NORMAL_READ);
				if (!mesh)
					continue;
				DrawSkeleton(mesh); //draw skeleton, is distance is less than equal to 110m
			
			}
		}
		
		//vehicle esp
		
		if (global::bVehicle)
		{
			if (isUaz(id)) 								
				DrawVehicle(entity, local, "uaz", data);
			else if (isDacia(id))
				DrawVehicle(entity, local, "dac", data);
			else if (isPickup(id))
				DrawVehicle(entity, local, "pic", data);
			else if (isBike(id))  
				DrawVehicle(entity, local, "bp", data);
			else if (isVan(id))
				DrawVehicle(entity, local, "van", data);
			else if (isBuggy(id))
				DrawVehicle(entity, local, "bug", data);
			else if (isBoat(id))
				DrawVehicle(entity, local, "boat", data);
		}
		
		if (/*false &&*/ global::bLoot && (id == itemtype[0] || id == itemtype[1]))
		{
			

			DWORD_PTR DroppedItemGroupArray = m_KReader->readType<DWORD_PTR>(entity + DROPPEDITEMGROUPARRAY, PROTO_NORMAL_READ);
			int count = m_KReader->readType<int>(entity + DROPCOUNT, PROTO_NORMAL_READ);

			if (!count)
				continue;
			for (int j = 0; j < count; j++)
			{
				DWORD_PTR pADroppedItemGroup = m_KReader->readType<DWORD_PTR>(DroppedItemGroupArray + j * 0x8, PROTO_NORMAL_READ);
				//DWORD_PTR USceneComponent = m_KReader->readType<DWORD_PTR>(entity + 0x0188, PROTO_NORMAL_READ);
				//DWORD_PTR pUItem = m_KReader->readType<DWORD_PTR>(USceneComponent + 0x0538, PROTO_NORMAL_READ);
				DWORD_PTR pUItem = m_KReader->readType<DWORD_PTR>(pADroppedItemGroup + 0x0538, PROTO_NORMAL_READ);
				if (!pUItem) continue;
				DWORD_PTR pUItemFString = m_KReader->readType<DWORD_PTR>(pUItem + 0x40, PROTO_NORMAL_READ);
				DWORD_PTR pItemName = m_KReader->readType<DWORD_PTR>(pUItemFString + 0x28, PROTO_NORMAL_READ);
				ZeroMemory(entityname, sizeof(entityname));
				if (m_KReader->readSize(pItemName, entityname, sizeof(entityname), PROTO_NORMAL_READ))
				{
					wcout << entityname << endl;
					if (wcscmp(entityname ,L"Helmet (Lv.3)") == 0) {
						//Vector3 pos = GetActorPos(entity);
						Vector3 pos = m_KReader->readType<Vector3>(pADroppedItemGroup + 0x2d0, PROTO_NORMAL_READ);
						data["i"].emplace_back(json::object({ { "n", "h3" },{ "x", pos.x + global::BaseLocation2D.x },{ "y", pos.y + global::BaseLocation2D.y },{ "z", pos.z + global::BaseLocation2D.z } }));
						//DrawString(screenloc.x, screenloc.y, D3DCOLOR_ARGB(255, 255, 144, 0), pFont, "%ws", "H3");
					}
					else if (wcscmp(entityname, L"Military Vest (Level 3)") == 0) {
						Vector3 pos = GetActorPos(entity);
						data["i"].emplace_back(json::object({ { "n", "v3" },{ "x", pos.x + global::BaseLocation2D.x },{ "y", pos.y + global::BaseLocation2D.y },{ "z", pos.z + global::BaseLocation2D.z } }));
						//DrawString(screenloc.x, screenloc.y, D3DCOLOR_ARGB(255, 255, 144, 0), pFont, "%ws", "4x");
					}	
					else if (wcscmp(entityname, L"SKS") == 0) {
						Vector3 pos = GetActorPos(entity);
						data["i"].emplace_back(json::object({ { "n", "sks" },{ "x", pos.x + global::BaseLocation2D.x },{ "y", pos.y + global::BaseLocation2D.y },{ "z", pos.z + global::BaseLocation2D.z } }));
						//DrawString(screenloc.x, screenloc.y, D3DCOLOR_ARGB(255, 255, 144, 0), pFont, "%ws", "4x");
					}
					else if (wcscmp(entityname, L"First Aid Kit") == 0) {
						Vector3 pos = GetActorPos(entity);
						data["i"].emplace_back(json::object({ { "n", "ms" },{ "x", pos.x + global::BaseLocation2D.x },{ "y", pos.y + global::BaseLocation2D.y },{ "z", pos.z + global::BaseLocation2D.z } }));
						//DrawString(screenloc.x, screenloc.y, D3DCOLOR_ARGB(255, 255, 144, 0), pFont, "%ws", "4x");
					}
					else if (wcscmp(entityname, L"Kar98k") == 0) {
						Vector3 pos = GetActorPos(entity);
						data["i"].emplace_back(json::object({ { "n", "98k" },{ "x", pos.x + global::BaseLocation2D.x },{ "y", pos.y + global::BaseLocation2D.y },{ "z", pos.z + global::BaseLocation2D.z } }));
						//DrawString(screenloc.x, screenloc.y, D3DCOLOR_ARGB(255, 255, 144, 0), pFont, "%ws", "4x");
					}
					else if (wcscmp(entityname, L"M416") == 0) {
						Vector3 pos = GetActorPos(entity);
						data["i"].emplace_back(json::object({ { "n", "m4" },{ "x", pos.x + global::BaseLocation2D.x },{ "y", pos.y + global::BaseLocation2D.y },{ "z", pos.z + global::BaseLocation2D.z } }));
						//DrawString(screenloc.x, screenloc.y, D3DCOLOR_ARGB(255, 255, 144, 0), pFont, "%ws", "4x");
					}
					else if (wcscmp(entityname, L"4x Scope") == 0) {
						Vector3 pos = GetActorPos(entity);
						data["i"].emplace_back(json::object({ { "n", "4x" },{ "x", pos.x + global::BaseLocation2D.x },{ "y", pos.y + global::BaseLocation2D.y },{ "z", pos.z + global::BaseLocation2D.z } }));
						//DrawString(screenloc.x, screenloc.y, D3DCOLOR_ARGB(255, 255, 144, 0), pFont, "%ws", "4x");
					}
					else if (wcscmp(entityname, L"8x Scope") == 0) {
						Vector3 pos = GetActorPos(entity);
						data["i"].emplace_back(json::object({ { "n", "8x" },{ "x", pos.x + global::BaseLocation2D.x },{ "y", pos.y + global::BaseLocation2D.y },{ "z", pos.z + global::BaseLocation2D.z } }));
						//DrawString(screenloc.x, screenloc.y, D3DCOLOR_ARGB(255, 255, 144, 0), pFont, "%ws", "8x");
					}
					//DrawString(screenloc.x, screenloc.y, D3DCOLOR_ARGB(255, 255, 144, 0), pFont, "%ws", entityname);
				}
				
			}
		}
		//ugly airdrop
		/*if (isAirDrop(id)) {

			Vector3 pos = GetActorPos(entity);
			Vector3 spos = World2Screen(pos, global::cameracache);
			auto items = m_KReader->readType<DWORD_PTR>(entity + 0x04F0);
			int itemsCount = m_KReader->readType<int>(entity + 0x04F0 + 0x8);
			for (int i = 0; i < itemsCount; ++i)
			{
				auto item = m_KReader->readType<DWORD_PTR>(items + i * 0x8);
				auto itemFString = m_KReader->readType<DWORD_PTR>(item + 0x40);
				auto itemNamePtr = m_KReader->readType<DWORD_PTR>(itemFString + 0x28);
				wchar_t itemNameString[64] = { 0 };
				m_KReader->readSize(itemNamePtr, itemNameString, sizeof(itemNameString), PROTO_NORMAL_READ);
				DrawString((int)spos.x, (int)spos.y + (i + 1) * 10, color, pFont, "%ws", itemNameString);
			}
			data["v"].emplace_back(json::object({ { "v", "Drop" },{ "x", pos.x + global::BaseLocation2D.x },{ "y", pos.y + global::BaseLocation2D.y }}));
		}*/
	
		
	}

	if (global::safeZone.blue.x > 0) {
		
	/*	data["zone"].emplace_back(json::object({
			{ "blue", json::object({ { "x",global::safeZone.blue.x },{ "y",global::safeZone.blue.y },{ "r",global::safeZone.blueR } }).dump() } ,
			{ "white", json::object({ { "x",global::safeZone.white.x },{ "y",global::safeZone.white.y },{ "r",global::safeZone.whiteR } }).dump() }
		}));*/
		data["zone"] = json::object({
			{ "blue", json::object({ { "x",global::safeZone.blue.x },{ "y",global::safeZone.blue.y },{ "r",global::safeZone.blueR } }) } ,
			{ "white", json::object({ { "x",global::safeZone.white.x },{ "y",global::safeZone.white.y },{ "r",global::safeZone.whiteR } })}
		});
	}
	if (global::bPostHttp && m_Curl->getReadyState() && !(data["i"].empty() && data["v"].empty() && data["p"].empty()))
	{
		m_Curl->putQueue(data.dump());
	}
	tsl_finit(&tsl);
}

//prevent memory leaks
void Shutdown()
{
	if (m_KReader)
		delete m_KReader;
	if (m_Curl)
		delete m_Curl;
	m_KReader = nullptr;
}

//render function
void render()
{
	// clear the window alpha
	
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();    // begins the 3D scene
	
	//calculate and and draw esp stuff
	double  allstart = GetTickCount();
	redraw();
	double  allend = GetTickCount();
	double time = allend - allstart;
	if (time > 50) {
		//cout << "time:" << time << endl;
	}
	
	d3ddev->EndScene();    // ends the 3D scene

	d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
	
}
void godLike()
{
	
	
	if (!m_KReader)
		return;
	
	
	
	if (global::bAimOn)
	{
		UpdateAddresses();
		auto playercontroller = m_KReader->readType<DWORD_PTR>(global::pLocalPlayer + PLAYERCONTROLLER, PROTO_NORMAL_READ);
		Vector3 LocalEye = World2Screen(global::cameracache.POV.Location, global::cameracache);
		DWORD_PTR localPlayerController = m_KReader->readType<DWORD_PTR>(global::pLocalPlayer + PLAYERCONTROLLER, PROTO_NORMAL_READ);
		DWORD_PTR enlist = global::pActorArray;
		int entitycount = m_KReader->readType<int>(global::pULevel + ACTORCOUNT, PROTO_NORMAL_READ);
		//global::aimRange = 100.f;
		global::temptarget = 0;
		Vector3 local = GetLocalPlayerPos();
		float distance = 0.f;
		float targetDistance = 0.f;
		for (int i = 0; i < entitycount; i++) // find best ent
		{
			auto entity = m_KReader->readType<DWORD_PTR>(enlist + (i * 0x8), PROTO_NORMAL_READ);
			if (!entity)
				continue;
			if (entity == m_KReader->readType<DWORD_PTR>(localPlayerController + APawn, PROTO_NORMAL_READ))
				continue;
			if (entity == global::pLocalPlayer)
				continue;
			int id = m_KReader->readType<int>(entity + ACTORID, PROTO_NORMAL_READ);
			if (id == ActorIds[0] || id == ActorIds[1] || id == ActorIds[2] || id == ActorIds[3])
			{
				auto mesh = m_KReader->readType<DWORD_PTR>(entity + MESH, PROTO_NORMAL_READ);
				Vector3 bone = World2Screen(GetActorPos(entity), global::cameracache);
				Vector3 pos = GetActorPos(entity);
				float tempDist = bone.calcDistance(LocalEye);
			    distance = local.calcDistance(pos) / 100.f;
				
				//cout << tempDist << endl;
				if ((distance < 250 && tempDist < global::aimRange) || (distance < 20 && tempDist < 300))
				{
					//global::tempclosest = tempDist;
					global::temptarget = i;
					targetDistance = distance;
				}
			}

		}
		//cout << global::temptarget << endl;
		auto entity = m_KReader->readType<DWORD_PTR>(enlist + (global::temptarget * 0x8), PROTO_NORMAL_READ);

		if (!entity)
			return;
		if (entity == m_KReader->readType<DWORD_PTR>(localPlayerController + APawn, PROTO_NORMAL_READ))
			return;
		if (entity == global::pLocalPlayer)
			return;

		auto mesh = m_KReader->readType<DWORD_PTR>(entity + MESH, PROTO_NORMAL_READ);
		Vector3 bone = GetBoneWithRotation(mesh, Bones::neck_01);
		bone = World2Screen(bone, global::cameracache);

		float boneX = bone.x - s_width / 2;
		float boneY = bone.y - s_height / 2;

		if (GetAsyncKeyState(VK_RSHIFT) && global::temptarget != 0 && global::aimRange != 0 && bone.x != 0 && bone.y != 0) {
			cout << GetKeyState(VK_RSHIFT) << ",buton," << "target :" << global::temptarget << ",x,y=" << boneX << "," << boneY << endl;
			mouse_event(0x0001, boneX, boneY, NULL, NULL);
		}
	
	}
		

	
}
//set up overlay window
void SetupWindow()
{
	
	RECT rc;

	
	while (!twnd) {
		wchar_t keystr[] = L"TlqadmUjjanu";
		int len = wcslen(keystr);
		for (int i = 0; i<len; i++)
			*(keystr + i) = (*(keystr + i) ^ global::key[i % 5]);												  //std:string(keystr);
		twnd = FindWindow(keystr, 0);
		
	}
	if (twnd != NULL) 
	{
		GetWindowRect(twnd, &rc);
		s_width = rc.right - rc.left;
		s_height = rc.bottom - rc.top;
	}
	else
	{
		cout << "Closing..." << GetLastError() << endl;
		Sleep(3000);
		Shutdown();
		ExitProcess(0);
	}
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(0);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	wc.lpszClassName = L"PandaTV";
	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT, wc.lpszClassName, L"", WS_POPUP, rc.left, rc.top, s_width, s_height, NULL, NULL, wc.hInstance, NULL);
	//hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT, wc.lpszClassName, L"", WS_POPUP, 0, 0, 2560, 1440, NULL, NULL, wc.hInstance, NULL);

	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
	SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
	
	ShowWindow(hWnd, SW_SHOW);
	initD3D(hWnd);
}

WPARAM MainLoop()
{
	MSG msg;
	RECT rc;
	while (TRUE)
	{
		
			ZeroMemory(&msg, sizeof(MSG));
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (msg.message == WM_QUIT)
				exit(0);
			twnd = NULL;
			wchar_t keystr[] = L"TlqadmUjjanu";
			int len = wcslen(keystr);
			for (int i = 0; i<len; i++)
				*(keystr + i) = (*(keystr + i) ^ global::key[i % 5]);
			twnd = FindWindow(keystr, 0);
							
			if (!twnd)
			{
				cout << "shutting" << endl;
				Shutdown();
				ExitProcess(0);
			}
			ZeroMemory(&rc, sizeof(RECT));
			GetWindowRect(twnd, &rc);
			s_width = rc.right - rc.left;
			s_height = rc.bottom - rc.top;			
			MoveWindow(hWnd, 0, 0, s_width, s_height, true);
		
			//render your esp
			render();

			Sleep(5);
			global::fps++;
		
	}
	return msg.wParam;
}


void aimLoop()
{
	while (1)
	{
		if (global::bAimOn)
		{
			double  allstart = GetTickCount();	
			godLike();
			double  allend = GetTickCount();
			double time = allend - allstart;
			if (time > 50) {
				cout <<"a-i-m Loop:" << time << endl;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}
}
void postLoop(CURLWrapper *m_curl)
{
	while (1)
	{
		if (global::bPostHttp)
		{
			m_curl->sendData();
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}
}
void fpsMonitor()
{
	while (1)
	{
		if (global::bPlayer)
		{
			const char fps[] = { global::fps };
			global::fps = 0;
			DrawString(s_width +50, s_height + 50 , D3DCOLOR_ARGB(255, 255, 255, 255), pFont, fps);
			
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}
 /*decFunction decptr = NULL;
bool loadDll()
{
	HINSTANCE hmod;
	//hmod = ::LoadLibrary(L"D:\\CPPWorkSpace\\pubgdec-orgin\\pubgdec-master\\pubgdec\\TrMath.dll");
	//hmod = ::LoadLibrary(L"D:\\CPPWorkSpace\\pubgdec-master-new\\pubgdec-master\\x64\\Debug\\pubgdec.dll");
	hmod = ::LoadLibrary(L"D:\\CPPWorkSpace\\pubgdec-master-new\\pubgdec-master\\pubgdec\\pubgdec.dll");

	if (NULL == hmod)
	{
		cout << "load TrMath.dll failed" << endl;
		return false;
	}
	decptr = (decFunction)GetProcAddress(hmod, "decptr");
	//addptr = (addFunction)GetProcAddress(hmod, "testAdd");
	cout << "dec function:" << decptr << endl;
	//cout << "addptr function:" << addptr << endl;
	// If the function address is valid, call the function.
	if (NULL == decptr)
	{
		//(ProcAdd)(L"Message sent to the DLL function\n");
	   FreeLibrary(hmod);
	}
	
	dummy decRequest{ m_KReader->getKernHandle(), GetCurrentProcessId(),0 ,m_KReader->getPUBase() };
	int64_t param = (m_KReader->getPUBase() + 0x4054650);
	void* param_ptr = (void *)&param;
	printf("dummy addr:%p \n", &decRequest);
	printf("dummy handle:%p\n", decRequest.handle);
	printf("dummy uid:%I64d\n", decRequest.user_pid);
	printf("param:%I64d \n", param);
	//(decptr)(&decRequest, &param);
	cout << "decptr adress:" << decptr << endl;
	global::pUWorld = (decptr)(&decRequest, param_ptr);
	//delete param_ptr;
	cout << "pUWorld :" << global::pUWorld << endl;
	
	//DWORD a = 5;
	//DWORD b = 1;
	//cout << "test add :"<< (addptr)(a,b) << endl;
}
*/

int main(int argc, char* argv[])
{

	//cout << hex << uppercase;
	m_KReader = new KReader;
	m_Curl = CURLWrapper::getInstance();
	
	std::string readLine = "";
	while (true)
	{
		std::cin >> readLine;
		if (readLine == "start") 
		{
			m_KReader->readPUBase();
			cout << "Direct to : " << m_KReader->getPUBase() << endl;
			break;
		}
		if (readLine == "cloud")
		{
			m_KReader->readPUBase();
			m_Curl->transferToOutnet();
			cout << "Direct to : " << m_KReader->getPUBase() << endl;
			break;
		}
	}
	//dummy decRequest{ m_KReader->getKernHandle(), GetCurrentProcessId(),0 ,m_KReader->getPUBase() };
	//decinit(&decRequest);
	//loadDll();

	atexit(Shutdown);
	SetupWindow();
	//GetNameFromId(0);
	CacheNames();
	//thread superAim(aimLoop);
    thread postLoop(postLoop, m_Curl);
	//thread fpsMonitorLoop(fpsMonitor);
	//superAim.detach(); //START
	uint32_t ret = (uint32_t)MainLoop();
	
	cin.get();
	return ret;//ret;
	
	
}


/*
printMD5("PlayerFemale_A");
printMD5("PlayerFemale_A_C");
printMD5("PlayerMale_A");
printMD5("PlayerMale_A_C");
printMD5("Uaz_A_01_C");
printMD5("Uaz_B_01_C");
printMD5("Uaz_C_01_C");

printMD5("ABP_Motorbike_03_C");
printMD5("ABP_Motorbike_04_C");
printMD5("BP_Motorbike_03_C");
printMD5("BP_Motorbike_04_C");
printMD5("ABP_Motorbike_03_Sidecart_C");
printMD5("Buggy_A_01_C");
printMD5("Buggy_A_02_C");
printMD5("Buggy_A_03_C");
printMD5("Boat_PG117_C");
printMD5("DroppedItemInteractionComponent");
printMD5("DroppedItemGroup");

system("pause");
*/