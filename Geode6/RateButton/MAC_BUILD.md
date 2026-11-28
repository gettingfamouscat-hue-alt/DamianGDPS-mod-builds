# Mac builds

This project now has a dedicated build repository:
https://github.com/gettingfamouscat-hue-alt/DamianGDPS-mod-builds

Run its **Build universal Mac GDPS mods** workflow. The two artifacts contain
native universal Intel/Apple Silicon libraries, not Windows DLL-only builds.
Install both Rate Button and Damian Versus in Geode; do not use tipp7 Versus.
GD 2.2081 and Geode 5.10.1 (or a compatible loader) are the declared targets.

PNG assets remain in `resources/m336.rate-button/` inside the package.
Custom-face rendering uses level flags and does not depend on binary URL
offsets. Other GDPS controls use Server API if available, or the explicit
**Custom GDPS compatibility** setting. No platform-specific memory offsets
are read for server detection.

For local builds, install the Geode CLI/SDK and CMake on macOS, set
`GEODE_SDK`, then build this project with the MacOS Geode target. The project
sets both arm64 and x86_64 architectures automatically.
