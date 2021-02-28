#include "lxgui/gui_layeredregion.hpp"
#include "lxgui/gui_frame.hpp"
#include "lxgui/gui_out.hpp"
#include "lxgui/gui_manager.hpp"

#include <lxgui/xml_document.hpp>
#include <lxgui/utils_string.hpp>

namespace lxgui {
namespace gui
{
void layered_region::parse_block(xml::block* pBlock)
{
    parse_attributes_(pBlock);

    parse_size_block_(pBlock);
    parse_anchor_block_(pBlock);
}

void layered_region::parse_attributes_(xml::block* pBlock)
{
    std::string sName = pBlock->get_attribute("name");
    if (utils::has_no_content(sName))
    {
        throw exception(pBlock->get_location(),
            "Cannot create an uiobject with a blank name. Skipped."
        );
    }

    if (!pManager_->check_uiobject_name(sName))
    {
        throw exception(pBlock->get_location(),
            "Cannot create an uiobject with an incorrect name. Skipped."
        );
    }

    bool bVirtual = utils::string_to_bool(pBlock->get_attribute("virtual"));
    if (bVirtual || (pParent_ && pParent_->is_virtual()))
        set_virtual();

    set_name(sName);

    if (pManager_->get_uiobject_by_name(sName_))
    {
        throw exception(pBlock->get_location(),
            std::string(bVirtual ? "A virtual" : "An")+" object with the name "+
            sName_+" already exists. Skipped."
        );
    }

    pManager_->add_uiobject(this);

    if (!bVirtual_)
        create_glue();

    std::string sInheritance = pBlock->get_attribute("inherits");
    if (!utils::has_no_content(sInheritance))
    {
        for (auto sParent : utils::cut(sInheritance, ","))
        {
            utils::trim(sParent, ' ');
            uiobject* pObj = pManager_->get_uiobject_by_name(sParent, true);
            if (pObj)
            {
                if (is_object_type(pObj->get_object_type()))
                {
                    // Inherit from the other region
                    copy_from(pObj);
                }
                else
                {
                    gui::out << gui::warning << pBlock->get_location() << " : "
                        << "\"" << sName_ << "\" (" << "gui::" << lType_.back() << ") cannot inherit "
                        << "from \"" << sParent << "\" (" << pObj->get_object_type() << "). "
                        << "Inheritance skipped." << std::endl;
                }
            }
            else
            {
                gui::out << gui::warning <<  pBlock->get_location() << " : "
                    << "Cannot find inherited object \"" << sParent << "\". Inheritance skipped." << std::endl;
            }
        }
    }

    if ((pBlock->is_provided("hidden") || !bInherits_) &&
        (utils::string_to_bool(pBlock->get_attribute("hidden"))))
        hide();

    if ((pBlock->is_provided("setAllPoints") || !bInherits_) &&
        (utils::string_to_bool(pBlock->get_attribute("setAllPoints"))))
        set_all_points("$parent");
}
}
}
