DROP TABLE IF EXISTS `c_service_liquidsoap`;

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
