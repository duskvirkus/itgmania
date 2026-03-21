## Why

Players who have mastered full-comboing charts often plateau when trying to improve timing accuracy, because the default scoring system doesn't penalize near-miss judgments (Good/W4) enough to motivate tighter play. A configurable profile system would let players define custom scoring and timing windows that create meaningful incentives for precision, without affecting tournament-legal defaults.

## What Changes

- Introduce a **scoring/timing profile** system defined in external `.ini` configuration files under a `ScoringProfiles/` directory
- Each profile specifies per-judgment score weights and optionally timing window sizes (W1–W5)
- An entry in the **options menu** lets players select the active profile
- The results/evaluation screen displays the score computed under the active profile, alongside reference ITG and FA+ scores for comparison
- Profiles that deviate from tournament-legal settings automatically fail the existing `ValidForGrooveStats` check in Simply Love (which already validates timing windows and `PercentScoreWeight*` metrics against expected values), and the engine's `m_bDisqualified` flag can be set via the existing disqualification system — no new submission-blocking mechanism is needed
- Two built-in profiles ship with ITGMania:
  - `default` — matches current FA+ configuration; passes all `ValidForGrooveStats` checks
  - `not_good_enough` — identical to FA+ except Good (W4) has a negative score weight; will fail GrooveStats validation automatically due to the changed weight

## Capabilities

### New Capabilities

- `scoring-timing-profile`: Definition, loading, and validation of scoring/timing profile config files; profile registry; active profile selection state persisted per player profile
- `profile-selector-menu`: Options menu entry for browsing and activating a scoring/timing profile
- `profile-results-display`: Evaluation screen panel showing active-profile score alongside computed ITG and FA+ reference scores

### Modified Capabilities

- (none — existing scoring behavior is fully preserved through the `default` profile; no existing spec-level requirements change)

## Impact

- **`src/ScoreKeeperNormal.cpp`** / **`src/Player.cpp`**: `PercentScoreWeight*`/`GradeWeight*` theme metrics and timing window values need to be overridable by the active profile at song start
- **`Themes/_fallback/metrics.ini`** / ScoreKeeperNormal theme metrics: values driven from active profile data at runtime when a non-default profile is active
- **`Themes/Simply Love/Scripts/SL-Helpers-GrooveStats.lua`**: `ValidForGrooveStats` already validates timing windows and `PercentScoreWeight*` against expected ITG values — non-default profiles will naturally fail this check with no changes required
- **`Themes/Simply Love/BGAnimations/ScreenEvaluation common/`**: additions to display multi-score panel (active profile, ITG, FA+)
- New directory: `ScoringProfiles/` under the ITGMania data path, containing `.ini` profile files loaded at startup
