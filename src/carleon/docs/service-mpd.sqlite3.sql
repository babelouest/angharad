DROP TABLE IF EXISTS `c_mpd_websocket`;
DROP TABLE IF EXISTS `c_service_mpd`;

CREATE TABLE `c_service_mpd` (
  `cmpd_name` TEXT PRIMARY KEY NOT NULL,
  `cmpd_description` TEXT,
  `cmpd_host` TEXT NOT NULL,
  `cmpd_port` INTEGER,
  `cmpd_password` TEXT,
  `cmpd_device` TEXT,
  `cmpd_switch` TEXT
);

CREATE TABLE `c_mpd_websocket` (
  `cmw_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `cmpd_id` INTEGER NOT NULL,
  `cmw_name` TEXT NOT NULL,
	`cmw_creation` INT DEFAULT CURRENT_TIMESTAMP,
  CONSTRAINT `service_mpd_ibfk_1` FOREIGN KEY (`cmpd_id`) REFERENCES `c_service_mpd` (`cmpd_id`)
);
CREATE INDEX `cmw_name_idx` ON `c_mpd_websocket` (`cmw_name`);
