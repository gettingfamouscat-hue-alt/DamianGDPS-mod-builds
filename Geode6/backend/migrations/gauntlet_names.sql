CREATE TABLE IF NOT EXISTS gdps_gauntlet_names (
    gauntletID INT NOT NULL PRIMARY KEY,
    name VARCHAR(80) CHARACTER SET utf8mb4 NOT NULL,
    accountID INT NOT NULL,
    updatedAt INT NOT NULL
) ENGINE=InnoDB;
