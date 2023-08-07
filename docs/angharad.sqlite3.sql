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
  `bdt_uid` TEXT PRIMARY KEY NOT NULL,
  `bdt_enabled` INTEGER DEFAULT 0,
  `bdt_name` TEXT NOT NULL UNIQUE,
  `bdt_description` TEXT,
  `bdt_options` TEXT
);

CREATE TABLE `b_device` (
  `bd_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `bd_name` TEXT NOT NULL UNIQUE,
  `bd_display` TEXT,
  `bd_description` TEXT,
  `bd_enabled` INTEGER DEFAULT 1,
  `bd_connected` INTEGER DEFAULT 0,
  `bdt_uid` TEXT REFERENCES `b_device_type`(`bdt_uid`),
  `bd_options` BLOB,
  `bd_last_seen` timestamp
);

CREATE TABLE `b_element` (
  `be_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `bd_name` TEXT REFERENCES `b_device`(`bd_name`) ON DELETE CASCADE,
  `be_name` TEXT NOT NULL,
  `be_display` TEXT,
  `be_type` INTEGER, -- 0 sensor, 1 switch, 2 dimmer, 3 heater
  `be_description` TEXT,
  `be_enabled` INTEGER DEFAULT 1,
  `be_options` TEXT,
  `be_monitored` INTEGER DEFAULT 0,
  `be_monitored_every` INTEGER DEFAULT 0,
  `be_monitored_next` INTEGER DEFAULT 0,
);

CREATE TABLE `b_monitor` (
  `bm_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `be_id` INTEGER,
  `bm_date` INT DEFAULT (strftime('%s', 'now')),
  `bm_value` TEXT
);

CREATE TABLE `c_service` (
  `cs_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `cs_name` TEXT,
  `cs_enabled` INTEGER DEFAULT 0,
  `cs_description` TEXT
);

CREATE TABLE `c_element` (
  `ce_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `cs_name` TEXT,
  `ce_name` TEXT,
  `ce_tag` TEXT
);

CREATE TABLE `g_alert_http` (
  `ah_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `ah_name` TEXT UNIQUE,
  `ah_method` TEXT,
  `ah_url` TEXT,
  `ah_body` TEXT
);

CREATE TABLE `g_alert_http_header` (
  `ahh_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `ah_id` INTEGER,
  `ahh_key` TEXT,
  `ahh_value` TEXT DEFAULT NULL,
  FOREIGN KEY(`ah_id`) REFERENCES `g_alert_http`(`ah_id`) ON DELETE CASCADE
);

CREATE TABLE `g_alert_smtp` (
  `as_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `as_name` TEXT UNIQUE,
  `as_host` TEXT,
  `as_port` INTEGER DEFAULT 0,
  `as_tls` INTEGER DEFAULT 0,
  `as_check_ca` INTEGER DEFAULT 1,
  `as_user` TEXT DEFAULT NULL,
  `as_password` TEXT DEFAULT NULL,
  `as_from` TEXT,
  `as_to` TEXT,
  `as_cc` TEXT DEFAULT NULL,
  `as_bcc` TEXT DEFAULT NULL,
  `as_subject` TEXT,
  `as_body` TEXT
);

CREATE TABLE `g_filter` (
  `f_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `f_name` TEXT UNIQUE,
  `f_description` TEXT
);

CREATE TABLE `g_filter_clause` (
  `fc_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `f_id` INTEGER,
  `fc_element` int(2),
  `fc_operator` INTEGER DEFAULT 0,
  `fc_value` TEXT,
  FOREIGN KEY(`f_id`) REFERENCES `g_filter`(`f_id`) ON DELETE CASCADE
);

CREATE TABLE `g_filter_alert` (
  `fa_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `f_id` INTEGER,
  `ah_name` TEXT NULL,
  `as_name` TEXT NULL,
  FOREIGN KEY(f_id) REFERENCES g_filter(f_id) ON DELETE CASCADE,
  FOREIGN KEY(`ah_name`) REFERENCES `g_alert_http`(`ah_name`) ON DELETE CASCADE,
  FOREIGN KEY(`as_name`) REFERENCES `g_alert_smtp`(`as_name`) ON DELETE CASCADE
);

CREATE TABLE `g_message` (
  `m_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `m_date` INT DEFAULT CURRENT_TIMESTAMP,
  `m_priority` INTEGER,
  `m_source` TEXT,
  `m_text` TEXT,
  `m_tags` TEXT
);
CREATE INDEX `i_message_date` ON `g_message`(`m_date`);

CREATE TABLE `a_submodule` (
  `as_name` TEXT PRIMARY KEY NOT NULL,
  `as_description` TEXT,
  `as_enabled` INTEGER default 1
);
INSERT INTO `a_submodule` (`as_name`, `as_description`, `as_enabled`) VALUES ('benoic', 'House automation devices management', 1);
INSERT INTO `a_submodule` (`as_name`, `as_description`, `as_enabled`) VALUES ('carleon', 'House automation side services management', 1);
INSERT INTO `a_submodule` (`as_name`, `as_description`, `as_enabled`) VALUES ('gareth', 'Messenger service', 1);

CREATE TABLE `a_script` (
  `asc_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `asc_name` TEXT NOT NULL UNIQUE,
  `asc_description` TEXT,
  `asc_actions` TEXT,
  `asc_options` TEXT
);

CREATE TABLE `a_scheduler` (
  `ash_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `ash_name` TEXT NOT NULL UNIQUE,
  `ash_description` TEXT,
  `ash_enabled` INTEGER DEFAULT 1,
  `ash_options` TEXT,
  `ash_conditions` TEXT,
  `ash_next_time` INTEGER,
  `ash_repeat` INTEGER DEFAULT -1, -- -1: None, 0: minute, 1: hours, 2: days, 3: day of week, 4: month, 5: year
  `ash_repeat_value` INTEGER,
  `ash_remove_after` INTEGER DEFAULT 0
);

CREATE TABLE `a_trigger` (
  `at_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `at_name` TEXT NOT NULL UNIQUE,
  `at_description` TEXT,
  `at_enabled` INTEGER DEFAULT 1,
  `at_options` TEXT,
  `at_conditions` TEXT,
  `at_submodule` TEXT NOT NULL,
  `at_source` TEXT NOT NULL,
  `at_element` TEXT NOT NULL,
  `at_message` TEXT,
  `at_message_match` INTEGER DEFAULT 0 -- 0 none, 1 equal, 2 different, 3 contains, 4 not contains, 5 empty, 6 not empty
);

CREATE TABLE `a_scheduler_script` (
  `ass_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `ash_id` INTEGER,
  `asc_id` INTEGER,
  `ass_enabled` INTEGER DEFAULT 1,
  CONSTRAINT `scheduler_id_ibfk_1` FOREIGN KEY (`ash_id`) REFERENCES `a_scheduler` (`ash_id`) ON DELETE CASCADE,
  CONSTRAINT `script_id_ibfk_1` FOREIGN KEY (`asc_id`) REFERENCES `a_script` (`asc_id`) ON DELETE CASCADE
);

CREATE TABLE `a_trigger_script` (
  `ats_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `at_id` INTEGER,
  `asc_id` INTEGER,
  `ats_enabled` INTEGER DEFAULT 1,
  CONSTRAINT `trigger_id_ibfk_1` FOREIGN KEY (`at_id`) REFERENCES `a_trigger` (`at_id`) ON DELETE CASCADE,
  CONSTRAINT `script_id_ibfk_2` FOREIGN KEY (`asc_id`) REFERENCES `a_script` (`asc_id`) ON DELETE CASCADE
);

CREATE TABLE `a_profile` (
  `ap_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `ap_name` TEXT NOT NULL UNIQUE,
  `ap_data` TEXT
);

CREATE TABLE `c_mock_service` (
  `cms_name` TEXT PRIMARY KEY NOT NULL,
  `cms_description` TEXT
);

CREATE TABLE `c_service_liquidsoap` (
  `csl_name` TEXT PRIMARY KEY NOT NULL,
  `csl_description` TEXT,
  `csl_radio_url` TEXT NOT NULL,
  `csl_radio_type` TEXT,
  `csl_control_enabled` INTEGER DEFAULT 0,
  `csl_control_host` TEXT,
  `csl_control_port` INTEGER,
  `csl_control_request_name` TEXT
);

CREATE TABLE `c_service_motion` (
  `csm_name` TEXT PRIMARY KEY NOT NULL,
  `csm_description` varchar(128),
  `csm_config_uri` varchar(512)
);

CREATE TABLE `c_service_motion_file_list` (
  `csmfl_name` TEXT NOT NULL,
  `csm_name` TEXT REFERENCES `c_service_motion`(`csm_name`) ON DELETE CASCADE,
  `csmfl_path` TEXT NOT NULL,
  `csmfl_thumbnail_path` TEXT NOT NULL
);

CREATE TABLE `c_service_motion_stream` (
  `csms_name` TEXT NOT NULL,
  `csm_name` TEXT REFERENCES `c_service_motion`(`csm_name`) ON DELETE CASCADE,
  `csms_uri` TEXT NOT NULL,
  `csms_snapshot_uri` TEXT
);

CREATE TABLE `c_service_mpd` (
  `cmpd_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `cmpd_name` TEXT NOT NULL,
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
  CONSTRAINT `service_mpd_ibfk_1` FOREIGN KEY (`cmpd_id`) REFERENCES `c_service_mpd` (`cmpd_id`) ON DELETE CASCADE
);
CREATE INDEX `cmw_name_idx` ON `c_mpd_websocket` (`cmw_name`);
