#include "cheat.h"
#include "../IniReader.h"
#include "../KeyBind.h"
#include <cSlowRateManager.h>
#include <Trigger.h>
#include <EntitySystem.h>
#include "pch.h"
#include <Pl0000.h>
#include <cGameUIManager.h>
#include <PlayerManagerImplement.h>
#include <BehaviorEmBase.h>
#include <EmBaseDLC.h>
#include <PhaseManager.h>
#include <Behavior.h>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include "Events.h"
#include <random>
#pragma comment(lib, "D3dx9.lib") 
#include <cCameraGame.h>

static int requestAnimationByMap2(Behavior* behavior, int a2, int a3, float a4, float a5, float a6, float a7, float a8)
{
	return ((int(__thiscall*)(Behavior * behavior, int a2, int a3, float a4, float a5, float a6, float a7, float a8))(shared::base + 0x6A4080))(behavior, a2, a3, a4, a5, a6, a7, a8);
}

static bool animationEnded(Behavior* behavior, int a2)
{
	return ((bool(__thiscall*)(Behavior * behavior, int a2))(shared::base + 0x694CE0))(behavior, a2);
}

static void AnimationPlayer(Behavior* behavior, float a2, float a3)
{
	((void(__thiscall*)(Behavior * behavior, float a2, float a3))(shared::base + 0x794790))(behavior, a2, a3);
}

static void CustomeAction(Pl0000*player,int AnimationMapID)
{
	if (player->m_nCurrentActionId % 2 == 0) {
		if (player->m_nCurrentActionId != 0) {
			requestAnimationByMap2(player, AnimationMapID, 0, 0.0, 1.0, 0x8100000, -1.0, 1.0);
			//RequestFaceAnimationById(PlayerFace, 0x11017, FaceMimicIds[player->m_nCurrentActionId - (player->m_nCurrentActionId / 2)], PlayerHair->m_pEntity, 0, 0.0, 1.0, 0, 0.0, 1.0);
		}
		else {


			requestAnimationByMap2(player, AnimationMapID, 0, 0.0, 1.0, 0x8100000, -1.0, 1.0);
			//RequestFaceAnimationById(PlayerFace, 0x11017, FaceMimicIds[0], PlayerHair->m_pEntity, 0, 0.0, 1.0, 0, 0.0, 1.0);
		}
		++player->m_nCurrentActionId;

	}
	else if (player->m_nCurrentActionId != 1)
	{
		AnimationPlayer(player, 1.0, 1.0);

		if (animationEnded(player, 0))
			++player->m_nCurrentActionId;


	}


}

void cheat::Enable() noexcept
{  
	auto player = cGameUIManager::Instance.m_pPlayer;
	if (player)
	{
		for (auto i = (Entity**)EntitySystem::Instance.m_EntityList.m_pFirst; i != (Entity**)EntitySystem::Instance.m_EntityList.m_pEnd; i = (Entity**)i[2])
		{	//随机数生成 以增强Boss Sam/Raiden
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> distrib(1, 7); //Generate Random Numbers 
	        int a = distrib(gen);
	

			// Light attack randomizer
			if (player->m_nCurrentAction == 0x4F || player->m_nCurrentAction == 0x6C)
			{
				switch (a)
				{
				case 1:
					CustomeAction(player, 670);

					break;
				case 2:
					CustomeAction(player, 331);

					break;
				case 3:
					CustomeAction(player, 392);

					break;
				case 4:
					CustomeAction(player, 393);

					break;
				case 5:
					CustomeAction(player, 394);

					break;
				case 6:
					CustomeAction(player, 401);

					break;
				case 7:
					CustomeAction(player, 402);

					break;
				default:
					break;
				}
			}		
		}
	}
}

int MGR_PlaySound(const char* se, int a2)
{
	return ((int(__cdecl*)(const char* se, int a2))(shared::base + 0xA5E050))(se, a2);
}

void DestroyCustomWeapon(Pl0000* player)
{
	((void(__thiscall*)(Pl0000 * player))(shared::base + 0x77D0E0))(player);
}

void PlayerChangeCustomWeapon(Pl0000* player, int WeaponId)
{
	((void(__thiscall*)(Pl0000 * player, int WeaponId))(shared::base + 0x77F5E0))(player, WeaponId);
}

void PlayerChangeCustomWeapon2(void* player)
{
	((void(__thiscall*)(void* player))(shared::base + 0x7948D0))(player);
}

unsigned int cheat::ReadDoublePointer(unsigned int baseAddress, unsigned int offset) noexcept
{
	unsigned int address = 0;
	address = injector::ReadMemory<unsigned int>(baseAddress, true);
	if (address) {
		return injector::ReadMemory<unsigned int>(address + offset, true);
	}
	else return 0;
}


int cheat::hex_to_int(const std::string& str)
{
	unsigned int hex_num = 0;
	for (size_t i = 0; i < str.length(); ++i) {
		hex_num = (hex_num << 8) | static_cast<unsigned int>(str[i]);
	}

	return ((hex_num & 0xFF000000) >> 24) | ((hex_num & 0x00FF0000) >> 8)
		| ((hex_num & 0x0000FF00) << 8) | ((hex_num & 0x000000FF) << 24);;
}

unsigned int cheat::ReadSinglePointer(unsigned int baseAddress, unsigned int offset) noexcept
{
	return injector::ReadMemory<unsigned int>(baseAddress + offset, true);
}


void cheat::SetFunctionToAddress(unsigned int addr1, unsigned int addr2)
{
	if (injector::ReadMemory<unsigned int>(addr1, true) != addr2) injector::WriteMemory<unsigned int>(addr1, addr2, true);
}


void cheat::Enable2() noexcept
{
	Pl0000* player = cGameUIManager::Instance.m_pPlayer;
	if (player) {
		if (shared::IsKeyPressed(0x47, true)) 
		{ // 'G' key pressed
			bool isGenosActive = 0;

			if (isGenosActive == 0) 
			{
					player->m_nSwordState = 1;
					DestroyCustomWeapon(player);
					cheat::SetFunctionToAddress(shared::base + 0x17E9FD8, 5);
					PlayerChangeCustomWeapon(player, 5);
					cheat::SetFunctionToAddress(shared::base + 0x19C074C, 1);
			}
			else 
			{
				// Deactivate GENOS mode
				if (pl->GetCustomWeaponEquipped() == 5) {
					pl->SetCustomWeaponEquipped(0);
				}
				player->m_nSwordState = 0;
				isGenosActive = 0;
				// Add any additional deactivation logic here if needed
			}
		}
	}
}






void cheat::HandleCheats() noexcept
{
	static bool once = false;
	if (!once)
	{
		LoadConfig();
		once = true;
	}

	Enable();
}


void cheat::LoadConfig() noexcept
{
CIniReader iniReader("ModMenu.ini");
CIniReader iniReader2("Styles.ini");


}


void cheat::SaveConfig() noexcept
{
CIniReader iniReader("ModMenu.ini");
}


void cheat::Reset() noexcept
{
	//Nothing
}