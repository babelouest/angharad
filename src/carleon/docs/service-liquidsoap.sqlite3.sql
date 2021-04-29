DROP TABLE IF EXISTS `c_service_liquidsoap`;

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
