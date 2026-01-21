/*
 * menu.h
 *
 *  Created on: May 14, 2025
 *      Author: Sorro
 */

#ifndef INC_MENU_SYSTEM_H_
#define INC_MENU_SYSTEM_H_

#include "lcd.h"

// 15 bcz of selectable menu
#define MENU_MAX_TEXT_LENGTH 16

typedef void (*MenuCallback)(void*);

typedef enum {
	MENU_TYPE_HOME,
	MENU_TYPE_SEL_BPM,
	MENU_TYPE_SEL_SIG,
	MENU_TYPE_BPM_DETECT,
	MENU_TYPE_BPM_PLAY,
	MENU_TYPE_BPM_PRACTICE
} MenuInstanceType;

typedef enum {
    MENU_ITEM_STATIC_TEXT,
    MENU_ITEM_DYNAMIC_TEXT
} MenuItemFormatType;

typedef struct MenuItemType {
	MenuItemFormatType format;

    uint8_t is_selectable;

    uint8_t col;
    uint8_t row;
    char text_format[MENU_MAX_TEXT_LENGTH+1];
    MenuCallback update_callback;

    MenuCallback action_callback;
    void* in_action_callback;

    char text_buffer[MENU_MAX_TEXT_LENGTH+1];
} MenuItemType;

typedef struct MenuType {
	TextLCDType* lcd;

	MenuInstanceType instance;

	MenuItemType* menu_items_non_sel;

	MenuItemType* menu_items_sel;

	uint8_t item_count_non_sel;

	uint8_t item_count_sel;

    uint8_t selected_index;
} MenuType;


void MENU_init(MenuType* menu, TextLCDType* lcd, MenuInstanceType menu_instance, MenuItemType* total_menu_items, uint8_t total_item_count);
void MENU_render(MenuType* menu, uint8_t f);
void MENU_change_ptr(void* menuTo);
void MENU_change(void* menuTo);

void MENU_item_render_text_ix(MenuType* menu, uint8_t item_ix, uint8_t type);
void MENU_item_render_text(MenuType* menu, MenuItemType* item);

void MENU_lcd_clear(MenuType* menu);

#endif /* INC_MENU_SYSTEM_H_ */
