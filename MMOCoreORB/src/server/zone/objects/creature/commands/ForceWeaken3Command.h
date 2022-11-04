/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef FORCEWEAKEN3COMMAND_H_
#define FORCEWEAKEN3COMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"
#include "ForcePowersQueueCommand.h"

class ForceWeaken3Command : public ForcePowersQueueCommand {
public:

	ForceWeaken3Command(const String& name, ZoneProcessServer* server)
		: ForcePowersQueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		if (isWearingArmor(creature)) {
			return NOJEDIARMOR;
		}

		ManagedReference<SceneObject*> targetObject = server->getZoneServer()->getObject(target);

		if (targetObject == nullptr || !targetObject->isCreatureObject()) {
			return INVALIDTARGET;
		}

		ManagedReference<CreatureObject*> creatureTarget = targetObject.castTo<CreatureObject*>();
		if (creatureTarget == nullptr){
			return INVALIDTARGET;
		}

		String name = "forceweaken3";
		uint32 crc = name.hashCode();

		String name1 = "forceweaken1";
		uint32 crc1 = name1.hashCode();

		String name2 = "forceweaken2";
		uint32 crc2 = name2.hashCode();

		if (creatureTarget->hasBuff(crc)){
			creature->sendSystemMessage("Target has already been weakened");
			return GENERALERROR;
		}

		int res = doCombatAction(creature, target);

		if (res == SUCCESS) {

			// Setup debuff.

			if (creatureTarget != nullptr) {

				Locker clocker(creatureTarget, creature);

				if (creatureTarget->hasBuff(crc1))
					creatureTarget->removeBuff(crc1);

				if (creatureTarget->hasBuff(crc2))
					creatureTarget->removeBuff(crc2);

				float frsBonus = creature->getFrsMod("power");
				ManagedReference<Buff*> buff = nullptr;

				if (creature->isPlayerCreature())
					buff = new Buff(creatureTarget, crc, 60, BuffType::JEDI);
				else
					buff = new Buff(creatureTarget, crc, 20, BuffType::JEDI);


				if (buff == nullptr)
					return GENERALERROR;

				Locker locker(buff);
				int hamStrength =  -1 * creatureTarget->getMaxHAM(CreatureAttribute::HEALTH) * .2 * frsBonus;
				buff->setAttributeModifier(CreatureAttribute::HEALTH, hamStrength);
				hamStrength =  -1 * creatureTarget->getMaxHAM(CreatureAttribute::ACTION) * .2 * frsBonus;
				buff->setAttributeModifier(CreatureAttribute::ACTION, hamStrength);
				hamStrength =  -1 * creatureTarget->getMaxHAM(CreatureAttribute::MIND) * .2 * frsBonus;
				buff->setAttributeModifier(CreatureAttribute::MIND, hamStrength);


				buff->setSkillModifier("weaken_delay", 15);
				buff->setSkillModifier("jedi_force_power_regen", -10);

				creatureTarget->addBuff(buff);

				CombatManager::instance()->broadcastCombatSpam(creature, creatureTarget, NULL, 0, "cbt_spam", combatSpam + "_hit", 1);
			}

		}

		return res;
	}

};

#endif //FORCEWEAKEN3COMMAND_H_
