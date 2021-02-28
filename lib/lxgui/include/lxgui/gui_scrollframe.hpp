#ifndef LXGUI_GUI_SCROLLFRAME_HPP
#define LXGUI_GUI_SCROLLFRAME_HPP

#include <lxgui/utils.hpp>
#include "lxgui/gui_frame.hpp"
#include "lxgui/gui_renderer.hpp"

namespace lxgui {
namespace gui
{
    class texture;

    /// A frame with scrollable content
    /** This widget has a special child frame, the scroll child.
    *   The scroll child is rendered on a render_target, which is
    *   then rendered on the screen.
    */
    class scroll_frame : public frame, public renderer
    {
    public :

        /// Constructor.
        explicit scroll_frame(manager* pManager);

        /// Destructor.
        ~scroll_frame() override;

        /// Updates this widget's logic.
        /** \param fDelta Time spent since last update
        *   \note Triggered callbacks could destroy the frame. If you need
        *         to use the frame again after calling this function, use
        *         the helper class alive_checker.
        */
        void update(float fDelta) override;

        /// Copies an uiobject's parameters into this scroll_frame (inheritance).
        /** \param pObj The uiobject to copy
        */
        void copy_from(uiobject* pObj) override;

        /// Returns 'true' if this scroll_frame can use a script.
        /** \param sScriptName The name of the script
        *   \note This method can be overriden if needed.
        */
        bool can_use_script(const std::string& sScriptName) const override;

        /// Calls a script.
        /** \param sScriptName The name of the script
        *   \param pEvent      Stores scripts arguments
        *   \note Triggered callbacks could destroy the frame. If you need
        *         to use the frame again after calling this function, use
        *         the helper class alive_checker.
        */
        void on(const std::string& sScriptName, event* pEvent = nullptr) override;

        /// Sets this scroll_frame's scroll child.
        /** \param pFrame The scroll child
        *   \note Creates the render_target and the associated sprite.
        */
        void set_scroll_child(std::unique_ptr<frame> pFrame);

        /// Returns this scroll_frame's scroll child.
        /** \return This scroll_frame's scroll child
        */
        frame* get_scroll_child();

        /// Sets the horizontal offset of the scroll child.
        /** \param iHorizontalScroll The horizontal offset
        */
        void set_horizontal_scroll(int iHorizontalScroll);

        /// Returns the horizontal offset of the scroll child.
        /** \return The horizontal offset of the scroll child
        */
        int get_horizontal_scroll() const;

        /// Returns the maximum horizontal offset of the scroll child.
        /** \return The maximum horizontal offset of the scroll child
        */
        int get_horizontal_scroll_range() const;

        /// Sets the vertical offset of the scroll child.
        /** \param iVerticalScroll The vertical offset
        */
        void set_vertical_scroll(int iVerticalScroll);

        /// Returns the vertical offset of the scroll child.
        /** \return The vertical offset of the scroll child
        */
        int get_vertical_scroll() const;

        /// Returns the maximum vertical offset of the scroll child.
        /** \return The maximum vertical offset of the scroll child
        */
        int get_vertical_scroll_range() const;

        /// Checks if the provided coordinates are in the scroll_frame.
        /** \param iX           The horizontal coordinate
        *   \param iY           The vertical coordinate
        *   \return 'true' if the provided coordinates are in the scroll_frame
        *   \note The scroll_frame version of this function also checks if the
        *         mouse is over the scroll texture (which means this function
        *         ignores positive hit rect insets).
        *   \note For scroll children to receive input, the scroll_frame must be
        *         keyboard/mouse/wheel enabled.
        */
        bool is_in_frame(int iX, int iY) const override;

        /// Tells this scroll_frame it is being overed by the mouse.
        /** \param bMouseInFrame 'true' if the mouse is above this scroll_frame
        *   \param iX            The horizontal mouse coordinate
        *   \param iY            The vertical mouse coordinate
        */
        void notify_mouse_in_frame(bool bMouseInFrame, int iX, int iY) override;

        /// Tells this renderer that one of its widget requires redraw.
        void fire_redraw(frame_strata mStrata) const override;

        /// Tells this renderer that it should (or not) render another frame.
        /** \param pFrame    The frame to render
        *   \param bRendered 'true' if this renderer needs to render that new object
        */
        void notify_rendered_frame(frame* pFrame, bool bRendered) override;

        /// Returns the width of of this renderer's main render target (e.g., screen).
        /** \return The render target width
        */
        uint get_target_width() const override;

        /// Returns the height of this renderer's main render target (e.g., screen).
        /** \return The render target height
        */
        uint get_target_height() const override;

        /// Returns this widget's Lua glue.
        void create_glue() override;

        /// Parses data from an xml::block.
        /** \param pBlock The scroll_frame's xml::block
        */
        void parse_block(xml::block* pBlock) override;

        /// Registers this widget to the provided lua::state
        static void register_glue(lua::state& mLua);

        static constexpr const char* CLASS_NAME = "ScrollFrame";

    protected :

        virtual void parse_scroll_child_block_(xml::block* pBlock);

        void update_scroll_range_();
        void update_scroll_child_input_();
        void rebuild_scroll_render_target_();
        void render_scroll_strata_list_();

        int iHorizontalScroll_ = 0;
        int iHorizontalScrollRange_ = 0;
        int iVerticalScroll_ = 0;
        int iVerticalScrollRange_ = 0;

        frame* pScrollChild_ = nullptr;

        mutable bool bRebuildScrollRenderTarget_ = false;
        mutable bool bRedrawScrollRenderTarget_ = false;
        mutable bool bUpdateScrollRange_ = false;
        std::shared_ptr<render_target> pScrollRenderTarget_;

        texture* pScrollTexture_ = nullptr;

        bool   bMouseInScrollTexture_ = false;
        frame* pHoveredScrollChild_ = nullptr;
    };

    /** \cond NOT_REMOVE_FROM_DOC
    */

    class lua_scroll_frame : public lua_frame
    {
    public :

        explicit lua_scroll_frame(lua_State* pLua);
        scroll_frame* get_object() { return static_cast<scroll_frame*>(pObject_); }

        // Glues
        int _get_horizontal_scroll(lua_State*);
        int _get_horizontal_scroll_range(lua_State*);
        int _get_scroll_child(lua_State*);
        int _get_vertical_scroll(lua_State*);
        int _get_vertical_scroll_range(lua_State*);
        int _set_horizontal_scroll(lua_State*);
        int _set_scroll_child(lua_State*);
        int _set_vertical_scroll(lua_State*);

        static const char className[];
        static const char* classList[];
        static lua::lunar_binding<lua_scroll_frame> methods[];
    };

    /** \endcond
    */
}
}

#endif
