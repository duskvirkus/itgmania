#ifndef SCORE_KEEPER_PROFILE_H
#define SCORE_KEEPER_PROFILE_H

#include "ScoreKeeperNormal.h"
#include "ScoringProfile.h"

/** @brief A ScoreKeeper that reads weights from a ScoringProfile instead of
 *  theme metrics. Used when a non-default scoring profile is active. */
class ScoreKeeperProfile : public ScoreKeeperNormal {
 public:
  ScoreKeeperProfile(PlayerState* pPlayerState,
                     PlayerStageStats* pPlayerStageStats,
                     const ScoringProfile& profile);

  void Load(const std::vector<Song*>& apSongs,
            const std::vector<Steps*>& apSteps,
            const std::vector<AttackArray>& asModifiers) override;

  void GetRowCounts(const NoteData& nd, int iRow, int& iNumHitContinueCombo,
                    int& iNumHitMaintainCombo,
                    int& iNumBreakCombo) override;
  void HandleRowComboInternal(TapNoteScore tns, int iNumTapsInRow,
                               int iRow = -1) override;

  // Override the static-style instance methods to use profile weights.
  int TapNoteScoreToDancePoints(TapNoteScore tns) const override;
  int HoldNoteScoreToDancePoints(HoldNoteScore hns) const override;
  int TapNoteScoreToGradePoints(TapNoteScore tns) const override;
  int HoldNoteScoreToGradePoints(HoldNoteScore hns) const override;

 private:
  const ScoringProfile& m_profile;
};

#endif  // SCORE_KEEPER_PROFILE_H
