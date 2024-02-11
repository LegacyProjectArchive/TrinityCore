DROP TABLE IF EXISTS `challenge_dungeon_template`;
CREATE TABLE `challenge_dungeon_template` (
  `challengeId` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Unique id from MapChallengeMode.db2',
  `name` VARCHAR(255) CHARACTER SET latin1 DEFAULT NULL,
  `safeLocId` INT(10) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Unique id from WorldSafeLocs.db2',
  `VerifiedBuild` SMALLINT(5) DEFAULT '0',
  PRIMARY KEY (`challengeId`)
) ENGINE=MYISAM DEFAULT CHARSET=utf8;

INSERT INTO `challenge_dungeon_template` (`challengeId`, `name`, `safeLocId`) VALUES
(207, "Vault of the Wardens", 5105),
(200, "Halls of Valor", 5098),
(198, "Darkheart Thicket", 5334);