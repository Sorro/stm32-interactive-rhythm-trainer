/*
 * menu.c
 *
 *  Created on: May 13, 2025
 *  Author: Sorro
 */

#include "system_state.h"

#include <assert.h>
#include <stdio.h>

void MENU_init(MenuType* menu, TextLCDType* lcd, MenuInstanceType menu_instance, MenuItemType* total_menu_items, uint8_t total_item_count){
    assert(total_item_count>0 && total_menu_items != NULL);

    menu->lcd = lcd;
    menu->instance = menu_instance;

    menu->item_count_sel = 0;
    menu->item_count_non_sel = 0;

    // Sets both sel item count and non sel item count.
    for(uint8_t* ix = &menu->item_count_non_sel; *ix < total_item_count; ++(*ix)) {
        if(total_menu_items[*ix].is_selectable == 1) {
            menu->item_count_sel = total_item_count - *ix;
            break;
        }
    }

    menu->menu_items_non_sel = total_menu_items;
    menu->menu_items_sel = total_menu_items + menu->item_count_non_sel;

    menu->selected_index = 0;
}

void MENU_lcd_clear(MenuType* menu) {
	TextLCD_Clear(menu->lcd);
}

void MENU_lcd_error(TextLCDType* lcd) {
	TextLCD_Clear(lcd);
	TextLCD_Position(lcd, 0, 0);

	char str[6] = {'\0'};
	sprintf(str, "ERROR");

	TextLCD_PutStr(lcd, str);
}

// f: sel = 0, non_sel = 1, all = 2
void MENU_render(MenuType* menu, uint8_t f) {
    if(menu->lcd == NULL || menu->menu_items_non_sel == NULL) return;

    hsystem_state.current_menu = menu;

    if(f == 2)
    	TextLCD_Clear(menu->lcd);

    TextLCD_Position(menu->lcd, 0, 0);

    MenuItemType* begin_item;
    uint8_t ix;
    switch(f) {
        case 0: // selected only
            begin_item = menu->menu_items_sel;
            ix = menu->item_count_sel;
            break;
        case 1: // non-selected only
            begin_item = menu->menu_items_non_sel;
            ix = menu->item_count_non_sel;
            break;
        case 2: // both
            begin_item = menu->menu_items_non_sel;
            ix = menu->item_count_non_sel + menu->item_count_sel;
            break;
        default:
            return; // invalid flag
    }

    MenuItemType* itemEnd = begin_item + ix;

    // Print items
    for (MenuItemType* curr_item = begin_item; curr_item<itemEnd; ++curr_item) {
    	MENU_item_render_text(menu, curr_item);
    }

    // call menu_render
}

void MENU_change_ptr(void* menuTo) {
	MenuType* target = *(MenuType**)menuTo;
	MENU_change(target);
}

void MENU_change(void* menuTo) {
	hsystem_state.current_menu->selected_index = 0;
	hsystem_state.previous_menu = hsystem_state.current_menu;
	MENU_render((MenuType*)menuTo, 2);
	// Call menu_change event.
}

void MENU_item_update_buffer(MenuItemType* item) {
	sprintf(item->text_buffer, item->text_format);
}

void MENU_item_render_text(MenuType* menu, MenuItemType* item) {
	if(hsystem_state.current_menu != menu)
		return;

    TextLCD_Position(menu->lcd, item->col, item->row);

    // Add selection indicator if option menu && item is selectable
    if (menu->menu_items_sel > 0 && item->is_selectable) {
    	if(item == &menu->menu_items_sel[menu->selected_index])
    		TextLCD_PutChar(menu->lcd, '>');
    	else
    		TextLCD_PutChar(menu->lcd, '-');
    }

    // Update buffer
    if (item->format == MENU_ITEM_DYNAMIC_TEXT && item->update_callback != NULL)
        item->update_callback(item);
    else
    	MENU_item_update_buffer(item);

    TextLCD_PutStr(menu->lcd, item->text_buffer);

    TextLCD_PutChar(menu->lcd, ' ');
}

void MENU_item_render_text_ix(MenuType* menu, uint8_t item_ix, uint8_t selectable) {

	MenuItemType* menuItems = (selectable ? menu->menu_items_sel : menu->menu_items_non_sel);
	uint8_t itemCount = (selectable ? menu->item_count_sel : menu->item_count_non_sel);

    if (item_ix >= itemCount) return;

    MENU_item_render_text(menu, &menuItems[item_ix]);
}

// Maybe move somewhere else
void MENU_handle_event (MenuType* menu, Event event) {

	switch(event.type) {
		case EVENT_UPDATE_DISPLAY_TEXT:
			MENU_item_render_text_ix(menu, (uint8_t)event.simple_data, 1);
			break;
		default:
			break;
	}

}

// update text, if is not current menu, return.
