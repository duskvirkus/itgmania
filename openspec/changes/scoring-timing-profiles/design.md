## Context

ITGMania's scoring system has two relevant layers:

1. **Engine layer** (`Player.cpp`, `ScoreKeeperNormal.cpp`): timing window sizes are loaded from `Preferences.ini` as `TimingWindowSecondsW1`–`W5`; score weights are read from theme metrics (`PercentScoreWeightW1`–`W5`, etc. from `ScoreKeeperNormal` metrics section). `ScoreKeeper` is an abstract base with `ScoreKeeperNormal` as the primary implementation.
2. **Theme layer** (Simply Love): `ValidForGrooveStats` in `SL-Helpers-GrooveStats.lua` independently validates current timing windows and `PercentScoreWeight*` metrics against known ITG values before submitting. Any deviation causes silent skip of submission. The existing `m_bDisqualified` / `IsDisqualified()` engine API controls the "Disqualified for Ranking" banner on eval.

The evaluation screen uses pane components (Pane2–Pane8). The FA+ score display already exists as an existing pane option using `CalculateExScore()` — a similar approach will work for profile-score display.

## Goals / Non-Goals

**Goals:**
- Define scoring profiles in `.ini` files; load and register them at startup
- Subclass `ScoreKeeperNormal` as `ScoreKeeperProfile` — loads weights/windows from an active profile instead of theme metrics and prefs
- Expose profile selection in the Simply Love options menu
- Show active-profile %, ITG %, and FA+ % on the evaluation screen when a non-default profile is active
- Non-default profiles automatically fail `ValidForGrooveStats` — no new submission logic needed

**Non-Goals:**
- Submitting custom-profile scores to GrooveStats
- Per-player or per-song profile overrides (global selection only)
- Real-time profile switching mid-song
- Versus mode support with differing profiles per player

## Decisions

### D1: Profile format — `.ini` files in `ScoringProfiles/`

Store profiles as `.ini` files in a `ScoringProfiles/` directory under the ITGMania data root. Two built-in profiles ship in `ScoringProfiles/`:

```ini
; ScoringProfiles/default.ini
[Meta]
DisplayName=Default (FA+)

[ScoringWeights]
W1=5
W2=4
W3=2
W4=0
W5=-6
Miss=-12
HitMine=-6
LetGo=0
Held=5

; [TimingWindows] omitted — inherits FA+ defaults from Preferences.ini
```

```ini
; ScoringProfiles/not_good_enough.ini
[Meta]
DisplayName=Not Good Enough
Description=Good scores count against you. Push for greats and better.

[ScoringWeights]
W1=5
W2=4
W3=2
W4=-2
W5=-6
Miss=-12
HitMine=-6
LetGo=0
Held=5

; [TimingWindows] omitted — same windows as FA+
```

An optional `[TimingWindows]` section allows overriding individual window values. If omitted, the profile inherits current preferences.

**Rationale**: `.ini` is the native format in use throughout the codebase. User-created profiles go in the same directory and are auto-discovered at startup.

---

### D2: C++ — `ScoreKeeperProfile` subclass

**Decision**: Create `ScoreKeeperProfile` as a subclass of `ScoreKeeperNormal`. It overrides the score weight lookup to use values loaded from the active profile's `[ScoringWeights]` section rather than reading theme metrics.

```
ScoreKeeper (abstract)
  └── ScoreKeeperNormal
        └── ScoreKeeperProfile   ← new
```

`ScoreKeeperProfile` holds a `ScoringProfile` struct (loaded from `.ini`) and overrides:
- `TapNoteScoreToDancePoints(tns, bBeginner)` — returns weight from profile instead of `g_iPercentScoreWeight`
- `HoldNoteScoreToDancePoints(hns, bBeginner)` — same for hold weights

Timing windows: if the active profile's `[TimingWindows]` section is present, `Player` must be told to use those values instead of the global preferences. The cleanest approach is to give `Player` a `SetTimingWindowOverrides(map<TimingWindow, float>)` method that, when set, bypasses `m_fTimingWindowSeconds[tw]` in `GetWindowSeconds()`. The overrides are cleared at song end.

`ScoreKeeperProfile` is instantiated by `Player::SetupPlayerData()` when the active profile is non-default.

**Rationale**: Inheritance is the correct OOP pattern here. No existing behavior is modified; the new subclass handles its own weight source. This avoids mutating global state (metrics or prefs) mid-session and makes the separation of concerns clear.

---

### D3: GrooveStats disqualification — no new mechanism needed

`ValidForGrooveStats` already validates `PercentScoreWeight*` metrics and timing windows against expected ITG values. However, with the `ScoreKeeperProfile` approach, the *theme metrics themselves are unchanged* — the C++ class reads weights from the profile, not from metrics.

This means `ValidForGrooveStats`'s metric checks would still pass even for a non-default profile. Two options:

**Option A**: Expose the active profile name via a Lua global (e.g., `SL.Global.ActiveScoringProfile`) and add a check to `ValidForGrooveStats`: if the active profile is not `"default"`, fail validation.

**Option B**: Have `ScoreKeeperProfile` set `m_bDisqualified = true` for all non-default profiles at song start.

**Decision**: Use **Option A** (Lua check in `ValidForGrooveStats`) combined with **Option B** (`m_bDisqualified = true`) for belt-and-suspenders. The Lua side blocks GrooveStats submission; the engine side shows the "Disqualified for Ranking" banner. The `default` profile never sets disqualification.

---

### D4: Reference score computation on eval screen

**Decision**: Follow the existing FA+ panel pattern. `CalculateExScore(player)` in `SL-Helpers.lua` already computes an alternative score from raw judgment counts using a fixed weight table. Add `CalculateProfileScore(player, weights)` as a parallel function, and call it twice on the eval screen — once with ITG weights and once with FA+ weights — when a non-default profile is active.

The raw judgment counts (W0–W5, Miss, Hold, etc.) are available from `GetExJudgmentCounts(player)`. Applying different weight tables to the same counts is a straightforward Lua operation requiring no engine changes.

Reference scores are shown **only when a non-default profile is active**.

---

### D5: Profile selection in options menu and persistence

Profile selection is added as an entry in the Simply Love options menu (alongside existing entries like Game Mode and Timing Display). The selected profile name is stored in `SL[pn].ActiveModifiers` and persisted via the existing `SL-PlayerProfiles.lua` mechanism.

At `ScreenSelectMusic` entry, the active profile is re-applied as a safety net in case of prior crash.

---

## Risks / Trade-offs

- **Timing window overrides and song rate**: If the profile overrides timing windows and the player uses a rate mod, the raw timing values still scale correctly because `GetWindowSeconds()` applies `TimingWindowScale` and `TimingWindowAdd` on top of whatever base value is provided.
- **Scoreboard pollution**: The internal score counter (`m_iScore`) still accumulates — it will reflect the profile's weights, not standard weights. This is acceptable since profile play is not tournament-legal.
- **Theme portability**: `ScoreKeeperProfile` is instantiated from C++ when a non-default profile is active; it works regardless of which theme is running. The eval-screen reference scores are theme (Simply Love) specific.
- **Profile discovery**: Profiles are loaded at startup. Adding a new `.ini` file requires a restart to appear in the menu.

## Open Questions

- Should `ScoreKeeperProfile` also affect `TapNoteScoreToGradePoints` (the grade-weight path), or only the percent-score path? Changing both creates a fully custom experience; changing only percent-score may be simpler for v1.
- Should timing window overrides in a profile be additive (delta from FA+ defaults) or absolute? Absolute is less error-prone.
