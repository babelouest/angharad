
DROP TABLE IF EXISTS `b_monitor`;
DROP TABLE IF EXISTS `b_element`;
DROP TABLE IF EXISTS `b_device`;
DROP TABLE IF EXISTS `b_device_type`;

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
  `be_monitored_next` INTEGER DEFAULT 0
);

CREATE TABLE `b_monitor` (
  `bm_id` INTEGER PRIMARY KEY AUTOINCREMENT,
  `be_id` INTEGER,
  `bm_date` INT DEFAULT (strftime('%s', 'now')),
  `bm_value` TEXT
);
