/*
 * MafiaHub OSS license
 * Copyright (c) 2022, MafiaHub. All rights reserved.
 *
 * This file comes from MafiaHub, hosted at https://github.com/MafiaHub/Framework.
 * See LICENSE file in the source repository for information regarding licensing.
 */

#pragma once

#include <string>

#include <external/cef/application.h>
#include "../api/backend.h"

namespace Framework::GUI {
    class CEF: public Backend {
      private:
        CefRefPtr<External::CEF::Application> _application;

      public:
        bool Init(const std::string &, bool windowLess = true);
        bool Shutdown();

        void Update();
    };
} // namespace Framework::GUI
