## 1. C++ — ScoringProfile Data Structure and Loader

- [ ] 1.1 Create `src/ScoringProfile.h` defining the `ScoringProfile` struct (display name, description, weight map for TapNoteScore/HoldNoteScore, optional timing window overrides map)
- [ ] 1.2 Create `src/ScoringProfile.cpp` implementing `ScoringProfile::LoadFromFile(path)` using `IniFile`-style parsing; validate all required weight keys are present
- [ ] 1.3 Create `src/ScoringProfileManager.h/.cpp` that scans `ScoringProfiles/` at startup, loads all valid `.ini` files, and exposes `GetProfiles()` and `GetActiveProfile()` / `SetActiveProfile(name)`
- [ ] 1.4 Add `ScoringProfiles/default.ini` with FA+ weights (W1=5, W2=4, W3=2, W4=0, W5=-6, Miss=-12, HitMine=-6, LetGo=0, Held=5) and no timing window overrides
- [ ] 1.5 Add `ScoringProfiles/not_good_enough.ini` identical to default except W4=-2
- [ ] 1.6 Wire `ScoringProfileManager` into the ITGMania startup sequence (initialize after preference loading)

## 2. C++ — ScoreKeeperProfile Subclass

- [ ] 2.1 Create `src/ScoreKeeperProfile.h` declaring `ScoreKeeperProfile` as a subclass of `ScoreKeeperNormal`
- [ ] 2.2 Implement `ScoreKeeperProfile.cpp` overriding `TapNoteScoreToDancePoints(tns, bBeginner)` and `HoldNoteScoreToDancePoints(hns, bBeginner)` to use `ScoringProfile` weights instead of `g_iPercentScoreWeight`
- [ ] 2.3 Override `TapNoteScoreToGradePoints` and `HoldNoteScoreToGradePoints` similarly so grade calculation also reflects the profile
- [ ] 2.4 Update `Player::SetupPlayerData()` to instantiate `ScoreKeeperProfile` when the active profile is non-default, and `ScoreKeeperNormal` otherwise
- [ ] 2.5 Add `ScoreKeeperProfile` to `CMakeData-src.cmake`

## 3. C++ — Timing Window Overrides in Player

- [ ] 3.1 Add `SetTimingWindowOverrides(const std::map<TimingWindow, float>&)` and `ClearTimingWindowOverrides()` methods to `Player`
- [ ] 3.2 Update `Player::GetWindowSeconds(TimingWindow tw)` to check for active overrides before falling back to `m_fTimingWindowSeconds[tw]`
- [ ] 3.3 Apply timing window overrides from the active profile in `Player::SetupPlayerData()` when a profile with `[TimingWindows]` is active
- [ ] 3.4 Clear timing window overrides in `Player` on song end / screen exit as safety net

## 4. C++ — Disqualification for Non-Default Profiles

- [ ] 4.1 In `ScoreKeeperProfile::Load()`, set `m_pPlayerStageStats->m_bDisqualified = true` when the active profile is non-default
- [ ] 4.2 Expose the active profile name to Lua via `ScoringProfileManager` Lua bindings (e.g., `SCORINGPROFILEMAN:GetActiveProfileName()`)

## 5. Theme (Simply Love) — Profile Registry Lua Bridge

- [ ] 5.1 Add `ScoringProfileManager` Lua bindings: `GetProfileNames()`, `GetActiveProfileName()`, `SetActiveProfile(name)`, `GetProfileDisplayName(name)`, `GetProfileDescription(name)`
- [ ] 5.2 In `SL_Init.lua`, initialize `SL.Global.ActiveScoringProfile` from `SCORINGPROFILEMAN:GetActiveProfileName()` on startup
- [ ] 5.3 Add profile name persistence: save/load `ActiveScoringProfile` via the existing SL player profile mechanism in `SL-PlayerProfiles.lua`
- [ ] 5.4 On `ScreenSelectMusic` entry, re-apply the persisted profile as a safety net (call `SCORINGPROFILEMAN:SetActiveProfile`)

## 6. Theme (Simply Love) — Options Menu Entry

- [ ] 6.1 Add a "Scoring Profile" option row to the Simply Love options menu in `SL-PlayerOptions.lua`
- [ ] 6.2 Populate the row's choices from `SCORINGPROFILEMAN:GetProfileNames()` and display each using `GetProfileDisplayName(name)`
- [ ] 6.3 Apply a distinct visual style (e.g., warning color) to the row when a non-default profile is selected
- [ ] 6.4 Show the profile's description as a subtitle in the option row when available
- [ ] 6.5 Update `SL.Global.ActiveScoringProfile` and call `SCORINGPROFILEMAN:SetActiveProfile()` when the player confirms the selection

## 7. Theme (Simply Love) — Evaluation Screen Reference Scores

- [ ] 7.1 Add `CalculateProfileScore(player, weights)` helper to `SL-Helpers.lua` that computes a percent score from `GetExJudgmentCounts(player)` and a provided weight table
- [ ] 7.2 Define ITG and FA+ weight tables as constants accessible to the eval screen Lua
- [ ] 7.3 Update `ValidForGrooveStats` in `SL-Helpers-GrooveStats.lua` to return `false` when `SL.Global.ActiveScoringProfile ~= "default"`
- [ ] 7.4 Add a reference scores panel to the evaluation screen (`ScreenEvaluation common`) that shows profile score %, ITG %, and FA+ % with labels — displayed only when a non-default profile is active
- [ ] 7.5 Style the reference score panel to be visually consistent with the existing FA+ pane approach (see `Pane2/JudgmentNumbers.lua` and `CalculateExScore`)

## 8. Testing and Validation

- [ ] 8.1 Verify `default` profile: complete a song and confirm score matches standard FA+ calculation, GrooveStats submission proceeds normally
- [ ] 8.2 Verify `not_good_enough` profile: earn Good judgments and confirm dance points are negative, "Disqualified for Ranking" banner appears, GrooveStats not submitted
- [ ] 8.3 Verify timing window override: create a test profile with modified W3 window, confirm altered judgment assignment during play
- [ ] 8.4 Verify bad profile `.ini` is rejected gracefully at startup with a logged warning and does not appear in the menu
- [ ] 8.5 Verify profile selection persists across game restart
- [ ] 8.6 Verify reference scores (ITG, FA+) appear on eval screen only when non-default profile is active
