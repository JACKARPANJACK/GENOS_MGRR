#include "Events.h"
#include <cWeaponSelectMenu.h>
#include <PlayerManagerImplement.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <random>
#include <Trigger.h>
#include <GameMenuStatus.h>
#include <Pl0000.h>
#include <cGameUIManager.h>
#include "cSlowRateManager.h"
#include <AnimationMapManagerImplement.h>
#include "KeyBinds.h"
#include "KeyBind.h"
#include <Windows.h>
#include <WinUser.h>
#include "pch.h"
#include <EntitySystem.h>
#include <EntityHandle.h>
#include <Entity.h>
#include <Behavior.h>
#include <BehaviorEmBase.h>
#include <thread> // Include thread header
#include <mutex> // Include mutex header

void AnimationPlayer(Behavior* behavior, float a2, float a3)
{
    ((void(__thiscall*)(Behavior * behavior, float a2, float a3))(shared::base + 0x794790))(behavior, a2, a3);
}

void AnimationStop(Behavior* behavior, int a2, float a3)
{
    ((void(__thiscall*)(Behavior * behavior, int a2, float a3))(shared::base + 0x694BC0))(behavior, a2, a3);
}

int MGR_PlaySound(const char* se, int a2)
{
    return ((int(__cdecl*)(const char* se, int a2))(shared::base + 0xA5E050))(se, a2);
}

int DirectAnim_file(Behavior* bh, void* mot, void* seq, int a4, float fInterpolation, float a6, int nFlags, float fStartFrame, float a9)
{
    return ((int(__thiscall*)(Behavior*, void*, void*, int, float, float, int, float, float))(shared::base + 0x69EFB0))(bh, mot, seq, a4, fInterpolation, a6, nFlags, fStartFrame, a9);
}

int setAnimation(Behavior* behavior, void* animationFile, const char* animName, int a4, float fInterpolation, float a6, int nFlags, float fStartFrame, float a9)
{
    return ((int(__thiscall*)(Behavior*, void*, const char*, int, float, float, int, float, float))(shared::base + 0xA3FD90))(behavior, animationFile, animName, a4, fInterpolation, a6, nFlags, fStartFrame, a9);
}

int datfilelod(int* dat, const char* a2)
{
    return ((int(__thiscall*)(int* dat, const char* a2))(shared::base + 0x9E4500))(dat, a2);
}

std::mutex animationMutex;

void PlayAnimationSequence(Pl0000* player)
{
    std::lock_guard<std::mutex> lock(animationMutex);

    struct AnimationData {
        const char* mot;
        const char* seq;
    };

    AnimationData animations[] = {
        {"pl0010_2250.mot", "pl0010_2250_0_seq.bxm"},
        {"pl0010_2251.mot", "pl0010_2251_0_seq.bxm"},
        {"pl0010_2252.mot", "pl0010_2252_0_seq.bxm"},
        {"pl0010_2260.mot", "pl0010_2260_0_seq.bxm"},
        {"pl0010_2261.mot", "pl0010_2261_0_seq.bxm"},
        {"pl0010_2262.mot", "pl0010_2262_0_seq.bxm"}
    };

    int numAnimations = sizeof(animations) / sizeof(AnimationData);
    for (int i = 0; i < numAnimations; i++)
    {
        void* mot = (void*)datfilelod(&player->field_494, animations[i].mot);
        void* seq = (void*)datfilelod(&player->field_494, animations[i].seq);
        DirectAnim_file(player, mot, seq, 0, 0.13333334f, 1.0f, 0, 0.0f, 1.0f);
        ++player->m_nCurrentActionId;
        int currentAction = player->getCurrentActionId();
        AnimationPlayer(player, 1.0f, 1.0f);

        auto fTicks = cSlowRateManager::Instance.m_fTicks;
        static float lastTime = fTicks;
        if (fTicks - lastTime > 1000.0f && shared::IsKeyPressed(VK_RBUTTON, false))
        {
            lastTime = fTicks;
        }
        else
        {
            break;
        }
    }
}

class GameManager {
public:
    static bool IS_GENOS;

    static void TurnOnGenos() {
        IS_GENOS = true;
    }

    static void TurnOffGenos() {
        IS_GENOS = false;
    }

    static bool IsGenosOn() {
        return IS_GENOS;
    }
};

bool GameManager::IS_GENOS = false;

class GENOS {
public:
    GENOS()
    {
        Events::OnTickEvent += []()
        {
            Pl0000* player = (Pl0000*)cGameUIManager::Instance.m_pPlayer;
            if (player != nullptr)
            {
                auto pl = PlayerManagerImplement::pInstance;
                int currentCustomWeapon = pl->GetCustomWeaponEquipped();
                if (shared::IsKeyPressed(0x47, false))
                {
                    if (!GameManager::IsGenosOn() && currentCustomWeapon != 5)
                    {
                        GameManager::TurnOnGenos();
                        player->setSwordLost(1);
                        player->m_bSwordHidden = 1;
                        pl->SetCustomWeaponEquipped(5);
                    }
                    else
                    {
                        player->setSwordLost(0);
                        player->m_bSwordHidden = 0;
                        pl->SetCustomWeaponEquipped(0);
                        GameManager::TurnOffGenos();
                    }
                }
            }
            if (GameManager::IsGenosOn() == 1)
            {
                bool in_air = player->isInAir();
                if (in_air == 0)
                {
                    if (shared::IsKeyPressed(VK_RBUTTON, false))
                    {
                        int currentAction = player->getCurrentAction();
                        player->setState(currentAction, 0, 0, 0);
                        std::thread animationThread(PlayAnimationSequence, player);
                        animationThread.detach();
                    }
                }
            }
        };
    }

}_GENOS;
