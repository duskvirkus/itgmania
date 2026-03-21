#include "ScoreKeeperProfile.h"

#include "GameState.h"
#include "PrefsManager.h"
#include "PlayerStageStats.h"
#include "global.h"

ScoreKeeperProfile::ScoreKeeperProfile(PlayerState* pPlayerState,
                                       PlayerStageStats* pPlayerStageStats,
                                       const ScoringProfile& profile)
    : ScoreKeeperNormal(pPlayerState, pPlayerStageStats), m_profile(profile) {}

void ScoreKeeperProfile::Load(const std::vector<Song*>& apSongs,
                               const std::vector<Steps*>& apSteps,
                               const std::vector<AttackArray>& asModifiers) {
  ScoreKeeperNormal::Load(apSongs, apSteps, asModifiers);
  // Non-default profiles disqualify the score from ranking/GrooveStats.
  if (!m_profile.IsDefault()) {
    m_pPlayerStageStats->m_bDisqualified = true;
  }
}

int ScoreKeeperProfile::TapNoteScoreToDancePoints(TapNoteScore tns) const {
  if (!GAMESTATE->ShowW1() && tns == TNS_W1) {
    tns = TNS_W2;
  }
  auto it = m_profile.tapWeights.find(tns);
  if (it != m_profile.tapWeights.end()) {
    int iWeight = it->second;
    if (m_bIsBeginner && PREFSMAN->m_bMercifulBeginner) {
      iWeight = std::max(0, iWeight);
    }
    return iWeight;
  }
  // Fall back to parent for any unspecified scores (e.g. TNS_None, TNS_AvoidMine)
  return ScoreKeeperNormal::TapNoteScoreToDancePoints(tns);
}

int ScoreKeeperProfile::HoldNoteScoreToDancePoints(HoldNoteScore hns) const {
  auto it = m_profile.holdWeights.find(hns);
  if (it != m_profile.holdWeights.end()) {
    int iWeight = it->second;
    if (m_bIsBeginner && PREFSMAN->m_bMercifulBeginner) {
      iWeight = std::max(0, iWeight);
    }
    return iWeight;
  }
  return ScoreKeeperNormal::HoldNoteScoreToDancePoints(hns);
}

int ScoreKeeperProfile::TapNoteScoreToGradePoints(TapNoteScore tns) const {
  // Reuse dance point weights for grade points as well so the grade reflects
  // the profile's scoring intent.
  return TapNoteScoreToDancePoints(tns);
}

int ScoreKeeperProfile::HoldNoteScoreToGradePoints(HoldNoteScore hns) const {
  return HoldNoteScoreToDancePoints(hns);
}
