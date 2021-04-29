-- Create database and user
-- CREATE DATABASE `carleon_dev`;
-- GRANT ALL PRIVILEGES ON carleon_dev.* TO 'carleon'@'%' identified BY 'carleon';
-- FLUSH PRIVILEGES;
-- USE `carleon_dev`;

DROP TABLE IF EXISTS `c_element`;
DROP TABLE IF EXISTS `c_service`;

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
