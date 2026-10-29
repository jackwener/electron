// Copyright (c) 2026 Kaze.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_UI_COCOA_KAZE_WEB_CONTENTS_VIEW_DELEGATE_MAC_H_
#define ELECTRON_SHELL_BROWSER_UI_COCOA_KAZE_WEB_CONTENTS_VIEW_DELEGATE_MAC_H_

#include <memory>

namespace content {
class WebContentsViewDelegate;
}

namespace electron {

// Kaze: a page's view on the Mac as Chrome's
// (ChromeWebContentsViewDelegateViewsMac) for one thing only — its render
// widget's delegate is Chrome's, with the HistorySwiper: two fingers left or
// right go back or forward. Everything else keeps content's defaults, as with
// no delegate (Electron shows its context menus and keeps focus itself).
std::unique_ptr<content::WebContentsViewDelegate>
CreateKazeWebContentsViewDelegate();

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_UI_COCOA_KAZE_WEB_CONTENTS_VIEW_DELEGATE_MAC_H_
