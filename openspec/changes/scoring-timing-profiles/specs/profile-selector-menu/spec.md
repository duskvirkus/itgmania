## ADDED Requirements

### Requirement: Profile option row in options menu
The Simply Love options menu SHALL contain an entry for selecting the active scoring/timing profile. The entry SHALL display the current profile's `DisplayName` and allow cycling through all registered profiles.

#### Scenario: Player opens options menu
- **WHEN** a player navigates to the options menu
- **THEN** a "Scoring Profile" entry SHALL be visible showing the currently active profile's display name

#### Scenario: Player cycles through profiles
- **WHEN** a player selects the Scoring Profile option row and presses left/right
- **THEN** the selection SHALL cycle through all registered profiles in order

#### Scenario: Player confirms selection
- **WHEN** a player confirms a profile selection and exits the options menu
- **THEN** `SL.Global.ActiveScoringProfile` SHALL be updated to the selected profile name

---

### Requirement: Non-default profile indicator
When a non-default profile is active, the options menu entry SHALL visually distinguish it from the default (e.g., a different color or label suffix) to make it clear the player is not in tournament-legal mode.

#### Scenario: Non-default profile highlighted
- **WHEN** a non-default profile is active in the options menu
- **THEN** the profile entry SHALL render in a visually distinct style compared to when `default` is selected

---

### Requirement: Profile description tooltip
When available, the profile's `Description` field SHALL be shown as a subtitle or tooltip in the options menu entry.

#### Scenario: Description shown for profile with description
- **WHEN** a player highlights a profile that has a `Description` field
- **THEN** the description text SHALL be visible in the options menu

#### Scenario: No description shown for profile without description
- **WHEN** a player highlights a profile that has no `Description` field
- **THEN** no description area SHALL show empty or placeholder text
