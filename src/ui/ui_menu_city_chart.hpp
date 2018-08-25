#pragma once
#include "ui_menu.hpp"

namespace elona
{
namespace ui
{

class ui_menu_city_chart : public ui_menu<dummy_result>
{
public:
    ui_menu_city_chart()
    {
    }

protected:
    virtual bool init();
    virtual void update();
    virtual void draw();
    virtual optional<ui_menu_city_chart::result_type> on_key(
        const std::string& key);
};

} // namespace ui
} // namespace elona
