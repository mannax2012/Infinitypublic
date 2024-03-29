/*
 * JediQueueCommand.h
 *
 *  Created on: Apr 9, 2015
 *      Author: valkyra
 */

#ifndef JEDIQUEUECOMMAND_H_
#define JEDIQUEUECOMMAND_H_


#include "server/zone/objects/creature/buffs/Buff.h"
#include "QueueCommand.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/managers/visibility/VisibilityManager.h"
#include "server/zone/objects/creature/buffs/SingleUseBuff.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/objects/group/GroupObject.h"

class JediQueueCommand : public QueueCommand {

protected:
	int forceCost;
	int duration;
	uint32 animationCRC;
	String clientEffect;
	float speedMod;
	int visMod;
	int buffClass;

	uint32 buffCRC;
	Vector<uint32> overrideableCRCs;
	Vector<uint32> blockingCRCs;
	Vector<unsigned int> singleUseEventTypes;


public:
	enum { BASE_BUFF, SINGLE_USE_BUFF };

	JediQueueCommand(const String& name, ZoneProcessServer* server) : QueueCommand(name, server) {
		forceCost = 0;
		duration = 0;
		animationCRC = 0;
		clientEffect = "";
		buffClass = BASE_BUFF;
		speedMod = 0;
		visMod = 10;
		buffCRC = 0;

	}



	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {
		return SUCCESS;
	}

	bool isJediQueueCommand() {
		return true;
	}

	int doJediSelfBuffCommand(CreatureObject* creature) const {
		// first and foremost, we need to toggle this buff off if we already have it
		if (creature->hasBuff(buffCRC)) {
			if (buffCRC == BuffCRC::JEDI_FORCE_RUN_2)
				return SUCCESS;
			Locker clocker(creature);
			creature->removeBuff(buffCRC);
			if (buffCRC != BuffCRC::JEDI_RESIST_STATES && buffCRC != BuffCRC::JEDI_FORCE_SPEED_1 && buffCRC != BuffCRC::JEDI_FORTIFY && buffCRC != BuffCRC::JEDI_FORCE_SPEED_2 && buffCRC != BuffCRC::JEDI_RESIST_DISEASE && buffCRC != BuffCRC::JEDI_RESIST_POISON && buffCRC != BuffCRC::JEDI_RESIST_BLEEDING ) //some buffs we want to toggle, others we want to refresh.
			return SUCCESS;
		}

		// Do checks first.
		int res = doCommonJediSelfChecks(creature);

		if (res != SUCCESS)
			return res;

        return doBuff(creature);

	}

	int doBuff(CreatureObject* creature) const {
		ManagedReference<Buff*> buff = createJediSelfBuff(creature);

		if (buff == nullptr)
			return GENERALERROR;

		Locker locker(buff);
 		creature->addBuff(buff);
 		doForceCost(creature);
		if (!clientEffect.isEmpty()) {
			creature->playEffect(clientEffect, "");
		}
		return SUCCESS;
	}

	int doJediForceCostCheck(CreatureObject* creature) const {
		//Check for Force Cost..
		ManagedReference<PlayerObject*> playerObject = creature->getPlayerObject();

		if (playerObject && playerObject->getForcePower() < forceCost) {
			creature->sendSystemMessage("@jedi_spam:no_force_power"); //"You do not have enough Force Power to peform that action.
			return GENERALERROR;
		}

		return SUCCESS;
	}

	int doCommonJediSelfChecks(CreatureObject* creature) const {
		int res = doCommonMedicalCommandChecks(creature);

		if (res != SUCCESS)
			return res;

		if (isWearingArmor(creature))
			return NOJEDIARMOR;

		for (int i=0; i < blockingCRCs.size(); ++i) {
			if (creature->hasBuff(blockingCRCs.get(i))) {
				return NOSTACKJEDIBUFF;
			}
		}

		res = doJediForceCostCheck(creature);
		return res;
	}

	ManagedReference<Buff*> createJediSelfBuff(CreatureObject* creature) const {

		for (int i=0; i < overrideableCRCs.size(); ++i) {
			int buff = overrideableCRCs.get(i);
			if (creature->hasBuff(buff)) {
				creature->removeBuff(buff);
			}
		}

		// Create buff object.
		ManagedReference<Buff*> buff = nullptr;
		int newDuration = duration;

		if(buffClass == BASE_BUFF || singleUseEventTypes.size() == 0) {

			/*  Save for possible future use
			if (buffCRC == BuffCRC::JEDI_FORCE_RUN_2 && creature->hasSkill("force_discipline_healing_master")) //Force Run 2 lasts twice as long for master healer
				newDuration *= 2;
			*/
			buff = new Buff(creature, buffCRC, newDuration, BuffType::JEDI);
		}
		else if (buffClass == SINGLE_USE_BUFF) {

			if (creature->hasSkill("force_discipline_enhancements_master") && (buffCRC == BuffCRC::JEDI_FORCE_ABSORB_1 || buffCRC == BuffCRC::JEDI_FORCE_ABSORB_2 || buffCRC == BuffCRC::JEDI_FORCE_FEEDBACK_1  || buffCRC == BuffCRC::JEDI_FORCE_FEEDBACK_2))  //Buff uration 2X for Master Enhancer
				newDuration *= 2;

			SingleUseBuff* suBuff = new SingleUseBuff(creature, buffCRC, newDuration, BuffType::JEDI, getNameCRC());

			buff = suBuff;

			// Otherwise the compiler complains we're modifying member variables in a "const" function
			Vector<uint32> singleUseTypesCopy = singleUseEventTypes;

			Locker blocker(suBuff);
			suBuff->init(&singleUseTypesCopy);

		} 
		else {
			error("Unknown buff type");
			return nullptr;
		}

		Locker locker(buff);

		if (speedMod > 0) {
			buff->setSpeedMultiplierMod(speedMod);
			buff->setAccelerationMultiplierMod(speedMod);
		}

		StringIdChatParameter start("jedi_spam", "apply_" + name);
		StringIdChatParameter end("jedi_spam", "remove_" + name);

		buff->setStartMessage(start);
		buff->setEndMessage(end);

		for (int i=0; i < skillMods.size(); ++i) {
			buff->setSkillModifier(skillMods.elementAt(i).getKey(), skillMods.elementAt(i).getValue());
		}

		return buff;
	}

	void doForceCost(CreatureObject* creature) const {
		// Force Cost.
		ManagedReference<PlayerObject*> playerObject =
				creature->getPlayerObject();
		playerObject->setForcePower(playerObject->getForcePower() - forceCost);

		VisibilityManager::instance()->increaseVisibility(creature, visMod);
	}

	void setForceCost(int fc) {
		forceCost = fc;
	}

	void setDuration(int dur) {
		duration = dur;
	}

	void setAnimationCRC(uint32 crc) {
		animationCRC = crc;
	}

	void setClientEffect(String ce) {
		clientEffect = ce;
	}

	void setSpeedMod(float sm) {
		speedMod = sm;
	}

	void setBuffClass(int bt) {
		buffClass = bt;
	}

	void setVisMod(int vm) {
		visMod = vm;
	}

	int getVisMod() const {
		return visMod;
	}
};


#endif /* SRC_SERVER_ZONE_OBJECTS_CREATURE_COMMANDS_JEDIQUEUECOMMAND_H_ */
