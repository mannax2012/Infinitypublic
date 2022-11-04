jawa_common = {
	description = "",
	minimumLevel = 0,
	maximumLevel = -1,
	lootItems = {
		{itemTemplate = "jawa_beads",			weight = 25 * (100000)},
		{groupTemplate = "wearables_common",	weight = 75 * (100000)}
	}
}

addLootGroupTemplate("jawa_common", jawa_common)
