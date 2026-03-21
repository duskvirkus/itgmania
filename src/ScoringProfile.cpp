#include "ScoringProfile.h"

#include <string>

#include "IniFile.h"
#include "RageLog.h"
#include "RageUtil.h"
#include "global.h"

bool ScoringProfile::LoadFromFile(const std::string& sPath) {
  IniFile ini;
  if (!ini.ReadFile(sPath)) {
    LOG->Warn("ScoringProfile: failed to read '%s': %s", sPath.c_str(),
              ini.GetError().c_str());
    return false;
  }

  // [Meta]
  if (!ini.GetValue("Meta", "DisplayName", sDisplayName) ||
      sDisplayName.empty()) {
    LOG->Warn("ScoringProfile: '%s' missing [Meta] DisplayName", sPath.c_str());
    return false;
  }
  ini.GetValue("Meta", "Description", sDescription);
  ini.GetValue("Meta", "GoodsBreakCombo", bGoodsBreakCombo);

  // [ScoringWeights] — all keys are required
  struct TapEntry {
    const char* key;
    TapNoteScore tns;
  };
  static const TapEntry kTapEntries[] = {
      {"W1", TNS_W1},           {"W2", TNS_W2},
      {"W3", TNS_W3},           {"W4", TNS_W4},
      {"W5", TNS_W5},           {"Miss", TNS_Miss},
      {"HitMine", TNS_HitMine}, {"CheckpointHit", TNS_CheckpointHit},
      {"CheckpointMiss", TNS_CheckpointMiss},
  };
  for (const auto& e : kTapEntries) {
    int weight = 0;
    if (!ini.GetValue("ScoringWeights", e.key, weight)) {
      LOG->Warn("ScoringProfile: '%s' missing [ScoringWeights] %s",
                sPath.c_str(), e.key);
      return false;
    }
    tapWeights[e.tns] = weight;
  }

  struct HoldEntry {
    const char* key;
    HoldNoteScore hns;
  };
  static const HoldEntry kHoldEntries[] = {
      {"Held", HNS_Held},
      {"LetGo", HNS_LetGo},
      {"MissedHold", HNS_Missed},
  };
  for (const auto& e : kHoldEntries) {
    int weight = 0;
    if (!ini.GetValue("ScoringWeights", e.key, weight)) {
      LOG->Warn("ScoringProfile: '%s' missing [ScoringWeights] %s",
                sPath.c_str(), e.key);
      return false;
    }
    holdWeights[e.hns] = weight;
  }

  // [TimingWindows] — optional; only override specified windows
  struct TWEntry {
    const char* key;
    TimingWindow tw;
  };
  static const TWEntry kTWEntries[] = {
      {"W1", TW_W1},         {"W2", TW_W2},     {"W3", TW_W3},
      {"W4", TW_W4},         {"W5", TW_W5},     {"Mine", TW_Mine},
      {"Attack", TW_Attack}, {"Hold", TW_Hold}, {"Roll", TW_Roll},
      {"Checkpoint", TW_Checkpoint},
  };
  for (const auto& e : kTWEntries) {
    float secs = 0.f;
    if (ini.GetValue("TimingWindows", e.key, secs)) {
      timingWindowOverrides[e.tw] = secs;
    }
  }

  return true;
}
