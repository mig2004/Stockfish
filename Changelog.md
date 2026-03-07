# Changelog - Aletheia Fish

All notable changes to this project will be documented in this file.

## [2.3] - 2026-03-07
### Added
- **Hybrid Phase-Control (1+2+3):** New time management logic that activates automatically at move 55 (`ply 110`).
- **Endgame Safety Floor:** Enforced a minimum thinking time (80% of the increment) to prevent impulsive tactical errors in simplified positions.
- **Dynamic Extension Scaling:** Increased `maximumTime` flexibility by 50% during the endgame to allow the engine to resolve tactical instabilities.

### Fixed
- Fixed the "Endgame Rupture" issue where the engine played at Blitz speeds despite having a large clock advantage.
- Improved defensive resilience in drawn positions through better increment reinvestment.

## [2.2] - 2026-03-01
### Added
- **Aletheia Protocol 99%:** Aggressive time allocation strategy (99% of available time) to maximize middlegame pressure.
- **FS Resistance Trigger:** Implemented a survival mode that triggers when time is < 30% of the opponent's clock.

## [2.1] - 2026-02-15
### Changed
- Initial modification of Stockfish 8 time management.
- Baseline Elo optimization for 300+6 time controls.

---
*Aletheia Fish is a specialized modification of Stockfish 8 focusing on aggressive clock management and tactical precision.*
