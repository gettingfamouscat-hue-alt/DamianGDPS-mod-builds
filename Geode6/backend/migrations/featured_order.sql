-- Run once with a database administrator before using Set Featured.
ALTER TABLE levels ADD COLUMN featuredOrder INT NOT NULL DEFAULT 0;
