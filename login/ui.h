#ifndef UI_H
#define UI_H
#include <stddef.h>
void ui_init(void);
void ui_finish(void);
int ui_menu(void);
int ui_get_input(const char *label, const char *hint, char *buffer, size_t size, int hidden);
void ui_message(const char *title, const char *message);
int ui_confirm(const char *title, const char *message);
#endif
