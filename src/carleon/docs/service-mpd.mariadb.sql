DROP TABLE IF EXISTS `c_mpd_websocket`;
DROP TABLE IF EXISTS `c_service_mpd`;

CREATE TABLE `c_service_mpd` (
  `cmpd_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `cmpd_name` varchar(64) NOT NULL UNIQUE,
  `cmpd_description` varchar(128),
  `cmpd_host` varchar(128) NOT NULL,
  `cmpd_port` INT(5),
  `cmpd_password` varchar(128),
	`cmpd_device` VARCHAR(64),
	`cmpd_switch` VARCHAR(64)
);

CREATE TABLE `c_mpd_websocket` (
  `cmw_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `cmpd_id` INT(11) NOT NULL,
  `cmw_name` VARCHAR(32) NOT NULL,
	`cmw_creation` TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  CONSTRAINT `service_mpd_ibfk_1` FOREIGN KEY (`cmpd_id`) REFERENCES `c_service_mpd` (`cmpd_id`)
);
CREATE INDEX `cmw_name_idx` ON `c_mpd_websocket` (`cmw_name`);
