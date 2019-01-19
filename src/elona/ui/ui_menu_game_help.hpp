#pragma once
#include "ui_menu.hpp"
namespace elona
{
namespace ui
{
class UIMenuGameHelp : public UIMenu<DummyResult>
{
public:
    UIMenuGameHelp()
    {
    }

protected:
    virtual bool init();
    virtual void update();
    virtual void draw();
    virtual optional<UIMenuGameHelp::ResultType> on_key(const std::string& key);

    void _draw_key_list();
    void _draw_regular_pages();
    void _draw_window();
    void _draw_navigation_menu();
    void _draw_background_vignette(int id, int type);
};
} // namespace ui
} // namespace elona
