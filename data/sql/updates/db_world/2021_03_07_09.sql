-- DB update 2021_03_07_08 -> 2021_03_07_09
DROP PROCEDURE IF EXISTS `updateDb`;
DELIMITER //
CREATE PROCEDURE updateDb ()
proc:BEGIN DECLARE OK VARCHAR(100) DEFAULT 'FALSE';
SELECT COUNT(*) INTO @COLEXISTS
FROM information_schema.COLUMNS
WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'version_db_world' AND COLUMN_NAME = '2021_03_07_08';
IF @COLEXISTS = 0 THEN LEAVE proc; END IF;
START TRANSACTION;
ALTER TABLE version_db_world CHANGE COLUMN 2021_03_07_08 2021_03_07_09 bit;
SELECT sql_rev INTO OK FROM version_db_world WHERE sql_rev = '1614901038211344456'; IF OK <> 'FALSE' THEN LEAVE proc; END IF;
--
-- START UPDATING QUERIES
--

INSERT INTO `version_db_world` (`sql_rev`) VALUES ('1614901038211344456');

DELETE FROM `broadcast_text` WHERE `ID`=5140;
DELETE FROM `broadcast_text_locale` WHERE `ID`=5140;
DELETE FROM `creature_text` WHERE `CreatureID`=620 AND `GroupID`=1 AND `BroadcastTextId`=5140;
UPDATE `creature_text` SET `comment`='cluck EMOTE_HELLO' WHERE `CreatureID`=620 AND `GroupID`=0;


--
-- END UPDATING QUERIES
--
COMMIT;
END //
DELIMITER ;
CALL updateDb();
DROP PROCEDURE IF EXISTS `updateDb`;
