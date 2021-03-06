#include "mod_manifest.hpp"
#include "../../thirdparty/json5/json5.hpp"



namespace elona
{
namespace lua
{

namespace
{

std::string _read_string(
    const std::string key,
    const json5::value::object_type& obj,
    const fs::path& path)
{
    // TODO: Clean up, as with lua::ConfigTable
    const auto itr = obj.find(key);
    if (itr != std::end(obj) && itr->second.is_string())
    {
        return itr->second.get_string();
    }
    else
    {
        throw std::runtime_error(
            filepathutil::to_utf8_path(path) + ": Missing \"" + key +
            "\" in mod manifest");
    }
}



semver::Version _read_mod_version(
    const json5::value::object_type& obj,
    const fs::path& path)
{
    // TODO: Clean up, as with lua::ConfigTable
    const auto itr = obj.find("version");
    if (itr == std::end(obj))
    {
        return semver::Version{};
    }

    if (itr->second.is_string())
    {
        if (const auto result =
                semver::Version::parse(itr->second.get_string()))
        {
            return result.right();
        }
        else
        {
            throw std::runtime_error{result.left()};
        }
    }
    else
    {
        throw std::runtime_error(
            filepathutil::to_utf8_path(path) +
            ": Missing \"version\" in mod manifest");
    }
}



ModManifest::Dependencies _read_dependencies(
    const json5::value::object_type& obj,
    const fs::path& path)
{
    ModManifest::Dependencies result;

    const auto itr = obj.find("dependencies");
    if (itr != std::end(obj))
    {
        if (itr->second.is_object())
        {
            const auto& dependencies = itr->second.get_object();

            for (const auto& kvp : dependencies)
            {
                const auto& mod = kvp.first;
                const auto& version = kvp.second;
                if (version.is_string())
                {
                    if (const auto req = semver::VersionRequirement::parse(
                            version.get_string()))
                    {
                        result.emplace(mod, req.right());
                    }
                    else
                    {
                        throw std::runtime_error(
                            filepathutil::to_utf8_path(path) + ": " +
                            req.left());
                    }
                }
                else
                {
                    throw std::runtime_error(
                        filepathutil::to_utf8_path(path) +
                        ": \"dependencies\" field must be an object.");
                }
            }
        }
        else
        {
            throw std::runtime_error(
                filepathutil::to_utf8_path(path) +
                ": \"dependencies\" field must be an object.");
        }
    }

    return result;
}

} // namespace



ModManifest ModManifest::load(const fs::path& path)
{
    // TODO loading error handling.
    std::ifstream in{path.native()};
    std::string source{std::istreambuf_iterator<char>{in},
                       std::istreambuf_iterator<char>{}};
    const auto value = json5::parse(source);
    const auto& obj = value.get_object();

    const auto mod_id = _read_string("id", obj, path);
    const auto mod_name = _read_string("name", obj, path);
    const auto mod_author = _read_string("author", obj, path);
    const auto mod_description = _read_string("description", obj, path);
    const auto mod_license = _read_string("license", obj, path);
    const auto version = _read_mod_version(obj, path);
    const auto mod_path = path.parent_path();
    const auto dependencies = _read_dependencies(obj, path);

    return ModManifest{mod_id,
                       mod_name,
                       mod_author,
                       mod_description,
                       mod_license,
                       version,
                       mod_path,
                       dependencies};
}

} // namespace lua
} // namespace elona
