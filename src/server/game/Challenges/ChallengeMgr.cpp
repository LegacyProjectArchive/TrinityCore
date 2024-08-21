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

#include "ChallengeMgr.h"
#include "DatabaseEnv.h"
#include "DB2Stores.h"
#include "InstanceChallenge.h"
#include "Log.h"
#include "Map.h"
#include <Containers.h>

ChallengeMgr* ChallengeMgr::Instance()
{
    static ChallengeMgr instance;
    return &instance;
}

void ChallengeMgr::LoadDBData()
{
    _challengeEntrances.clear();

    uint32 oldMSTime = getMSTime();

    // Fill teleport locations from DB
    QueryResult result = WorldDatabase.Query("SELECT challengeId, safeLocId FROM challenge_dungeon_template");
    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 challenge dungeon templates. DB table `challenge_dungeon_template` is empty!");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        uint32 challengeId = fields[0].GetUInt32();
        uint32 safeLocId = fields[1].GetUInt32();

        /*
        TO DO: Use MapChallenge.db2
        if (!sWorldSafeLocsStore.LookupEntry(safeLocId))
        {
            TC_LOG_ERROR("sql.sql", "ChallengeMgr::LoadDBData: DB Table `challenge_dungeon_template`, column challengeId contained an invalid MapChallenge (Id: %u)!", challengeId);
            continue;
        }*/

        if (!sWorldSafeLocsStore.LookupEntry(safeLocId))
        {
            TC_LOG_ERROR("sql.sql", "ChallengeMgr::LoadDBData: DB Table `challenge_dungeon_template`, column safeLocId contained an invalid WorldSafeLoc (Id: %u)!", safeLocId);
            continue;
        }

        _challengeEntrances[challengeId] = safeLocId;

        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u challenge templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

InstanceChallenge* ChallengeMgr::CreateInstanceChallenge(Map const* map) const
{
    return new InstanceChallenge(map);
}

void ChallengeMgr::GenerateWeekAffixes()
{
    std::array<Affixes, 5> l_AffixOne = { Affixes::Teeming, Affixes::Bolstering, Affixes::Bursting, Affixes::Raging, Affixes::Sanguine };
    std::array<Affixes, 7> l_AffixTwo = { Affixes::Overflowing, Affixes::Skittish, Affixes::Volcanic, Affixes::Necrotic, Affixes::Grievous, Affixes::Explosive, Affixes::Quaking };
    std::array<Affixes, 2> l_AffixThree = { Affixes::Fortified, Affixes::Tyrannical };

    _weekAffixes =
    {
        Trinity::Containers::SelectRandomContainerElement(l_AffixOne),
        Trinity::Containers::SelectRandomContainerElement(l_AffixTwo),
        Trinity::Containers::SelectRandomContainerElement(l_AffixThree),
    };
}

WorldLocation ChallengeMgr::GetChallengeEntrance(uint32 challengeId) const
{
    if (auto itr = _challengeEntrances.find(challengeId); itr != _challengeEntrances.end())
    {
        WorldSafeLocsEntry const* loc = sWorldSafeLocsStore.LookupEntry(itr->second);

        if (!loc)
            return WorldLocation();

        return WorldLocation{ loc->MapID, loc->Loc.X, loc->Loc.Y, loc->Loc.Z, loc->Facing };
    }

    return WorldLocation{};
}

ChallengeData const* ChallengeMgr::GetChallengeData(ObjectGuid guid) const
{
    auto itr = _currentChallenges.find(guid);

    if (itr != _currentChallenges.end())
        return &itr->second;

    return nullptr;
}

void ChallengeMgr::AddChallengeData(ObjectGuid guid, ChallengeData data)
{
    auto itr = _currentChallenges.find(guid);

    if (itr == _currentChallenges.end())
        _currentChallenges.try_emplace(guid, data);
}

void ChallengeMgr::RemoveChallengeData(ObjectGuid guid)
{
    _currentChallenges.erase(guid);
}





