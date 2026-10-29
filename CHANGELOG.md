# Changelog

All notable changes to GDSiMS are recorded here. The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and the project uses [Semantic Versioning](https://semver.org/).

The current version number lives in the top-level `VERSION` file. When releasing, bump `VERSION` and rename the `Unreleased` heading below to the new version and date.

## [Unreleased]

### Changed
- New CMake + Makefile build system (`make`, `make debug`, `make profile`, `make test`, `make ship`). The configurations are defined in `CMakePresets.json` (requires CMake ≥ 3.21), so IDEs see the same builds; settings mirroring (`lsj`) runs as part of the CMake build.
- Replaced the old configuration and command-line handling with a reflected settings system (JSL + `lsj`).
- New settings validation with fatal/error/warning severities.
