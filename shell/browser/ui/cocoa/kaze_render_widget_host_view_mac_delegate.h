// Copyright (c) 2026 Kaze.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_UI_COCOA_KAZE_RENDER_WIDGET_HOST_VIEW_MAC_DELEGATE_H_
#define ELECTRON_SHELL_BROWSER_UI_COCOA_KAZE_RENDER_WIDGET_HOST_VIEW_MAC_DELEGATE_H_

#import <Cocoa/Cocoa.h>

#import "content/public/browser/render_widget_host_view_mac_delegate.h"

namespace content {
class RenderWidgetHost;
}

// Kaze: Chrome's ChromeRenderWidgetHostViewMacDelegate, its history swiping
// alone: the wheel and touch events go to the HistorySwiper first, and the
// renderer's answer (did the page scroll? did it overscroll?) after, so a
// swipe goes back or forward only where the page itself doesn't scroll
// sideways — and only as the system's "Swipe between pages" allows.
@interface KazeRenderWidgetHostViewMacDelegate
    : NSObject <RenderWidgetHostViewMacDelegate>

- (instancetype)initWithRenderWidgetHost:
    (content::RenderWidgetHost*)renderWidgetHost;

@end

#endif  // ELECTRON_SHELL_BROWSER_UI_COCOA_KAZE_RENDER_WIDGET_HOST_VIEW_MAC_DELEGATE_H_
