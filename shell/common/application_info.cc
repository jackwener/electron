// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/common/application_info.h"

#include "base/i18n/rtl.h"
#include "base/no_destructor.h"
#include "base/strings/string_util.h"
#include "base/synchronization/lock.h"
#include "chrome/browser/browser_process.h"
#include "chrome/common/chrome_version.h"
#include "components/embedder_support/user_agent_utils.h"
#include "electron/electron_version.h"
#include "shell/browser/browser.h"
#include "third_party/abseil-cpp/absl/strings/str_format.h"

namespace electron {

std::string& OverriddenApplicationName() {
  static base::NoDestructor<std::string> overridden_application_name;
  return *overridden_application_name;
}

std::string& OverriddenApplicationVersion() {
  static base::NoDestructor<std::string> overridden_application_version;
  return *overridden_application_version;
}

std::string GetPossiblyOverriddenApplicationName() {
  std::string ret = OverriddenApplicationName();
  if (!ret.empty())
    return ret;
  return GetApplicationName();
}

namespace {

// Kaze: the user agent is Chrome's own (embedder_support::GetUserAgent: the reduced
// "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) ... Chrome/<major>.0.0.0 Safari/537.36"),
// with no Electron or app token. A browser that names Electron or itself there is an embedded
// browser to sign-in pages and risk checks; and an app that rewrites the string itself
// (setUserAgent) loses Chromium's client hints and its userAgentData agreeing with it. Left
// as Chromium makes it, the string, the Sec-CH-UA hints and navigator.userAgentData all come
// from one place and agree, as in Chrome.
std::string BuildApplicationUserAgent() {
  return embedder_support::GetUserAgent();
}

base::Lock& UserAgentLock() {
  static base::NoDestructor<base::Lock> lock;
  return *lock;
}

std::string& CachedUserAgent() {
  static base::NoDestructor<std::string> cached;
  return *cached;
}

// Bumped by every invalidation, so a value built from inputs that have since
// changed is not stored.
uint64_t& UserAgentGeneration() {
  static uint64_t generation = 0;
  return generation;
}

}  // namespace

std::string GetApplicationUserAgent() {
  // Built once; invalidated when the name or version it is built from changes.
  uint64_t generation;
  {
    base::AutoLock guard{UserAgentLock()};
    if (!CachedUserAgent().empty())
      return CachedUserAgent();
    generation = UserAgentGeneration();
  }

  std::string built = BuildApplicationUserAgent();

  base::AutoLock guard{UserAgentLock()};
  if (UserAgentGeneration() != generation)
    return built;  // Already stale: hand it back but do not keep it.
  if (CachedUserAgent().empty())
    CachedUserAgent() = std::move(built);
  return CachedUserAgent();
}

void InvalidateApplicationUserAgent() {
  base::AutoLock guard{UserAgentLock()};
  CachedUserAgent().clear();
  ++UserAgentGeneration();
}

bool IsAppRTL() {
  const std::string& locale = g_browser_process->GetApplicationLocale();
  base::i18n::TextDirection text_direction =
      base::i18n::GetTextDirectionForLocale(locale.c_str());
  return text_direction == base::i18n::RIGHT_TO_LEFT;
}

}  // namespace electron
