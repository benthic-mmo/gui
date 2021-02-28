#include "lxgui/gui_scrollframe.hpp"
#include "lxgui/gui_frame.hpp"
#include "lxgui/gui_texture.hpp"
#include "lxgui/gui_manager.hpp"
#include "lxgui/gui_rendertarget.hpp"
#include "lxgui/gui_out.hpp"
#include "lxgui/gui_alive_checker.hpp"
#include "lxgui/gui_uiobject_tpl.hpp"

namespace lxgui {
namespace gui
{
scroll_frame::scroll_frame(manager* pManager) : frame(pManager), renderer(pManager->get_renderer())
{
    lType_.push_back(CLASS_NAME);
}

scroll_frame::~scroll_frame()
{
    // Make sure the scroll child is destroyed now.
    // It relies on this scroll_frame still being alive
    // when being destroyed, but the scroll_frame destructor
    // would be called before its inherited frame destructor
    // (which would otherwise take care of destroying the scroll child).
    if (pScrollChild_)
        remove_child(pScrollChild_);
}

bool scroll_frame::can_use_script(const std::string& sScriptName) const
{
    if (frame::can_use_script(sScriptName))
        return true;
    else if ((sScriptName == "OnHorizontalScroll") ||
        (sScriptName == "OnScrollRangeChanged") ||
        (sScriptName == "OnVerticalScroll"))
        return true;
    else
        return false;
}

void scroll_frame::on(const std::string& sScriptName, event* pEvent)
{
    alive_checker mChecker(this);
    frame::on(sScriptName, pEvent);
    if (!mChecker.is_alive())
        return;

    if (sScriptName == "SizeChanged")
        bRebuildScrollRenderTarget_ = true;
}

void scroll_frame::copy_from(uiobject* pObj)
{
    frame::copy_from(pObj);

    scroll_frame* pScrollFrame = pObj->down_cast<scroll_frame>();
    if (!pScrollFrame)
        return;

    this->set_horizontal_scroll(pScrollFrame->get_horizontal_scroll());
    this->set_vertical_scroll(pScrollFrame->get_vertical_scroll());

    auto* pOtherChild = pScrollFrame->get_scroll_child();
    if (pOtherChild)
    {
        frame* pScrollChild = create_child(pOtherChild->get_object_type(),
                pOtherChild->get_raw_name(), {pOtherChild});
        pScrollChild->notify_loaded();

        if (pScrollChild)
            this->set_scroll_child(remove_child(pScrollChild));
    }
}

void scroll_frame::set_scroll_child(std::unique_ptr<frame> pFrame)
{
    if (pScrollChild_)
    {
        pScrollChild_->set_renderer(nullptr);
        pScrollChild_->modify_point(anchor_point::TOPLEFT)->set_abs_offset(
            lBorderList_.top_left() - vector2i(iHorizontalScroll_, iVerticalScroll_)
        );

        clear_strata_list_();
    }
    else if (!is_virtual() && !pScrollTexture_)
    {
        // Create the scroll texture
        std::unique_ptr<texture> pScrollTexture(new texture(pManager_));
        pScrollTexture->set_special();
        pScrollTexture->set_parent(this);
        pScrollTexture->set_draw_layer("ARTWORK");
        pScrollTexture->set_name("$parentScrollTexture");

        if (!pManager_->add_uiobject(pScrollTexture.get()))
        {
            gui::out << gui::warning << "gui::" << lType_.back() << " : "
                "Trying to create scroll texture for \""+sName_+"\", "
                "but its name was already taken : \""+pScrollTexture->get_name()+"\". Skipped." << std::endl;
            return;
        }

        pScrollTexture->create_glue();
        pScrollTexture->set_all_points(this);

        if (pScrollRenderTarget_)
            pScrollTexture->set_texture(pScrollRenderTarget_);

        pScrollTexture->notify_loaded();
        pScrollTexture_ = pScrollTexture.get();
        add_region(std::move(pScrollTexture));

        bRebuildScrollRenderTarget_ = true;
    }

    pScrollChild_ = pFrame.get();

    if (pScrollChild_)
    {
        pScrollChild_->set_parent(this);

        add_child(std::move(pFrame));

        pScrollChild_->set_special();
        if (!is_virtual())
            pScrollChild_->set_renderer(this);
        pScrollChild_->clear_all_points();
        pScrollChild_->set_abs_point(anchor_point::TOPLEFT, "", anchor_point::TOPLEFT, -iHorizontalScroll_, -iVerticalScroll_);

        iHorizontalScrollRange_ = int(pScrollChild_->get_abs_width()) - int(uiAbsWidth_);
        if (iHorizontalScrollRange_ < 0) iHorizontalScrollRange_ = 0;

        iVerticalScrollRange_ = int(pScrollChild_->get_abs_height()) - int(uiAbsHeight_);
        if (iVerticalScrollRange_ < 0) iVerticalScrollRange_ = 0;

        if (!is_virtual())
        {
            alive_checker mChecker(this);
            on("ScrollRangeChanged");
            if (!mChecker.is_alive())
                return;
        }

        bUpdateScrollRange_ = false;
    }

    bRedrawScrollRenderTarget_ = true;
}

frame* scroll_frame::get_scroll_child()
{
    return pScrollChild_;
}

void scroll_frame::set_horizontal_scroll(int iHorizontalScroll)
{
    if (iHorizontalScroll_ != iHorizontalScroll)
    {
        iHorizontalScroll_ = iHorizontalScroll;
        lQueuedEventList_.push_back("HorizontalScroll");

        pScrollChild_->modify_point(anchor_point::TOPLEFT)->set_abs_offset(-iHorizontalScroll_, -iVerticalScroll_);
        bRedrawScrollRenderTarget_ = true;
    }
}

int scroll_frame::get_horizontal_scroll() const
{
    return iHorizontalScroll_;
}

int scroll_frame::get_horizontal_scroll_range() const
{
    return iHorizontalScrollRange_;
}

void scroll_frame::set_vertical_scroll(int iVerticalScroll)
{
    if (iVerticalScroll_ != iVerticalScroll)
    {
        iVerticalScroll_ = iVerticalScroll;
        lQueuedEventList_.push_back("VerticalScroll");

        pScrollChild_->modify_point(anchor_point::TOPLEFT)->set_abs_offset(-iHorizontalScroll_, -iVerticalScroll_);
        bRedrawScrollRenderTarget_ = true;
    }
}

int scroll_frame::get_vertical_scroll() const
{
    return iVerticalScroll_;
}

int scroll_frame::get_vertical_scroll_range() const
{
    return iVerticalScrollRange_;
}

void scroll_frame::update(float fDelta)
{
    uint uiOldChildWidth = 0;
    uint uiOldChildHeight = 0;

    if (pScrollChild_)
    {
        uiOldChildWidth = pScrollChild_->get_abs_width();
        uiOldChildHeight = pScrollChild_->get_abs_height();
    }

    alive_checker mChecker(this);
    frame::update(fDelta);
    if (!mChecker.is_alive())
        return;

    if (pScrollChild_ && (uiOldChildWidth != pScrollChild_->get_abs_width() ||
        uiOldChildHeight != pScrollChild_->get_abs_height()))
    {
        bUpdateScrollRange_ = true;
        bRedrawScrollRenderTarget_ = true;
    }

    if (is_visible())
    {
        if (bRebuildScrollRenderTarget_ && pScrollTexture_)
        {
            rebuild_scroll_render_target_();
            bRebuildScrollRenderTarget_ = false;
            bRedrawScrollRenderTarget_ = true;
        }

        if (bUpdateScrollRange_)
        {
            update_scroll_range_();
            bUpdateScrollRange_ = false;
        }

        if (pScrollChild_)
            update_scroll_child_input_();

        if (pScrollChild_ && pScrollRenderTarget_ && bRedrawScrollRenderTarget_)
        {
            render_scroll_strata_list_();
            bRedrawScrollRenderTarget_ = false;
        }
    }
}

void scroll_frame::update_scroll_range_()
{
    iHorizontalScrollRange_ = int(pScrollChild_->get_abs_width()) - int(uiAbsWidth_);
    if (iHorizontalScrollRange_ < 0) iHorizontalScrollRange_ = 0;

    iVerticalScrollRange_ = int(pScrollChild_->get_abs_height()) - int(uiAbsHeight_);
    if (iVerticalScrollRange_ < 0) iVerticalScrollRange_ = 0;

    if (!is_virtual())
    {
        alive_checker mChecker(this);
        on("ScrollRangeChanged");
        if (!mChecker.is_alive())
            return;
    }
}

void scroll_frame::update_scroll_child_input_()
{
    int iX = iMousePosX_ - lBorderList_.left;
    int iY = iMousePosY_ - lBorderList_.top;

    if (bMouseInScrollTexture_)
    {
        frame* pHoveredFrame = find_hovered_frame_(iX, iY);

        if (pHoveredFrame != pHoveredScrollChild_)
        {
            if (pHoveredScrollChild_)
                pHoveredScrollChild_->notify_mouse_in_frame(false, iX, iY);

            pHoveredScrollChild_ = pHoveredFrame;
        }

        if (pHoveredScrollChild_)
            pHoveredScrollChild_->notify_mouse_in_frame(true, iX, iY);
    }
    else if (pHoveredScrollChild_)
    {
        pHoveredScrollChild_->notify_mouse_in_frame(false, iX, iY);
        pHoveredScrollChild_ = nullptr;
    }
}

void scroll_frame::rebuild_scroll_render_target_()
{
    if (uiAbsWidth_ == 0 || uiAbsHeight_ == 0)
        return;

    if (pScrollRenderTarget_)
    {
        pScrollRenderTarget_->set_dimensions(uiAbsWidth_, uiAbsHeight_);
        std::array<float,4> lTexCoords;
        lTexCoords[0] = 0.0f; lTexCoords[1] = 0.0f;
        lTexCoords[2] = float(uiAbsWidth_)/pScrollRenderTarget_->get_real_width();
        lTexCoords[3] = float(uiAbsHeight_)/pScrollRenderTarget_->get_real_height();
        pScrollTexture_->set_tex_coord(lTexCoords);
        bUpdateScrollRange_ = true;
    }
    else
    {
        pScrollRenderTarget_ = create_render_target(uiAbsWidth_, uiAbsHeight_);

        if (pScrollRenderTarget_)
            pScrollTexture_->set_texture(pScrollRenderTarget_);
    }
}

void scroll_frame::render_scroll_strata_list_()
{
    begin(pScrollRenderTarget_);
    pScrollRenderTarget_->clear(color::EMPTY);

    for (const auto& mStrata : lStrataList_)
    {
        render_strata_(mStrata);
    }

    end();
}

bool scroll_frame::is_in_frame(int iX, int iY) const
{
    if (pScrollTexture_)
        return frame::is_in_frame(iX, iY) || pScrollTexture_->is_in_region(iX, iY);
    else
        return frame::is_in_frame(iX, iY);
}

void scroll_frame::notify_mouse_in_frame(bool bMouseInFrame, int iX, int iY)
{
    frame::notify_mouse_in_frame(bMouseInFrame, iX, iY);
    bMouseInScrollTexture_ = (bMouseInFrame && pScrollTexture_ && pScrollTexture_->is_in_region(iX, iY));
}

void scroll_frame::fire_redraw(frame_strata mStrata) const
{
    renderer::fire_redraw(mStrata);

    bRedrawScrollRenderTarget_ = true;
    notify_renderer_need_redraw();
}

void scroll_frame::create_glue()
{
    create_glue_<lua_scroll_frame>();
}

void scroll_frame::notify_rendered_frame(frame* pFrame, bool bRendered)
{
    if (!pFrame)
        return;

    renderer::notify_rendered_frame(pFrame, bRendered);

    if (!bRendered)
    {
        if (pFrame == pHoveredScrollChild_)
        {
            pHoveredScrollChild_->notify_mouse_in_frame(false, 0, 0);
            pHoveredScrollChild_ = nullptr;
        }
    }

    bRedrawScrollRenderTarget_ = true;
}

uint scroll_frame::get_target_width() const
{
    return get_apparent_width();
}

uint scroll_frame::get_target_height() const
{
    return get_apparent_height();
}
}
}
