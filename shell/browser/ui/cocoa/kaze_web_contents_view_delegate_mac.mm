// Copyright (c) 2026 Kaze.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#import "shell/browser/ui/cocoa/kaze_web_contents_view_delegate_mac.h"

#include "content/public/browser/web_contents_view_delegate.h"
#import "shell/browser/ui/cocoa/kaze_render_widget_host_view_mac_delegate.h"

namespace electron {

namespace {

class KazeWebContentsViewDelegateMac : public content::WebContentsViewDelegate {
 public:
  // content::WebContentsViewDelegate:
  NSObject<RenderWidgetHostViewMacDelegate>* GetDelegateForHost(
      content::RenderWidgetHost* render_widget_host,
      bool is_popup) override {
    // As Chrome: popups (a <select>'s list) don't overscroll.
    if (is_popup) {
      return nil;
    }
    return [[KazeRenderWidgetHostViewMacDelegate alloc]
        initWithRenderWidgetHost:render_widget_host];
  }
};

}  // namespace

std::unique_ptr<content::WebContentsViewDelegate>
CreateKazeWebContentsViewDelegate() {
  return std::make_unique<KazeWebContentsViewDelegateMac>();
}

}  // namespace electron
