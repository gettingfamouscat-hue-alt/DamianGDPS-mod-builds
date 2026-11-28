# Damian Auto Update

Adds **Auto Update** to the main menu. A click reinstalls the latest Damian GDPS
pack and its required dependencies, then uses Geode's save-and-restart function.
The updater itself is part of the pack, so future releases update it too.

The updater reads the latest release from
`gettingfamouscat-hue-alt/DamianGDPS-mod-builds`. It verifies GitHub's asset SHA256,
the release manifest, each package hash, the game/loader target, native module,
and required dependency constraints before replacing packages. Missing required
dependencies come from the official Geode index and are checked against its
published hashes. Optional dependencies are left alone.

Settings, account data, levels and platformer saves are preserved. Old packages
are copied to `geode/mod-update-backups` in Geode's save folder. Renamed duplicate
packages for updated mod IDs are backed up and removed after installation.
Handled installation failures restore the previous files. Cancel is available
while downloading; installation and the automatic restart finish once started.

An update also checks dependency constraints and breaking incompatibilities of
other enabled mods. It stops instead of installing a conflicting combination.
Installed packages outside Geode's default Mods folder must be moved there first.

`tests/transaction.cpp` tests reinstalling, dependency installation, duplicate
cleanup, backups, a failure after partially installing, path validation, and
preservation of unrelated mods and save files. It runs on Windows locally and
on the Mac/Linux build runners.
