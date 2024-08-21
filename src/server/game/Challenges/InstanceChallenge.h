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

#ifndef InstanceChallenge_h__
#define InstanceChallenge_h__

#include "ChallengeMgr.h"
#include <TaskScheduler.h>

class Map;

enum ChallengeSpells
{
    SPELL_CHALLENGERS_BURDEN = 206151,
    SPELL_CHALLENGERS_MIGHT  = 206150,
};

class TC_GAME_API InstanceChallenge
{
    public:
        explicit InstanceChallenge(Map const* map);

        void SaveToDB();
        void LoadInstanceData(uint32 instanceId);

        void Update(uint32 const diff);
        void Complete();


        void OnPlayerEnter(Player* player);
        void OnPlayerExit(Player* player);
        void OnUnitDeath(Unit* unit);

        uint32 GetChallengeLevel() const;
        bool HasDataInitialized() const { return _challengeData.has_value(); }
        void SetChallengeData(ChallengeData data);

    private:
        std::string GetOwnerInfo() const;
        void SendPacket(WorldPacket const* data) const;

        void Start();

        void SendStartChallenge(Player const* player, bool justForPlayer = true) const;
        void SendStartTimer(Player const* player, bool justForPlayer = true) const;
        void SendChallengeModeReset(Player const* player, bool justForPlayer = true) const;
        void SendStartElapsedTimer(Player const* player, bool justForPlayer = true) const;
        void SendStopElapsedTimer(Player const* player, bool justForPlayer = true) const;
        void SendCompleteChallenge(Player const* player, bool justForPlayer = true) const;
        void SendUpdateDeathCount(Player const* player, bool justForPlayer = true) const;

        uint32 GetChallengeId() const;
        ObjectGuid GetKeystoneGUID() const;
        ObjectGuid GetKeystoneOwnerGUID() const;
        std::array<Affixes, 3> GetAffixes() const;

        bool HasReachTimer() const;

        Map const* _map{ nullptr };
        uint8 _deathCount = 0;

        Optional<ChallengeData> _challengeData;

        uint64 _timePassed{ 0 };

        bool _isComplete{ false };
        bool _isInTime{false};
        bool _started{false};
        bool _isLoading{ false };

        MapChallengeModeEntry const* _challengeEntry{ nullptr };

        TaskScheduler scheduler;
};

#endif // InstanceChallenge_h__
