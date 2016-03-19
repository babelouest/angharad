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

DROP TABLE IF EXISTS `a_event_trigger`;
DROP TABLE IF EXISTS `a_event_scheduler`;
DROP TABLE IF EXISTS `a_event`;
DROP TABLE IF EXISTS `a_trigger`;
DROP TABLE IF EXISTS `a_scheduler`;
DROP TABLE IF EXISTS `a_script`;
DROP TABLE IF EXISTS `a_session`;
DROP TABLE IF EXISTS `a_submodule`;

CREATE TABLE `b_device_type` (
  `bdt_uid` varchar(64) PRIMARY KEY NOT NULL,
  `bdt_enabled` tinyint(1) DEFAULT 0,
  `bdt_name` varchar(64) NOT NULL UNIQUE,
  `bdt_description` varchar(512),
  `bdt_options` BLOB
);

CREATE TABLE `b_device` (
  `bd_id` int(11) PRIMARY KEY AUTO_INCREMENT,
  `bd_name` varchar(64) NOT NULL UNIQUE,
  `bd_display` varchar(128),
  `bd_description` varchar(512),
  `bd_enabled` tinyint(1) DEFAULT 1,
  `bd_connected` tinyint(1) DEFAULT 0,
  `bdt_uid` varchar(64),
  `bd_options` BLOB,
  `bd_last_seen` timestamp,
  CONSTRAINT `device_type_ibfk_1` FOREIGN KEY (`bdt_uid`) REFERENCES `b_device_type` (`bdt_uid`)
);

CREATE TABLE `b_element` (
  `be_id` int(11) PRIMARY KEY AUTO_INCREMENT,
  `bd_name` varchar(64),
  `be_name` varchar(64) NOT NULL,
  `be_display` varchar(128),
  `be_type` tinyint(1), -- 0 sensor, 1 switch, 2 dimmer, 3 heater
  `be_description` varchar(128),
  `be_enabled` tinyint(1) DEFAULT 1,
  `be_options` BLOB,
  `be_monitored` tinyint(1) DEFAULT 0,
  `be_monitored_every` tinyint(1) DEFAULT 0,
  `be_monitored_next` timestamp,
  CONSTRAINT `device_ibfk_1` FOREIGN KEY (`bd_name`) REFERENCES `b_device` (`bd_name`) ON DELETE CASCADE
);

CREATE TABLE `b_monitor` (
  `bm_id` int(11) PRIMARY KEY AUTO_INCREMENT,
  `be_id` int(11),
  `bm_date` timestamp DEFAULT CURRENT_TIMESTAMP,
  `bm_value` varchar(16)
);

CREATE TABLE `c_service` (
  `cs_uid` varchar(64) PRIMARY KEY NOT NULL,
  `cs_enabled` tinyint(1) DEFAULT 0,
  `cs_name` varchar(64) NOT NULL UNIQUE,
  `cs_description` varchar(512)
);

CREATE TABLE `c_element` (
  `cs_uid` varchar(64),
  `ce_name` varchar(64),
  `ce_tag` blob,
  CONSTRAINT `service_ibfk_1` FOREIGN KEY (`cs_uid`) REFERENCES `c_service` (`cs_uid`)
);

CREATE TABLE `c_profile` (
  `cp_id` int(11) PRIMARY KEY AUTO_INCREMENT,
  `cp_name` varchar(64) NOT NULL UNIQUE,
  `cp_data` BLOB -- profile data, json in a blob for old mysql versions compatibility
);

CREATE TABLE `g_alert_http` (
  `ah_id` int(11) NOT NULL AUTO_INCREMENT,
  `ah_name` varchar(64) NOT NULL UNIQUE,
  `ah_method` varchar(16),
  `ah_url` varchar(128) NOT NULL,
  `ah_body` varchar(512),
  PRIMARY KEY (`ah_id`)
);

CREATE TABLE `g_alert_http_header` (
  `ahh_id` int(11) NOT NULL AUTO_INCREMENT,
  `ah_id` int(11) NOT NULL,
  `ahh_key` varchar(64) NOT NULL,
  `ahh_value` varchar(128) DEFAULT NULL,
  PRIMARY KEY (`ahh_id`),
  KEY `ah_id` (`ah_id`),
  CONSTRAINT `alert_http_header_ibfk_1` FOREIGN KEY (`ah_id`) REFERENCES `g_alert_http` (`ah_id`) ON DELETE CASCADE
);

CREATE TABLE `g_alert_smtp` (
  `as_id` int(11) NOT NULL AUTO_INCREMENT,
  `as_name` varchar(64) NOT NULL UNIQUE,
  `as_host` varchar(128) NOT NULL,
  `as_port` int(5) DEFAULT 0 NOT NULL,
  `as_tls` int(1) DEFAULT 0 NOT NULL,
  `as_check_ca` int(1) DEFAULT 1 NOT NULL,
  `as_user` varchar(128) DEFAULT NULL,
  `as_password` varchar(128) DEFAULT NULL,
  `as_from` varchar(128) NOT NULL,
  `as_to` varchar(128) NOT NULL,
  `as_cc` varchar(128) DEFAULT NULL,
  `as_bcc` varchar(128) DEFAULT NULL,
  `as_subject` varchar(128) NOT NULL,
  `as_body` varchar(512) NOT NULL,
  PRIMARY KEY (`as_id`)
);

CREATE TABLE `g_filter` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_name` varchar(64) NOT NULL UNIQUE,
  `f_description` varchar(128),
  PRIMARY KEY (`f_id`)
);

CREATE TABLE `g_filter_clause` (
  `fc_id` int(11) NOT NULL AUTO_INCREMENT,
  `f_id` int(11) NOT NULL,
  `fc_element` int(2) NOT NULL,
  `fc_operator` int(1) DEFAULT 0,
  `fc_value` varchar(128),
  PRIMARY KEY (`fc_id`),
  CONSTRAINT `filter_condition_ibfk_1` FOREIGN KEY (`f_id`) REFERENCES `g_filter` (`f_id`) ON DELETE CASCADE
);

CREATE TABLE `g_filter_alert` (
  `f_id` int(11) NOT NULL,
  `ah_name` varchar(64) NULL,
  `as_name` varchar(64) NULL,
  CONSTRAINT `filter_alert_ibfk_1` FOREIGN KEY (`f_id`) REFERENCES `g_filter` (`f_id`) ON DELETE CASCADE,
  CONSTRAINT `filter_alert_ibfk_2` FOREIGN KEY (`ah_name`) REFERENCES `g_alert_http` (`ah_name`) ON DELETE CASCADE,
  CONSTRAINT `filter_alert_ibfk_3` FOREIGN KEY (`as_name`) REFERENCES `g_alert_smtp` (`as_name`) ON DELETE CASCADE
);

CREATE TABLE `g_message` (
  `m_id` int(11) NOT NULL AUTO_INCREMENT,
  `m_date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `m_priority` int(11) NOT NULL,
  `m_source` varchar(64) NOT NULL,
  `m_text` varchar(256) NOT NULL,
  `m_tags` varchar(576),
  PRIMARY KEY (`m_id`)
);

CREATE TABLE `a_submodule` (
  `as_name` varchar(64) NOT NULL UNIQUE,
  `as_description` varchar(128),
  `as_enabled` tinyint(1) default 1
);
INSERT INTO `a_submodule` (`as_name`, `as_description`, `as_enabled`) VALUES ('benoic', 'House automation devices management', 1);
INSERT INTO `a_submodule` (`as_name`, `as_description`, `as_enabled`) VALUES ('carleon', 'House automation side services management', 1);
INSERT INTO `a_submodule` (`as_name`, `as_description`, `as_enabled`) VALUES ('gareth', 'Messenger service', 1);

CREATE TABLE `a_session` (
  `ass_session_token` varchar(128) NOT NULL UNIQUE,
  `ass_login` varchar(64) NOT NULL,
  `ass_validity` timestamp,
  `ass_security` tinyint(1) default 0 -- 0 public, 1 private
);

CREATE TABLE `a_script` (
  `asc_id` int(11) NOT NULL AUTO_INCREMENT,
  `asc_name` varchar(64) NOT NULL,
  `asc_description` varchar(128),
  `asc_actions` BLOB,
  `asc_options` BLOB,
  PRIMARY KEY (`asc_id`)
);

CREATE TABLE `a_scheduler` (
  `ash_id` int(11) NOT NULL AUTO_INCREMENT,
  `ash_name` varchar(64) NOT NULL,
  `ash_description` varchar(128),
  `ash_options` BLOB,
  `ash_next_time` timestamp,
  `ash_duration` INT(11),
  `ash_repeat_schedule` INT(11), -- -1: None, 0: minute, 1: hours, 2: days, 3: day of week, 4: month, 5: year
  `ash_repeat_schedule_value` INT(11),
  `ash_remove_after_done` INT(11),
  PRIMARY KEY (`ash_id`)
);

CREATE TABLE `a_trigger` (
  `at_id` int(11) NOT NULL AUTO_INCREMENT,
  `at_name` varchar(64) NOT NULL,
  `at_description` varchar(128),
  `at_submodule` varchar(64) NOT NULL,
  `at_source` varchar(64) NOT NULL,
  `at_element` varchar(64) NOT NULL,
  `at_message` varchar(128),
  PRIMARY KEY (`at_id`)
);

CREATE TABLE `a_event` (
  `ae_id` int(11) NOT NULL AUTO_INCREMENT,
  `ae_name` varchar(64) NOT NULL,
  `ae_description` varchar(128),
  `ae_enabled` tinyint(1) DEFAULT 1,
  `ae_options` BLOB,
  PRIMARY KEY (`ae_id`)
);

CREATE TABLE `a_event_scheduler` (
  `ae_id` int(11),
  `ash_id` int(11),
  CONSTRAINT `event_id_ibfk_1` FOREIGN KEY (`ae_id`) REFERENCES `a_event` (`ae_id`),
  CONSTRAINT `scheduler_id_ibfk_1` FOREIGN KEY (`ash_id`) REFERENCES `a_scheduler` (`ash_id`)
);

CREATE TABLE `a_event_trigger` (
  `ae_id` int(11),
  `at_id` int(11),
  CONSTRAINT `event_id_ibfk_2` FOREIGN KEY (`ae_id`) REFERENCES `a_event` (`ae_id`),
  CONSTRAINT `trigger_id_ibfk_1` FOREIGN KEY (`at_id`) REFERENCES `a_trigger` (`at_id`)
);
