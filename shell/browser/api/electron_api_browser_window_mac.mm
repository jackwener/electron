// Copyright (c) 2026 Electron contributors.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/api/electron_api_browser_window.h"

#import <Cocoa/Cocoa.h>

#include "content/public/browser/web_contents.h"

namespace electron::api {

bool BrowserWindow::FocusIsInAnotherView() {
  NSView* primary = web_contents()->GetNativeView().GetNativeNSView();
  NSResponder* responder = [[primary window] firstResponder];
  if (![responder isKindOfClass:[NSView class]])
    return false;
  return ![(NSView*)responder isDescendantOf:primary];
}

}  // namespace electron::api
