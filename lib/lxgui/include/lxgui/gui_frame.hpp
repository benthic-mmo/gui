#ifndef LXGUI_GUI_FRAME_HPP
#define LXGUI_GUI_FRAME_HPP

#include "lxgui/gui_region.hpp"
#include "lxgui/gui_backdrop.hpp"
#include "lxgui/gui_layeredregion.hpp"
#include "lxgui/gui_eventreceiver.hpp"

#include <lxgui/utils.hpp>
#include <lxgui/utils_view.hpp>

#include <set>
#include <list>
#include <functional>

namespace lxgui {
namespace gui
{
    class renderer;

    /// Contains gui::layered_region
    struct layer
    {
        layer();

        bool                         bDisabled;
        std::vector<layered_region*> lRegionList;

        static layer_type get_layer_type(const std::string& sLayer);
    };

    /// Basic GUI container
    /** Can contain other frames, or layered_regions
    *   (text, images, ...).
    */
    class frame : public event_receiver, public region
    {
    public :

        /// Type of the frame child list.
        /** \note Constraints on the choice container type:
        *          - must not invalidate iterators on back insertion
        *          - must allow forward iteration
        *          - iterators can be invalidated on removal
        *          - most common use is iteration, not addition or removal
        *          - ordering of elements is irrelevant
        */
        using child_list = std::list<std::unique_ptr<frame>>;
        using child_list_view = utils::view::adaptor<child_list,
            utils::view::unique_ptr_dereferencer,
            utils::view::non_null_filter>;

        /// Type of the region list.
        /** \note Constraints on the choice container type:
        *          - must not invalidate iterators on back insertion
        *          - must allow forward iteration
        *          - iterators can be invalidated on removal
        *          - most common use is iteration, not addition or removal
        *          - ordering of elements is irrelevant
        */
        using region_list = std::list<std::unique_ptr<layered_region>>;
        using region_list_view = utils::view::adaptor<region_list,
            utils::view::unique_ptr_dereferencer,
            utils::view::non_null_filter>;

        /// Constructor.
        explicit frame(manager* pManager);

        /// Destructor.
        ~frame() override;

        /// Renders this widget on the current render target.
        void render() override;

        /// Updates this widget's logic.
        /** \param fDelta Time spent since last update
        *   \note Triggered callbacks could destroy the frame. If you need
        *         to use the frame again after calling this function, use
        *         the helper class alive_checker.
        */
        void update(float fDelta) override;

        /// Prints all relevant information about this widget in a string.
        /** \param sTab The offset to give to all lines
        *   \return All relevant information about this widget
        */
        std::string serialize(const std::string& sTab) const override;

        /// Returns 'true' if this frame can use a script.
        /** \param sScriptName The name of the script
        *   \note This method can be overriden if needed.
        */
        virtual bool can_use_script(const std::string& sScriptName) const;

        /// Checks if this frame's position is valid.
        void check_position() const;

        /// Copies an uiobject's parameters into this frame (inheritance).
        /** \param pObj The uiobject to copy
        */
        void copy_from(uiobject* pObj) override;

        /// Creates a new title region for this frame.
        /** \note You can get it by calling get_title_region().
        */
        void create_title_region();

        /// Disables a layer.
        /** \param mLayerID The id of the layer to disable
        */
        void disable_draw_layer(layer_type mLayerID);

        /// Enables a layer.
        /** \param mLayerID The id of the layer to enable
        */
        void enable_draw_layer(layer_type mLayerID);

        /// Sets if this frame can receive keyboard input.
        /** \param bIsKeyboardEnabled 'true' to enable
        */
        virtual void enable_keyboard(bool bIsKeyboardEnabled);

        /// Sets if this frame can receive mouse input.
        /** \param bIsMouseEnabled 'true' to enable
        *   \param bAllowWorldInput 'true' to allow world input
        */
        virtual void enable_mouse(bool bIsMouseEnabled, bool bAllowWorldInput = false);

        /// Sets if this frame can receive mouse wheel input.
        /** \param bIsMouseWheelEnabled 'true' to enable
        */
        virtual void enable_mouse_wheel(bool bIsMouseWheelEnabled);

        /// Checks if this frame has a script defined.
        /** \param sScriptName The name of the script to check
        *   \return 'true' if this script is defined
        */
        bool has_script(const std::string& sScriptName) const;

        /// Adds a layered_region to this frame's children.
        /** \param pRegion The layered_region to add
        */
        layered_region* add_region(std::unique_ptr<layered_region> pRegion);

        /// Adds a layered_region to this frame's children.
        /** \param pRegion The layered_region to add
        */
        template<typename region_type, typename enable = typename std::enable_if<std::is_base_of<gui::layered_region, region_type>::value>::type>
        region_type* add_region(std::unique_ptr<region_type> pRegion)
        {
            return static_cast<region_type*>(add_region(std::unique_ptr<layered_region>(pRegion.release())));
        }

        /// Removes a layered_region from this frame's children.
        /** \param pRegion The layered_region to remove
        *   \return A unique_ptr to the region, ignore it to destroy the region.
        */
        std::unique_ptr<layered_region> remove_region(layered_region* pRegion);

        /// Creates a new region as child of this frame.
        /** \param mLayer       The layer on which to create the region
        *   \param sClassName   The name of the region class ("FontString", ...)
        *   \param sName        The name of the region
        *   \param lInheritance The objects to inherit from
        *   \return The created region.
        *   \note You don't have the reponsibility to delete this region.
        *         It will be done automatically when its parent is deleted.
        *   \note This function takes care of the basic initializing :
        *         you can directly use the created region.
        */
        layered_region* create_region(
            layer_type mLayer, const std::string& sClassName,
            const std::string& sName, const std::vector<uiobject*>& lInheritance = {}
        );

        /// Creates a new region as child of this frame.
        /** \param mLayer       The layer on which to create the region
        *   \param sName        The name of the region
        *   \param lInheritance The objects to inherit from
        *   \return The created region.
        *   \note You don't have the reponsibility to delete this region.
        *         It will be done automatically when its parent is deleted.
        *   \note This function takes care of the basic initializing :
        *         you can directly use the created region.
        */
        template<typename region_type, typename enable = typename std::enable_if<std::is_base_of<gui::layered_region, region_type>::value>::type>
        region_type* create_region(layer_type mLayer, const std::string& sName, const std::vector<uiobject*>& lInheritance = {})
        {
            return static_cast<region_type*>(create_region(mLayer, region_type::CLASS_NAME, sName, lInheritance));
        }

        /// Creates a new frame as child of this frame.
        /** \param sClassName   The name of the frame class ("Button", ...)
        *   \param sName        The name of the frame
        *   \param lInheritance The objects to inherit from
        *   \return The created frame.
        *   \note You don't have the reponsibility to delete this frame.
        *         It will be done automatically when its parent is deleted.
        *   \note This function takes care of the basic initializing :
        *         you can directly use the created frame. However, you still
        *         need to call notify_loaded() when you are done with any extra
        *         initialization you require on this frame. If you do not,
        *         the frame's OnLoad callback will not fire.
        */
        frame* create_child(const std::string& sClassName, const std::string& sName, const std::vector<uiobject*>& lInheritance = {});

        /// Creates a new frame as child of this frame.
        /** \param sName        The name of the frame
        *   \param lInheritance The objects to inherit from
        *   \return The created frame.
        *   \note You don't have the reponsibility to delete this frame.
        *         It will be done automatically when its parent is deleted.
        *   \note This function takes care of the basic initializing :
        *         you can directly use the created frame.
        */
        template<typename frame_type, typename enable = typename std::enable_if<std::is_base_of<gui::frame, frame_type>::value>::type>
        frame_type* create_child(const std::string& sName, const std::vector<uiobject*>& lInheritance = {})
        {
            return static_cast<frame_type*>(create_child(frame_type::CLASS_NAME, sName, lInheritance));
        }

        /// Adds a frame to this frame's children.
        /** \param pChild The frame to add
        */
        frame* add_child(std::unique_ptr<frame> pChild);

        /// Adds a frame to this frame's children.
        /** \param pChild The frame to add
        */
        template<typename frame_type, typename enable = typename std::enable_if<std::is_base_of<gui::frame, frame_type>::value>::type>
        frame_type* add_child(std::unique_ptr<frame_type> pChild)
        {
            return static_cast<frame_type*>(add_child(std::unique_ptr<frame>(pChild.release())));
        }

        /// Removes a frame from this frame's children.
        /** \param pChild The frame to remove
            \return A unique_ptr to the child, ignore it to destroy the child.
        */
        std::unique_ptr<frame> remove_child(frame* pChild);

        /// Returns the child list.
        /** \return The child list
        */
        child_list_view get_children() const;

        /// Returns one of this frame's children.
        /** \param sName The name of the child
        *   \return One of this frame's children
        *   \note The provided name can either be the full name or the relative name
        *         (i.e. without the "$parent" in front). This function first looks
        *         for matches on the full name, then if no child is found, on the
        *         relative name.
        */
        frame* get_child(const std::string& sName);

        /// Returns one of this frame's children.
        /** \param sName The name of the child
        *   \return One of this frame's children
        *   \note The provided name can either be the full name or the relative name
        *         (i.e. without the "$parent" in front). This function first looks
        *         for matches on the full name, then if no child is found, on the
        *         relative name.
        */
        template<typename frame_type, typename enable = typename std::enable_if<std::is_base_of<gui::frame, frame_type>::value>::type>
        frame_type* get_child(const std::string& sName)
        {
            return get_child(sName)->down_cast<frame_type>();
        }

        /// Returns the region list.
        /** \return The region list
        */
        region_list_view get_regions() const;

        /// Returns one of this frame's region.
        /** \param sName The name of the region
        *   \return One of this frame's region
        *   \note The provided name can either be the full name or the relative name
        *         (i.e. without the "$parent" in front). This function first looks
        *         for matches on the full name, then if no region is found, on the
        *         relative name.
        */
        layered_region* get_region(const std::string& sName);

        /// Returns one of this frame's region.
        /** \param sName The name of the region
        *   \return One of this frame's region
        *   \note The provided name can either be the full name or the relative name
        *         (i.e. without the "$parent" in front). This function first looks
        *         for matches on the full name, then if no region is found, on the
        *         relative name.
        */
        template<typename region_type, typename enable = typename std::enable_if<std::is_base_of<gui::layered_region, region_type>::value>::type>
        region_type* get_region(const std::string& sName)
        {
            return get_region(sName)->down_cast<region_type>();
        }

        /// Calculates effective alpha.
        /** \return Effective alpha (alpha*parent->alpha)
        */
        float get_effective_alpha() const;

        /// Calculates effective scale.
        /** \return Effective scale (scale*parent->scale)
        */
        float get_effective_scale() const;

        /// Returns this frame's level.
        /** \return This frame's level
        */
        int get_level() const;

        /// Returns this frame's strata.
        /** \return This frame's strata
        */
        frame_strata get_frame_strata() const;

        /// Returns this frame's backdrop.
        /** \return This frame's backdrop
        */
        const backdrop* get_backdrop() const;

        /// Returns this frame's backdrop.
        /** \return This frame's backdrop
        */
        backdrop* get_backdrop();

        /// Returns this frame's type.
        /** \return This frame's type (Frame, Slider, ...)
        */
        const std::string& get_frame_type() const;

        /// Returns this frame's absolute hit rect insets.
        /** \return This frame's absolute hit rect insets
        */
        const quad2i& get_abs_hit_rect_insets() const;

        /// Returns this frame's relative hit rect insets.
        /** \return This frame's relative hit rect insets
        */
        const quad2f& get_rel_hit_rect_insets() const;

        /// Returns this frame's max dimensions.
        /** \return This frame's max dimensions
        */
        vector2ui get_max_resize() const;

        /// Returns this frame's min dimensions.
        /** \return This frame's min dimensions
        */
        vector2ui get_min_resize() const;

        /// Returns the number of children of this frame.
        /** \return The number of children of this frame
        */
        uint get_num_children() const;

        /// Returns the number of region of this frame.
        /** \return The number of region of this frame
        */
        uint get_num_regions() const;

        /// Returns this frame's scale.
        /** \return This frame's scale
        *   \note If you want it's true scale on the screen,
        *         use get_effective_scale().
        */
        float get_scale() const;

        /// Returns this frame's title region.
        region* get_title_region() const;

        /// Checks if this frame is clamped to screen.
        /** \return 'true' if this frame is clamed to screen
        */
        bool is_clamped_to_screen() const;

        /// Checks if the provided coordinates are in the frame.
        /** \param iX The horizontal coordinate
        *   \param iY The vertical coordinate
        *   \return 'true' if the provided coordinates are in the frame
        */
        virtual bool is_in_frame(int iX, int iY) const;

        /// Checks if this frame can receive keyboard input.
        /** \return 'true' if this frame can receive keyboard input
        */
        bool is_keyboard_enabled() const;

        /// Checks if this frame can receive mouse input.
        /** \return 'true' if this frame can receive mouse input
        */
        bool is_mouse_enabled() const;

        /// Checks if this frame allows world input.
        /** \return 'true' if this frame allows world input
        */
        bool is_world_input_allowed() const;

        /// Checks if this frame can receive mouse wheel input.
        /** \return 'true' if this frame can receive mouse wheel input
        */
        bool is_mouse_wheel_enabled() const;

        /// Checks if this frame can be moved.
        /** \return 'true' if this frame can be moved
        */
        bool is_movable() const;

        /// Checks if this frame can be resized.
        /** \return 'true' if this frame can be resized
        */
        bool is_resizable() const;

        /// Checks if this frame is at top level.
        /** \return 'true' if this frame is at top level
        */
        bool is_top_level() const;

        /// Checks if this frame has been moved by the user.
        /** \return 'true' if this frame has been moved by the user
        */
        bool is_user_placed() const;

        /// Returns the "adjusted" script name: "OnEvent" becomes "on_event"
        /** \param sScriptName The CamelCase name of the script
        *   \return the snake_case name of the script
        */
        static std::string get_adjusted_script_name(const std::string& sScriptName);

        /// Registers a handler script to this frame.
        /** \param sScriptName The name of the script
        *   \param sContent    The content ot the script
        *   \param sFile       The file in which this script has been found
        *   \param uiLineNbr   The line number at which this script is located in the file
        *   \note The last two informations are required for error messages.
        */
        void define_script(const std::string& sScriptName, const std::string& sContent,
            const std::string& sFile, uint uiLineNbr);

        typedef std::function<void(frame*, event*)> handler;

        /// Registers a handler script to this frame.
        /** \param sScriptName The name of the script
        *   \param mHandler    The handler ot the script
        */
        void define_script(const std::string& sScriptName, handler mHandler);

        /// Tells this frame that a script has been defined.
        /** \param sScriptName The name of the script
        *   \param bDefined    'true' if the script is defined
        */
        void notify_script_defined(const std::string& sScriptName, bool bDefined);

        /// Calls a script.
        /** \param sScriptName The name of the script
        *   \param pEvent      Stores scripts arguments
        *   \note Triggered callbacks could destroy the frame. If you need
        *         to use the frame again after calling this function, use
        *         the helper class alive_checker.
        */
        virtual void on(const std::string& sScriptName, event* pEvent = nullptr);

        /// Calls the on_event script.
        /** \param mEvent The Event that occured
        *   \note Triggered callbacks could destroy the frame. If you need
        *         to use the frame again after calling this function, use
        *         the helper class alive_checker.
        */
        void on_event(const event& mEvent) override;

        /// Tells this frame to react to every event in the game.
        void register_all_events();

        /// Tells this frame to react to a certain event.
        /** \param sEventName The name of the event
        */
        void register_event(const std::string& sEventName) override;

        /// Tells this frame to react to mouse drag.
        /** \param lButtonList The list of mouse button allowed
        */
        void register_for_drag(const std::vector<std::string>& lButtonList);

        /// Sets if this frame is clamped to screen.
        /** \param bIsClampedToScreen 'true' if this frame is clamped to screen
        *   \note If 'true', the frame can't go out of the screen.
        */
        void set_clamped_to_screen(bool bIsClampedToScreen);

        /// Sets this frame's strata.
        /** \param mStrata The new strata
        */
        void set_frame_strata(frame_strata mStrata);

        /// Sets this frame's strata.
        /** \param sStrata The new strata
        */
        void set_frame_strata(const std::string& sStrata);

        /// Sets this frames' backdrop.
        /** \param pBackdrop The new backdrop
        */
        void set_backdrop(std::unique_ptr<backdrop> pBackdrop);

        /// Sets this frame's absolute hit rect insets.
        /** \param iLeft   Offset from the left border
        *   \param iRight  Offset from the right border
        *   \param iTop    Offset from the top border
        *   \param iBottom Offset from the bottom border
        *   \note This is the zone on which you can click.
        */
        void set_abs_hit_rect_insets(int iLeft, int iRight, int iTop, int iBottom);

        /// Sets this frame's absolute hit rect insets.
        /** \param lInsets Offsets
        *   \note This is the zone on which you can click.
        */
        void set_abs_hit_rect_insets(const quad2i& lInsets);

        /// Sets this frame's relative hit rect insets.
        /** \param fLeft   Offset from the left border
        *   \param fRight  Offset from the right border
        *   \param fTop    Offset from the top border
        *   \param fBottom Offset from the bottom border
        *   \note This is the zone on which you can click.
        */
        void set_rel_hit_rect_insets(float fLeft, float fRight, float fTop, float fBottom);

        /// Sets this frame's relative hit rect insets.
        /** \param lInsets Offsets
        *   \note This is the zone on which you can click.
        */
        void set_rel_hit_rect_insets(const quad2f& lInsets);

        /// Sets this frame's level.
        /** \param iLevel The new level
        */
        void set_level(int iLevel);

        /// Sets this frame's maximum size.
        /** \param uiMaxWidth  The maximum width this frame can have
        *   \param uiMaxHeight The maximum height this frame can have
        */
        void set_max_resize(uint uiMaxWidth, uint uiMaxHeight);

        /// Sets this frame's maximum size.
        /** \param mMax The maximum dimensions of this frame
        */
        void set_max_resize(const vector2ui& mMax);

        /// Sets this frame's minimum size.
        /** \param uiMinWidth  The minimum width this frame can have
        *   \param uiMinHeight The minimum height this frame can have
        */
        void set_min_resize(uint uiMinWidth, uint uiMinHeight);

        /// Sets this frame's minimum size.
        /** \param mMin Minimum dimensions of this frame
        */
        void set_min_resize(const vector2ui& mMin);

        /// Sets this frame's maximum height.
        /** \param uiMaxHeight The maximum height this frame can have
        */
        void set_max_height(uint uiMaxHeight);

        /// Sets this frame's maximum width.
        /** \param uiMaxWidth  The maximum width this frame can have
        */
        void set_max_width(uint uiMaxWidth);

        /// Sets this frame's minimum height.
        /** \param uiMinHeight The minimum height this frame can have
        */
        void set_min_height(uint uiMinHeight);

        /// Sets this frame's minimum width.
        /** \param uiMinWidth  The minimum width this frame can have
        */
        void set_min_width(uint uiMinWidth);

        /// Sets if this frame can be moved by the user.
        /** \param bIsMovable 'true' to allow the user to move this frame
        */
        void set_movable(bool bIsMovable);

        /// Changes this widget's parent.
        /** \param pParent The new parent
        *   \note Default is nullptr.<br>
        *         Overrides uiobject's implementation.
        */
        void set_parent(frame* pParent) override;

        /// Removes this widget from its parent and return an owning pointer.
        /** \return An owning pointer to this widget
        */
        std::unique_ptr<uiobject> release_from_parent() override;

        /// Sets if this frame can be resized by the user.
        /** \param bIsResizable 'true' to allow the user to resize this frame
        */
        void set_resizable(bool bIsResizable);

        /// Sets this frame's scale.
        /** \param fScale The new scale
        */
        void set_scale(float fScale);

        /// Sets if this frame is at top level.
        /** \param bIsTopLevel 'true' to put the frame at top level
        */
        void set_top_level(bool bIsTopLevel);

        /// Increases this frame's level so it's the highest of the strata.
        /** \note All its children are raised of the same ammount.
        *   \note Only works for top level frames.
        */
        void raise();

        /// Sets if this frame has been moved by the user.
        /** \param bIsUserPlaced 'true' if this frame has been moved by the user
        */
        void set_user_placed(bool bIsUserPlaced);

        /// Starts moving this frame with the mouse.
        void start_moving();

        /// ends moving this frame.
        void stop_moving();

        /// Starts resizing this frame with the mouse.
        /** \param mPoint The corner to move
        */
        void start_sizing(const anchor_point& mPoint);

        /// ends resizing this frame.
        void stop_sizing();

        /// shows this widget.
        /** \note Its parent must be shown for it to appear on
        *         the screen.
        */
        void show() override;

        /// hides this widget.
        /** \note All its children won't be visible on the screen
        *         anymore, even if they are still marked as shown.
        */
        void hide() override;

        /// shows/hides this widget.
        /** \param bIsShown 'true' if you want to show this widget
        *   \note See show() and hide() for more infos.
        *   \note Contrary to show() and hide(), this function doesn't
        *         trigger any event ("OnShow" or "OnHide"). It should
        *         only be used to set the initial state of the widget.
        */
        void set_shown(bool bIsShown) override;

        /// Flags this object as rendered by another object.
        /** \param pRenderer The object that will take care of rendering this widget
        *   \note By default, objects are rendered by the gui::manager.
        *   \note The renderer also takes care of providing inputs.
        *   \note If the renderer is set to nullptr, the frame will inherit the renderer of its
        *         parent. If the frame has no parent, this will default to the gui::manager.
        */
        void set_renderer(renderer* pRenderer);

        /// Returns the renderer of this object, nullptr if none.
        /** \return The renderer of this object, nullptr if none
        *   \note For more informations, see set_renderer().
        */
        const renderer* get_renderer() const;

        /// Returns the renderer of this object or its parents, nullptr if none.
        /** \return The renderer of this object or its parents, nullptr if none
        *   \note For more informations, see set_renderer().
        */
        renderer* get_top_level_renderer() override;

        /// Returns the renderer of this object or its parents, nullptr if none.
        /** \return The renderer of this object or its parents, nullptr if none
        *   \note For more informations, see set_renderer().
        */
        const renderer* get_top_level_renderer() const override;

        /// Notifies the renderer of this widget that it needs to be redrawn.
        /** \note Automatically called by any shape changing function.
        */
        void notify_renderer_need_redraw() const override;

        /// Changes this widget's absolute dimensions (in pixels).
        /** \param uiAbsWidth  The new width
        *   \param uiAbsHeight The new height
        */
        void set_abs_dimensions(uint uiAbsWidth, uint uiAbsHeight) override;

        /// Changes this widget's absolute width (in pixels).
        /** \param uiAbsWidth The new width
        */
        void set_abs_width(uint uiAbsWidth) override;

        /// Changes this widget's absolute height (in pixels).
        /** \param uiAbsHeight The new height
        */
        void set_abs_height(uint uiAbsHeight) override;

        /// Tells this frame it is being overed by the mouse.
        /** \param bMouseInFrame 'true' if the mouse is above this frame
        *   \param iX            The horizontal mouse coordinate
        *   \param iY            The vertical mouse coordinate
        *   \note Always use the mouse position set by this function and
        *         not the one returned by the InputManager, because there
        *         can be an offset to apply (for example with ScrollFrame).
        */
        virtual void notify_mouse_in_frame(bool bMouseInFrame, int iX, int iY);

        /// Notifies this widget that it has been fully loaded.
        /** \note Calls the "OnLoad" script.
        */
        void notify_loaded() override;

        /// Tells this frame to rebuilt the layer list.
        /** \note Automatically called by add_region(), remove_region(), and
        *         layered_region::set_draw_layer().
        */
        void fire_build_layer_list();

        /// Tells the frame not to react to all events.
        void unregister_all_events();

        /// Tells the frame not to react to a certain event.
        /** \param sEventName The name of the event
        */
        void unregister_event(const std::string& sEventName) override;

        /// Sets the addon this frame belongs to.
        /** \param pAddOn The addon this frame belongs to
        */
        void set_addon(addon* pAddOn);

        /// Returns this frame's addon.
        /** \return This frame's addon
        *   \note Returns "nullptr" if the frame has been created
        *         by Lua code and wasn't assigned a parent.
        */
        addon* get_addon() const;

        /// Creates the associated Lua glue.
        void create_glue() override;

        /// Parses data from an xml::block.
        /** \param pBlock The frame's xml::block
        */
        void parse_block(xml::block* pBlock) override;

        static constexpr const char* CLASS_NAME = "Frame";

    protected :

        // XML parsing
        void parse_attributes_(xml::block* pBlock) override;
        virtual void parse_resize_bounds_block_(xml::block* pBlock);
        virtual void parse_title_region_block_(xml::block* pBlock);
        virtual void parse_backdrop_block_(xml::block* pBlock);
        virtual void parse_hit_rect_insets_block_(xml::block* pBlock);
        virtual void parse_layers_block_(xml::block* pBlock);
        virtual void parse_frames_block_(xml::block* pBlock);
        virtual void parse_scripts_block_(xml::block* pBlock);

        virtual void notify_visible_(bool bTriggerEvents = true);
        virtual void notify_invisible_(bool bTriggerEvents = true);

        virtual void notify_top_level_parent_(bool bTopLevel, frame* pParent);

        void add_level_(int iAmount);

        void propagate_renderer_(bool bRendered);

        void update_borders_() const override;

        struct script_info
        {
            std::string sFile;
            uint        uiLineNbr;
        };

        child_list  lChildList_;
        region_list lRegionList_;

        std::map<layer_type, layer>                lLayerList_;
        std::map<std::string, std::string>         lDefinedScriptList_;
        std::map<std::string, script_info>         lXMLScriptInfoList_;
        std::vector<std::string>                   lQueuedEventList_;
        std::set<std::string>                      lRegEventList_;
        std::set<std::string>                      lRegDragList_;

        std::map<std::string, handler> lDefinedHandlerList_;

        addon* pAddOn_ = nullptr;

        int iLevel_ = 0;

        frame_strata mStrata_ = frame_strata::MEDIUM;
        bool         bIsTopLevel_ = false;
        frame*       pTopLevelParent_ = nullptr;
        renderer*    pRenderer_ = nullptr;

        std::unique_ptr<backdrop> pBackdrop_;

        bool bHasAllEventsRegistred_ = false;

        bool bIsKeyboardEnabled_ = false;
        bool bIsMouseEnabled_ = false;
        bool bAllowWorldInput_ = false;
        bool bIsMouseWheelEnabled_ = false;
        bool bIsMovable_ = false;
        bool bIsClampedToScreen_ = false;
        bool bIsResizable_ = false;
        bool bIsUserPlaced_ = false;

        bool bBuildLayerList_ = false;

        quad2i lAbsHitRectInsetList_ = quad2i::ZERO;
        quad2f lRelHitRectInsetList_ = quad2f::ZERO;

        uint uiMinWidth_ = 0u;
        uint uiMaxWidth_ = uint(-1);
        uint uiMinHeight_ = 0u;
        uint uiMaxHeight_ = uint(-1);

        uint uiOldWidth_ = 0u;
        uint uiOldHeight_ = 0u;

        float fScale_ = 1.0f;

        bool bMouseInFrame_ = false;
        bool bMouseInTitleRegion_ = false;
        int  iMousePosX_ = 0, iMousePosY_ = 0;

        std::unique_ptr<region> pTitleRegion_ = nullptr;

        std::vector<std::string> lMouseButtonList_;
        bool                     bMouseDragged_ = false;
    };

    /** \cond NOT_REMOVE_FROM_DOC
    */

    class lua_frame : public lua_uiobject
    {
    public :

        explicit lua_frame(lua_State* pLua);
        frame* get_object() { return static_cast<frame*>(pObject_); }

        int _create_font_string(lua_State*);
        int _create_texture(lua_State*);
        int _create_title_region(lua_State*);
        int _disable_draw_layer(lua_State*);
        int _enable_draw_layer(lua_State*);
        int _enable_keyboard(lua_State*);
        int _enable_mouse(lua_State*);
        int _enable_mouse_wheel(lua_State*);
        int _get_backdrop(lua_State*);
        int _get_backdrop_border_color(lua_State*);
        int _get_backdrop_color(lua_State*);
        int _get_children(lua_State*);
        int _get_effective_alpha(lua_State*);
        int _get_effective_scale(lua_State*);
        int _get_frame_level(lua_State*);
        int _get_frame_strata(lua_State*);
        int _get_frame_type(lua_State*);
        int _get_hit_rect_insets(lua_State*);
        int _get_id(lua_State*);
        int _get_max_resize(lua_State*);
        int _get_min_resize(lua_State*);
        int _set_max_width(lua_State*);
        int _set_max_height(lua_State*);
        int _set_min_width(lua_State*);
        int _set_min_height(lua_State*);
        int _get_num_children(lua_State*);
        int _get_num_regions(lua_State*);
        int _get_scale(lua_State*);
        int _get_script(lua_State*);
        int _get_title_region(lua_State*);
        int _has_script(lua_State*);
        int _is_clamped_to_screen(lua_State*);
        int _is_frame_type(lua_State*);
        int _is_keyboard_enabled(lua_State*);
        int _is_mouse_enabled(lua_State*);
        int _is_mouse_wheel_enabled(lua_State*);
        int _is_movable(lua_State*);
        int _is_resizable(lua_State*);
        int _is_top_level(lua_State*);
        int _is_user_placed(lua_State*);
        int _on(lua_State*);
        int _raise(lua_State*);
        int _register_all_events(lua_State*);
        int _register_event(lua_State*);
        int _register_for_drag(lua_State*);
        int _set_backdrop(lua_State*);
        int _set_backdrop_border_color(lua_State*);
        int _set_backdrop_color(lua_State*);
        int _set_clamped_to_screen(lua_State*);
        int _set_frame_level(lua_State*);
        int _set_frame_strata(lua_State*);
        int _set_hit_rect_insets(lua_State*);
        int _set_max_resize(lua_State*);
        int _set_min_resize(lua_State*);
        int _set_movable(lua_State*);
        int _set_resizable(lua_State*);
        int _set_scale(lua_State*);
        int _set_script(lua_State*);
        int _set_top_level(lua_State*);
        int _set_user_placed(lua_State*);
        int _start_moving(lua_State*);
        int _start_sizing(lua_State*);
        int _stop_moving_or_sizing(lua_State*);
        int _unregister_all_events(lua_State*);
        int _unregister_event(lua_State*);

        static const char className[];
        static const char* classList[];
        static lua::lunar_binding<lua_frame> methods[];
    };

    /** \endcond
    */
}
}

#endif
