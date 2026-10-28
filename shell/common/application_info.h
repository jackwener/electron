// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_COMMON_APPLICATION_INFO_H_
#define ELECTRON_SHELL_COMMON_APPLICATION_INFO_H_

#include "build/build_config.h"

#include <string>

#if BUILDFLAG(IS_WIN)
#include "base/win/scoped_hstring.h"
#endif

namespace electron {

std::string& OverriddenApplicationName();
std::string& OverriddenApplicationVersion();

std::string GetPossiblyOverriddenApplicationName();

std::string GetApplicationName();
std::string GetApplicationVersion();
// Returns the user agent of Electron.
std::string GetApplicationUserAgent();

// Kaze: the languages pages are told, as Chrome's default (its
// IDS_ACCEPT_LANGUAGES): the app's locale, then its base language ("en-US,en",
// "zh-CN,zh") — one list for navigator.languages and Accept-Language, so the
// two agree. (Electron appended the OS's own list as it spells it, "en-CN",
// "zh-Hans-CN", which no Chrome sends, to navigator.languages alone.)
std::string GetAcceptLanguages(const std::string& locale);

// Drops the cached user agent so that the next caller rebuilds it. Call this
// whenever something the user agent is built from changes.
void InvalidateApplicationUserAgent();

bool IsAppRTL();

#if BUILDFLAG(IS_WIN)
PCWSTR GetRawAppUserModelID();
base::win::ScopedHString GetAppUserModelID();
void SetAppUserModelID(const std::wstring& name);
bool IsRunningInDesktopBridge();
PCWSTR GetAppToastActivatorCLSID();
void SetAppToastActivatorCLSID(const std::wstring& clsid);
#endif

}  // namespace electron

#endif  // ELECTRON_SHELL_COMMON_APPLICATION_INFO_H_
