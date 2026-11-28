# GD 2.2091 / Geode 6 ports

This directory contains the custom Damian GDPS builds and the dependencies they need.
Fix 100% is intentionally excluded. Existing Geode 5 sources and releases remain separate.

## Build

Use Geode SDK 6.0.0, CLI 4.0.2, and the official 2.2091 bindings at
42c3eed43010fa531e9bc5d0aca6f3c3c4f49608. Configure this directory as one CMake
project so Platformer Saves links to the matching PersistenceAPI.
On Windows, use clang-cl 20 or newer (MSVC 14.44 has an internal compiler error
with Editor Tab API 3). Mac packages include Intel and Apple Silicon.

Editor Tab API v3.0.0-beta.5 and Node IDs v1.24.0 are upstream runtime dependencies.
The EditorTabAPI source folder supplies its public headers; the upstream package
is used at runtime. Original authors and licenses remain in each project.

## Compatibility changes

* Native PlayLayer initialization and creation now use the 2.2091 signatures.
* Share Level Settings and game saving use their current signatures.
* Server API rewrites network requests without reading old executable offsets.
* Custom editor tabs use Editor Tab API 3.
* Geode events, paths, web timeouts, and standard containers use the v6 interfaces.
* Pause-menu percentage formatting runs after customSetup because the previous
  setupProgressBars hook is now inline.
* Platformer snapshots serialize current native field names and the added state.
  New saves use PSF 0.1.2 in a gd-2.2091 subfolder. Old snapshots are preserved
  and cannot be loaded into the new game layout.
* The old didClick memory write is removed from load cancellation.

Compilation and package checks do not replace gameplay testing. In particular,
check platformer save/continue, normal level completion, golden coins, rate
controls, custom tabs, and a two-player Versus match after a game restart.
