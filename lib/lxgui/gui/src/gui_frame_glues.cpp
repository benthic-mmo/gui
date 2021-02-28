#include "lxgui/gui_frame.hpp"
#include "lxgui/gui_backdrop.hpp"
#include "lxgui/gui_region.hpp"
#include "lxgui/gui_fontstring.hpp"
#include "lxgui/gui_texture.hpp"
#include "lxgui/gui_manager.hpp"
#include "lxgui/gui_out.hpp"

#include <lxgui/luapp_state.hpp>
#include <lxgui/luapp_function.hpp>

namespace lxgui {
namespace gui
{
lua_frame::lua_frame(lua_State* pLua) : lua_uiobject(pLua)
{
}

int lua_frame::_create_font_string(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:create_font_string", pLua, 1);
    mFunc.add(0, "name", lua::type::STRING);
    mFunc.add(1, "layer", lua::type::STRING, true);
    mFunc.add(2, "inherits", lua::type::STRING, true);

    if (mFunc.check())
    {
        std::string sName = mFunc.get(0)->get_string();

        layer_type mLayer;
        if (mFunc.is_provided(1) && mFunc.get(1)->get_type() == lua::type::STRING)
            mLayer = layer::get_layer_type(mFunc.get(1)->get_string());
        else
            mLayer = layer_type::ARTWORK;

        std::string sInheritance;
        if (mFunc.is_provided(2))
            sInheritance = mFunc.get(2)->get_string();

        region* pRegion = get_object()->create_region(
            mLayer, "FontString", sName,
            get_object()->get_manager()->get_virtual_uiobject_list(sInheritance)
        );

        if (pRegion)
        {
            pRegion->push_on_lua(mFunc.get_state());
            mFunc.notify_pushed();
        }
        else
            mFunc.push_nil();
    }

    return mFunc.on_return();
}

int lua_frame::_create_texture(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:create_texture", pLua, 1);
    mFunc.add(0, "name", lua::type::STRING);
    mFunc.add(1, "layer", lua::type::STRING, true);
    mFunc.add(2, "inherits", lua::type::STRING, true);

    if (mFunc.check())
    {
        std::string sName = mFunc.get(0)->get_string();

        layer_type mLayer;
        if (mFunc.is_provided(1) && mFunc.get(1)->get_type() == lua::type::STRING)
            mLayer = layer::get_layer_type(mFunc.get(1)->get_string());
        else
            mLayer = layer_type::ARTWORK;

        std::string sInheritance;
        if (mFunc.is_provided(2))
            sInheritance = mFunc.get(2)->get_string();

        region* pRegion = get_object()->create_region(
            mLayer, "Texture", sName,
            get_object()->get_manager()->get_virtual_uiobject_list(sInheritance)
        );

        if (pRegion)
        {
            pRegion->push_on_lua(mFunc.get_state());
            mFunc.notify_pushed();
        }
        else
            mFunc.push_nil();
    }

    return mFunc.on_return();
}

int lua_frame::_create_title_region(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:create_title_region", pLua);

    get_object()->create_title_region();

    return mFunc.on_return();
}

int lua_frame::_disable_draw_layer(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:disable_draw_layer", pLua);
    mFunc.add(0, "layer", lua::type::STRING);
    if (mFunc.check())
    {
        std::string sLayer = mFunc.get(0)->get_string();
        if (sLayer == "BACKGROUND")
            get_object()->disable_draw_layer(layer_type::BACKGROUND);
        else if (sLayer == "BORDER")
            get_object()->disable_draw_layer(layer_type::BORDER);
        else if (sLayer == "ARTWORK")
            get_object()->disable_draw_layer(layer_type::ARTWORK);
        else if (sLayer == "OVERLAY")
            get_object()->disable_draw_layer(layer_type::OVERLAY);
        else if (sLayer == "HIGHLIGHT")
            get_object()->disable_draw_layer(layer_type::HIGHLIGHT);
        else
            gui::out << gui::warning << mFunc.get_name() << " : Unknown layer : \"" << sLayer << "\"." << std::endl;
    }

    return mFunc.on_return();
}

int lua_frame::_enable_draw_layer(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:enable_draw_layer", pLua);
    mFunc.add(0, "layer", lua::type::STRING);
    if (mFunc.check())
    {
        std::string sLayer = mFunc.get(0)->get_string();
        if (sLayer == "BACKGROUND")
            get_object()->enable_draw_layer(layer_type::BACKGROUND);
        else if (sLayer == "BORDER")
            get_object()->enable_draw_layer(layer_type::BORDER);
        else if (sLayer == "ARTWORK")
            get_object()->enable_draw_layer(layer_type::ARTWORK);
        else if (sLayer == "OVERLAY")
            get_object()->enable_draw_layer(layer_type::OVERLAY);
        else if (sLayer == "HIGHLIGHT")
            get_object()->enable_draw_layer(layer_type::HIGHLIGHT);
        else
            gui::out << gui::warning << mFunc.get_name() << " : Unknown layer : \"" << sLayer << "\"." << std::endl;
    }

    return mFunc.on_return();
}

int lua_frame::_enable_keyboard(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:enable_keyboard", pLua);
    mFunc.add(0, "is keyboard enable", lua::type::BOOLEAN);
    if (mFunc.check())
        get_object()->enable_keyboard(mFunc.get(0)->get_bool());

    return mFunc.on_return();
}

int lua_frame::_enable_mouse(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:enable_mouse", pLua);
    mFunc.add(0, "is mouse enabled", lua::type::BOOLEAN);
    mFunc.add(1, "is world input allowed", lua::type::BOOLEAN, true);
    if (mFunc.check())
    {
        if (mFunc.is_provided(1))
            get_object()->enable_mouse(mFunc.get(0)->get_bool(), mFunc.get(1)->get_bool());
        else
            get_object()->enable_mouse(mFunc.get(0)->get_bool());
    }

    return mFunc.on_return();
}

int lua_frame::_enable_mouse_wheel(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:enable_mouse_wheel", pLua);
    mFunc.add(0, "is mouse wheel enabled", lua::type::BOOLEAN);
    if (mFunc.check())
        get_object()->enable_mouse_wheel(mFunc.get(0)->get_bool());

    return mFunc.on_return();
}

int lua_frame::_get_backdrop(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:get_backdrop", pLua, 1);

    backdrop* pBackdrop = get_object()->get_backdrop();
    if (pBackdrop)
    {
        lua::state& mState = mFunc.get_state();

        mState.new_table();
        mState.set_field_string("bgFile", pBackdrop->get_background_file());
        mState.set_field_string("edgeFile", pBackdrop->get_edge_file());
        mState.set_field_bool("tile", pBackdrop->is_background_tilling());

        mState.set_field_int("tileSize", pBackdrop->get_tile_size());
        mState.set_field_int("edgeSize", pBackdrop->get_edge_size());

        mState.new_table();
        mState.set_field("insets");
        mState.get_field("insets");

        const quad2i& lInsets = pBackdrop->get_background_insets();
        mState.set_field_int("left",   lInsets.left);
        mState.set_field_int("right",  lInsets.right);
        mState.set_field_int("top",    lInsets.top);
        mState.set_field_int("bottom", lInsets.bottom);

        mState.pop();

        mFunc.notify_pushed();
    }
    else
        mFunc.push_nil();

    return mFunc.on_return();
}

int lua_frame::_get_backdrop_border_color(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:get_backdrop_border_color", pLua, 4);

    if (get_object()->get_backdrop())
    {
        const color& mColor = get_object()->get_backdrop()->get_edge_color();
        mFunc.push(mColor.r);
        mFunc.push(mColor.g);
        mFunc.push(mColor.b);
        mFunc.push(mColor.a);
    }
    else
        mFunc.push_nil(4);

    return mFunc.on_return();
}

int lua_frame::_get_backdrop_color(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:get_backdrop_color", pLua, 4);

    if (get_object()->get_backdrop())
    {
        const color& mColor = get_object()->get_backdrop()->get_background_color();
        mFunc.push(mColor.r);
        mFunc.push(mColor.g);
        mFunc.push(mColor.b);
        mFunc.push(mColor.a);
    }
    else
        mFunc.push_nil(4);

    return mFunc.on_return();
}

int lua_frame::_get_children(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:get_children", pLua, get_object()->get_num_children());

    for (auto* pChild : get_object()->get_children())
    {
        pChild->push_on_lua(mFunc.get_state());
        mFunc.notify_pushed();
    }

    return mFunc.on_return();
}

int lua_frame::_get_effective_alpha(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:get_effective_alpha", pLua, 1);

    mFunc.push(get_object()->get_effective_alpha());

    return mFunc.on_return();
}

int lua_frame::_get_effective_scale(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:get_effective_scale", pLua, 1);

    mFunc.push(get_object()->get_effective_scale());

    return mFunc.on_return();
}

int lua_frame::_get_frame_level(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:get_frame_level", pLua, 1);

    mFunc.push(get_object()->get_level());

    return mFunc.on_return();
}

int lua_frame::_get_frame_strata(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:get_frame_strata", pLua, 1);

    frame_strata mStrata = get_object()->get_frame_strata();
    std::string sStrata;

    if (mStrata == frame_strata::BACKGROUND)
        sStrata = "BACKGROUND";
    else if (mStrata == frame_strata::LOW)
        sStrata = "LOW";
    else if (mStrata == frame_strata::MEDIUM)
        sStrata = "MEDIUM";
    else if (mStrata == frame_strata::HIGH)
        sStrata = "HIGH";
    else if (mStrata == frame_strata::DIALOG)
        sStrata = "DIALOG";
    else if (mStrata == frame_strata::FULLSCREEN)
        sStrata = "FULLSCREEN";
    else if (mStrata == frame_strata::FULLSCREEN_DIALOG)
        sStrata = "FULLSCREEN_DIALOG";
    else if (mStrata == frame_strata::TOOLTIP)
        sStrata = "TOOLTIP";

    mFunc.push(sStrata);

    return mFunc.on_return();
}

int lua_frame::_get_frame_type(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:get_frame_type", pLua, 1);

    mFunc.push(get_object()->get_frame_type());

    return mFunc.on_return();
}

int lua_frame::_get_hit_rect_insets(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:get_hit_rect_inset", pLua, 4);

    const quad2i& lInsets = get_object()->get_abs_hit_rect_insets();

    mFunc.push(lInsets.left);
    mFunc.push(lInsets.right);
    mFunc.push(lInsets.top);
    mFunc.push(lInsets.bottom);

    return mFunc.on_return();
}

int lua_frame::_get_id(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:get_id", pLua, 1);

    mFunc.push(get_object()->get_id());

    return mFunc.on_return();
}

int lua_frame::_get_max_resize(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:get_max_resize", pLua, 2);

    vector2ui lMax = get_object()->get_max_resize();

    mFunc.push(lMax.x);
    mFunc.push(lMax.y);

    return mFunc.on_return();
}

int lua_frame::_get_min_resize(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:get_min_resize", pLua, 2);

    vector2ui lMin = get_object()->get_min_resize();

    mFunc.push(lMin.x);
    mFunc.push(lMin.y);

    return mFunc.on_return();
}

int lua_frame::_get_num_children(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:get_num_children", pLua, 1);

    mFunc.push(get_object()->get_num_children());

    return mFunc.on_return();
}

int lua_frame::_get_num_regions(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:get_num_regions", pLua, 1);

    mFunc.push(get_object()->get_num_regions());

    return mFunc.on_return();
}

int lua_frame::_get_scale(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:get_scale", pLua, 1);

    mFunc.push(get_object()->get_scale());

    return mFunc.on_return();
}

int lua_frame::_get_script(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:get_script", pLua, 1);
    mFunc.add(0, "script name", lua::type::STRING);
    if (mFunc.check())
    {
        std::string sScriptName = mFunc.get(0)->get_string();
        if (get_object()->has_script(sScriptName))
        {
            lua_getglobal(pLua, (get_object()->get_name() + ":" + sScriptName).c_str());
            mFunc.notify_pushed();
        }
    }

    return mFunc.on_return();
}

int lua_frame::_get_title_region(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:get_title_region", pLua, 1);

    if (get_object()->get_title_region())
    {
        get_object()->get_title_region()->push_on_lua(mFunc.get_state());
        mFunc.notify_pushed();
    }
    else
        mFunc.push_nil();

    return mFunc.on_return();
}

int lua_frame::_has_script(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:has_script", pLua, 1);
    mFunc.add(0, "script name", lua::type::STRING);
    if (mFunc.check())
        mFunc.push(get_object()->can_use_script(mFunc.get(0)->get_string()));

    return mFunc.on_return();
}

int lua_frame::_is_clamped_to_screen(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:is_clamped_to_screen", pLua, 1);

    mFunc.push(get_object()->is_clamped_to_screen());

    return mFunc.on_return();
}

int lua_frame::_is_frame_type(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:is_frame_type", pLua, 1);
    mFunc.add(0, "Frame type", lua::type::STRING);
    if (mFunc.check())
    {
        if (get_object()->get_frame_type() == mFunc.get(0)->get_string())
            mFunc.push(bool(true));
        else
            mFunc.push(bool(false));
    }

    return mFunc.on_return();
}

int lua_frame::_is_keyboard_enabled(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:is_keyboard_enabled", pLua, 1);

    mFunc.push(get_object()->is_keyboard_enabled());

    return mFunc.on_return();
}

int lua_frame::_is_mouse_enabled(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:is_mouse_enabled", pLua, 1);

    mFunc.push(get_object()->is_mouse_enabled());

    return mFunc.on_return();
}

int lua_frame::_is_mouse_wheel_enabled(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:is_mouse_wheel_enabled", pLua, 1);

    mFunc.push(get_object()->is_mouse_wheel_enabled());

    return mFunc.on_return();
}

int lua_frame::_is_movable(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:is_movable", pLua, 1);

    mFunc.push(get_object()->is_movable());

    return mFunc.on_return();
}

int lua_frame::_is_resizable(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:is_resizable", pLua, 1);

    mFunc.push(get_object()->is_resizable());

    return mFunc.on_return();
}

int lua_frame::_is_top_level(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:is_top_level", pLua, 1);

    mFunc.push(get_object()->is_top_level());

    return mFunc.on_return();
}

int lua_frame::_is_user_placed(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:is_user_placed", pLua, 1);

    mFunc.push(get_object()->is_user_placed());

    return mFunc.on_return();
}

int lua_frame::_on(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:on", pLua);
    mFunc.add(0, "script", lua::type::STRING);
    if (mFunc.check())
    {
        std::string sScript = mFunc.get(0)->get_string();
        if (!sScript.empty())
        {
            if ('a' <= sScript[0] && sScript[0] <= 'z')
            {
                sScript[0] = toupper(sScript[0]);
                for (std::string::iterator iter = sScript.begin(); iter != sScript.end(); ++iter)
                {
                    if (*iter == '_')
                    {
                        iter = sScript.erase(iter);
                        if (iter == sScript.end())
                            break;

                        *iter = toupper(*iter);
                    }
                }
            }

            get_object()->on(sScript);
        }
    }

    return mFunc.on_return();
}

int lua_frame::_raise(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:raise", pLua);

    get_object()->raise();

    return mFunc.on_return();
}

int lua_frame::_register_all_events(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:register_all_events", pLua);

    get_object()->register_all_events();

    return mFunc.on_return();
}

int lua_frame::_register_event(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:register_event", pLua);
    mFunc.add(0, "event name", lua::type::STRING);
    if (mFunc.check())
        get_object()->register_event(mFunc.get(0)->get_string());

    return mFunc.on_return();
}

int lua_frame::_register_for_drag(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:register_for_drag", pLua);
    mFunc.add(0, "button 1", lua::type::STRING, true);
    mFunc.add(1, "button 2", lua::type::STRING, true);
    mFunc.add(2, "button 3", lua::type::STRING, true);
    if (mFunc.check())
    {
        std::vector<std::string> lButtonList;
        for (uint i = 0; i < 3; ++i)
        {
            if (mFunc.is_provided(i))
                lButtonList.push_back(mFunc.get(i)->get_string());
            else
                break;
        }
        get_object()->register_for_drag(lButtonList);
    }

    return mFunc.on_return();
}

int lua_frame::_set_backdrop(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:set_backdrop", pLua);
    mFunc.add(0, "backdrop table", lua::type::TABLE);
    mFunc.add(0, "nil", lua::type::NIL);
    if (mFunc.check())
    {
        if (mFunc.get(0)->get_type() == lua::type::NIL)
        {
            get_object()->set_backdrop(nullptr);
        }
        else
        {
            std::unique_ptr<backdrop> pBackdrop(new backdrop(get_object()));

            lua::state& mState = mFunc.get_state();
            manager* pManager = manager::get_manager(mState);

            pBackdrop->set_background(pManager->parse_file_name(mState.get_field_string("bgFile", false, "")));
            pBackdrop->set_edge(pManager->parse_file_name(mState.get_field_string("edgeFile", false, "")));
            pBackdrop->set_backgrond_tilling(mState.get_field_bool("tile", false, false));

            uint uiTileSize = uint(mState.get_field_int("tileSize", false, 0));
            if (uiTileSize != 0)
                pBackdrop->set_tile_size(uiTileSize);

            uint uiEdgeSize = uint(mState.get_field_int("edgeSize", false, 0));
            if (uiEdgeSize != 0)
                pBackdrop->set_edge_size(uiEdgeSize);

            mState.get_field("insets");

            if (mState.get_type() == lua::type::TABLE)
            {
                pBackdrop->set_background_insets(quad2i(
                    mState.get_field_int("left",   false, 0),
                    mState.get_field_int("right",  false, 0),
                    mState.get_field_int("top",    false, 0),
                    mState.get_field_int("bottom", false, 0)
                ));
            }

            get_object()->set_backdrop(std::move(pBackdrop));
        }
    }

    return mFunc.on_return();
}

int lua_frame::_set_backdrop_border_color(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:set_backdrop_border_color", pLua);
    mFunc.add(0, "red", lua::type::NUMBER);
    mFunc.add(1, "green", lua::type::NUMBER);
    mFunc.add(2, "blue", lua::type::NUMBER);
    mFunc.add(3, "alpha", lua::type::NUMBER, true);
    mFunc.new_param_set();
    mFunc.add(0, "color", lua::type::STRING);

    if (mFunc.check())
    {
        backdrop* pBackdrop = get_object()->get_backdrop();
        if (!pBackdrop)
        {
            get_object()->set_backdrop(std::unique_ptr<backdrop>(new backdrop(get_object())));
            pBackdrop = get_object()->get_backdrop();
        }

        color mColor;
        if (mFunc.get_param_set_rank() == 0)
        {
            if (mFunc.is_provided(3))
            {
                mColor = color(
                    mFunc.get(0)->get_number(),
                    mFunc.get(1)->get_number(),
                    mFunc.get(2)->get_number(),
                    mFunc.get(3)->get_number()
                );
            }
            else
            {
                mColor = color(
                    mFunc.get(0)->get_number(),
                    mFunc.get(1)->get_number(),
                    mFunc.get(2)->get_number()
                );
            }
        }
        else
            mColor = color(mFunc.get(0)->get_string());

        pBackdrop->set_edge_color(mColor);
    }

    return mFunc.on_return();
}

int lua_frame::_set_backdrop_color(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:set_backdrop_color", pLua);
    mFunc.add(0, "red", lua::type::NUMBER);
    mFunc.add(1, "green", lua::type::NUMBER);
    mFunc.add(2, "blue", lua::type::NUMBER);
    mFunc.add(3, "alpha", lua::type::NUMBER, true);
    mFunc.new_param_set();
    mFunc.add(0, "color", lua::type::STRING);

    if (mFunc.check())
    {
        backdrop* pBackdrop = get_object()->get_backdrop();
        if (!pBackdrop)
        {
            get_object()->set_backdrop(std::unique_ptr<backdrop>(new backdrop(get_object())));
            pBackdrop = get_object()->get_backdrop();
        }

        color mColor;
        if (mFunc.get_param_set_rank() == 0)
        {
            if (mFunc.is_provided(3))
            {
                mColor = color(
                    mFunc.get(0)->get_number(),
                    mFunc.get(1)->get_number(),
                    mFunc.get(2)->get_number(),
                    mFunc.get(3)->get_number()
                );
            }
            else
            {
                mColor = color(
                    mFunc.get(0)->get_number(),
                    mFunc.get(1)->get_number(),
                    mFunc.get(2)->get_number()
                );
            }
        }
        else
            mColor = color(mFunc.get(0)->get_string());

        pBackdrop->set_background_color(mColor);
    }

    return mFunc.on_return();
}


int lua_frame::_set_clamped_to_screen(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:set_clamped_to_screen", pLua);
    mFunc.add(0, "is clamped to screen", lua::type::BOOLEAN);
    if (mFunc.check())
        get_object()->set_clamped_to_screen(mFunc.get(0)->get_bool());

    return mFunc.on_return();
}

int lua_frame::_set_frame_level(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:set_frame_level", pLua);
    mFunc.add(0, "level", lua::type::NUMBER);
    if (mFunc.check())
        get_object()->set_level(mFunc.get(0)->get_number());

    return mFunc.on_return();
}

int lua_frame::_set_frame_strata(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:set_frame_strata", pLua);
    mFunc.add(0, "strata", lua::type::STRING);
    if (mFunc.check())
        get_object()->set_frame_strata(mFunc.get(0)->get_string());

    return mFunc.on_return();
}

int lua_frame::_set_hit_rect_insets(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:set_hit_rect_insets", pLua);
    mFunc.add(0, "left", lua::type::NUMBER);
    mFunc.add(1, "right", lua::type::NUMBER);
    mFunc.add(2, "top", lua::type::NUMBER);
    mFunc.add(3, "bottom", lua::type::NUMBER);
    if (mFunc.check())
    {
        get_object()->set_abs_hit_rect_insets(
            int(mFunc.get(0)->get_number()),
            int(mFunc.get(1)->get_number()),
            int(mFunc.get(2)->get_number()),
            int(mFunc.get(3)->get_number())
        );
    }

    return mFunc.on_return();
}

int lua_frame::_set_max_resize(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:set_max_resize", pLua);
    mFunc.add(0, "width", lua::type::NUMBER);
    mFunc.add(1, "height", lua::type::NUMBER);
    if (mFunc.check())
    {
        get_object()->set_max_resize(
            uint(mFunc.get(0)->get_number()),
            uint(mFunc.get(1)->get_number())
        );
    }

    return mFunc.on_return();
}

int lua_frame::_set_min_resize(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:set_min_resize", pLua);
    mFunc.add(0, "width", lua::type::NUMBER);
    mFunc.add(1, "height", lua::type::NUMBER);
    if (mFunc.check())
    {
        get_object()->set_min_resize(
            uint(mFunc.get(0)->get_number()),
            uint(mFunc.get(1)->get_number())
        );
    }

    return mFunc.on_return();
}

int lua_frame::_set_max_width(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:set_max_width", pLua);
    mFunc.add(0, "width", lua::type::NUMBER);
    if (mFunc.check())
    {
        get_object()->set_max_width(
            uint(mFunc.get(0)->get_number())
        );
    }

    return mFunc.on_return();
}

int lua_frame::_set_max_height(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:set_max_height", pLua);
    mFunc.add(0, "height", lua::type::NUMBER);
    if (mFunc.check())
    {
        get_object()->set_max_height(
            uint(mFunc.get(0)->get_number())
        );
    }

    return mFunc.on_return();
}

int lua_frame::_set_min_width(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:set_min_width", pLua);
    mFunc.add(0, "width", lua::type::NUMBER);
    if (mFunc.check())
    {
        get_object()->set_min_width(
            uint(mFunc.get(0)->get_number())
        );
    }

    return mFunc.on_return();
}

int lua_frame::_set_min_height(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:set_min_height", pLua);
    mFunc.add(0, "height", lua::type::NUMBER);
    if (mFunc.check())
    {
        get_object()->set_min_height(
            uint(mFunc.get(0)->get_number())
        );
    }

    return mFunc.on_return();
}

int lua_frame::_set_movable(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:set_movable", pLua);
    mFunc.add(0, "is movable", lua::type::BOOLEAN);
    if (mFunc.check())
        get_object()->set_movable(mFunc.get(0)->get_bool());

    return mFunc.on_return();
}

int lua_frame::_set_resizable(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:set_resizable", pLua);
    mFunc.add(0, "is resizable", lua::type::BOOLEAN);
    if (mFunc.check())
        get_object()->set_resizable(mFunc.get(0)->get_bool());

    return mFunc.on_return();
}

int lua_frame::_set_scale(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:set_scale", pLua);
    mFunc.add(0, "scale", lua::type::NUMBER);
    if (mFunc.check())
        get_object()->set_scale(mFunc.get(0)->get_number());

    return mFunc.on_return();
}

int lua_frame::_set_script(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:set_script", pLua);
    mFunc.add(0, "script name", lua::type::STRING);
    mFunc.add(1, "function", lua::type::FUNCTION, true);
    mFunc.add(1, "nil", lua::type::NIL, true);
    if (mFunc.check())
    {
        std::string sScriptName = mFunc.get(0)->get_string();
        std::string sAdjustedScriptName = frame::get_adjusted_script_name(sScriptName);
        if (get_object()->can_use_script(sScriptName))
        {
            lua::state& mState = mFunc.get_state();
            lua::argument* pArg = mFunc.get(1);
            if (pArg->is_provided() && pArg->get_type() == lua::type::FUNCTION)
            {
                mState.push_value(pArg->get_index());
                mState.set_global(get_object()->get_name() + ":" + sAdjustedScriptName);
                get_object()->notify_script_defined(sScriptName, true);
            }
            else
            {
                mState.push_nil();
                mState.set_global(get_object()->get_name() + ":" + sAdjustedScriptName);
                get_object()->notify_script_defined(sScriptName, false);
            }
        }
        else
        {
            gui::out << gui::error << get_object()->get_frame_type() << " : "
                << "\"" << get_object()->get_name() << "\" cannot use script \"" << sScriptName << "\"." << std::endl;
        }
    }

    return mFunc.on_return();
}

int lua_frame::_set_top_level(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:set_top_level", pLua);
    mFunc.add(0, "is top level", lua::type::BOOLEAN);
    if (mFunc.check())
        get_object()->set_top_level(mFunc.get(0)->get_bool());

    return mFunc.on_return();
}

int lua_frame::_set_user_placed(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:set_user_placed", pLua);
    mFunc.add(0, "is user placed", lua::type::BOOLEAN);
    if (mFunc.check())
        get_object()->set_user_placed(mFunc.get(0)->get_bool());

    return mFunc.on_return();
}

int lua_frame::_start_moving(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:start_moving", pLua);

    get_object()->start_moving();

    return mFunc.on_return();
}

int lua_frame::_start_sizing(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:start_sizing", pLua);
    mFunc.add(0, "point", lua::type::STRING);
    if (mFunc.check())
        get_object()->start_sizing(anchor::get_anchor_point(mFunc.get(0)->get_string()));

    return mFunc.on_return();
}

int lua_frame::_stop_moving_or_sizing(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:stop_moving_or_sizing", pLua);

    get_object()->stop_moving();
    get_object()->stop_sizing();

    return mFunc.on_return();
}

int lua_frame::_unregister_all_events(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:unregister_all_events", pLua);

    get_object()->unregister_all_events();

    return mFunc.on_return();
}

int lua_frame::_unregister_event(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("Frame:unregister_event", pLua);
    mFunc.add(0, "event name", lua::type::STRING);
    if (mFunc.check())
        get_object()->unregister_event(mFunc.get(0)->get_string());

    return mFunc.on_return();
}
}
}
