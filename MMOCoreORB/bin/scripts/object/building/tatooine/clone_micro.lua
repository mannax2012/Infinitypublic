
object_building_tatooine_clone_micro = object_building_tatooine_shared_clone_micro:new {
	planetMapCategory = "cloningfacility",
	facilityType = CLONER_STANDARD,

	skillMods = {
		{"private_buff_mind", 100},
		{"private_faction_buff_mind", 25},
		{"private_med_battle_fatigue", 5},
		{"private_faction_mind_heal", 10},
		{"private_medical_rating", 100},
		{"private_faction_medical_rating", 25},
	},

	childObjects = {
		{templateFile = "object/tangible/terminal/terminal_cloning.iff", x = 2.7, z = .3, y = 0, ox = 0, oy = .7, oz = 0, ow = -.7, cellid = 1, containmentType = -1},
		{templateFile = "object/tangible/terminal/terminal_insurance.iff", x = 0, z = .3, y = -2, ox = 0, oy = 0, oz = 0, ow = 0, cellid = 1, containmentType = -1}
	},

	spawningPoints = {
		{ x = 0, z = 0, y = 0, ow = 0.704268, ox = 0, oz = 0, oy = 0, cellid = 1 },
	},
	templateType = CLONINGBUILDING
}
--
ObjectTemplates:addTemplate(object_building_tatooine_clone_micro, "object/building/tatooine/clone_micro.iff")
