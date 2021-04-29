-- Create database and user
-- CREATE DATABASE `gareth_dev`;
-- GRANT ALL PRIVILEGES ON gareth_dev.* TO 'gareth'@'%' identified BY 'gareth';
-- FLUSH PRIVILEGES;
-- USE `gareth_dev`;

DROP TABLE IF EXISTS `g_message`;
DROP TABLE IF EXISTS `g_filter_alert`;
DROP TABLE IF EXISTS `g_filter_clause`;
DROP TABLE IF EXISTS `g_filter`;
DROP TABLE IF EXISTS `g_alert_smtp`;
DROP TABLE IF EXISTS `g_alert_http_header`;
DROP TABLE IF EXISTS `g_alert_http`;

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
  `f_id` INT(11),
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
