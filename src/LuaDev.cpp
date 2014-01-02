// Copyright © 2008-2014 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "LuaDev.h"
#include "LuaObject.h"
#include "Pi.h"
#include "Player.h"
#include "Ship.h"
#include "WorldView.h"
#include "Game.h"
#include "scenegraph/ModelSkin.h"

/*
 * Lua commands used in development & debugging
 * Everything here is subject to rapid changes
 */

/*
 * Set current camera offset to vector,
 * (the offset will reset when switching cameras)
 *
 * Dev.SetCameraOffset(x, y, z)
 */
static int l_dev_set_camera_offset(lua_State *l)
{
	if (!Pi::worldView)
		return luaL_error(l, "Dev.SetCameraOffset only works when there is a game running");
	CameraController *cam = Pi::worldView->GetCameraController();
	const float x = luaL_checknumber(l, 1);
	const float y = luaL_checknumber(l, 2);
	const float z = luaL_checknumber(l, 3);
	cam->SetPosition(vector3d(x, y, z));
	return 0;
}

static int l_dev_spawn_target_drone(lua_State *l)
{
	SceneGraph::ModelSkin skin;
	skin.SetPrimaryColor(Color::YELLOW);
	skin.SetSecondaryColor(Color::YELLOW);
	skin.SetTrimColor(Color::YELLOW);

	const vector3d dir = -Pi::player->GetOrient().VectorZ();
	Ship *ship = new Ship("targetdrone");
	ship->AIFlyTo(Pi::player);
	ship->SetFrame(Pi::player->GetFrame());
	ship->SetPosition(Pi::player->GetPosition()+500.0*dir);
	ship->SetVelocity(Pi::player->GetVelocity());
	ship->UpdateStats();
	ship->SetSkin(skin);

	Pi::game->GetSpace()->AddBody(ship);
	return 0;
}

static int l_dev_spawn_enemy(lua_State *l)
{
	SceneGraph::ModelSkin skin;
	skin.SetPrimaryColor(Color::RED);
	skin.SetSecondaryColor(Color::RED);
	skin.SetTrimColor(Color::RED);

	const vector3d dir = -Pi::player->GetOrient().VectorZ();
	Ship *ship = new Ship("lunarshuttle");
	ship->m_equipment.Set(Equip::SLOT_LASER, 0, Equip::PULSECANNON_DUAL_1MW);
	ship->AIKill(Pi::player);
	ship->SetFrame(Pi::player->GetFrame());
	ship->SetPosition(Pi::player->GetPosition()+500.0*dir);
	ship->SetVelocity(Pi::player->GetVelocity());
	ship->UpdateStats();
	ship->SetSkin(skin);
	Pi::game->GetSpace()->AddBody(ship);
	return 0;
}

void LuaDev::Register()
{
	lua_State *l = Lua::manager->GetLuaState();

	LUA_DEBUG_START(l);

	static const luaL_Reg methods[]= {
		{ "SetCameraOffset", l_dev_set_camera_offset },
		{ "SpawnTargetDrone", l_dev_spawn_target_drone },
		{ "SpawnEnemy", l_dev_spawn_enemy },
		{ 0, 0 }
	};

	lua_getfield(l, LUA_REGISTRYINDEX, "CoreImports");
	luaL_newlib(l, methods);
	lua_setfield(l, -2, "Dev");
	lua_pop(l, 1);

	LUA_DEBUG_END(l, 0);
}
