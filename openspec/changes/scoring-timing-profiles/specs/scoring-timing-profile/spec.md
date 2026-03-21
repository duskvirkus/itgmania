## ADDED Requirements

### Requirement: Profile file format
A scoring/timing profile SHALL be defined as an `.ini` file with a `[Meta]` section (containing `DisplayName` and optional `Description`), a `[ScoringWeights]` section (containing integer weights for W1–W5, Miss, HitMine, LetGo, Held), and an optional `[TimingWindows]` section (containing float values in seconds for any subset of W1–W5, Mine, Hold, Roll, Attack, Checkpoint).

#### Scenario: Valid profile with weights only
- **WHEN** an `.ini` file contains `[Meta]` and `[ScoringWeights]` sections with all required weight keys
- **THEN** the profile SHALL be loaded successfully with timing windows inherited from the current FA+ preferences

#### Scenario: Valid profile with timing window overrides
- **WHEN** an `.ini` file contains a `[TimingWindows]` section
- **THEN** the profile SHALL override only the specified windows; unspecified windows SHALL inherit from FA+ preferences

#### Scenario: Missing required weight key
- **WHEN** an `.ini` file is missing one or more required keys in `[ScoringWeights]`
- **THEN** the profile SHALL fail to load and a warning SHALL be logged; the profile SHALL NOT appear in the selector menu

#### Scenario: Missing DisplayName
- **WHEN** an `.ini` file is missing `DisplayName` in `[Meta]`
- **THEN** the profile SHALL fail to load and a warning SHALL be logged

---

### Requirement: Built-in profiles
ITGMania SHALL ship two built-in profiles in `ScoringProfiles/`:
- `default.ini` — FA+ scoring weights; no timing window overrides; this is the tournament-legal baseline
- `not_good_enough.ini` — identical to `default` except W4 (Good) weight is negative (-2)

#### Scenario: Default profile matches FA+ expected values
- **WHEN** the `default` profile is active
- **THEN** all `PercentScoreWeight*` values used during scoring SHALL match the expected ITG values checked by `ValidForGrooveStats`

#### Scenario: Not-good-enough profile penalizes Good
- **WHEN** the `not_good_enough` profile is active and a player earns a Good (W4) judgment
- **THEN** the resulting dance points contribution for that judgment SHALL be negative

---

### Requirement: Profile discovery and registry
At startup, ITGMania SHALL scan the `ScoringProfiles/` directory, load all valid `.ini` files, and register them in an ordered profile registry accessible to the theme via Lua.

#### Scenario: Multiple profiles discovered
- **WHEN** multiple valid `.ini` files exist in `ScoringProfiles/`
- **THEN** all valid profiles SHALL appear in the registry and be selectable in the options menu

#### Scenario: User-created profile
- **WHEN** a user places a valid `.ini` file in `ScoringProfiles/` and restarts ITGMania
- **THEN** the new profile SHALL appear in the selector menu

---

### Requirement: ScoreKeeperProfile C++ class
A `ScoreKeeperProfile` class SHALL extend `ScoreKeeperNormal` and override `TapNoteScoreToDancePoints` and `HoldNoteScoreToDancePoints` to use weights loaded from the active profile rather than theme metrics.

#### Scenario: Profile score keeper instantiated for non-default profile
- **WHEN** a non-default profile is active at song start
- **THEN** `Player::SetupPlayerData()` SHALL instantiate `ScoreKeeperProfile` instead of `ScoreKeeperNormal`

#### Scenario: Default profile uses standard score keeper
- **WHEN** the `default` profile is active
- **THEN** `Player::SetupPlayerData()` SHALL instantiate the standard `ScoreKeeperNormal` with no changes to existing behavior

---

### Requirement: Timing window overrides
When a profile specifies `[TimingWindows]`, `Player::GetWindowSeconds()` SHALL use those values for the duration of the song instead of reading from `m_fTimingWindowSeconds`.

#### Scenario: Timing override applied at song start
- **WHEN** a profile with timing window overrides is active
- **THEN** `Player` SHALL apply those overrides before the first note is judged

#### Scenario: Timing overrides cleared after song
- **WHEN** a song ends or is exited
- **THEN** timing overrides SHALL be cleared and global preferences SHALL resume

---

### Requirement: Active selection state
The active profile name SHALL be stored as a global state value accessible to theme Lua (e.g., `SL.Global.ActiveScoringProfile`).

#### Scenario: Default profile on first launch
- **WHEN** no profile has been previously selected
- **THEN** the active profile SHALL default to `"default"`

#### Scenario: Profile name persisted across sessions
- **WHEN** a player selects a non-default profile and exits
- **THEN** on next launch, the same profile SHALL be pre-selected
