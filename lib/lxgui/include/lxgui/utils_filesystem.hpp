#ifndef LXGUI_UTILS_FILESYSTEM_HPP
#define LXGUI_UTILS_FILESYSTEM_HPP

#include "lxgui/utils.hpp"
#include <string>
#include <vector>

namespace lxgui {
namespace utils
{
typedef std::vector<std::string> string_vector;

bool file_exists(const std::string& sFile);

bool make_directory(const std::string& sPath);
string_vector get_directory_list(const std::string& sRelPath);
string_vector get_file_list(const std::string& sRelPath, bool bWithPath = false);
string_vector get_file_list(const std::string& sRelPath, bool bWithPath, const std::string& sExtensions);
}
}

#endif
