warren_altered_atst = Creature:new {
	objectName = "@mob/creature_names:warren_agro_droid_atst",
	socialGroup = "warren_imperial",
	faction = "",
	level = 46,
	chanceHit = 0.36,
	damageMin = 400,
	damageMax = 600,
	baseXp = 4000,
	baseHAM = 12000,
	baseHAMmax = 15000,
	armor = 0,
	resists = {30,30,40,40,-1,40,-1,-1,-1},
	meatType = "",
	meatAmount = 0,
	hideType = "",
	hideAmount = 0,
	boneType = "",
	boneAmount = 0,
	milk = 0,
	tamingChance = 0,
	ferocity = 0,
	pvpBitmask = ATTACKABLE + AGGRESSIVE + ENEMY,
	creatureBitmask = PACK + KILLER,
	optionsBitmask = AIENABLED,
	diet = HERBIVORE,

	templates = {"object/mobile/atst.iff"},
	lootGroups = {
	},
	conversationTemplate = "",
	defaultAttack = "defaultdroidattack",
	defaultWeapon = "object/weapon/ranged/vehicle/vehicle_atst_ranged.iff"
}

CreatureTemplates:addCreatureTemplate(warren_altered_atst, "warren_altered_atst")
