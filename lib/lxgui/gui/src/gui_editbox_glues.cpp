#include "lxgui/gui_editbox.hpp"

#include <lxgui/luapp_state.hpp>
#include <lxgui/luapp_function.hpp>

namespace lxgui {
namespace gui
{
void edit_box::register_glue(lua::state& mLua)
{
    mLua.reg<lua_edit_box>();
}

lua_edit_box::lua_edit_box(lua_State* pLua) : lua_focus_frame(pLua)
{
}

int lua_edit_box::_add_history_line(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:add_history_line", pLua);
    mFunc.add(0, "line", lua::type::STRING);
    if (mFunc.check())
        get_object()->add_history_line(mFunc.get(0)->get_string());

    return mFunc.on_return();
}

int lua_edit_box::_clear_history(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:clear_history", pLua);

    get_object()->clear_history();

    return mFunc.on_return();
}

int lua_edit_box::_get_blink_speed(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:get_blink_speed", pLua, 1);

    mFunc.push(get_object()->get_blink_speed());

    return mFunc.on_return();
}

int lua_edit_box::_get_cursor_position(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:get_cursor_position", pLua, 1);

    mFunc.push(get_object()->get_cursor_position());

    return mFunc.on_return();
}

int lua_edit_box::_get_history_lines(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    const auto& lHistoryLine = get_object()->get_history_lines();
    lua::function mFunc("EditBox:get_history_lines", pLua, lHistoryLine.size());

    for (const auto& sLine : lHistoryLine)
        mFunc.push(sLine);

    return mFunc.on_return();
}

int lua_edit_box::_get_max_letters(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:get_max_letters", pLua, 1);

    mFunc.push(get_object()->get_max_letters());

    return mFunc.on_return();
}

int lua_edit_box::_get_num_letters(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:get_num_letters", pLua, 1);

    mFunc.push(get_object()->get_num_letters());

    return mFunc.on_return();
}

int lua_edit_box::_get_number(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:get_number", pLua, 1);

    mFunc.push(utils::string_to_float(get_object()->get_text()));

    return mFunc.on_return();
}

int lua_edit_box::_get_text(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:get_text", pLua, 1);

    mFunc.push(get_object()->get_text());

    return mFunc.on_return();
}

int lua_edit_box::_get_text_insets(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:get_text_insets", pLua, 4);

    const quad2i& lInsets = get_object()->get_text_insets();

    mFunc.push(lInsets.left);
    mFunc.push(lInsets.right);
    mFunc.push(lInsets.top);
    mFunc.push(lInsets.bottom);

    return mFunc.on_return();
}

int lua_edit_box::_highlight_text(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:highlight_text", pLua);
    mFunc.add(0, "start", lua::type::NUMBER, true);
    mFunc.add(1, "end", lua::type::NUMBER, true);
    if (mFunc.check())
    {
        uint uiStart = 0;
        uint uiEnd = uint(-1);

        if (mFunc.is_provided(0))
            uiStart = uint(mFunc.get(0)->get_number());
        if (mFunc.is_provided(1))
            uiEnd = uint(mFunc.get(1)->get_number());

        get_object()->highlight_text(uiStart, uiEnd);
    }

    return mFunc.on_return();
}

int lua_edit_box::_insert(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:insert", pLua);
    mFunc.add(0, "text", lua::type::STRING);
    if (mFunc.check())
        get_object()->insert_after_cursor(mFunc.get(0)->get_string());

    return mFunc.on_return();
}

int lua_edit_box::_is_multi_line(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:is_multi_line", pLua);

    mFunc.push(get_object()->is_multi_line());

    return mFunc.on_return();
}

int lua_edit_box::_is_numeric(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:is_numeric", pLua);

    mFunc.push(get_object()->is_numeric_only());

    return mFunc.on_return();
}

int lua_edit_box::_is_password(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:is_password", pLua);

    mFunc.push(get_object()->is_password_mode_enabled());

    return mFunc.on_return();
}

int lua_edit_box::_set_blink_speed(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:set_blink_speed", pLua);
    mFunc.add(0, "blink speed", lua::type::NUMBER);
    if (mFunc.check())
        get_object()->set_blink_speed(double(mFunc.get(0)->get_number()));

    return mFunc.on_return();
}

int lua_edit_box::_set_cursor_position(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:set_cursor_position", pLua);
    mFunc.add(0, "cursor position", lua::type::NUMBER);
    if (mFunc.check())
        get_object()->set_cursor_position(mFunc.get(0)->get_number());

    return mFunc.on_return();
}

int lua_edit_box::_set_max_history_lines(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:set_max_history_lines", pLua);
    mFunc.add(0, "max lines", lua::type::NUMBER);
    if (mFunc.check())
        get_object()->set_max_history_lines(mFunc.get(0)->get_number());

    return mFunc.on_return();
}

int lua_edit_box::_set_max_letters(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:set_max_letters", pLua);
    mFunc.add(0, "max letters", lua::type::NUMBER);
    if (mFunc.check())
        get_object()->set_max_letters(mFunc.get(0)->get_number());

    return mFunc.on_return();
}

int lua_edit_box::_set_multi_line(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:set_multi_line", pLua);
    mFunc.add(0, "multiLine", lua::type::BOOLEAN);
    if (mFunc.check())
        get_object()->set_multi_line(mFunc.get(0)->get_bool());

    return mFunc.on_return();
}

int lua_edit_box::_set_number(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:set_number", pLua);
    mFunc.add(0, "number", lua::type::NUMBER);
    if (mFunc.check())
        get_object()->set_text(utils::to_string(mFunc.get(0)->get_number()));

    return mFunc.on_return();
}

int lua_edit_box::_set_numeric(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:set_numeric", pLua);
    mFunc.add(0, "numeric", lua::type::NUMBER);
    if (mFunc.check())
        get_object()->set_numeric_only(mFunc.get(0)->get_bool());

    return mFunc.on_return();
}

int lua_edit_box::_set_password(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:set_password", pLua);
    mFunc.add(0, "enable", lua::type::NUMBER);
    if (mFunc.check())
        get_object()->enable_password_mode(mFunc.get(0)->get_bool());

    return mFunc.on_return();
}

int lua_edit_box::_set_text(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:set_text", pLua);
    mFunc.add(0, "text", lua::type::STRING);
    if (mFunc.check())
        get_object()->set_text(mFunc.get(0)->get_string());

    return mFunc.on_return();
}

int lua_edit_box::_set_text_insets(lua_State* pLua)
{
    if (!check_object_())
        return 0;

    lua::function mFunc("EditBox:set_text_insets", pLua);
    mFunc.add(0, "left", lua::type::NUMBER);
    mFunc.add(1, "right", lua::type::NUMBER);
    mFunc.add(2, "top", lua::type::NUMBER);
    mFunc.add(3, "bottom", lua::type::NUMBER);
    if (mFunc.check())
    {
        get_object()->set_text_insets(
            int(mFunc.get(0)->get_number()),
            int(mFunc.get(1)->get_number()),
            int(mFunc.get(2)->get_number()),
            int(mFunc.get(3)->get_number())
        );
    }

    return mFunc.on_return();
}
}
}
