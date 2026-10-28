// Copyright (c) 2026 Kaze.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/kaze_share_service.h"

#import <AppKit/AppKit.h>
#import <objc/runtime.h>

#include <utility>

#include "base/strings/sys_string_conversions.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/web_contents.h"
#include "services/network/public/mojom/permissions_policy/permissions_policy_feature.mojom.h"
#include "ui/gfx/native_ui_types.h"
#include "url/gurl.h"

// Pickers showing now, kept alive until they answer (AppKit holds neither the
// picker nor its delegate strongly).
static NSMutableSet* ShowingPickers() {
  static NSMutableSet* pickers = [NSMutableSet set];
  return pickers;
}

// The picker's answer: a service chosen (shared) or none (cancelled), once.
@interface KazeSharePickerDelegate : NSObject <NSSharingServicePickerDelegate>
- (instancetype)initWithCallback:
    (blink::mojom::ShareService::ShareCallback)callback;
@end

@implementation KazeSharePickerDelegate {
  blink::mojom::ShareService::ShareCallback _callback;
}

- (instancetype)initWithCallback:
    (blink::mojom::ShareService::ShareCallback)callback {
  if ((self = [super init]))
    _callback = std::move(callback);
  return self;
}

- (void)sharingServicePicker:(NSSharingServicePicker*)picker
     didChooseSharingService:(NSSharingService*)service {
  if (_callback) {
    std::move(_callback).Run(service ? blink::mojom::ShareError::OK
                                     : blink::mojom::ShareError::CANCELED);
  }
  [ShowingPickers() removeObject:picker];
}

@end

namespace electron {

// static
void KazeShareService::Create(
    content::RenderFrameHost* render_frame_host,
    mojo::PendingReceiver<blink::mojom::ShareService> receiver) {
  CHECK(render_frame_host);
  // The renderer refuses fenced frames itself; one that asks anyway gets no service.
  if (render_frame_host->IsNestedWithinFencedFrame())
    return;
  new KazeShareService(*render_frame_host, std::move(receiver));
}

KazeShareService::KazeShareService(
    content::RenderFrameHost& render_frame_host,
    mojo::PendingReceiver<blink::mojom::ShareService> receiver)
    : content::DocumentService<blink::mojom::ShareService>(
          render_frame_host,
          std::move(receiver)) {}

KazeShareService::~KazeShareService() = default;

void KazeShareService::Share(const std::string& title,
                             const std::string& text,
                             const GURL& share_url,
                             std::vector<blink::mojom::SharedFilePtr> files,
                             ShareCallback callback) {
  // As Chrome: only where the page's permissions policy allows web-share.
  if (!render_frame_host().IsFeatureEnabled(
          network::mojom::PermissionsPolicyFeature::kWebShare)) {
    std::move(callback).Run(blink::mojom::ShareError::PERMISSION_DENIED);
    return;
  }
  if (!files.empty()) {
    std::move(callback).Run(blink::mojom::ShareError::PERMISSION_DENIED);
    return;
  }
  auto* web_contents =
      content::WebContents::FromRenderFrameHost(&render_frame_host());
  NSView* view = web_contents ? web_contents->GetNativeView().GetNativeNSView()
                              : nil;
  if (!view || !view.window) {
    std::move(callback).Run(blink::mojom::ShareError::INTERNAL_ERROR);
    return;
  }

  NSMutableArray* items = [NSMutableArray array];
  if (!title.empty())
    [items addObject:base::SysUTF8ToNSString(title)];
  if (!text.empty())
    [items addObject:base::SysUTF8ToNSString(text)];
  if (share_url.is_valid()) {
    NSURL* url = [NSURL URLWithString:base::SysUTF8ToNSString(share_url.spec())];
    if (url)
      [items addObject:url];
  }

  NSSharingServicePicker* picker =
      [[NSSharingServicePicker alloc] initWithItems:items];
  KazeSharePickerDelegate* delegate =
      [[KazeSharePickerDelegate alloc] initWithCallback:std::move(callback)];
  picker.delegate = delegate;
  // The picker holds its delegate weakly: the delegate goes with the picker,
  // which is kept until it answers.
  objc_setAssociatedObject(picker, @selector(delegate), delegate,
                           OBJC_ASSOCIATION_RETAIN_NONATOMIC);
  [ShowingPickers() addObject:picker];
  // Over the page's top centre, where Chrome's anchors near the omnibox.
  NSRect bounds = view.bounds;
  NSRect anchor = NSMakeRect(NSMidX(bounds) - 1,
                             view.isFlipped ? NSMinY(bounds) : NSMaxY(bounds) - 2,
                             2, 2);
  [picker showRelativeToRect:anchor ofView:view preferredEdge:NSRectEdgeMinY];
}

}  // namespace electron
