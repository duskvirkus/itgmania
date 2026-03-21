#include "ScoringProfileManager.h"

#include <algorithm>
#include <string>
#include <vector>

#include "LuaBinding.h"
#include "LuaManager.h"
#include "RageFileManager.h"
#include "RageLog.h"
#include "RageUtil.h"
#include "global.h"

ScoringProfileManager* SCORINGPROFILEMAN = nullptr;

static const std::string PROFILES_DIR = "ScoringProfiles/";

ScoringProfileManager::ScoringProfileManager()
    : m_sActiveProfileName("default") {}

void ScoringProfileManager::Init() {
  m_profiles.clear();

  std::vector<std::string> files;
  GetDirListing(PROFILES_DIR + "*.ini", files, false, true);
  std::sort(files.begin(), files.end());

  // Ensure "default" is loaded first if present; others follow alphabetically.
  auto it =
      std::find_if(files.begin(), files.end(), [](const std::string& path) {
        std::string name = Basename(path);
        name = name.substr(0, name.rfind('.'));
        return name == "default";
      });
  if (it != files.end()) {
    std::rotate(files.begin(), it, it + 1);
  }

  for (const auto& path : files) {
    ScoringProfile profile;
    std::string name = Basename(path);
    name = name.substr(0, name.rfind('.'));
    profile.sName = name;
    if (profile.LoadFromFile(path)) {
      m_profiles.push_back(profile);
      LOG->Trace("ScoringProfileManager: loaded profile '%s' from '%s'",
                 name.c_str(), path.c_str());
    }
  }

  if (m_profiles.empty()) {
    LOG->Warn(
        "ScoringProfileManager: no valid profiles found in '%s'; scoring will "
        "use theme metrics as fallback",
        PROFILES_DIR.c_str());
  }

  // Validate that the stored active name still exists after reload.
  if (!FindProfile(m_sActiveProfileName)) {
    m_sActiveProfileName = "default";
  }
}

const ScoringProfile& ScoringProfileManager::GetActiveProfile() const {
  const ScoringProfile* p = FindProfile(m_sActiveProfileName);
  if (p) return *p;
  // Fall back to first profile (should be "default") or a static empty one.
  if (!m_profiles.empty()) return m_profiles[0];
  static ScoringProfile sEmpty;
  return sEmpty;
}

void ScoringProfileManager::SetActiveProfile(const std::string& sName) {
  if (FindProfile(sName)) {
    m_sActiveProfileName = sName;
  } else {
    LOG->Warn("ScoringProfileManager: unknown profile '%s'", sName.c_str());
  }
}

const ScoringProfile* ScoringProfileManager::FindProfile(
    const std::string& sName) const {
  for (const auto& p : m_profiles) {
    if (p.sName == sName) return &p;
  }
  return nullptr;
}

// ---------------------------------------------------------------------------
// Lua bindings

class LunaScoringProfileManager
    : public Luna<ScoringProfileManager> {
 public:
  static int GetProfileNames(T* p, lua_State* L) {
    lua_newtable(L);
    int i = 1;
    for (const auto& profile : p->GetProfiles()) {
      lua_pushstring(L, profile.sName.c_str());
      lua_rawseti(L, -2, i++);
    }
    return 1;
  }
  static int GetActiveProfileName(T* p, lua_State* L) {
    lua_pushstring(L, p->GetActiveProfileName().c_str());
    return 1;
  }
  static int SetActiveProfile(T* p, lua_State* L) {
    std::string name = SArg(1);
    p->SetActiveProfile(name);
    return 0;
  }
  static int GetProfileDisplayName(T* p, lua_State* L) {
    std::string name = SArg(1);
    for (const auto& profile : p->GetProfiles()) {
      if (profile.sName == name) {
        lua_pushstring(L, profile.sDisplayName.c_str());
        return 1;
      }
    }
    lua_pushnil(L);
    return 1;
  }
  static int GetProfileDescription(T* p, lua_State* L) {
    std::string name = SArg(1);
    for (const auto& profile : p->GetProfiles()) {
      if (profile.sName == name) {
        lua_pushstring(L, profile.sDescription.c_str());
        return 1;
      }
    }
    lua_pushnil(L);
    return 1;
  }

  LunaScoringProfileManager() {
    ADD_METHOD(GetProfileNames);
    ADD_METHOD(GetActiveProfileName);
    ADD_METHOD(SetActiveProfile);
    ADD_METHOD(GetProfileDisplayName);
    ADD_METHOD(GetProfileDescription);
  }
};

LUA_REGISTER_CLASS(ScoringProfileManager)
