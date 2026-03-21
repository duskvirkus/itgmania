#include "ScoreKeeperProfile.h"

#include "GameState.h"
#include "NoteData.h"
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

void ScoreKeeperProfile::GetRowCounts(const NoteData& nd, int iRow,
                                       int& iNumHitContinueCombo,
                                       int& iNumHitMaintainCombo,
                                       int& iNumBreakCombo) {
  if (!m_profile.bGoodsBreakCombo) {
    ScoreKeeperNormal::GetRowCounts(nd, iRow, iNumHitContinueCombo,
                                    iNumHitMaintainCombo, iNumBreakCombo);
    return;
  }

  // With GoodsBreakCombo, treat W4 (Good) as a combo-break rather than
  // a combo-maintain. W1/W2/W3 continue combo; W4 and below break it.
  iNumHitContinueCombo = iNumHitMaintainCombo = iNumBreakCombo = 0;
  for (int track = 0; track < nd.GetNumTracks(); ++track) {
    const TapNote& tn = nd.GetTapNote(track, iRow);
    if (tn.type != TapNoteType_Tap && tn.type != TapNoteType_HoldHead &&
        tn.type != TapNoteType_Lift) {
      continue;
    }
    TapNoteScore tns = tn.result.tns;
    if (tns >= TNS_W3) {
      ++iNumHitContinueCombo;
    } else {
      ++iNumBreakCombo;
    }
  }
}

int ScoreKeeperProfile::TapNoteScoreToDancePoints(TapNoteScore tns) const {
  if (!GAMESTATE->ShowW1() && tns == TNS_W1) {
    tns = TNS_W2;
  }
  auto it = m_profile.tapWeights.find(tns);
  if (it != m_profile.tapWeights.end()) {
    int iWeight = it->second;
    if (GetIsBeginner() && PREFSMAN->m_bMercifulBeginner) {
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
    if (GetIsBeginner() && PREFSMAN->m_bMercifulBeginner) {
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
