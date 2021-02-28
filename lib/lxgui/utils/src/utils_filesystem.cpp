#include "lxgui/utils_filesystem.hpp"
#include "lxgui/utils_string.hpp"

#include <lxgui/utils_std.hpp>
#include <filesystem>

namespace lxgui {
namespace utils
{
bool file_exists(const std::string& sFile)
{
    return std::filesystem::exists(sFile) && std::filesystem::is_regular_file(sFile);
}

string_vector get_directory_list(const std::string& sRelPath)
{
    string_vector lDirList;
    for (auto& mEntry : std::filesystem::directory_iterator(sRelPath))
    {
        if (mEntry.is_directory())
            lDirList.push_back(mEntry.path().filename().u8string());
    }

    return lDirList;
}

string_vector get_file_list(const std::string& sRelPath, bool bWithPath)
{
    string_vector lFileList;
    for (auto& mEntry : std::filesystem::directory_iterator(sRelPath))
    {
        if (mEntry.is_regular_file())
        {
            if (bWithPath)
                lFileList.push_back(mEntry.path().relative_path().u8string());
            else
                lFileList.push_back(mEntry.path().filename().u8string());
        }
    }

    return lFileList;
}

string_vector get_file_list(const std::string& sRelPath, bool bWithPath, const std::string& sExtensions)
{
    string_vector lExtensions = utils::cut(sExtensions, ",");
    for (auto& sExtension : lExtensions)
        utils::trim(sExtension, ' ');

    string_vector lFileList;
    for (auto& mEntry : std::filesystem::directory_iterator(sRelPath))
    {
        if (mEntry.is_regular_file())
        {
            if (utils::find(lExtensions, mEntry.path().extension().u8string()) != lExtensions.end())
            {
                if (bWithPath)
                    lFileList.push_back(mEntry.path().relative_path().u8string());
                else
                    lFileList.push_back(mEntry.path().filename().u8string());
            }
        }
    }

    return lFileList;
}

bool make_directory(const std::string& sPath)
{
    std::filesystem::create_directories(sPath);
    return std::filesystem::exists(sPath) && std::filesystem::is_directory(sPath);
}
}
}
