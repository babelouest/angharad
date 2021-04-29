
DROP TABLE IF EXISTS `g_message`;
DROP TABLE IF EXISTS `g_filter_alert`;
DROP TABLE IF EXISTS `g_filter_clause`;
DROP TABLE IF EXISTS `g_filter`;
DROP TABLE IF EXISTS `g_alert_smtp`;
DROP TABLE IF EXISTS `g_alert_http_header`;
DROP TABLE IF EXISTS `g_alert_http`;

CREATE TABLE `g_alert_http` (
  `ah_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `ah_name` TEXT UNIQUE,
  `ah_method` TEXT,
  `ah_url` TEXT,
  `ah_body` TEXT
);
CREATE INDEX `i_alert_http` ON `g_alert_http`(`ah_id`);

CREATE TABLE `g_alert_http_header` (
  `ahh_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `ah_id` INTEGER,
  `ahh_key` TEXT,
  `ahh_value` TEXT DEFAULT NULL,
  FOREIGN KEY(`ah_id`) REFERENCES `g_alert_http`(`ah_id`)
);
CREATE INDEX `i_alert_http_header` ON `g_alert_http_header`(`ahh_id`);

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
CREATE INDEX `i_alert_smtp` ON `g_alert_smtp`(`as_id`);

CREATE TABLE `g_filter` (
  `f_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `f_name` TEXT UNIQUE,
  `f_description` TEXT
);
CREATE INDEX `i_filter` ON `g_filter`(`f_id`);

CREATE TABLE `g_filter_clause` (
  `fc_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `f_id` INTEGER,
  `fc_element` int(2),
  `fc_operator` INTEGER DEFAULT 0,
  `fc_value` TEXT,
  FOREIGN KEY(`f_id`) REFERENCES `g_filter`(`f_id`)
);
CREATE INDEX `i_filter_clause` ON `g_filter_clause`(`fc_id`);

CREATE TABLE `g_filter_alert` (
  `fa_id` INT(11) PRIMARY KEY AUTO_INCREMENT,
  `f_id` INTEGER,
  `ah_name` TEXT NULL,
  `as_name` TEXT NULL,
  FOREIGN KEY(f_id) REFERENCES g_filter(f_id),
  FOREIGN KEY(`ah_name`) REFERENCES `g_alert_http`(`ah_name`),
  FOREIGN KEY(`as_name`) REFERENCES `g_alert_smtp`(`as_name`)
);
CREATE INDEX `i_filter_alert` ON `g_filter_alert`(`f_id`);

CREATE TABLE `g_message` (
  `m_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `m_date` INT DEFAULT CURRENT_TIMESTAMP,
  `m_priority` INTEGER,
  `m_source` TEXT,
  `m_text` TEXT,
  `m_tags` TEXT
);
CREATE INDEX `i_message_id` ON `g_message`(`m_id`);
CREATE INDEX `i_message_date` ON `g_message`(`m_date`);
