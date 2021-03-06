#pragma once

#include "optional.hpp"



namespace elona
{

struct Character;
struct Item;
enum class TurnResult;



void rowact_check(Character& chara);
void rowact_item(const Item& item);

void activity_handle_damage(Character& chara);
optional<TurnResult> activity_proc(Character& chara);

void activity_perform(Character& performer);
void activity_sex();
void activity_blending();
void activity_eating(Character& eater, Item& food);
void activity_eating_finish(Character& eater, Item& food);
void activity_others(Character& doer);

void spot_fishing();
void spot_material();
void spot_digging();
void spot_mining_or_wall();
TurnResult do_dig_after_sp_check(Character& chara);
void matdelmain(int material_id, int amount = 1);
void matgetmain(int material_id, int amount = 1, int spot_type = 0);

} // namespace elona
