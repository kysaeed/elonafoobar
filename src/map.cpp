#include "map.hpp"
#include "character.hpp"
#include "ctrl_file.hpp"
#include "db_item.hpp"
#include "elona.hpp"
#include "item.hpp"
#include "itemgen.hpp"
#include "map_cell.hpp"
#include "mef.hpp"
#include "position.hpp"
#include "variables.hpp"



namespace elona
{


CellData cell_data;



#define MAP_PACK(n, ident) legacy_map(x, y, n) = ident;
#define MAP_UNPACK(n, ident) ident = legacy_map(x, y, n);

#define SERIALIZE_ALL() \
    SERIALIZE(0, chip_id_memory); \
    SERIALIZE(1, chara_index_plus_one); \
    SERIALIZE(2, chip_id_actual); \
    /* 3 */ \
    SERIALIZE(4, item_appearances_memory); \
    SERIALIZE(5, item_appearances_actual); \
    SERIALIZE(6, feats); \
    SERIALIZE(7, blood_and_fragments); \
    SERIALIZE(8, mef_id_plus_one); \
    SERIALIZE(9, light);


#define SERIALIZE MAP_PACK
void Cell::pack_to(elona_vector3<int>& legacy_map, int x, int y)
{
    SERIALIZE_ALL();
}
#undef SERIALIZE


#define SERIALIZE MAP_UNPACK
void Cell::unpack_from(elona_vector3<int>& legacy_map, int x, int y)
{
    SERIALIZE_ALL();
}
#undef SERIALIZE



void Cell::clear()
{
    *this = {};
}



void CellData::init(int width, int height)
{
    cells.clear();
    width_ = width;
    height_ = height;

    for (int y = 0; y < height_; y++)
    {
        std::vector<Cell> column;
        for (int x = 0; x < width_; x++)
        {
            column.emplace_back(Cell{});
        }
        cells.emplace_back(std::move(column));
    }
}



void CellData::pack_to(elona_vector3<int>& legacy_map)
{
    assert(legacy_map.j_size() == width_);
    assert(legacy_map.k_size() == height_);

    for (int y = 0; y < height_; y++)
    {
        for (int x = 0; x < width_; x++)
        {
            at(x, y).pack_to(legacy_map, x, y);
        }
    }
}



void CellData::unpack_from(elona_vector3<int>& legacy_map)
{
    assert(legacy_map.j_size() == width_);
    assert(legacy_map.k_size() == height_);

    for (int y = 0; y < height_; y++)
    {
        for (int x = 0; x < width_; x++)
        {
            at(x, y).unpack_from(legacy_map, x, y);
        }
    }
}



void map_reload(const std::string& map_filename)
{
    fmapfile = (filesystem::dir::map() / map_filename).generic_string();
    ctrl_file(FileOperation::map_load_map_obj_files);

    for (int y = 0; y < mdata_map_height; ++y)
    {
        for (int x = 0; x < mdata_map_width; ++x)
        {
            map(x, y, 8) = 0;
            map(x, y, 9) = 0;
        }
    }

    mef_clear_all();

    for (const auto& i : items(-1))
    {
        if (inv[i].number() > 0)
        {
            if (inv[i].own_state == 1)
            {
                if (the_item_db[inv[i].id]->category == 57000)
                {
                    inv[i].remove();
                    cell_refresh(inv[i].position.x, inv[i].position.y);
                }
            }
        }
    }

    for (int i = 0; i < 400; ++i)
    {
        if (cmapdata(0, i) == 0)
            continue;
        const auto x = cmapdata(1, i);
        const auto y = cmapdata(2, i);
        if (cmapdata(4, i) == 0)
        {
            if (map(x, y, 4) == 0)
            {
                flt();
                int stat = itemcreate(-1, cmapdata(0, i), x, y, 0);
                if (stat != 0)
                {
                    inv[ci].own_state = cmapdata(3, i);
                }
            }
            cell_refresh(x, y);
        }
    }
}



} // namespace elona
