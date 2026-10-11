#include "ui.h"
#include <ncurses.h>
#include <string.h>

#define BLUE 1
#define MUTED 2
#define SUCCESS 3
#define ERRORC 4
#define FIELD 5

static int colors_enabled;

void ui_init(void)
{
    initscr(); cbreak(); noecho(); keypad(stdscr, TRUE); curs_set(1);
    if (has_colors()) {
        start_color(); use_default_colors(); colors_enabled = 1;
        init_pair(BLUE, COLOR_WHITE, COLOR_BLUE);
        init_pair(MUTED, COLOR_CYAN, -1);
        init_pair(SUCCESS, COLOR_GREEN, -1);
        init_pair(ERRORC, COLOR_RED, -1);
        init_pair(FIELD, COLOR_WHITE, -1);
    }
}

void ui_finish(void) { endwin(); }

static void centered(int y, const char *s, int attr)
{
    int rows, cols, x;
    (void)rows; getmaxyx(stdscr, rows, cols);
    x = (cols - (int)strlen(s)) / 2;
    if (x < 1) x = 1;
    if (attr) attron(attr);
    mvaddnstr(y, x, s, cols - x - 1);
    if (attr) attroff(attr);
}

static void frame(const char *eyebrow, const char *title, const char *subtitle)
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    erase();
    if (rows < 18 || cols < 58) {
        centered(rows / 2 - 1, "Amplia la terminal a 58x18 o mas", A_BOLD);
        centered(rows / 2 + 1, "ESC para volver / salir", A_DIM);
        refresh();
        return;
    }
    attron(A_DIM); box(stdscr, 0, 0); attroff(A_DIM);
    centered(2, "J D M   E - T I C K E T S", (colors_enabled ? COLOR_PAIR(BLUE) : A_BOLD) | A_BOLD);
    centered(3, eyebrow, colors_enabled ? COLOR_PAIR(MUTED) : A_DIM);
    attron(A_DIM); mvhline(5, 4, ACS_HLINE, cols - 8); attroff(A_DIM);
    centered(7, title, A_BOLD);
    centered(8, subtitle, A_DIM);
}

static void draw_button(int y, const char *label, int selected, int cols)
{
    int width = (int)strlen(label) + 8;
    int x = (cols - width) / 2;
    if (x < 2) x = 2;
    if (selected && colors_enabled) attron(COLOR_PAIR(BLUE) | A_BOLD);
    else if (selected) attron(A_REVERSE | A_BOLD);
    else attron(A_BOLD);
    mvaddstr(y, x, "   "); addnstr(label, width - 6); addstr("   ");
    if (selected && colors_enabled) attroff(COLOR_PAIR(BLUE) | A_BOLD);
    else if (selected) attroff(A_REVERSE | A_BOLD);
    else attroff(A_BOLD);
}

int ui_menu(void)
{
    const char *items[] = {"Iniciar sesion", "Crear cuenta", "Salir"};
    int selected = 0, key, rows, cols;
    for (;;) {
        frame("ACCESO SEGURO", "Bienvenido", "Administra tus boletos con tu cuenta");
        getmaxyx(stdscr, rows, cols);
        if (rows < 18 || cols < 58) { key = getch(); if (key == 27) return 0; continue; }
        for (int i = 0; i < 3; ++i) draw_button(rows / 2 + i * 2 - 1, items[i], i == selected, cols);
        centered(rows - 4, "Flechas: navegar     ENTER: seleccionar     ESC: salir", A_DIM);
        refresh(); key = getch();
        if (key == KEY_UP) selected = (selected + 2) % 3;
        else if (key == KEY_DOWN) selected = (selected + 1) % 3;
        else if (key == '\n' || key == '\r' || key == KEY_ENTER) return selected == 2 ? 0 : selected + 1;
        else if (key == 27) return 0;
    }
}

int ui_get_input(const char *label, const char *hint, char *buffer, size_t size, int hidden)
{
    size_t len;
    int key, rows, cols, y, x, width;
    if (!buffer || size < 2) return 0;
    len = strlen(buffer);
    if (len >= size) { buffer[size - 1] = '\0'; len = size - 1; }
    for (;;) {
        frame(hidden ? "PROTECCION DE CUENTA" : "DATOS DE CUENTA", label, hint ? hint : "Escribe y pulsa ENTER");
        getmaxyx(stdscr, rows, cols);
        if (rows < 18 || cols < 58) { key = getch(); if (key == 27) return 0; continue; }
        y = rows / 2; width = cols - 18; if (width > 62) width = 62;
        x = (cols - width) / 2;
        attron(A_DIM); mvaddstr(y - 2, x, "┌"); mvhline(y - 2, x + 1, ACS_HLINE, width - 2); mvaddstr(y - 2, x + width - 1, "┐");
        mvaddstr(y, x, "└"); mvhline(y, x + 1, ACS_HLINE, width - 2); mvaddstr(y, x + width - 1, "┘"); attroff(A_DIM);
        if (colors_enabled) attron(COLOR_PAIR(FIELD));
        for (int i = 0; i < width - 2; ++i) mvaddch(y - 1, x + 1 + i, ' ');
        for (size_t i = 0; i < len && i < (size_t)(width - 3); ++i) mvaddch(y - 1, x + 1 + (int)i, hidden ? '*' : (unsigned char)buffer[i]);
        if (colors_enabled) attroff(COLOR_PAIR(FIELD));
        centered(rows - 4, "ENTER: continuar     ESC: cancelar     BACKSPACE: borrar", A_DIM);
        move(y - 1, x + 1 + (int)(len < (size_t)(width - 3) ? len : (size_t)(width - 3)));
        refresh(); key = getch();
        if (key == 27) { memset(buffer, 0, size); return 0; }
        if (key == '\n' || key == '\r' || key == KEY_ENTER) { if (len == 0) { centered(rows - 2, "El campo no puede estar vacio", colors_enabled ? COLOR_PAIR(ERRORC) : A_BOLD); refresh(); napms(650); continue; } return 1; }
        if (key == KEY_BACKSPACE || key == 127 || key == 8) { if (len) buffer[--len] = '\0'; }
        else if (key >= 33 && key <= 126 && len < size - 1) { buffer[len++] = (char)key; buffer[len] = '\0'; }
    }
}

void ui_message(const char *title, const char *message)
{
    int rows, cols, key;
    for (;;) {
        frame("ESTADO DE LA OPERACION", title, message);
        getmaxyx(stdscr, rows, cols); (void)cols;
        centered(rows / 2 + 2, "ENTER para continuar", colors_enabled ? COLOR_PAIR(BLUE) | A_BOLD : A_BOLD);
        refresh(); key = getch();
        if (key == '\n' || key == '\r' || key == KEY_ENTER || key == 27) return;
    }
}

int ui_confirm(const char *title, const char *message)
{
    int selected = 0, key, rows, cols;
    for (;;) {
        frame("CONFIRMACION", title, message);
        getmaxyx(stdscr, rows, cols);
        draw_button(rows / 2 + 2, "Continuar", selected == 0, cols);
        draw_button(rows / 2 + 4, "Volver", selected == 1, cols);
        centered(rows - 4, "Flechas: elegir     ENTER: confirmar     ESC: volver", A_DIM);
        refresh(); key = getch();
        if (key == KEY_UP || key == KEY_DOWN || key == KEY_LEFT || key == KEY_RIGHT) selected = !selected;
        else if (key == '\n' || key == '\r' || key == KEY_ENTER) return selected == 0;
        else if (key == 27) return 0;
    }
}
