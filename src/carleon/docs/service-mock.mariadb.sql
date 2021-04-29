DROP TABLE IF EXISTS `c_mock_service`;

CREATE TABLE `c_mock_service` (
  `cms_name` varchar(64) NOT NULL UNIQUE,
  `cms_description` varchar(128)
);
