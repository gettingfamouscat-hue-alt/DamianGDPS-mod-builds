CREATE TABLE IF NOT EXISTS content_reports (
    reportID BIGINT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    targetAccountID INT NOT NULL,
    reporterAccountID INT NOT NULL DEFAULT 0,
    reporterKey CHAR(64) CHARACTER SET ascii NOT NULL,
    kind TINYINT UNSIGNED NOT NULL,
    itemID INT NOT NULL,
    special INT NOT NULL DEFAULT 0,
    reason TINYINT UNSIGNED NOT NULL,
    createdAt INT NOT NULL,
    status TINYINT UNSIGNED NOT NULL DEFAULT 0,
    UNIQUE KEY reporter_item_reason (reporterKey,kind,itemID,reason),
    KEY open_target (status,targetAccountID),
    KEY reporter_time (reporterKey,createdAt)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
