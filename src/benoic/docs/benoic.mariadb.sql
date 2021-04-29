-- Create database and user
-- CREATE DATABASE `benoic_dev`;
-- GRANT ALL PRIVILEGES ON benoic_dev.* TO 'benoic'@'%' identified BY 'benoic';
-- FLUSH PRIVILEGES;
-- USE `benoic_dev`;

DROP TABLE IF EXISTS `b_monitor`;
DROP TABLE IF EXISTS `b_element`;
DROP TABLE IF EXISTS `b_device`;
DROP TABLE IF EXISTS `b_device_type`;

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
