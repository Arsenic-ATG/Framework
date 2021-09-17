#include "web.h"

#include <logging/logger.h>

namespace Framework::GUI {
    bool Web::Init(const std::string &rootPath, bool windowLess) {
        // Create the application instance
        _application = new Integrations::CEF::Application();

        // Prepare the launch arguments
        // TODO: make it UNIX compatible
        CefMainArgs args(0);

        // Prepare the settings
        CefSettings settings;
        CefString(&settings.resources_dir_path)      = rootPath + "\\cef";
        CefString(&settings.log_file)                = rootPath + "\\logs\\cef.txt";
        CefString(&settings.locales_dir_path)        = rootPath + "\\cef\\locales";
        CefString(&settings.cache_path)              = rootPath + "\\cef\\cache";
        CefString(&settings.user_data_path)          = rootPath + "\\cef\\userdata";
        CefString(&settings.browser_subprocess_path) = rootPath + "\\CefWorker.exe"; // TODO: make it UNIX compatible
        settings.multi_threaded_message_loop    = false;
        settings.log_severity                   = LOGSEVERITY_WARNING;
        settings.remote_debugging_port          = 7777;
        settings.windowless_rendering_enabled   = windowLess;
        settings.background_color               = 0x00000000;

        // Initialize the app
        if(!CefInitialize(args, settings, _application, nullptr)){
            //TODO: log it out
            return false;
        }

        // Register the custom URI schemes
        //TODO: implement

        return true;
    }

    bool Web::Shutdown(){
        if(!_application){
            return false;
        }

        CefShutdown();
        return true;
    }

    void Web::Update(){
        if(!_application){
            return;
        }

        CefDoMessageLoopWork();
    }
} // namespace Framework::GUI
