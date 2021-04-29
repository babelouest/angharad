
DROP TABLE IF EXISTS `c_element`;
DROP TABLE IF EXISTS `c_service`;

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
