Server changes for Damian GDPS (GD 2.2091)

These are updates to the existing Damian GDPS backend, not a standalone server.
Back up the database and corresponding files before deploying on another host.
Apply migrations/content_reports.sql with a schema administrator, then copy the
PHP files to their matching backend paths. Existing RoleManager, Collections,
GJPCheck, WebSecurity and normal GMDprivateServer tables are required.

Top Lists now shows public lists even when unrated. Ratings/reward fields are
unchanged; private lists remain hidden. The tool explains where saved lists go.

adminGetReportQueue.php authenticates staff and reads legacy and new reports.
reportGJContent.php accepts the server's existing content types 0-6, checks
ownership for private messages/requests, and deduplicates reports. No automated
flags or penalties are invented. Detailed account-review and moderation action
endpoints are not included in this update.

Checks use connection-local temporary tables and do not mutate real player data.
Local Damian GDPS has these files and the migration installed. Other hosts need
the same deployment; updating a Geode client cannot update their PHP database.
