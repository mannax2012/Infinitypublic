/*
 * DestructibleCityDecorationMenuComponent.cpp
 *
 *  Created on: July 5, 2022
 *      Author: wickedHangover
 */

#include "server/zone/objects/creature/CreatureObject.h"
#include "DestructibleCityDecorationMenuComponent.h"
#include "server/zone/packets/object/ObjectMenuResponse.h"
#include "server/zone/objects/region/CityRegion.h"
#include "server/zone/managers/city/CityManager.h"
#include "server/zone/managers/city/CityDecorationTask.h"
#include "server/zone/managers/director/DirectorManager.h"

void DestructibleCityDecorationMenuComponent::fillObjectMenuResponse(SceneObject* sceneObject, ObjectMenuResponse* menuResponse, CreatureObject* player) const {
	TangibleObjectMenuComponent::fillObjectMenuResponse(sceneObject, menuResponse, player);

	ManagedReference<CityRegion*> city = player->getCityRegion().get();

	PlayerObject* playerObject = player->getPlayerObject();

	if (playerObject == NULL)
		return;

	if (player->getPlayerObject() != NULL && player->getPlayerObject()->isPrivileged()) {
		menuResponse->addRadialMenuItem(231, 3, "@city/city:mgmt_destroy"); // Place Decoration
		menuResponse->addRadialMenuItem(235, 3, "@city/city:mgmt_putallback"); // Place Decoration
	}

	if (player->hasSkill("combat_demolition_novice")) {
		menuResponse->addRadialMenuItem(232, 3, "@city/city:demo_setexplosive"); // Place Decoration
	}

	if(isInInventory(sceneObject, player) && player->getParent() == NULL) {
		menuResponse->addRadialMenuItem(233, 3, "@city/city:place"); // Place Decoration
	}

	if(city != NULL && sceneObject->getParent() == NULL && sceneObject->getCityRegion() == city && city->isMayor(player->getObjectID()) ) {
		menuResponse->addRadialMenuItem(234, 3, "@city/city:mt_remove"); // Remove

		menuResponse->addRadialMenuItem(73, 3, "@city/city:align"); // Align
		menuResponse->addRadialMenuItemToRadialID(73, 74, 3, "@city/city:north"); // North
		menuResponse->addRadialMenuItemToRadialID(73, 75, 3, "@city/city:east"); // East
		menuResponse->addRadialMenuItemToRadialID(73, 76, 3, "@city/city:south"); // South
		menuResponse->addRadialMenuItemToRadialID(73, 77, 3, "@city/city:west"); // West
	}

}

int DestructibleCityDecorationMenuComponent::handleObjectMenuSelect(SceneObject* sceneObject, CreatureObject* player, byte selectedID) const {
	if (!sceneObject->isTangibleObject())
		return 0;

	if (!player->isPlayerCreature())
		return 0;
	if (selectedID == 231) {

		PlayerObject* ghost = player->getPlayerObject();

		if (ghost != NULL) {
			Lua* lua = DirectorManager::instance()->getLuaInstance();
			Reference<LuaFunction*> luaObtainData = lua->createFunction("demolitions", "AdminBlowItUp", 0);
			*luaObtainData << player;
			*luaObtainData << sceneObject;

			luaObtainData->callFunction();
		}
		return 0;
	}else if (selectedID == 232) {

		PlayerObject* ghost = player->getPlayerObject();

		if (ghost != NULL) {
			Lua* lua = DirectorManager::instance()->getLuaInstance();
			Reference<LuaFunction*> luaObtainData = lua->createFunction("demolitions", "blowItUp", 0);
			*luaObtainData << player;
			*luaObtainData << sceneObject;

			luaObtainData->callFunction();
		}
		return 0;
	} else if (selectedID == 233) {

		Reference<CityDecorationTask*> task = new CityDecorationTask(player, cast<TangibleObject*>(sceneObject), CityDecorationTask::PLACE);
		task->execute();
		return 0;

	}
	else if (selectedID == 234) {

		Reference<CityDecorationTask*> task = new CityDecorationTask(player, cast<TangibleObject*>(sceneObject), CityDecorationTask::REMOVE);
		task->execute();
		return 0;

	}
	else if (selectedID == 235) {

		PlayerObject* ghost = player->getPlayerObject();

		if (ghost != NULL) {
			Lua* lua = DirectorManager::instance()->getLuaInstance();
			Reference<LuaFunction*> luaObtainData = lua->createFunction("demolitions", "replaceAllObjects", 0);

			luaObtainData->callFunction();
		}
		return 0;

	}
	else if (selectedID == 74 || selectedID == 75 || selectedID == 76 || selectedID == 77) {
		ManagedReference<CityRegion*> city = player->getCityRegion().get();

		CityManager* cityManager = sceneObject->getZoneServer()->getCityManager();
		cityManager->alignAmenity(city, player, sceneObject, selectedID - 74);

		return 0;
	}

	return TangibleObjectMenuComponent::handleObjectMenuSelect(sceneObject, player, selectedID);
}

bool DestructibleCityDecorationMenuComponent::isInInventory(SceneObject* sceneObject, CreatureObject* player) const {
	return sceneObject->isASubChildOf(player);
}





