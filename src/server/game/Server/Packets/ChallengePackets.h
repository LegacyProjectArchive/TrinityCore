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

#ifndef ChallengePackets_h__
#define ChallengePackets_h__

#include "Packet.h"
#include "ObjectGuid.h"

namespace WorldPackets::Challenge
{
        class StartChallengeMode final : public ClientPacket
        {
            public:
                StartChallengeMode(WorldPacket&& packet) : ClientPacket(OpcodeClient::CMSG_START_CHALLENGE_MODE, std::move(packet))
                {}

                void Read() override;

                uint8 Bag = 0;
                int32 Slot = 0;
                ObjectGuid GobGUID;
        };

        class RequestLeaders : public ClientPacket
        {
        public:
            RequestLeaders(WorldPacket&& packet) : ClientPacket(CMSG_CHALLENGE_MODE_REQUEST_LEADERS, std::move(packet)) {}

            void Read() override;

            uint32 MapId = 0;
            time_t LastGuildUpdate = time_t(nullptr);
            time_t LastRealmUpdate = time_t(nullptr);
            time_t CompletionDate  = time_t(nullptr);
            
        };

        class ResetChallenge final : public ServerPacket
        {
            public:
                ResetChallenge() : ServerPacket(OpcodeServer::SMSG_CHALLENGE_MODE_RESET, 4)
                {}

                WorldPacket const* Write() override;

                uint32 MapId = 0;
        };

        class StartChallenge final : public ServerPacket
        {
            public:
                StartChallenge() : ServerPacket(SMSG_CHALLENGE_MODE_START, 33), Affixes()
                {
                }

                WorldPacket const* Write() override;

                uint32 MapId = 0;
                int32 ChallengeId = 0;
                std::array<uint32, 3> Affixes;
                uint32 DeathCount = 0;
                bool IsKeyNoDepleted = true;
                uint32 StartedChallengeLevel = 2;
                uint32 Unk0 = 0;
        };

        class CompleteChallenge final : public ServerPacket
        {
            public:
                CompleteChallenge() : ServerPacket(SMSG_CHALLENGE_MODE_COMPLETE, 17) { }

                WorldPacket const* Write() override;

                uint32 MapId = 0;
                Duration<Milliseconds, int32> CompletionMilliseconds = 0ms;
                int32 RewardLevel = 0;
                uint32 ChallengeId = 0;
                bool IsCompletedInTimer = false;
        };

        class UpdateDeathCount final : public ServerPacket
        {
            public:
                UpdateDeathCount() : ServerPacket(OpcodeServer::SMSG_CHALLENGE_MODE_UPDATE_DEATH_COUNT, 4) { }

                WorldPacket const* Write() override;
                uint32 NewDeathCount = 0;
        };

        class ChangePlayerDifficultyResult : public ServerPacket
        {
            public:
                ChangePlayerDifficultyResult() : ServerPacket(SMSG_CHANGE_PLAYER_DIFFICULTY_RESULT, 9) {}

                WorldPacket const* Write() override;

                ObjectGuid Guid;
                uint32 CooldownReason = 0;
                uint32 MapId = 0;
                uint32 DifficultyRecID = 0;
                uint32 MapID = 0;
                uint8 Result = 0;
                bool Cooldown = false;
        };

        class NewPlayerRecord : public ServerPacket
        {
            public:
                NewPlayerRecord() : ServerPacket(SMSG_CHALLENGE_MODE_NEW_PLAYER_RECORD, 12)
                {}

                WorldPacket const* Write() override;

                uint32 MapChallengeModId = 0;
                Duration<Milliseconds, int32> CompletionTime = 0ms;
                uint32 KeystoneLevel = 0;
        };
        
}

#endif // ChallengePackets_h__
