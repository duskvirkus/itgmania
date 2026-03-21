#ifndef SCORING_PROFILE_H
#define SCORING_PROFILE_H

#include <map>
#include <string>

#include "GameConstantsAndTypes.h"

/** @brief A scoring/timing profile loaded from an .ini file. */
struct ScoringProfile {
  std::string sName;         ///< Internal name (filename without extension)
  std::string sDisplayName;  ///< Human-readable name from [Meta] DisplayName
  std::string sDescription;  ///< Optional description from [Meta] Description

  /** Per-judgment dance point weights, keyed by TapNoteScore/HoldNoteScore
   *  integer value.  Stored as int to match PercentScoreWeight* theme metrics.
   */
  std::map<TapNoteScore, int> tapWeights;
  std::map<HoldNoteScore, int> holdWeights;

  /** Optional timing window overrides in seconds.  Only windows present in
   *  this map will override the global preferences. */
  std::map<TimingWindow, float> timingWindowOverrides;

  /** If true, W4 (Good) judgments break combo instead of maintaining it. */
  bool bGoodsBreakCombo = false;

  /** Returns true if this profile is the tournament-legal default. */
  bool IsDefault() const { return sName == "default"; }

  /** Attempts to load from the given file path.
   *  Returns true on success, false on failure (logs a warning). */
  bool LoadFromFile(const std::string& sPath);
};

#endif  // SCORING_PROFILE_H
