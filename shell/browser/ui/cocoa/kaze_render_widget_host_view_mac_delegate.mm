// Copyright (c) 2026 Kaze.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#import "shell/browser/ui/cocoa/kaze_render_widget_host_view_mac_delegate.h"

#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/preloading.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "content/public/browser/web_contents.h"
#import "shell/browser/ui/cocoa/history_swiper.h"
#include "ui/base/window_open_disposition.h"

@interface KazeRenderWidgetHostViewMacDelegate () <HistorySwiperDelegate>
@end

@implementation KazeRenderWidgetHostViewMacDelegate {
  // The widget, by id (as Chrome): the delegate may outlive it.
  int _widgetProcessId;
  int _widgetRoutingId;

  HistorySwiper* __strong _historySwiper;
}

- (instancetype)initWithRenderWidgetHost:
    (content::RenderWidgetHost*)renderWidgetHost {
  self = [super init];
  if (self) {
    _widgetProcessId = renderWidgetHost->GetProcess()->GetDeprecatedID();
    _widgetRoutingId = renderWidgetHost->GetRoutingID();
    _historySwiper = [[HistorySwiper alloc] initWithDelegate:self];
    // The overlay stays within the page's view.
    [self viewThatWantsHistoryOverlay].clipsToBounds = YES;
  }
  return self;
}

- (void)dealloc {
  [_historySwiper setDelegate:nil];
}

- (content::RenderWidgetHost*)renderWidgetHost {
  return content::RenderWidgetHost::FromID(_widgetProcessId, _widgetRoutingId);
}

- (content::WebContents*)webContents {
  content::RenderWidgetHost* renderWidgetHost = self.renderWidgetHost;
  if (!renderWidgetHost) {
    return nullptr;
  }
  content::RenderViewHost* renderViewHost =
      content::RenderViewHost::From(renderWidgetHost);
  if (!renderViewHost) {
    return nullptr;
  }
  return content::WebContents::FromRenderViewHost(renderViewHost);
}

// RenderWidgetHostViewMacDelegate

- (BOOL)handleEvent:(NSEvent*)event {
  return [_historySwiper handleEvent:event];
}

- (void)touchesMovedWithEvent:(NSEvent*)event {
  [_historySwiper touchesMovedWithEvent:event];
}

- (void)touchesBeganWithEvent:(NSEvent*)event {
  [_historySwiper touchesBeganWithEvent:event];
}

- (void)touchesCancelledWithEvent:(NSEvent*)event {
  [_historySwiper touchesCancelledWithEvent:event];
}

- (void)touchesEndedWithEvent:(NSEvent*)event {
  [_historySwiper touchesEndedWithEvent:event];
}

- (void)rendererHandledGestureScrollEvent:(const blink::WebGestureEvent&)event
                                 consumed:(BOOL)consumed {
  [_historySwiper rendererHandledGestureScrollEvent:event consumed:consumed];
}

- (void)rendererHandledOverscrollEvent:(const ui::DidOverscrollParams&)params {
  [_historySwiper onOverscrolled:params];
}

// HistorySwiperDelegate

- (BOOL)shouldAllowHistorySwiping {
  return self.webContents != nullptr;
}

- (NSView*)viewThatWantsHistoryOverlay {
  content::RenderWidgetHost* renderWidgetHost = self.renderWidgetHost;
  if (!renderWidgetHost || !renderWidgetHost->GetView()) {
    return nil;
  }
  return renderWidgetHost->GetView()->GetNativeView().GetNativeNSView();
}

- (BOOL)canNavigateInDirection:(history_swiper::NavigationDirection)direction
                      onWindow:(NSWindow*)window {
  content::WebContents* webContents = self.webContents;
  if (!webContents) {
    return NO;
  }
  content::NavigationController& controller = webContents->GetController();
  return direction == history_swiper::kForwards ? controller.CanGoForward()
                                                : controller.CanGoBack();
}

- (void)navigateInDirection:(history_swiper::NavigationDirection)direction
                   onWindow:(NSWindow*)window {
  content::WebContents* webContents = self.webContents;
  if (!webContents) {
    return;
  }
  content::NavigationController& controller = webContents->GetController();
  if (direction == history_swiper::kForwards) {
    if (controller.CanGoForward()) {
      controller.GoForward();
    }
  } else if (controller.CanGoBack()) {
    controller.GoBack();
  }
}

- (void)backwardsSwipeNavigationLikely {
  content::WebContents* webContents = self.webContents;
  if (!webContents) {
    return;
  }
  webContents->BackNavigationLikely(
      content::preloading_predictor::kBackGestureNavigation,
      WindowOpenDisposition::CURRENT_TAB);
}

@end
