#include "menu_element.h"


static Menu_Element *__menu_elements = NULL;
static int __menu_element_count = 0;


void Menel_Init() {
	if (__menu_elements != NULL) return;

	__menu_elements = SDL_malloc(sizeof(Menu_Element) * MENEL_MAX_ELEMENTS);
	__menu_element_count = 0;
}

void Menel_Term() {
	if (__menu_elements == NULL) return;
	SDL_free(__menu_elements);
	__menu_elements = NULL;
}

void Menel_ClearAll() {
	__menu_element_count = 0;
}

void Menel_HandleEvents(SDL_Event event) {
	for (int i=0; i<__menu_element_count; i++) {
		Menu_Element *mel = &(__menu_elements[i]);
		mel->handle_input(mel, &event);
	}
}

void Menel_DrawElements() {
	for (int i=0; i<__menu_element_count; i++) {
		Menu_Element *mel = &(__menu_elements[i]);
		mel->draw_element(mel);
	}
}

void Menel_CreateButton(SDL_Rect shape, SDL_Texture *tex, Menu_Callback on_highlight, Menu_Callback on_select) {

}