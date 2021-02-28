#include "lxgui/impl/gui_gl_renderer.hpp"
#include "lxgui/impl/gui_gl_material.hpp"
#include "lxgui/impl/gui_gl_rendertarget.hpp"
#include "lxgui/impl/gui_gl_font.hpp"

#include <lxgui/gui_sprite.hpp>
#include <lxgui/gui_out.hpp>
#include <lxgui/gui_manager.hpp>
#include <lxgui/utils_string.hpp>

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include <GL/glew.h>
#ifdef MACOSX
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <cstring>

namespace lxgui {
namespace gui {
namespace gl
{
renderer::renderer(bool bInitGLEW)
{
    if (bInitGLEW)
        glewInit();

    render_target::check_availability();
    material::check_availability();
}

void renderer::begin(std::shared_ptr<gui::render_target> pTarget) const
{
    if (pTarget)
    {
        pCurrentTarget_ = std::static_pointer_cast<gl::render_target>(pTarget);
        pCurrentTarget_->begin();
    }
    else
    {
        if (bUpdateViewMatrix_)
            update_view_matrix_();

        glViewport(0.0f, 0.0f, pParent_->get_target_width(), pParent_->get_target_height());
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // Premultipled alpha
        glDisable(GL_LIGHTING);
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_CULL_FACE);

        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(reinterpret_cast<float*>(&mViewMatrix_));

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
}

void renderer::update_view_matrix_() const
{
    float fWidth = pParent_->get_target_width();
    float fHeight = pParent_->get_target_height();

    mViewMatrix_ = {
        2.0f/fWidth, 0.0f, -1.0f, -1.0f,
        0.0f, -2.0f/fHeight, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    bUpdateViewMatrix_ = false;
}

void renderer::end() const
{
    if (pCurrentTarget_)
    {
        pCurrentTarget_->end();
        pCurrentTarget_ = nullptr;
    }
}

void renderer::render_quad(const quad& mQuad) const
{
    static const std::array<uint, 6> ids = {{0, 1, 2, 2, 3, 0}};

    glColor4ub(255, 255, 255, 255);

    std::shared_ptr<gl::material> pMat = std::static_pointer_cast<gl::material>(mQuad.mat);
    if (pMat->get_type() == material::type::TEXTURE)
    {
        pMat->bind();

        glEnable(GL_TEXTURE_2D);
        glBegin(GL_TRIANGLES);
        for (uint i = 0; i < 6; ++i)
        {
            uint j = ids[i];
            float a = mQuad.v[j].col.a;
            glColor4f(mQuad.v[j].col.r*a, mQuad.v[j].col.g*a, mQuad.v[j].col.b*a, a); // Premultipled alpha
            glTexCoord2f(mQuad.v[j].uvs.x, mQuad.v[j].uvs.y);
            glVertex2f(mQuad.v[j].pos.x, mQuad.v[j].pos.y);
        }
        glEnd();
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_TRIANGLES);
        for (uint i = 0; i < 6; ++i)
        {
            uint j = ids[i];
            color c = mQuad.v[j].col*pMat->get_color();
            c.r *= c.a; c.g *= c.a; c.b *= c.a;  // Premultipled alpha
            glColor4f(c.r, c.g, c.b, c.a);
            glVertex2f(mQuad.v[j].pos.x, mQuad.v[j].pos.y);
        }
        glEnd();
    }
}

void renderer::render_quads(const quad& mQuad, const std::vector<std::array<vertex,4>>& lQuadList) const
{
    static const std::array<uint, 6> ids = {{0, 1, 2, 2, 3, 0}};

    glColor4ub(255, 255, 255, 255);

    std::shared_ptr<gl::material> pMat = std::static_pointer_cast<gl::material>(mQuad.mat);
    if (pMat->get_type() == material::type::TEXTURE)
    {
        pMat->bind();

        glEnable(GL_TEXTURE_2D);
        glBegin(GL_TRIANGLES);
        for (const auto& v :lQuadList)
        {
            for (uint i = 0; i < 6; ++i)
            {
                uint j = ids[i];
                float a = v[j].col.a;
                glColor4f(v[j].col.r*a, v[j].col.g*a, v[j].col.b*a, a); // Premultipled alpha
                glTexCoord2f(v[j].uvs.x, v[j].uvs.y);
                glVertex2f(v[j].pos.x, v[j].pos.y);
            }
        }
        glEnd();
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_TRIANGLES);
        for (const auto& v : lQuadList)
        {
            for (uint i = 0; i < 6; ++i)
            {
                uint j = ids[i];
                color c = v[j].col*pMat->get_color();
                c.r *= c.a; c.g *= c.a; c.b *= c.a; // Premultipled alpha
                glColor4f(c.r, c.g, c.b, c.a);
                glVertex2f(v[j].pos.x, v[j].pos.y);
            }
        }
        glEnd();
    }
}

std::shared_ptr<gui::material> renderer::create_material(const std::string& sFileName, material::filter mFilter) const
{
    std::string sBackedName = utils::to_string((int)mFilter) + '|' + sFileName;
    std::map<std::string, std::weak_ptr<gui::material>>::iterator iter = lTextureList_.find(sBackedName);
    if (iter != lTextureList_.end())
    {
        if (std::shared_ptr<gui::material> pLock = iter->second.lock())
            return pLock;
        else
            lTextureList_.erase(iter);
    }

    if (utils::ends_with(sFileName, ".png"))
        return create_material_png(sFileName, mFilter);
    else
    {
        gui::out << gui::warning << "gui::gl::renderer : Unsupported texture format '"
            << sFileName << "'." << std::endl;
        return nullptr;
    }
}

std::shared_ptr<gui::material> renderer::create_material(const color& mColor) const
{
    return std::make_shared<material>(mColor);
}

std::shared_ptr<gui::material> renderer::create_material(std::shared_ptr<gui::render_target> pRenderTarget) const
{
    return std::static_pointer_cast<gl::render_target>(pRenderTarget)->get_material().lock();
}

std::shared_ptr<gui::render_target> renderer::create_render_target(uint uiWidth, uint uiHeight) const
{
    return std::make_shared<render_target>(uiWidth, uiHeight);
}

std::shared_ptr<gui::font> renderer::create_font(const std::string& sFontFile, uint uiSize) const
{
    std::string sFontName = sFontFile + "|" + utils::to_string(uiSize);
    std::map<std::string, std::weak_ptr<gui::font>>::iterator iter = lFontList_.find(sFontName);
    if (iter != lFontList_.end())
    {
        if (std::shared_ptr<gui::font> pLock = iter->second.lock())
            return pLock;
        else
            lFontList_.erase(iter);
    }

    std::shared_ptr<gui::font> pFont = std::make_shared<gl::font>(sFontFile, uiSize);
    lFontList_[sFontName] = pFont;
    return pFont;
}

bool renderer::is_gl_extension_supported(const std::string& sExtension)
{
    // Code taken from :
    // http://nehe.gamedev.net/tutorial/vertex_buffer_objects/22002/

    const unsigned char *pszExtensions = nullptr;
    const unsigned char *pszStart;
    unsigned char *pszWhere, *pszTerminator;

    // Extension names should not have spaces
    pszWhere = (unsigned char*)strchr(sExtension.c_str(), ' ');
    if (pszWhere || sExtension.empty())
        return false;

    // Get Extensions String
    pszExtensions = glGetString(GL_EXTENSIONS);

    // Search The Extensions String For An Exact Copy
    pszStart = pszExtensions;

    for (;;)
    {
        pszWhere = (unsigned char*)strstr((const char*)pszStart, sExtension.c_str());
        if (!pszWhere)
            break;

        pszTerminator = pszWhere + sExtension.size();

        if ((pszWhere == pszStart || *(pszWhere - 1) == ' ') &&
            (*pszTerminator == ' ' || *pszTerminator == '\0'))
            return true;

        pszStart = pszTerminator;
    }

    return false;
}
}
}
}
