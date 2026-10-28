// Copyright (c) 2026 Kaze.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_KAZE_SHARE_SERVICE_H_
#define ELECTRON_SHELL_BROWSER_KAZE_SHARE_SERVICE_H_

#include "content/public/browser/document_service.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "third_party/blink/public/mojom/webshare/webshare.mojom.h"

namespace content {
class RenderFrameHost;
}

namespace electron {

// Kaze: navigator.share() as Chrome has it on the Mac (its ShareServiceImpl
// with SharingServiceOperation): the system's share menu (NSSharingServicePicker)
// over the page, with the title, text and URL. Files aren't shared yet (Chrome
// stores them to a temporary directory first): such a share is refused as
// Chrome refuses a file type it won't share.
class KazeShareService
    : public content::DocumentService<blink::mojom::ShareService> {
 public:
  static void Create(content::RenderFrameHost* render_frame_host,
                     mojo::PendingReceiver<blink::mojom::ShareService> receiver);

  KazeShareService(const KazeShareService&) = delete;
  KazeShareService& operator=(const KazeShareService&) = delete;
  ~KazeShareService() override;

  // blink::mojom::ShareService:
  void Share(const std::string& title,
             const std::string& text,
             const GURL& share_url,
             std::vector<blink::mojom::SharedFilePtr> files,
             ShareCallback callback) override;

 private:
  KazeShareService(content::RenderFrameHost& render_frame_host,
                   mojo::PendingReceiver<blink::mojom::ShareService> receiver);
};

}  // namespace electron

#endif  // ELECTRON_SHELL_BROWSER_KAZE_SHARE_SERVICE_H_
