MODULE := engines/mm

MODULE_OBJS := \
	metaengine.o \
	mm.o \
	shared/utils/bitmap_font.o \
	shared/utils/engine_data.o \
	shared/utils/strings.o \
	shared/utils/strings_data.o \
	shared/utils/xeen_font.o \
	shared/xeen/cc_archive.o \
	shared/xeen/file.o \
	shared/xeen/sound.o \
	shared/xeen/sound_driver.o \
	shared/xeen/sound_driver_adlib.o \
	shared/xeen/sprites.o \
	shared/xeen/xsurface.o

ifdef ENABLE_MM1
MODULE_OBJS += \
	mm1/console.o \
	mm1/events.o \
	mm1/globals.o \
	mm1/messages.o \
	mm1/metaengine.o \
	mm1/mm1.o \
	mm1/sound.o \
	mm1/data/active_spells.o \
	mm1/data/character.o \
	mm1/data/game_state.o \
	mm1/data/items.o \
	mm1/data/locations.o \
	mm1/data/monsters.o \
	mm1/data/party.o \
	mm1/data/roster.o \
	mm1/data/text_parser.o \
	mm1/data/trap.o \
	mm1/data/treasure.o \
	mm1/game/arrested.o \
	mm1/game/game_logic.o \
	mm1/game/combat.o \
	mm1/game/encounter.o \
	mm1/game/equip_remove.o \
	mm1/game/monster_touch.o \
	mm1/game/rest.o \
	mm1/game/spell_casting.o \
	mm1/game/spells_party.o \
	mm1/game/spells_monsters.o \
	mm1/game/use_item.o \
	mm1/game/view_base.o \
	mm1/gfx/dta.o \
	mm1/gfx/gfx.o \
	mm1/gfx/screen_decoder.o \
	mm1/utils/mouse.o \
	mm1/utils/strings.o \
	mm1/views/locations/location.o \
	mm1/views/locations/blacksmith.o \
	mm1/views/locations/blacksmith_subview.o \
	mm1/views/locations/blacksmith_buy_weapons.o \
	mm1/views/locations/blacksmith_buy_armor.o \
	mm1/views/locations/blacksmith_buy_misc.o \
	mm1/views/locations/blacksmith_sell_item.o \
	mm1/views/locations/inn.o \
	mm1/views/locations/market.o \
	mm1/views/locations/statue.o \
	mm1/views/locations/tavern.o \
	mm1/views/locations/temple.o \
	mm1/views/locations/training.o \
	mm1/views/maps/access_code.o \
	mm1/views/maps/alamar.o \
	mm1/views/maps/alien.o \
	mm1/views/maps/answer_entry.o \
	mm1/views/maps/arrested.o \
	mm1/views/maps/chess.o \
	mm1/views/maps/dog_statue.o \
	mm1/views/maps/ghost.o \
	mm1/views/maps/giant.o \
	mm1/views/maps/gypsy.o \
	mm1/views/maps/hacker.o \
	mm1/views/maps/ice_princess.o \
	mm1/views/maps/inspectron.o \
	mm1/views/maps/keeper.o \
	mm1/views/maps/lion.o \
	mm1/views/maps/lord_archer.o \
	mm1/views/maps/lord_ironfist.o \
	mm1/views/maps/orango.o \
	mm1/views/maps/prisoners.o \
	mm1/views/maps/resistances.o \
	mm1/views/maps/ruby.o \
	mm1/views/maps/trivia.o \
	mm1/views/maps/volcano_god.o \
	mm1/views/maps/won_game.o \
	mm1/views/spells/spell_view.o \
	mm1/views/spells/cast_spell.o \
	mm1/views/spells/detect_magic.o \
	mm1/views/spells/duplication.o \
	mm1/views/spells/fly.o \
	mm1/views/spells/location.o \
	mm1/views/spells/recharge_item.o \
	mm1/views/spells/teleport.o \
	mm1/views/are_you_ready.o \
	mm1/views/bash.o \
	mm1/views/character_base.o \
	mm1/views/character_info.o \
	mm1/views/character_manage.o \
	mm1/views/character_view_combat.o \
	mm1/views/characters.o \
	mm1/views/combat.o \
	mm1/views/create_characters.o \
	mm1/views/dead.o \
	mm1/views/encounter.o \
	mm1/views/game.o \
	mm1/views/game_commands.o \
	mm1/views/game_messages.o \
	mm1/views/game_party.o \
	mm1/views/game_view.o \
	mm1/views/main_menu.o \
	mm1/views/order.o \
	mm1/views/protect.o \
	mm1/views/quick_ref.o \
	mm1/views/rest.o \
	mm1/views/search.o \
	mm1/views/title.o \
	mm1/views/text_entry.o \
	mm1/views/text_view.o \
	mm1/views/trap.o \
	mm1/views/unlock.o \
	mm1/views_enh/spells/cast_spell.o \
	mm1/views_enh/spells/spellbook.o \
	mm1/views_enh/button_container.o \
	mm1/views_enh/character_base.o \
	mm1/views_enh/character_info.o \
	mm1/views_enh/character_inventory.o \
	mm1/views_enh/character_manage.o \
	mm1/views_enh/character_select.o \
	mm1/views_enh/character_view.o \
	mm1/views_enh/characters.o \
	mm1/views_enh/combat.o \
	mm1/views_enh/confirm.o \
	mm1/views_enh/create_characters.o \
	mm1/views_enh/dialogs.o \
	mm1/views_enh/encounter.o \
	mm1/views_enh/exchange.o \
	mm1/views_enh/game.o \
	mm1/views_enh/game_commands.o \
	mm1/views_enh/game_messages.o \
	mm1/views_enh/game_party.o \
	mm1/views_enh/game_view.o \
	mm1/views_enh/items_view.o \
	mm1/views_enh/main_menu.o \
	mm1/views_enh/map.o \
	mm1/views_enh/map_popup.o \
	mm1/views_enh/party_view.o \
	mm1/views_enh/protect.o \
	mm1/views_enh/rest.o \
	mm1/views_enh/quick_ref.o \
	mm1/views_enh/scroll_popup.o \
	mm1/views_enh/scroll_text.o \
	mm1/views_enh/scroll_view.o \
	mm1/views_enh/search.o \
	mm1/views_enh/select_number.o \
	mm1/views_enh/text_entry.o \
	mm1/views_enh/text_view.o \
	mm1/views_enh/title.o \
	mm1/views_enh/trade.o \
	mm1/views_enh/trap.o \
	mm1/views_enh/unlock.o \
	mm1/views_enh/which_character.o \
	mm1/views_enh/which_item.o \
	mm1/views_enh/who_will_try.o \
	mm1/views_enh/yes_no.o \
	mm1/views_enh/interactions/access_code.o \
	mm1/views_enh/interactions/arrested.o \
	mm1/views_enh/interactions/interaction.o \
	mm1/views_enh/interactions/interaction_query.o \
	mm1/views_enh/interactions/resistances.o \
	mm1/views_enh/interactions/statue.o \
	mm1/views_enh/locations/blacksmith.o \
	mm1/views_enh/locations/blacksmith_items.o \
	mm1/views_enh/locations/inn.o \
	mm1/views_enh/locations/location.o \
	mm1/views_enh/locations/market.o \
	mm1/views_enh/locations/tavern.o \
	mm1/views_enh/locations/temple.o \
	mm1/views_enh/locations/training.o \
	mm1/maps/maps.o \
	mm1/maps/map.o \
	mm1/maps/map_desert.o \
	mm1/maps/map_town.o \
	mm1/maps/map00.o \
	mm1/maps/map01.o \
	mm1/maps/map02.o \
	mm1/maps/map03.o \
	mm1/maps/map04.o \
	mm1/maps/map05.o \
	mm1/maps/map06.o \
	mm1/maps/map07.o \
	mm1/maps/map08.o \
	mm1/maps/map09.o \
	mm1/maps/map10.o \
	mm1/maps/map11.o \
	mm1/maps/map12.o \
	mm1/maps/map13.o \
	mm1/maps/map14.o \
	mm1/maps/map15.o \
	mm1/maps/map16.o \
	mm1/maps/map17.o \
	mm1/maps/map18.o \
	mm1/maps/map19.o \
	mm1/maps/map20.o \
	mm1/maps/map21.o \
	mm1/maps/map22.o \
	mm1/maps/map23.o \
	mm1/maps/map24.o \
	mm1/maps/map25.o \
	mm1/maps/map26.o \
	mm1/maps/map27.o \
	mm1/maps/map28.o \
	mm1/maps/map29.o \
	mm1/maps/map30.o \
	mm1/maps/map31.o \
	mm1/maps/map32.o \
	mm1/maps/map33.o \
	mm1/maps/map34.o \
	mm1/maps/map35.o \
	mm1/maps/map36.o \
	mm1/maps/map37.o \
	mm1/maps/map38.o \
	mm1/maps/map39.o \
	mm1/maps/map40.o \
	mm1/maps/map41.o \
	mm1/maps/map42.o \
	mm1/maps/map43.o \
	mm1/maps/map44.o \
	mm1/maps/map45.o \
	mm1/maps/map46.o \
	mm1/maps/map47.o \
	mm1/maps/map48.o \
	mm1/maps/map49.o \
	mm1/maps/map50.o \
	mm1/maps/map51.o \
	mm1/maps/map52.o \
	mm1/maps/map53.o \
	mm1/maps/map54.o
endif

ifdef ENABLE_XEEN
MODULE_OBJS += \
	xeen/worldofxeen/clouds_cutscenes.o \
	xeen/worldofxeen/darkside_cutscenes.o \
	xeen/worldofxeen/worldofxeen_cutscenes.o \
	xeen/worldofxeen/worldofxeen_menu.o \
	xeen/worldofxeen/worldofxeen.o \
	xeen/swordsofxeen/swordsofxeen.o \
	xeen/swordsofxeen/swordsofxeen_menu.o \
	xeen/dialogs/credits_screen.o \
	xeen/dialogs/dialogs.o \
	xeen/dialogs/dialogs_awards.o \
	xeen/dialogs/dialogs_char_info.o \
	xeen/dialogs/dialogs_control_panel.o \
	xeen/dialogs/dialogs_copy_protection.o \
	xeen/dialogs/dialogs_create_char.o \
	xeen/dialogs/dialogs_difficulty.o \
	xeen/dialogs/dialogs_dismiss.o \
	xeen/dialogs/dialogs_exchange.o \
	xeen/dialogs/dialogs_info.o \
	xeen/dialogs/dialogs_input.o \
	xeen/dialogs/dialogs_items.o \
	xeen/dialogs/dialogs_map.o \
	xeen/dialogs/dialogs_message.o \
	xeen/dialogs/dialogs_party.o \
	xeen/dialogs/dialogs_query.o \
	xeen/dialogs/dialogs_quests.o \
	xeen/dialogs/dialogs_quick_fight.o \
	xeen/dialogs/dialogs_quick_ref.o \
	xeen/dialogs/dialogs_spells.o \
	xeen/dialogs/dialogs_whowill.o \
	xeen/dialogs/please_wait.o \
	xeen/metaengine.o \
	xeen/character.o \
	xeen/combat.o \
	xeen/cutscenes.o \
	xeen/debugger.o \
	xeen/events.o \
	xeen/files.o \
	xeen/font.o \
	xeen/interface.o \
	xeen/interface_minimap.o \
	xeen/interface_scene.o \
	xeen/item.o \
	xeen/locations.o \
	xeen/map.o \
	xeen/party.o \
	xeen/patcher.o \
	xeen/resources.o \
	xeen/saves.o \
	xeen/screen.o \
	xeen/scripts.o \
	xeen/spells.o \
	xeen/sprites.o \
	xeen/subtitles.o \
	xeen/window.o \
	xeen/xeen.o
endif

# This module can be built as a plugin
ifeq ($(ENABLE_MM), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
