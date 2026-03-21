#ifndef SCORING_PROFILE_MANAGER_H
#define SCORING_PROFILE_MANAGER_H

#include <string>
#include <vector>

#include "ScoringProfile.h"

/** @brief Loads and manages scoring/timing profiles from ScoringProfiles/. */
class ScoringProfileManager {
 public:
  ScoringProfileManager();
  ~ScoringProfileManager() {}

  /** Scan ScoringProfiles/ and load all valid .ini files. */
  void Init();

  /** Returns all loaded profiles in load order. */
  const std::vector<ScoringProfile>& GetProfiles() const { return m_profiles; }

  /** Returns the currently active profile, or the default profile if none set.
   */
  const ScoringProfile& GetActiveProfile() const;

  /** Returns the active profile name (internal key). */
  const std::string& GetActiveProfileName() const {
    return m_sActiveProfileName;
  }

  /** Set the active profile by internal name. No-op if name not found. */
  void SetActiveProfile(const std::string& sName);

 private:
  std::vector<ScoringProfile> m_profiles;
  std::string m_sActiveProfileName;

  const ScoringProfile* FindProfile(const std::string& sName) const;
};

extern ScoringProfileManager* SCORINGPROFILEMAN;

#endif  // SCORING_PROFILE_MANAGER_H
