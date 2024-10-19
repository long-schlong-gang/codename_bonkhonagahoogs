#include "menu_element.h"


//	TODO: Make more abstract?
//	
//static Menel_Element *__menu_elements = NULL;
//static int __menu_element_count = 0;


//void Menel_Init() {
//	if (__menu_elements != NULL) return;
//
//	__menu_elements = SDL_malloc(sizeof(Menel_Element) * MENEL_MAX_ELEMENTS);
//	__menu_element_count = 0;
//}
//
//void Menel_Term() {
//	if (__menu_elements == NULL) return;
//	SDL_free(__menu_elements);
//	__menu_elements = NULL;
//}
//
//void Menel_ClearAll() {
//	for (int i=0; i<__menu_element_count; i++) {
//		Menel_Element *mel = &(__menu_elements[i]);
//		if (mel->data == NULL)
//
//		SDL_free(mel->data);
//	}
//	__menu_element_count = 0;
//}
//
//void Menel_HandleEvents(SDL_Event event) {
//	for (int i=0; i<__menu_element_count; i++) {
//		Menel_Element *mel = &(__menu_elements[i]);
//		if (mel->handle_input == NULL) continue;
//		mel->handle_input(mel, &event);
//	}
//}
//
//void Menel_DrawElements() {
//	for (int i=0; i<__menu_element_count; i++) {
//		Menel_Element *mel = &(__menu_elements[i]);
//		if (mel->draw_element == NULL) continue;
//		mel->draw_element(mel);
//	}
//}


Menel_TextButton *Menel_TextBtn_Create(SDL_Rect shape, char *text, Menel_Callback on_highlight, Menel_Callback on_select) {
	if (text == NULL) {
		Log_Message(LOG_WARNING, "Tried to create a text button with no text");
		return NULL;
	}

	// Create Button
	Menel_TextButton *btn = SDL_malloc(sizeof(Menel_TextButton));
	btn->state = MENEL_BTN_NORMAL;
	btn->bounding_box = shape;
	btn->on_highlight = on_highlight;
	btn->on_select = on_select;
	btn->text = text;
	btn->user_data = NULL;

	// No box size defined; calculate it
	if (shape.w == 0 || shape.h == 0) {
		size_t chars = SDL_utf8strlen(text);
		
		#ifndef TTFTEXT_GLYPH_W
		btn->bounding_box.w = TTFTEXT_FONT_SIZE;
		btn->bounding_box.h = TTFTEXT_FONT_SIZE;
		Log_Message(LOG_WARNING, "No Rendered glyph size defined! Text-Buttons, will probably have incorrect outlines!");
		#else
		btn->bounding_box.w = chars * TTFTEXT_GLYPH_W;
		btn->bounding_box.h = TTFTEXT_GLYPH_H;
		#endif
	}

	return btn;
}

void Menel_TextBtn_Destroy(Menel_TextButton *btn) {
	if (btn == NULL) return;
	SDL_free(btn);
}

void Menel_TextBtn_HandleEvent(Menel_TextButton *btn, SDL_Event event) {
	if (btn == NULL) return;
	if (btn->state == MENEL_BTN_DISABLED) return;

	SDL_Rect test_box = btn->bounding_box;
	test_box.w += 2*MENEL_TXTBTN_PADDING + 2*MENEL_TXTBTN_OUTLINE;
	test_box.h += 2*MENEL_TXTBTN_PADDING + 2*MENEL_TXTBTN_OUTLINE;

	switch (event.type) {
		case SDL_MOUSEMOTION: {
			// Threshold?
			//int dist = SDL_max(SDL_abs(event.motion.xrel), SDL_abs(event.motion.xrel));
			//if (dist < 5) return;

			if (SDL_PointInRect(&(SDL_Point){ event.motion.x, event.motion.y }, &test_box)) {
				btn->state = MENEL_BTN_HIGHLIGHTED;
				if (btn->on_highlight != NULL) btn->on_highlight(btn->user_data);
			} else {
				btn->state = MENEL_BTN_NORMAL;
			}
		} return;

		case SDL_MOUSEBUTTONUP: {
			if (btn->state != MENEL_BTN_HIGHLIGHTED
				|| !SDL_PointInRect(&(SDL_Point){ event.button.x, event.button.y }, &test_box)
			) return;

			//btn->state = MENEL_BTN_SELECTED;
			if (btn->on_select != NULL) btn->on_select(btn->user_data);
		} return;

		case SDL_KEYUP: {
			if (btn->state != MENEL_BTN_HIGHLIGHTED) return;

			Input_Type in = Binding_ConvKeyCode(event.key.keysym.sym);
			if (in == INPUT_SELECT) {
				//btn->state = MENEL_BTN_SELECTED;
				if (btn->on_select != NULL) btn->on_select(btn->user_data);
			}
		} return;
	}
}

void Menel_TextBtn_Draw(Menel_TextButton *btn) {
	if (btn == NULL) return;

	PaletteColour text_clr = CLR_BTN_NORMAL;
	switch (btn->state) {
		case MENEL_BTN_NORMAL: break;
		case MENEL_BTN_DISABLED: text_clr = CLR_BTN_BLOCKED; break;
		case MENEL_BTN_HIGHLIGHTED: text_clr = CLR_BTN_SELECT; break;
		//case MENEL_BTN_SELECTED: text_clr = CLR_BTN_SELECT; break;
	}

	if (btn->state == MENEL_BTN_HIGHLIGHTED) {
		Colours_SetRenderer(text_clr);
		SDL_RenderFillRects(g_renderer, (struct SDL_Rect[4]){
			{
				btn->bounding_box.x,
				btn->bounding_box.y,
				btn->bounding_box.w + 2*MENEL_TXTBTN_PADDING + 2*MENEL_TXTBTN_OUTLINE,
				MENEL_TXTBTN_OUTLINE
			}, {
				btn->bounding_box.x,
				btn->bounding_box.y,
				MENEL_TXTBTN_OUTLINE,
				btn->bounding_box.h + 2*MENEL_TXTBTN_PADDING + 2*MENEL_TXTBTN_OUTLINE,
			}, {
				btn->bounding_box.x + btn->bounding_box.w + 2*MENEL_TXTBTN_PADDING + MENEL_TXTBTN_OUTLINE,
				btn->bounding_box.y,
				MENEL_TXTBTN_OUTLINE,
				btn->bounding_box.h + 2*MENEL_TXTBTN_PADDING + 2*MENEL_TXTBTN_OUTLINE,
			}, {
				btn->bounding_box.x,
				btn->bounding_box.y + btn->bounding_box.h + 2*MENEL_TXTBTN_PADDING + MENEL_TXTBTN_OUTLINE,
				btn->bounding_box.w + 2*MENEL_TXTBTN_PADDING + 2*MENEL_TXTBTN_OUTLINE,
				MENEL_TXTBTN_OUTLINE
			}
		}, 4);
	}

	TTFText_RenderText(
		btn->bounding_box.x + MENEL_TXTBTN_PADDING + MENEL_TXTBTN_OUTLINE,
		btn->bounding_box.y + MENEL_TXTBTN_PADDING + MENEL_TXTBTN_OUTLINE,
		text_clr, btn->text
	);
}


Menel_TextButtonArray *Menel_TBtnArr_Create(int num, Menel_TextButton *buttons) {
	if (buttons == NULL || num <= 0) return NULL;

	Menel_TextButtonArray *arr = SDL_malloc(sizeof(Menel_TextButtonArray));
	arr->btns = SDL_malloc(sizeof(Menel_TextButton) * num);
	for (int i=0; i<num; i++) {
		arr->btns[i] = buttons[i];

		// No box size defined; calculate it
		if (arr->btns[i].bounding_box.w == 0 || arr->btns[i].bounding_box.h == 0) {
			size_t chars = SDL_utf8strlen(arr->btns[i].text);
			
			#ifndef TTFTEXT_GLYPH_W
			arr->btns[i].bounding_box.w = TTFTEXT_FONT_SIZE;
			arr->btns[i].bounding_box.h = TTFTEXT_FONT_SIZE;
			Log_Message(LOG_WARNING, "No Rendered glyph size defined! Text-Buttons, will probably have incorrect outlines!");
			#else
			arr->btns[i].bounding_box.w = chars * TTFTEXT_GLYPH_W;
			arr->btns[i].bounding_box.h = TTFTEXT_GLYPH_H;
			#endif
		}
	}

	arr->btn_count = num;
	arr->sel_index = -1;

	return arr;
}

void Menel_TBtnArr_Destroy(Menel_TextButtonArray *arr) {
	if (arr == NULL) return;
	if (arr->btns != NULL) SDL_free(arr->btns);
	SDL_free(arr);
}

void Menel_TBtnArr_SetAllState(Menel_TextButtonArray *arr, Menel_Btn_State state) {
	if (arr == NULL) return;
	for (int i=0; i<arr->btn_count; i++) {
		arr->btns[i].state = state;
	}
}

void Menel_TBtnArr_ClearSelection(Menel_TextButtonArray *arr) {
	if (arr == NULL) return;
	for (int i=0; i<arr->btn_count; i++) {
		if (arr->btns[i].state == MENEL_BTN_DISABLED) continue;
		arr->btns[i].state = MENEL_BTN_NORMAL;
	}
}

void Menel_TBtnArr_MoveSelectionWrap(Menel_TextButtonArray *arr, int move) {
	if (arr == NULL) return;

	// Special case? Deselect?
	if (move == 0) return;

	int new = arr->sel_index;
	if (new < 0) new = 0;
	else new += move;

	int loop_count = 0;
	int corr = 0;
	do {
		new += corr;

		// Cap/Wrap at the ends
		if (new < 0) new += arr->btn_count;
		if (new >= arr->btn_count) new -= arr->btn_count;

		// Keep going in the same direction if the current button is disabled
		corr = (move < 0) ? -1 : 1;
		loop_count++;
	} while (arr->btns[new].state == MENEL_BTN_DISABLED && loop_count < 1000);

	if (loop_count >= 1000) {
		Log_Message(LOG_WARNING, "TextButton Movement Algorithm looped too many times (>= 1000)");
		return;
	}

	arr->sel_index = new;
	Menel_TBtnArr_ClearSelection(arr);
	arr->btns[new].state = MENEL_BTN_HIGHLIGHTED;
}

void Menel_TBtnArr_HandleEvent(Menel_TextButtonArray *arr, SDL_Event event) {
	if (arr == NULL) return;

	if (event.type == SDL_KEYDOWN) {
		Input_Type in = Binding_ConvKeyCode(event.key.keysym.sym);

		switch (in) {
			case INPUT_LEFT:
			case INPUT_UP: {
				Menel_TBtnArr_MoveSelectionWrap(arr, -1);
			} return;
			case INPUT_RIGHT:
			case INPUT_DOWN: {
				Menel_TBtnArr_MoveSelectionWrap(arr, 1);
			} return;
		}
	}

	for (int i=0; i<arr->btn_count; i++) {
		Menel_TextBtn_HandleEvent(&arr->btns[i], event);

		if (arr->btns[i].state == MENEL_BTN_HIGHLIGHTED) {
			arr->sel_index = i;
		} else {
			if (i == arr->sel_index) arr->sel_index = -1;
		}
	}
}

void Menel_TBtnArr_Draw(Menel_TextButtonArray *arr) {
	if (arr == NULL) return;

	for (int i=0; i<arr->btn_count; i++) {
		Menel_TextBtn_Draw(&arr->btns[i]);
	}
}