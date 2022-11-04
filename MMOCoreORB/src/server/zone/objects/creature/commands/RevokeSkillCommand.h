/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef REVOKESKILLCOMMAND_H_
#define REVOKESKILLCOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/managers/skill/SkillManager.h"

class RevokeSkillCommand : public QueueCommand {
public:

	RevokeSkillCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {
		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		ManagedReference<SceneObject*> object = server->getZoneServer()->getObject(target);

		if (object == nullptr || !object->isCreatureObject())
			return INVALIDTARGET;

		CreatureObject* targetCreature = cast<CreatureObject*>( object.get());

		Locker clocker(targetCreature, creature);

		SkillManager* skillManager = SkillManager::instance();
		skillManager->surrenderSkill(arguments.toString(), targetCreature, true);
		bool skillRemoved = skillManager->surrenderSkill(arguments.toString(), targetCreature, true, false, true);

		if (skillRemoved) {

			targetCreature->sendSystemMessage("An admin removed your skill " + arguments.toString() + ".");

			creature->sendSystemMessage("Removed skill " + arguments.toString() + " from " + targetCreature->getFirstName());
		}
		else {

			targetCreature->sendSystemMessage("An admin failed to remove your skill " + arguments.toString() + ".");

			creature->sendSystemMessage("Failed to remove skill " + arguments.toString() + " from " + targetCreature->getFirstName());
		}
		return SUCCESS;
	}

};

#endif //REVOKESKILLCOMMAND_H_
