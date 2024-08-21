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

#include "WorldSession.h"
#include "ChallengeMgr.h"
#include "ChallengePackets.h"
#include "InstanceChallenge.h"
#include "Item.h"
#include "Group.h"
#include <InstanceSaveMgr.h>

void WorldSession::HandleStartChallengeMode(WorldPackets::Challenge::StartChallengeMode& startChallengeMode)
{
    Player* owner = GetPlayer();

    if (!owner)
        return;

    InstanceMap const* map = owner->GetMap()->ToInstanceMap();

    if (!map)
        return;

    Item const* keystone = owner->GetItemByPos(startChallengeMode.Bag, startChallengeMode.Slot);

    if (!keystone)
        return;

    uint32 challengeId = keystone->GetModifier(ITEM_MODIFIER_CHALLENGE_MAP_CHALLENGE_MODE_ID);
    uint32 challengeLevel = keystone->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_LEVEL);

    ChallengeData challengeData;

    challengeData.ChallengeId = challengeId;
    challengeData.ChallengeLevel = challengeLevel;
    challengeData.KeystoneOwnerGUID = owner->GetGUID();
    challengeData.KeystoneItemGUID = keystone->GetGUID();

    challengeData.Affixes.at(0) = Affixes{ keystone->GetModifier(ItemModifier::ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_1) };
    challengeData.Affixes.at(1) = Affixes{ keystone->GetModifier(ItemModifier::ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_2) };
    challengeData.Affixes.at(2) = Affixes{ keystone->GetModifier(ItemModifier::ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_3) };

    Group* group = owner->GetGroup();

    sChallengeMgr->AddChallengeData(group ? group->GetGUID() : owner->GetGUID(), challengeData);

    auto StartChallengeFor = [challengeId, map](Player* target)
    {
        WorldPackets::Challenge::ChangePlayerDifficultyResult changePlayerDiffResultFirst;

        changePlayerDiffResultFirst.Result = 5;
        changePlayerDiffResultFirst.Cooldown = false;
        changePlayerDiffResultFirst.CooldownReason = 2778939101;

        target->GetSession()->SendPacket(changePlayerDiffResultFirst.Write());
        target->SetDungeonDifficultyID(Difficulty::DIFFICULTY_MYTHIC_KEYSTONE);

        WorldPackets::Challenge::ChangePlayerDifficultyResult changePlayerDiffResultSecond;

        changePlayerDiffResultSecond.Result = 11;
        changePlayerDiffResultSecond.MapId = map->GetId();
        changePlayerDiffResultSecond.DifficultyRecID = Difficulty::DIFFICULTY_MYTHIC_KEYSTONE;

        target->GetSession()->SendPacket(changePlayerDiffResultSecond.Write());

        WorldLocation teleDest = sChallengeMgr->GetChallengeEntrance(challengeId);
        target->TeleportTo(teleDest, TELE_TO_CHALLENGE_MAP);
    };

    if (group)
    {
        group->SetDungeonDifficultyID(Difficulty::DIFFICULTY_MYTHIC_KEYSTONE);
        group->UnbindInstance(map->GetId(), Difficulty::DIFFICULTY_MYTHIC, false);

        Group::MemberSlotList const& members = group->GetMemberSlots();

        for (auto itr = members.begin(); itr != members.end(); ++itr)
        {
            if (Player* p = ObjectAccessor::FindPlayer((*itr).guid))
                StartChallengeFor(p);
        }
    }
    else
        StartChallengeFor(owner);
}


void WorldSession::HandleRequestLeadersChallengeMode(WorldPackets::Challenge::RequestLeaders& requestLeaders)
{

}
