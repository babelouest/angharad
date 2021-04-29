DROP TABLE IF EXISTS `c_service_motion_file_list`;
DROP TABLE IF EXISTS `c_service_motion_stream`;
DROP TABLE IF EXISTS `c_service_motion`;

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
