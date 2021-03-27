#include <lxgui/gui_manager.hpp>
#include <lxgui/gui_texture.hpp>
#include <lxgui/gui_fontstring.hpp>
#include <lxgui/gui_button.hpp>
#include <lxgui/gui_slider.hpp>
#include <lxgui/gui_editbox.hpp>
#include <lxgui/gui_scrollframe.hpp>
#include <lxgui/gui_statusbar.hpp>
#include <lxgui/gui_event.hpp>
#include <lxgui/gui_out.hpp>
#include <lxgui/luapp_state.hpp>
#include <lxgui/luapp_function.hpp>
#include <lxgui/utils_filesystem.hpp>
#include <lxgui/utils_string.hpp>

#include <sol/state.hpp>

#include <SFML/Window.hpp>

//#define GL_GUI

#ifdef GL_GUI
    #include <lxgui/impl/gui_gl_renderer.hpp>
    #ifdef MACOSX
        #include <OpenGL/gl.h>
    #else
        #include <GL/gl.h>
    #endif
#else
    #include <lxgui/impl/gui_sfml.hpp>
    #include <SFML/Graphics/RenderWindow.hpp>
#endif

#include <lxgui/impl/input_sfml_source.hpp>

#ifdef WIN32
    #include <windows.h>
    #ifdef MSVC
        #pragma comment(linker, "/entry:mainCRTStartup")
    #endif
#endif

#include <fstream>
#include <plog/Log.h>
#include "plog/Initializers/RollingFileInitializer.h"

#define PROJECTNAME "unnamed-mmo"

using namespace lxgui;

// default window size
int uiWindowWidth = 800;
int uiWindowHeight = 600;
bool bFullScreen = false; 
bool bWindowedMode = false;
std::string sLocale = "enGB";
bool enableCaching = true;

const char* BoolToString(bool b)
{
  return b ? "true" : "false";
}

int main(int argc, char* argv[]){
    // initialize logging 
    plog::init(plog::debug, "logs/cout.txt");

       try{
        // Read the configuration data
        if (utils::file_exists("config.lua")){ 
            LOGD << "Reading from config.lua";
            lua::state mLua; 
            try{
                mLua.do_file("config.lua");
                uiWindowWidth  = mLua.get_global_int("window_width",  false, uiWindowWidth);
                uiWindowHeight = mLua.get_global_int("window_height", false, uiWindowHeight); 
                bFullScreen    = mLua.get_global_bool("fullscreen",   false, bFullScreen);
                bWindowedMode  = mLua.get_global_bool("windowed_mode", false, bWindowedMode);
                sLocale        = mLua.get_global_string("locale",     false, sLocale);
                enableCaching  = mLua.get_global_bool("enable_caching", false, enableCaching);
            }catch (...){
                LOGD << "Error reading config file. using defaults";
            }
        }else{
            // write config.lua file with defaults
            LOGD << "Config file missing, creating config from defaults";
            std::ofstream outfile("config.lua");
            outfile << "locale = " + sLocale + ";" << std::endl; 
            outfile << "window_width = " + std::to_string(uiWindowWidth) +";"<< std::endl;
            outfile << "window_height = " + std::to_string(uiWindowHeight) + ";" << std::endl;
            outfile << "fullscreen = " + std::string(BoolToString(bFullScreen)) + ";" << std::endl; 
            outfile << "windowed_mode = " + std::string(BoolToString(bWindowedMode)) + ";" << std::endl;
            outfile << "enable_caching = " + std::string(BoolToString(enableCaching)) + ";" << std::endl;
            outfile.close();
        } 

        // Create a window
        LOGD << "Creating window....";
    #ifdef GL_GUI
        sf::Window mWindow;
    #else
        sf::RenderWindow mWindow;
    #endif
        // set ui window size to max if windowed or fullscreen is enabled
        if (bWindowedMode or bFullScreen){
            uiWindowWidth = sf::VideoMode::getDesktopMode().width;
            uiWindowHeight = sf::VideoMode::getDesktopMode().height;
        }
        if (bFullScreen){
            mWindow.create(sf::VideoMode(uiWindowWidth, uiWindowHeight, 32), PROJECTNAME, sf::Style::Fullscreen);
        }
        else{
            mWindow.create(sf::VideoMode(uiWindowWidth, uiWindowHeight, 32), PROJECTNAME);
        }

        // Initialize the gui
        LOGD << "Creating gui manager....";
        std::unique_ptr<gui::manager> pManager;

    #ifdef GL_GUI
        LOGD << "Using GL for gui";
        // Define the GUI renderer
        std::unique_ptr<gui::renderer_impl> pRendererImpl =
            std::unique_ptr<gui::renderer_impl>(new gui::gl::renderer());

        // Define the input manager
        std::unique_ptr<input::source_impl> pInputSource;
        // Use SFML (only implementation available for now)
        pInputSource = std::unique_ptr<input::source_impl>(new input::sfml::source(mWindow));

        pManager = std::unique_ptr<gui::manager>(new gui::manager(
            // Provide the input source
            std::move(pInputSource),
            // The locale
            sLocale,
            // Dimensions of the render window
            mWindow.getSize().x, mWindow.getSize().y,
            // Provide the GUI renderer implementation
            std::move(pRendererImpl)
        ));
    #else
        LOGD << "Using sfml for gui";
        // Use full SFML implementation
        pManager = gui::sfml::create_manager(mWindow, sLocale);
    #endif

        pManager->enable_caching(enableCaching);

        // Load files :
        //  - first set the directory in which the interface is located
        pManager->add_addon_directory("interface");
        //  - create the lua::state
        LOGD << "Creating lua state...";
        pManager->create_lua([](gui::manager& mManager) {
            // We use a lambda function because this code might be called
            // again later on, for example when one reloads the GUI (the
            // lua state is destroyed and created again).
            //  - register the needed widgets
            mManager.register_region_type<gui::texture>();
            mManager.register_region_type<gui::font_string>();
            mManager.register_frame_type<gui::button>();
            mManager.register_frame_type<gui::slider>();
            mManager.register_frame_type<gui::edit_box>();
            mManager.register_frame_type<gui::scroll_frame>();
            mManager.register_frame_type<gui::status_bar>();
            //  - register additional lua functions
            sol::state& mSol = mManager.get_lua();
            mSol.set_function("get_folder_list", [](const std::string& sDir) {
                return utils::get_directory_list(sDir);
            });
            mSol.set_function("get_file_list", [](const std::string& sDir) {
                return utils::get_file_list(sDir);
            });
        });

        //  - and load all files
        LOGD << "Reading gui files..."; 
        try{
        pManager->read_files();
        } catch (...) {
            LOGE << "file read failed";

        } 
        
        // Start the main loop
        bool bRunning = true;
        bool bFocus = true;
        float fDelta = 0.1f;
        sf::Clock mClock, mPerfClock;
        uint uiFrameCount = 0;

        input::manager* pInputMgr = pManager->get_input_manager();

        LOGD << "Entering main loop";
        while (bRunning)
        {
            // Get events from SFML
            sf::Event mEvent;
            while (mWindow.pollEvent(mEvent))
            {
                if (mEvent.type      == sf::Event::Closed)
                    bRunning = false;
                else if (mEvent.type == sf::Event::LostFocus)
                    bFocus = false;
                else if (mEvent.type == sf::Event::GainedFocus)
                    bFocus = true;
                else if (mEvent.type == sf::Event::KeyReleased)
                {
                    switch (mEvent.key.code)
                    {
                        case sf::Keyboard::Key::Escape:
                            bRunning = false;
                            break;
                        case sf::Keyboard::Key::P:
                            gui::out << pManager->print_ui() << std::endl;
                            break;
                        case sf::Keyboard::Key::K:
                            gui::out << "###" << std::endl;
                            break;
                        case sf::Keyboard::Key::C:
                            pManager->enable_caching(!pManager->is_caching_enabled());
                            break;
                        case sf::Keyboard::Key::R:
                            pManager->reload_ui();
                            break;
                        default:
                            break;
                    }
                }
                static_cast<input::sfml::source*>(pInputMgr->get_source())->on_sfml_event(mEvent);
            }

            // Check if WORLD input is allowed
            if (pInputMgr->can_receive_input("WORLD"))
            {
                // Process mouse and click events in the game...
            }

            if (!bFocus)
            {
                sf::sleep(sf::seconds(0.1f));
                continue;
            }

            // Update the gui
            pManager->update(fDelta);

            // Clear the window
        #ifdef GL_GUI
            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        #else
            mWindow.clear(sf::Color(51,51,51));
        #endif

            // Render the gui
            pManager->render_ui();

            // Display the window
            mWindow.display();

            fDelta = mClock.getElapsedTime().asSeconds();
            mClock.restart();

            ++uiFrameCount;
        }
        LOGD << "End of loop, mean FPS : " << uiFrameCount/mPerfClock.getElapsedTime().asSeconds();
    }
    catch (const std::exception& e)
    {
        LOGE << e.what();
        return 1;
    }
    catch (...)
    {
        LOGE << "Unhandled exception !";
        return 1;
    }

    LOGD << "End of program.";
    return 0;
}

