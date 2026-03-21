## ADDED Requirements

### Requirement: Profile score shown on evaluation screen
When a non-default scoring profile is active, the evaluation screen SHALL display the score computed under that profile as the primary result.

#### Scenario: Non-default profile score displayed
- **WHEN** a song is completed with a non-default profile active
- **THEN** the evaluation screen SHALL prominently show the profile-weighted percent score

#### Scenario: Default profile — no change to display
- **WHEN** a song is completed with the `default` profile active
- **THEN** the evaluation screen SHALL show the standard display with no additional reference scores

---

### Requirement: ITG and FA+ reference scores shown alongside profile score
When a non-default scoring profile is active, the evaluation screen SHALL also show the computed ITG score and FA+ score derived from the same judgment counts, clearly labeled.

#### Scenario: Reference scores computed from judgment counts
- **WHEN** a song ends with a non-default profile active
- **THEN** ITG and FA+ reference scores SHALL be computed by applying the standard ITG and FA+ weight tables to the collected judgment counts

#### Scenario: Reference scores labeled
- **WHEN** reference scores are displayed
- **THEN** each score SHALL be accompanied by a label identifying it as "ITG" or "FA+"

#### Scenario: Reference scores are approximations
- **WHEN** the active profile has different timing windows than ITG/FA+
- **THEN** the reference scores are computed from the judgment counts as awarded by the profile's windows, not re-judged under ITG/FA+ windows; this approximation SHALL be accepted for a practice tool

---

### Requirement: GrooveStats disqualification for non-default profiles
When a non-default profile is active, the score SHALL be disqualified from GrooveStats submission. The "Disqualified for Ranking" banner SHALL appear on the evaluation screen.

#### Scenario: Non-default profile sets disqualified flag
- **WHEN** a song is played with a non-default profile
- **THEN** `m_bDisqualified` SHALL be set to `true` for that player's stage stats

#### Scenario: ValidForGrooveStats rejects non-default profile
- **WHEN** `ValidForGrooveStats` is called for a player with a non-default profile active
- **THEN** the check SHALL return `false`, preventing GrooveStats submission

#### Scenario: Default profile is not disqualified
- **WHEN** a song is played with the `default` profile
- **THEN** `m_bDisqualified` SHALL NOT be set by the profile system, and standard GrooveStats validation SHALL proceed normally
