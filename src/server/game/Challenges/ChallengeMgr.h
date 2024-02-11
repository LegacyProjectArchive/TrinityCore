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

#ifndef ChallengeMgr_h__
#define ChallengeMgr_h__

#include "Common.h"
#include "Hash.h"
#include "SharedDefines.h"
#include <map>
#include <unordered_map>
#include <vector>

class InstanceChallenge;

enum class Affixes : uint32
{
    None = 0,
    Overflowing = 1,
    Skittish = 2,
    Volcanic = 3,
    Necrotic = 4,
    Teeming = 5,
    Raging = 6,
    Bolstering = 7,
    Sanguine = 8,
    Tyrannical = 9,
    Fortified = 10,
    Bursting = 11,
    Grievous = 12,
    Explosive = 13,
    Quaking = 14,
    Relentless = 15
};

struct ChallengeData
{
    uint32 ChallengeLevel;
    uint32 ChallengeId;
    ObjectGuid KeystoneOwnerGUID;
    ObjectGuid KeystoneItemGUID;
    std::array<Affixes, 3> Affixes{ Affixes::None, Affixes::None, Affixes::None };
};

using ChallengeDataContainer = std::unordered_map<ObjectGuid, ChallengeData>;
using ChallengeDBDataContainer = std::unordered_map<uint32, uint32>;

class TC_GAME_API ChallengeMgr
{
private:
    ChallengeMgr() = default;
    ~ChallengeMgr() = default;

public:
    static ChallengeMgr* Instance();
    void LoadDBData();
    InstanceChallenge* CreateInstanceChallenge(Map const* map) const;
    std::array<Affixes, 3> const & GetCurrentWeekAffixes() const { return _weekAffixes; }
    void GenerateWeekAffixes();

    WorldLocation GetChallengeEntrance(uint32 challengeId) const;

    void AddChallengeData(ObjectGuid guid, ChallengeData data);
    ChallengeData const* GetChallengeData(ObjectGuid guid) const;
    void RemoveChallengeData(ObjectGuid guid);

private:
    ChallengeMgr(ChallengeMgr const&) = delete;
    ChallengeMgr& operator=(ChallengeMgr const&) = delete;


    std::array<Affixes, 3> _weekAffixes;
    ChallengeDBDataContainer _challengeEntrances;
    ChallengeDataContainer _currentChallenges;
};

#define sChallengeMgr ChallengeMgr::Instance()

#endif // ScenarioMgr_h__
