#include "filepathutil.hpp"
#include <algorithm>
#include <boost/predef.h>

// For get_executable_path()
#if BOOST_OS_WINDOWS
#include <windows.h> // GetModuleFileName
#include "unicode_utf16.hpp"
#elif BOOST_OS_MACOS
#include <limits.h> // PATH_MAX
#include <mach-o/dyld.h> // _NSGetExecutablePath
#elif BOOST_OS_LINUX
#include <limits.h> // PATH_MAX
#include <unistd.h> // readlink
#else
#error Unsupported OS
#endif


namespace filepathutil
{

namespace fs = boost::filesystem;



fs::path u8path(const std::string& str)
{
#if BOOST_OS_WINDOWS
    return lib::unicode::utf8_to_utf16(str);
#else
    return str;
#endif
}



std::string make_preferred_path_in_utf8(const fs::path& path)
{
    auto path_ = path;
#if BOOST_OS_WINDOWS
    return lib::unicode::utf16_to_utf8(path_.make_preferred().native());
#else
    return path_.make_preferred().native();
#endif
}



std::string to_utf8_path(const fs::path& path)
{
#if BOOST_OS_WINDOWS
    return lib::unicode::utf16_to_utf8(path.native());
#else
    return path.native();
#endif
}



std::string to_forward_slashes(const fs::path& path)
{
    std::string path_str = to_utf8_path(path);
    std::replace(path_str.begin(), path_str.end(), '\\', '/');
    return path_str;
}



boost::optional<boost::filesystem::path::string_type> get_executable_path()
{
#if BOOST_OS_WINDOWS
    wchar_t buf[1024 + 1];
    size_t buf_size = sizeof(buf);
    if (GetModuleFileName(nullptr, buf, buf_size) == 0)
    {
        return boost::none;
    }
#elif BOOST_OS_MACOS
    char buf[PATH_MAX + 1];
    uint32_t buf_size = sizeof(buf);
    if (_NSGetExecutablePath(buf, &buf_size) != 0)
    {
        return boost::none;
    }
#elif BOOST_OS_LINUX
    char buf[PATH_MAX + 1];
    size_t buf_size = sizeof(buf);
    ssize_t result = readlink("/proc/self/exe", buf, buf_size);
    if (result == -1)
    {
        return boost::none;
    }
    buf[result] = '\0';
#else
#error Unsupported OS
#endif

    return fs::path::string_type(buf);
}



bool is_portable_path(const boost::filesystem::path& filename)
{
    // General check by Boost.Filesystem such as invalid characters.
    auto u8str = to_utf8_path(filename);
    if (!fs::portable_name(u8str))
        return false;

    // boost::filesyste::portable_name() function does not check the paths
    // reserved by Windows.
    // https://www.boost.org/doc/libs/1_69_0/libs/filesystem/doc/portability_guide.htm
    for (const auto fname : u8path(u8str))
    {
        // https://docs.microsoft.com/en-us/windows/win32/fileio/naming-a-file
        static constexpr const char* reserved_names[] = {
            "con",  "prn",  "aux",  "nul",  "com1", "com2", "com3", "com4",
            "com5", "com6", "com7", "com8", "com9", "lpt1", "lpt2", "lpt3",
            "lpt4", "lpt5", "lpt6", "lpt7", "lpt8", "lpt9",
        };

        bool is_reserved = std::any_of(
            std::begin(reserved_names),
            std::end(reserved_names),
            [&](const char* reserved_name) {
                // Reserved name following extensions is also reserved.
                auto copy = fname;
                copy.replace_extension(); // Remove extension.
                return copy == reserved_name;
            });
        if (is_reserved)
            return false;
    }

    // portable name
    return true;
}

} // namespace filepathutil
