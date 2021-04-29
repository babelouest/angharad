DROP TABLE IF EXISTS `c_service_motion_file_list`;
DROP TABLE IF EXISTS `c_service_motion_stream`;
DROP TABLE IF EXISTS `c_service_motion`;

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
