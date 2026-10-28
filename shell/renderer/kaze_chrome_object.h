// Copyright (c) 2026 Kaze.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_RENDERER_KAZE_CHROME_OBJECT_H_
#define ELECTRON_SHELL_RENDERER_KAZE_CHROME_OBJECT_H_

#include "v8/include/v8-forward.h"

namespace electron {

// Kaze: a page's `window.chrome` as Chrome gives it, native — chrome.loadTimes() and
// chrome.csi() (Chrome's own LoadTimesBindings) and chrome.app (what Chrome's app hooks
// answer for a page that is no installed app). Electron's is missing, and filling it in
// from script leaves script's marks (a function's source, property shapes) that sign-in
// and risk checks read.
void InstallKazeChromeObject(v8::Local<v8::Context> context);

}  // namespace electron

#endif  // ELECTRON_SHELL_RENDERER_KAZE_CHROME_OBJECT_H_
