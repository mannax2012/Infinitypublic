/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions. */

#ifndef FORCERUN2COMMAND_H_
#define FORCERUN2COMMAND_H_

#include "server/zone/objects/creature/buffs/PrivateSkillMultiplierBuff.h"
#include "JediQueueCommand.h"

class ForceRun2Command : public JediQueueCommand {
public:

	ForceRun2Command(const String& name, ZoneProcessServer* server)
	: JediQueueCommand(name, server) {

		// BuffCRC's, first one is used.
		buffCRC = BuffCRC::JEDI_FORCE_RUN_2;

        // If these are active they will block buff use
		//blockingCRCs.add(BuffCRC::JEDI_FORCE_RUN_1);
		//blockingCRCs.add(BuffCRC::JEDI_FORCE_RUN_3);
        
		skillMods.put("force_run", 2);
		skillMods.put("slope_move", 66);
	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {


   		if (!creature->checkCooldownRecovery("forcerun2")) {
                    creature->sendSystemMessage("You cannot use Force Run 2 yet.");
                    return GENERALERROR;
                }

        int res = doJediSelfBuffCommand(creature);

		if (res != SUCCESS) {
			return res;
		}

/*		Removing damage reduction on force run 2 buff
		// need to apply the damage reduction in a separate buff so that the multiplication and division applies right
		Buff* buff = creature->getBuff(BuffCRC::JEDI_FORCE_RUN_2);
		if (buff == nullptr)
			return GENERALERROR;

		ManagedReference<PrivateSkillMultiplierBuff*> multBuff = new PrivateSkillMultiplierBuff(creature, name.hashCode(), duration, BuffType::JEDI);
		Locker locker(multBuff);  //damage reduction
		multBuff->setSkillModifier("private_damage_divisor", 20);
		creature->addBuff(multBuff);
		locker.release();

		Locker blocker(buff);

		//buff->addSecondaryBuffCRC(multBuff->getBuffCRC());
*/

		creature->updateCooldownTimer("forcerun2", 150000);		//Cooldown is 150 seconds from start of buff

		if (creature->hasBuff(BuffCRC::JEDI_FORCE_RUN_3)) {		//Cancel Force Run 3 if Force Run 2 activated
			creature->removeBuff(BuffCRC::JEDI_FORCE_RUN_3);
		}

		if (creature->hasBuff(BuffCRC::JEDI_FORCE_RUN_1)) {		//Cancel Force Run 1 if Force Run 2 activated
			creature->removeBuff(BuffCRC::JEDI_FORCE_RUN_1);
		}

		if (creature->hasBuff(STRING_HASHCODE("burstrun")) || creature->hasBuff(STRING_HASHCODE("retreat"))) {  //Cancel Burst Run / Retreat if Force Run 2 activated
			creature->removeBuff(STRING_HASHCODE("burstrun"));
			creature->removeBuff(STRING_HASHCODE("retreat"));
		}

		return SUCCESS;
	}

};

#endif //FORCERUN2COMMAND_H_
