-- Create database and user
-- CREATE DATABASE `angharad_dev`;
-- GRANT ALL PRIVILEGES ON angharad_dev.* TO 'angharad'@'%' identified BY 'angharad';
-- FLUSH PRIVILEGES;
-- USE `benoic_dev`;

DROP TABLE IF EXISTS `b_monitor`;
DROP TABLE IF EXISTS `b_element`;
DROP TABLE IF EXISTS `b_device`;
DROP TABLE IF EXISTS `b_device_type`;

DROP TABLE IF EXISTS `c_profile`;
DROP TABLE IF EXISTS `c_element`;
DROP TABLE IF EXISTS `c_service`;

DROP TABLE IF EXISTS `g_message`;
DROP TABLE IF EXISTS `g_filter_alert`;
DROP TABLE IF EXISTS `g_filter_clause`;
DROP TABLE IF EXISTS `g_filter`;
DROP TABLE IF EXISTS `g_alert_smtp`;
DROP TABLE IF EXISTS `g_alert_http_header`;
DROP TABLE IF EXISTS `g_alert_http`;

DROP TABLE IF EXISTS `a_user`;
DROP TABLE IF EXISTS `a_session`;
DROP TABLE IF EXISTS `a_scheduler_script`;
DROP TABLE IF EXISTS `a_trigger_script`;
DROP TABLE IF EXISTS `a_trigger`;
DROP TABLE IF EXISTS `a_scheduler`;
DROP TABLE IF EXISTS `a_script`;
DROP TABLE IF EXISTS `a_submodule`;

CREATE TABLE `b_device_type` (
  `bdt_uid` VARCHAR(64) PRIMARY KEY NOT NULL,
  `bdt_enabled` TINYINT(1) DEFAULT 0,
  `bdt_name` VARCHAR(64) NOT NULL UNIQUE,
  `bdt_description` VARCHAR(512),
  `bdt_options` BLOB
);

CREATE TABLE `b_device` (
  `bd_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `bd_name` VARCHAR(64) NOT NULL UNIQUE,
  `bd_description` VARCHAR(512),
  `bd_enabled` TINYINT(1) DEFAULT 1,
  `bd_connected` TINYINT(1) DEFAULT 0,
  `bdt_uid` VARCHAR(64),
  `bd_options` BLOB,
  `bd_last_seen` TIMESTAMP,
  CONSTRAINT `device_type_ibfk_1` FOREIGN KEY (`bdt_uid`) REFERENCES `b_device_type` (`bdt_uid`)
);

CREATE TABLE `b_element` (
  `be_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `bd_name` VARCHAR(64),
  `be_name` VARCHAR(64) NOT NULL,
  `be_display` VARCHAR(128),
  `be_type` TINYINT(1), -- 0 sensor, 1 switch, 2 dimmer, 3 heater
  `be_description` VARCHAR(128),
  `be_enabled` TINYINT(1) DEFAULT 1,
  `be_options` BLOB,
  `be_monitored` TINYINT(1) DEFAULT 0,
  `be_monitored_every` INT(11) DEFAULT 0,
  `be_monitored_next` TIMESTAMP,
  CONSTRAINT `device_ibfk_1` FOREIGN KEY (`bd_name`) REFERENCES `b_device` (`bd_name`) ON DELETE CASCADE
);

CREATE TABLE `b_monitor` (
  `bm_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `be_id` INT(11),
  `bm_date` TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  `bm_value` VARCHAR(16)
);

CREATE TABLE `c_service` (
  `cs_name` varchar(64) PRIMARY KEY,
  `cs_enabled` tinyint(1) DEFAULT 0,
  `cs_description` varchar(512)
);

CREATE TABLE `c_element` (
  `cs_name` VARCHAR(64),
  `ce_name` VARCHAR(64),
  `ce_tag` blob,
  CONSTRAINT `service_ibfk_1` FOREIGN KEY (`cs_name`) REFERENCES `c_service` (`cs_name`)
);

CREATE TABLE `c_profile` (
  `cp_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `cp_name` VARCHAR(64) NOT NULL UNIQUE,
  `cp_data` BLOB -- profile data, json in a blob for old mysql versions compatibility
);

CREATE TABLE `g_alert_http` (
  `ah_id` INT(11) NOT NULL AUTO_INCREMENT,
  `ah_name` VARCHAR(64) NOT NULL UNIQUE,
  `ah_method` VARCHAR(16),
  `ah_url` VARCHAR(128) NOT NULL,
  `ah_body` VARCHAR(512),
  PRIMARY KEY (`ah_id`)
);

CREATE TABLE `g_alert_http_header` (
  `ahh_id` INT(11) NOT NULL AUTO_INCREMENT,
  `ah_id` INT(11) NOT NULL,
  `ahh_key` VARCHAR(64) NOT NULL,
  `ahh_value` VARCHAR(128) DEFAULT NULL,
  PRIMARY KEY (`ahh_id`),
  KEY `ah_id` (`ah_id`),
  CONSTRAINT `alert_http_header_ibfk_1` FOREIGN KEY (`ah_id`) REFERENCES `g_alert_http` (`ah_id`) ON DELETE CASCADE
);

CREATE TABLE `g_alert_smtp` (
  `as_id` INT(11) NOT NULL AUTO_INCREMENT,
  `as_name` VARCHAR(64) NOT NULL UNIQUE,
  `as_host` VARCHAR(128) NOT NULL,
  `as_port` INT(5) DEFAULT 0 NOT NULL,
  `as_tls` INT(1) DEFAULT 0 NOT NULL,
  `as_check_ca` INT(1) DEFAULT 1 NOT NULL,
  `as_user` VARCHAR(128) DEFAULT NULL,
  `as_password` VARCHAR(128) DEFAULT NULL,
  `as_from` VARCHAR(128) NOT NULL,
  `as_to` VARCHAR(128) NOT NULL,
  `as_cc` VARCHAR(128) DEFAULT NULL,
  `as_bcc` VARCHAR(128) DEFAULT NULL,
  `as_subject` VARCHAR(128) NOT NULL,
  `as_body` VARCHAR(512) NOT NULL,
  PRIMARY KEY (`as_id`)
);

CREATE TABLE `g_filter` (
  `f_id` INT(11) NOT NULL AUTO_INCREMENT,
  `f_name` VARCHAR(64) NOT NULL UNIQUE,
  `f_description` VARCHAR(128),
  PRIMARY KEY (`f_id`)
);

CREATE TABLE `g_filter_clause` (
  `fc_id` INT(11) NOT NULL AUTO_INCREMENT,
  `f_id` INT(11) NOT NULL,
  `fc_element` INT(2) NOT NULL,
  `fc_operator` INT(1) DEFAULT 0,
  `fc_value` VARCHAR(128),
  PRIMARY KEY (`fc_id`),
  CONSTRAINT `filter_condition_ibfk_1` FOREIGN KEY (`f_id`) REFERENCES `g_filter` (`f_id`) ON DELETE CASCADE
);

CREATE TABLE `g_filter_alert` (
  `f_id` INT(11) NOT NULL,
  `ah_name` VARCHAR(64) NULL,
  `as_name` VARCHAR(64) NULL,
  CONSTRAINT `filter_alert_ibfk_1` FOREIGN KEY (`f_id`) REFERENCES `g_filter` (`f_id`) ON DELETE CASCADE,
  CONSTRAINT `filter_alert_ibfk_2` FOREIGN KEY (`ah_name`) REFERENCES `g_alert_http` (`ah_name`) ON DELETE CASCADE,
  CONSTRAINT `filter_alert_ibfk_3` FOREIGN KEY (`as_name`) REFERENCES `g_alert_smtp` (`as_name`) ON DELETE CASCADE
);

CREATE TABLE `g_message` (
  `m_id` INT(11) NOT NULL AUTO_INCREMENT,
  `m_date` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `m_priority` INT(11) NOT NULL,
  `m_source` VARCHAR(64) NOT NULL,
  `m_text` VARCHAR(256) NOT NULL,
  `m_tags` VARCHAR(576),
  PRIMARY KEY (`m_id`)
);

CREATE TABLE `a_submodule` (
  `as_name` VARCHAR(64) NOT NULL UNIQUE,
  `as_description` VARCHAR(128),
  `as_enabled` TINYINT(1) default 1
);
INSERT INTO `a_submodule` (`as_name`, `as_description`, `as_enabled`) VALUES ('benoic', 'House automation devices management', 1);
INSERT INTO `a_submodule` (`as_name`, `as_description`, `as_enabled`) VALUES ('carleon', 'House automation side services management', 1);
INSERT INTO `a_submodule` (`as_name`, `as_description`, `as_enabled`) VALUES ('gareth', 'Messenger service', 1);

CREATE TABLE `a_script` (
  `asc_id` INT(11) NOT NULL AUTO_INCREMENT,
  `asc_name` VARCHAR(64) NOT NULL UNIQUE,
  `asc_description` VARCHAR(128),
  `asc_actions` BLOB,
  `asc_options` BLOB,
  PRIMARY KEY (`asc_id`)
);

CREATE TABLE `a_scheduler` (
  `ash_id` INT(11) NOT NULL AUTO_INCREMENT,
  `ash_name` VARCHAR(64) NOT NULL UNIQUE,
  `ash_description` VARCHAR(128),
  `ash_enabled` TINYINT(1) DEFAULT 1,
  `ash_options` BLOB,
  `ash_conditions` BLOB,
  `ash_next_time` TIMESTAMP,
  `ash_repeat` TINYINT(1) DEFAULT -1, -- -1: None, 0: minute, 1: hours, 2: days, 3: day of week, 4: month, 5: year
  `ash_repeat_value` INT(11),
  `ash_remove_after` INT(11) DEFAULT 0,
  PRIMARY KEY (`ash_id`)
);

CREATE TABLE `a_trigger` (
  `at_id` INT(11) NOT NULL AUTO_INCREMENT,
  `at_name` VARCHAR(64) NOT NULL UNIQUE,
  `at_description` VARCHAR(128),
  `at_enabled` TINYINT(1) DEFAULT 1,
  `at_options` BLOB,
  `at_conditions` BLOB,
  `at_submodule` VARCHAR(64) NOT NULL,
  `at_source` VARCHAR(64) NOT NULL,
  `at_element` VARCHAR(64) NOT NULL,
  `at_message` VARCHAR(128),
  `at_message_match` TINYINT(1) DEFAULT 0, -- 0 none, 1 equal, 2 different, 3 contains, 4 not contains, 5 empty, 6 not empty
  PRIMARY KEY (`at_id`)
);

CREATE TABLE `a_scheduler_script` (
  `ash_id` INT(11),
  `asc_id` INT(11),
  `ass_enabled` TINYINT(1) DEFAULT 1,
  CONSTRAINT `scheduler_id_ibfk_1` FOREIGN KEY (`ash_id`) REFERENCES `a_scheduler` (`ash_id`) ON DELETE CASCADE,
  CONSTRAINT `script_id_ibfk_1` FOREIGN KEY (`asc_id`) REFERENCES `a_script` (`asc_id`) ON DELETE CASCADE
);

CREATE TABLE `a_trigger_script` (
  `at_id` INT(11),
  `asc_id` INT(11),
  `ats_enabled` TINYINT(1) DEFAULT 1,
  CONSTRAINT `trigger_id_ibfk_1` FOREIGN KEY (`at_id`) REFERENCES `a_trigger` (`at_id`) ON DELETE CASCADE,
  CONSTRAINT `script_id_ibfk_2` FOREIGN KEY (`asc_id`) REFERENCES `a_script` (`asc_id`) ON DELETE CASCADE
);

CREATE TABLE `a_session` (
  `ass_session_token` VARCHAR(128) NOT NULL UNIQUE,
  `ass_enabled` TINYINT(1) DEFAULT 1,
  `ass_login` VARCHAR(64),
  `ass_validity` TIMESTAMP,
  `ass_lastseen` TIMESTAMP
);
CREATE INDEX `session_uuid_idx` ON `a_session` (`ass_session_token`);

CREATE TABLE `a_user` (
  `au_login` VARCHAR(128) NOT NULL UNIQUE,
  `au_password` VARCHAR(128) NOT NULL,
  `au_enabled` TINYINT(1) DEFAULT 1,
  PRIMARY KEY (`au_login`)
);
