MODULE := engines/m4

MODULE_OBJS = \
	m4.o \
	console.o \
	globals.o \
	kernel.o \
	metaengine.o \
	param.o \
	res.o \
	adv_db_r/db_catalog.o \
	adv_r/adv_been.o \
	adv_r/adv_control.o \
	adv_r/adv_file.o \
	adv_r/adv_inv.o \
	adv_r/adv_player.o \
	adv_r/adv_rails.o \
	adv_r/adv_trigger.o \
	adv_r/db_env.o \
	core/cstring.o \
	core/errors.o \
	core/imath.o \
	core/mouse.o \
	core/term.o \
	events/mickey.o \
	events/mouse_handler.o \
	fileio/extensions.o \
	fileio/fileio.o \
	fileio/fstream.o \
	fileio/info.o \
	fileio/sys_file.o \
	graphics/gr_buff.o \
	graphics/gr_draw.o \
	graphics/gr_color.o \
	graphics/gr_font.o \
	graphics/gr_line.o \
	graphics/gr_pal.o \
	graphics/gr_series.o \
	graphics/graphics.o \
	graphics/krn_pal.o \
	gui/gui_buffer.o \
	gui/gui_dialog.o \
	gui/gui_item.o \
	gui/gui_mouse.o \
	gui/gui_sys.o \
	gui/gui_vmng.o \
	gui/gui_vmng_core.o \
	gui/gui_vmng_rectangles.o \
	gui/gui_vmng_screen.o \
	mem/mem.o \
	mem/memman.o \
	mem/reloc.o \
	platform/timer.o \
	wscript/ws_cruncher.o \
	wscript/ws_hal.o \
	wscript/ws_load.o \
	wscript/ws_machine.o \
	wscript/ws_timer.o \
	wscript/wscript.o \
	burger/gui/game_menu.o \
	burger/gui/gui_cheapo.o \
	burger/gui/interface.o \
	burger/burger.o \
	burger/burger_globals.o \
	burger/hotkeys.o \
	burger/inventory.o \
	burger/other.o \
	burger/vars.o

# This module can be built as a plugin
ifeq ($(ENABLE_M4), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
