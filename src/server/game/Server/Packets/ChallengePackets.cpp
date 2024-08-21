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

#include "ChallengePackets.h"

WorldPacket const* WorldPackets::Challenge::UpdateDeathCount::Write()
{
    _worldPacket << uint32(NewDeathCount);
    return &_worldPacket;
}

WorldPacket const* WorldPackets::Challenge::CompleteChallenge::Write()
{
    _worldPacket << CompletionMilliseconds;
    _worldPacket << MapId;
    _worldPacket << ChallengeId;
    _worldPacket << RewardLevel;

    _worldPacket.WriteBit(IsCompletedInTimer);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Challenge::StartChallenge::Write()
{
    _worldPacket << MapId;
    _worldPacket << ChallengeId;
    _worldPacket << StartedChallengeLevel;

    _worldPacket << Affixes[0];
    _worldPacket << Affixes[1];
    _worldPacket << Affixes[2];

    _worldPacket << DeathCount;
    _worldPacket << Unk0;
    _worldPacket.WriteBit(IsKeyNoDepleted);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Challenge::ResetChallenge::Write()
{
    _worldPacket << MapId;
    return &_worldPacket;
}

void WorldPackets::Challenge::StartChallengeMode::Read()
{
    _worldPacket >> Bag;
    _worldPacket >> Slot;
    _worldPacket >> GobGUID;
}

WorldPacket const* WorldPackets::Challenge::ChangePlayerDifficultyResult::Write()
{
    _worldPacket.FlushBits();
    _worldPacket.WriteBits(Result, 4);

    switch (Result)
    {
        case 5:
        case 8:
        {
            _worldPacket.WriteBit(Cooldown);
            _worldPacket.FlushBits();
            _worldPacket << CooldownReason;
            break;
        }
        case 11:
        {
            _worldPacket << MapId;
            _worldPacket << DifficultyRecID;
            break;
        }
    }

    return &_worldPacket;
}

void WorldPackets::Challenge::RequestLeaders::Read()
{
    _worldPacket >> MapId;
    _worldPacket.AppendPackedTime(LastGuildUpdate);
    _worldPacket.AppendPackedTime(LastRealmUpdate);
    _worldPacket.AppendPackedTime(CompletionDate);
}

WorldPacket const* WorldPackets::Challenge::NewPlayerRecord::Write()
{
    _worldPacket << MapChallengeModId;
    _worldPacket << CompletionTime;
    _worldPacket << KeystoneLevel;
    return &_worldPacket;
}
