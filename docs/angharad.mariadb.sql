-- Create database and user
-- CREATE DATABASE `angharad_dev`;
-- GRANT ALL PRIVILEGES ON angharad_dev.* TO 'angharad'@'%' identified BY 'angharad';
-- FLUSH PRIVILEGES;
-- USE `benoic_dev`;

DROP TABLE IF EXISTS `c_mpd_websocket`;
DROP TABLE IF EXISTS `c_service_mpd`;

DROP TABLE IF EXISTS `c_service_liquidsoap`;

DROP TABLE IF EXISTS `c_service_motion_file_list`;
DROP TABLE IF EXISTS `c_service_motion_stream`;
DROP TABLE IF EXISTS `c_service_motion`;

DROP TABLE IF EXISTS `c_mock_service`;

DROP TABLE IF EXISTS `b_monitor`;
DROP TABLE IF EXISTS `b_element`;
DROP TABLE IF EXISTS `b_device`;
DROP TABLE IF EXISTS `b_device_type`;

DROP TABLE IF EXISTS `c_element`;
DROP TABLE IF EXISTS `c_service`;

DROP TABLE IF EXISTS `g_message`;
DROP TABLE IF EXISTS `g_filter_alert`;
DROP TABLE IF EXISTS `g_filter_clause`;
DROP TABLE IF EXISTS `g_filter`;
DROP TABLE IF EXISTS `g_alert_smtp`;
DROP TABLE IF EXISTS `g_alert_http_header`;
DROP TABLE IF EXISTS `g_alert_http`;

DROP TABLE IF EXISTS `a_scheduler_script`;
DROP TABLE IF EXISTS `a_trigger_script`;
DROP TABLE IF EXISTS `a_trigger`;
DROP TABLE IF EXISTS `a_scheduler`;
DROP TABLE IF EXISTS `a_script`;
DROP TABLE IF EXISTS `a_submodule`;
DROP TABLE IF EXISTS `a_profile`;

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
CREATE INDEX `bm_date_idx` ON `b_monitor` (`bm_date`);

CREATE TABLE `c_service` (
  `cs_name` varchar(64) PRIMARY KEY,
  `cs_enabled` tinyint(1) DEFAULT 0,
  `cs_description` varchar(512)
);

CREATE TABLE `c_element` (
  `ce_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `cs_name` VARCHAR(64),
  `ce_name` VARCHAR(64),
  `ce_tag` blob,
  CONSTRAINT `service_ibfk_1` FOREIGN KEY (`cs_name`) REFERENCES `c_service` (`cs_name`)
);

CREATE TABLE `g_alert_http` (
  `ah_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `ah_name` VARCHAR(64) NOT NULL UNIQUE,
  `ah_method` VARCHAR(16),
  `ah_url` VARCHAR(128) NOT NULL,
  `ah_body` VARCHAR(512)
);

CREATE TABLE `g_alert_http_header` (
  `ahh_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `ah_id` INT(11) NOT NULL,
  `ahh_key` VARCHAR(64) NOT NULL,
  `ahh_value` VARCHAR(128) DEFAULT NULL,
  KEY `ah_id` (`ah_id`),
  CONSTRAINT `alert_http_header_ibfk_1` FOREIGN KEY (`ah_id`) REFERENCES `g_alert_http` (`ah_id`) ON DELETE CASCADE
);

CREATE TABLE `g_alert_smtp` (
  `as_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
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
  `as_body` VARCHAR(512) NOT NULL
);

CREATE TABLE `g_filter` (
  `f_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `f_name` VARCHAR(64) NOT NULL UNIQUE,
  `f_description` VARCHAR(128)
);

CREATE TABLE `g_filter_clause` (
  `fc_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `f_id` INT(11) NOT NULL,
  `fc_element` INT(2) NOT NULL,
  `fc_operator` INT(1) DEFAULT 0,
  `fc_value` VARCHAR(128),
  CONSTRAINT `filter_condition_ibfk_1` FOREIGN KEY (`f_id`) REFERENCES `g_filter` (`f_id`) ON DELETE CASCADE
);

CREATE TABLE `g_filter_alert` (
  `fa_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `f_id` INT(11) NOT NULL,
  `ah_name` VARCHAR(64) NULL,
  `as_name` VARCHAR(64) NULL,
  CONSTRAINT `filter_alert_ibfk_1` FOREIGN KEY (`f_id`) REFERENCES `g_filter` (`f_id`) ON DELETE CASCADE,
  CONSTRAINT `filter_alert_ibfk_2` FOREIGN KEY (`ah_name`) REFERENCES `g_alert_http` (`ah_name`) ON DELETE CASCADE,
  CONSTRAINT `filter_alert_ibfk_3` FOREIGN KEY (`as_name`) REFERENCES `g_alert_smtp` (`as_name`) ON DELETE CASCADE
);

CREATE TABLE `g_message` (
  `m_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `m_date` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `m_priority` INT(11) NOT NULL,
  `m_source` VARCHAR(64) NOT NULL,
  `m_text` VARCHAR(256) NOT NULL,
  `m_tags` VARCHAR(576)
);

CREATE TABLE `a_submodule` (
  `as_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `as_name` VARCHAR(64) NOT NULL UNIQUE,
  `as_description` VARCHAR(128),
  `as_enabled` TINYINT(1) default 1
);
INSERT INTO `a_submodule` (`as_name`, `as_description`, `as_enabled`) VALUES ('benoic', 'House automation devices management', 1);
INSERT INTO `a_submodule` (`as_name`, `as_description`, `as_enabled`) VALUES ('carleon', 'House automation side services management', 1);
INSERT INTO `a_submodule` (`as_name`, `as_description`, `as_enabled`) VALUES ('gareth', 'Messenger service', 1);

CREATE TABLE `a_script` (
  `asc_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `asc_name` VARCHAR(64) NOT NULL UNIQUE,
  `asc_description` VARCHAR(128),
  `asc_actions` BLOB,
  `asc_options` BLOB
);

CREATE TABLE `a_scheduler` (
  `ash_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `ash_name` VARCHAR(64) NOT NULL UNIQUE,
  `ash_description` VARCHAR(128),
  `ash_enabled` TINYINT(1) DEFAULT 1,
  `ash_options` BLOB,
  `ash_conditions` BLOB,
  `ash_next_time` TIMESTAMP,
  `ash_repeat` TINYINT(1) DEFAULT -1, -- -1: None, 0: minute, 1: hours, 2: days, 3: day of week, 4: month, 5: year
  `ash_repeat_value` INT(11),
  `ash_remove_after` INT(11) DEFAULT 0
);

CREATE TABLE `a_trigger` (
  `at_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `at_name` VARCHAR(64) NOT NULL UNIQUE,
  `at_description` VARCHAR(128),
  `at_enabled` TINYINT(1) DEFAULT 1,
  `at_options` BLOB,
  `at_conditions` BLOB,
  `at_submodule` VARCHAR(64) NOT NULL,
  `at_source` VARCHAR(64) NOT NULL,
  `at_element` VARCHAR(64) NOT NULL,
  `at_message` VARCHAR(128),
  `at_message_match` TINYINT(1) DEFAULT 0 -- 0 none, 1 equal, 2 different, 3 contains, 4 not contains, 5 empty, 6 not empty
);

CREATE TABLE `a_scheduler_script` (
  `ass_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `ash_id` INT(11),
  `asc_id` INT(11),
  `ass_enabled` TINYINT(1) DEFAULT 1,
  CONSTRAINT `scheduler_id_ibfk_1` FOREIGN KEY (`ash_id`) REFERENCES `a_scheduler` (`ash_id`) ON DELETE CASCADE,
  CONSTRAINT `script_id_ibfk_1` FOREIGN KEY (`asc_id`) REFERENCES `a_script` (`asc_id`) ON DELETE CASCADE
);

CREATE TABLE `a_trigger_script` (
  `ats_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `at_id` INT(11),
  `asc_id` INT(11),
  `ats_enabled` TINYINT(1) DEFAULT 1,
  CONSTRAINT `trigger_id_ibfk_1` FOREIGN KEY (`at_id`) REFERENCES `a_trigger` (`at_id`) ON DELETE CASCADE,
  CONSTRAINT `script_id_ibfk_2` FOREIGN KEY (`asc_id`) REFERENCES `a_script` (`asc_id`) ON DELETE CASCADE
);

CREATE TABLE `a_profile` (
  `ap_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `ap_name` VARCHAR(64) NOT NULL UNIQUE,
  `ap_data` BLOB -- profile data, json in a blob for old mysql versions compatibility
);

CREATE TABLE `c_service_liquidsoap` (
  `csl_name` varchar(64) NOT NULL UNIQUE,
  `csl_description` varchar(128),
  `csl_radio_url` varchar(512) NOT NULL,
  `csl_radio_type` varchar(64),
  `csl_control_enabled` tinyint(1) DEFAULT 0,
  `csl_control_host` varchar(128),
  `csl_control_port` int(11),
  `csl_control_request_name` varchar(64)
);

CREATE TABLE `c_mock_service` (
  `cms_name` varchar(64) NOT NULL UNIQUE,
  `cms_description` varchar(128)
);

CREATE TABLE `c_service_motion` (
  `csm_name` varchar(64) PRIMARY KEY,
  `csm_description` varchar(128),
  `csm_config_uri` varchar(512)
);

CREATE TABLE `c_service_motion_file_list` (
  `csmfl_name` varchar(64),
  `csm_name` varchar(64) NOT NULL,
  `csmfl_path` varchar(512) NOT NULL,
  `csmfl_thumbnail_path` varchar(512) NOT NULL,
  CONSTRAINT `service_motion_ibfk_1` FOREIGN KEY (`csm_name`) REFERENCES `c_service_motion` (`csm_name`) ON DELETE CASCADE
);

CREATE TABLE `c_service_motion_stream` (
  `csms_name` varchar(64),
  `csm_name` varchar(64) NOT NULL,
  `csms_uri` varchar(512) NOT NULL,
  `csms_snapshot_uri` varchar(512),
  CONSTRAINT `service_motion_ibfk_2` FOREIGN KEY (`csm_name`) REFERENCES `c_service_motion` (`csm_name`) ON DELETE CASCADE
);

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
