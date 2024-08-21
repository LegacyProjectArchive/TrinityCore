/*
 * Copyright (C) 2008-2018 TrinityCore <https://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "InstanceChallenge.h"
#include "DatabaseEnv.h"
#include "DB2Stores.h"
#include "InstanceSaveMgr.h"
#include "Log.h"
#include "Map.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ChallengePackets.h"
#include "MiscPackets.h"
#include "Item.h"

InstanceChallenge::InstanceChallenge(Map const* map) :
    _map(map)
{
    ASSERT(_map);
    LoadInstanceData(_map->GetInstanceId());
}

void InstanceChallenge::SaveToDB()
{
    /*TODO: Save Challenge Progress*/
}

void InstanceChallenge::LoadInstanceData(uint32 instanceId)
{
    /*TODO: Load Challenge Progress in case of crash*/
}

void InstanceChallenge::Update(uint32 const diff)
{
    _timePassed += diff;
    scheduler.Update(diff);

    if (!_started)
        return;

    if (_isComplete)
        return;

    if (_isInTime && HasReachTimer())
        _isInTime = false;
}

void InstanceChallenge::Complete()
{
    SendStopElapsedTimer(nullptr, false);
    SendCompleteChallenge(nullptr, false);
}

bool InstanceChallenge::HasReachTimer() const
{
    Seconds timePassInSeconds = std::chrono::duration_cast<Seconds>(Milliseconds{ _timePassed });
    return timePassInSeconds.count() > _challengeEntry->CriteriaCount[0];
}

void InstanceChallenge::Start()
{
    _timePassed = 0;
    _isLoading = true;
    scheduler.Schedule(10s, [this](auto /**/)
    {
        _timePassed = 0;
        SendStartElapsedTimer(nullptr, false);
        _started = true;
    });
}

void InstanceChallenge::OnPlayerEnter(Player* player)
{
    if (!_isLoading)
        Start();

    scheduler.Schedule(500ms, [this, player](auto /*task*/)
    {
        SendChallengeModeReset(player);

        if (_started)
            SendStartElapsedTimer(player);
        else
            SendStartTimer(player);

        SendStartChallenge(player);

        player->CastSpell(player, ChallengeSpells::SPELL_CHALLENGERS_BURDEN, true);
    });
}

void InstanceChallenge::OnPlayerExit(Player* player)
{
    player->RemoveAurasDueToSpell(ChallengeSpells::SPELL_CHALLENGERS_BURDEN);
}

void InstanceChallenge::OnUnitDeath(Unit* unit)
{
    if (Player const* player = unit->ToPlayer())
    {
        _deathCount++;
        _timePassed += Seconds(5).count();

        SendStartElapsedTimer(player, false);
        SendUpdateDeathCount(player, false);
    }
}

void InstanceChallenge::SendChallengeModeReset(Player const* player, bool justForPlayer) const
{
    WorldPackets::Challenge::ResetChallenge resetChallenge;
    resetChallenge.MapId = _map->GetId();

    if (justForPlayer && player)
        player->GetSession()->SendPacket(resetChallenge.Write());
    else
        SendPacket(resetChallenge.Write());
}

void InstanceChallenge::SendStartElapsedTimer(Player const* player, bool justForPlayer) const
{
    WorldPackets::Misc::StartElapsedTimer elapsedTimer;

    Milliseconds currentDuration{ _timePassed };

    elapsedTimer.Type = CountdownTimerType::ChallengeMode;
    elapsedTimer.CurrentDuration = std::chrono::duration_cast<std::chrono::seconds>(currentDuration);

    if (justForPlayer && player)
        player->GetSession()->SendPacket(elapsedTimer.Write());
    else
        SendPacket(elapsedTimer.Write());
}

void InstanceChallenge::SendStopElapsedTimer(Player const* player, bool justForPlayer) const
{
    WorldPackets::Misc::StopElapsedTimer stopElapsedTimer;

    stopElapsedTimer.Type = CountdownTimerType::ChallengeMode;
    stopElapsedTimer.KeepTimer = false;

    if (justForPlayer && player)
        player->GetSession()->SendPacket(stopElapsedTimer.Write());
    else
        SendPacket(stopElapsedTimer.Write());
}

void InstanceChallenge::SendStartChallenge(Player const* player, bool justForPlayer) const
{
    ASSERT(_challengeData);

    WorldPackets::Challenge::StartChallenge startChallenge;

    startChallenge.MapId = _map->GetId();
    startChallenge.IsKeyNoDepleted = true;
    startChallenge.ChallengeId = GetChallengeId();
    startChallenge.StartedChallengeLevel = GetChallengeLevel();
    startChallenge.DeathCount = _deathCount;

    auto affixes = GetAffixes();

    for (auto i = 0; i < affixes.size(); ++i)
        startChallenge.Affixes.at(i) = static_cast<uint32>(affixes.at(i));

    if (justForPlayer && player)
        player->GetSession()->SendPacket(startChallenge.Write());
    else
        SendPacket(startChallenge.Write());
}

void InstanceChallenge::SendStartTimer(Player const* player, bool justForPlayer) const
{
    WorldPackets::Misc::StartTimer startTimer;
    startTimer.TimeLeft = 9s - std::chrono::duration_cast<std::chrono::seconds>(Milliseconds{ _timePassed });
    startTimer.TotalTime = 10s;
    startTimer.Type = CountdownTimerType::ChallengeMode;

    if (justForPlayer && player)
        player->GetSession()->SendPacket(startTimer.Write());
    else
        SendPacket(startTimer.Write());
}

void InstanceChallenge::SendCompleteChallenge(Player const* player, bool justForPlayer) const
{
    if (_isComplete)
        return;

    WorldPackets::Challenge::CompleteChallenge completeChallenge;

    completeChallenge.ChallengeId = GetChallengeId();
    completeChallenge.MapId = _map->GetId();
    completeChallenge.RewardLevel = GetChallengeLevel();
    completeChallenge.IsCompletedInTimer = _isInTime;
    completeChallenge.CompletionMilliseconds = Milliseconds{ _timePassed };

    if (justForPlayer && player)
        player->GetSession()->SendPacket(completeChallenge.Write());
    else
        SendPacket(completeChallenge.Write());
}

void InstanceChallenge::SendUpdateDeathCount(Player const* player, bool justForPlayer) const
{
    WorldPackets::Challenge::UpdateDeathCount updateDeathCount;
    updateDeathCount.NewDeathCount = _deathCount;

    if (justForPlayer && player)
        player->GetSession()->SendPacket(updateDeathCount.Write());
    else
        SendPacket(updateDeathCount.Write());
}

uint32 InstanceChallenge::GetChallengeLevel() const
{
    ASSERT(_challengeData);
    return _challengeData->ChallengeLevel;
}

std::array<Affixes, 3> InstanceChallenge::GetAffixes() const
{
    ASSERT(_challengeData);
    return _challengeData->Affixes;
}

void InstanceChallenge::SetChallengeData(ChallengeData data)
{
    _challengeData = data;
    _challengeEntry = sMapChallengeModeStore.LookupEntry(_challengeData->ChallengeId);
    ASSERT(_challengeEntry);
}

uint32 InstanceChallenge::GetChallengeId() const
{
    ASSERT(_challengeData);
    return _challengeData->ChallengeId;
}

ObjectGuid InstanceChallenge::GetKeystoneGUID() const
{
    ASSERT(_challengeData);
    return _challengeData->KeystoneItemGUID;
}

ObjectGuid InstanceChallenge::GetKeystoneOwnerGUID() const
{
    ASSERT(_challengeData);
    return _challengeData->KeystoneOwnerGUID;
}

std::string InstanceChallenge::GetOwnerInfo() const
{
    return Trinity::StringFormat("Instance ID %u", _map->GetInstanceId());
}

void InstanceChallenge::SendPacket(WorldPacket const* data) const
{
    _map->SendToPlayers(data);
}
