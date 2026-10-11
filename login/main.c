#include "auth.h"
#include "ui.h"
#include <sodium.h>
#include <stdio.h>
#include <string.h>

static void show_result(AuthResult r, int registering)
{
    switch (r) {
    case AUTH_OK: ui_message(registering ? "CUENTA CREADA" : "ACCESO CORRECTO", registering ? "Tu cuenta se guardo correctamente." : "Bienvenido a JDM E-TICKETS."); break;
    case AUTH_INVALID_INPUT: ui_message("DATOS NO VALIDOS", "Revisa el campo indicado e intenta de nuevo."); break;
    case AUTH_USER_EXISTS: ui_message("USERNAME OCUPADO", "Ese username ya esta registrado."); break;
    case AUTH_EMAIL_EXISTS: ui_message("CORREO EN USO", "Ese correo ya esta asociado a una cuenta."); break;
    case AUTH_USER_NOT_FOUND: ui_message("USUARIO NO EXISTE", "No encontramos ese username."); break;
    case AUTH_WRONG_PASSWORD: ui_message("CONTRASENA INCORRECTA", "La contrasena ingresada no es correcta."); break;
    case AUTH_IO_ERROR: ui_message("ERROR DE ARCHIVO", "No se pudo leer o guardar usuarios.txt."); break;
    case AUTH_CRYPTO_ERROR: ui_message("ERROR DE SEGURIDAD", "No se pudo proteger la contrasena."); break;
    }
}

static void login_flow(void)
{
    char username[USERNAME_MAX + 1] = {0}, password[PASSWORD_MAX + 1] = {0};
    AuthResult r;
    for (;;) {
        if (!ui_get_input("Iniciar sesion / Username", "Escribe tu username y pulsa ENTER", username, sizeof username, 0)) goto done;
        for (;;) {
            if (!ui_get_input("Iniciar sesion / Contrasena", "La contrasena se muestra oculta", password, sizeof password, 1)) goto done;
            r = auth_login_user(username, password);
            if (r == AUTH_OK) { show_result(r, 0); goto done; }
            if (r == AUTH_WRONG_PASSWORD) {
                sodium_memzero(password, sizeof password);
                ui_message("CONTRASENA INCORRECTA", "Intenta de nuevo.");
                continue;
            }
            if (r == AUTH_USER_NOT_FOUND) {
                sodium_memzero(username, sizeof username);
                sodium_memzero(password, sizeof password);
                ui_message("USUARIO NO EXISTE", "Revisa tu username.");
                break;
            }
            show_result(r, 0);
            sodium_memzero(password, sizeof password);
            goto done;
        }
    }
done:
    sodium_memzero(password, sizeof password);
}

static void registration_flow(void)
{
    char username[USERNAME_MAX + 1] = {0}, email[EMAIL_MAX + 1] = {0};
    char password[PASSWORD_MAX + 1] = {0}, confirm[PASSWORD_MAX + 1] = {0};
    AuthResult r;

    for (;;) {
        if (!ui_get_input("Registro / Username", "5-24 caracteres: letras, numeros, _ o -", username, sizeof username, 0)) goto cleanup;
        if (!auth_valid_username(username)) { ui_message("USERNAME NO VALIDO", "Usa 5-24 caracteres: letras, numeros, _ o -."); continue; }
        break;
    }
    for (;;) {
        if (!ui_get_input("Registro / Correo", "Formato: nombre@dominio.com", email, sizeof email, 0)) goto cleanup;
        if (!auth_valid_email(email)) { ui_message("CORREO NO VALIDO", "Ingresa un correo valido."); continue; }
        break;
    }
    for (;;) {
        if (!ui_get_input("Registro / Contrasena", "8+ caracteres, mayuscula, minuscula, numero y caracter especial", password, sizeof password, 1)) goto cleanup;
        if (!auth_valid_password(password)) { ui_message("CONTRASENA DEBIL", "Incluye mayuscula, minuscula, numero y caracter especial; minimo 8."); continue; }
        break;
    }
    for (;;) {
        if (!ui_get_input("Registro / Confirmar contrasena", "Vuelve a escribir exactamente la misma contrasena", confirm, sizeof confirm, 1)) goto cleanup;
        if (strcmp(password, confirm) != 0) { sodium_memzero(confirm, sizeof confirm); ui_message("NO COINCIDEN", "Vuelve a ingresar tu contrasena."); continue; }
        break;
    }
    for (;;) {
        r = auth_register_user(username, email, password);
        if (r == AUTH_USER_EXISTS) {
            ui_message("USERNAME OCUPADO", "Elige otro username.");
            if (!ui_get_input("Registro / Username", "Ese username ya existe; escribe uno distinto", username, sizeof username, 0)) goto cleanup;
            if (!auth_valid_username(username)) { ui_message("USERNAME NO VALIDO", "Usa 5-24 caracteres permitidos."); continue; }
            continue;
        }
        if (r == AUTH_EMAIL_EXISTS) {
            ui_message("CORREO EN USO", "Usa otro correo.");
            if (!ui_get_input("Registro / Correo", "Ese correo ya existe; escribe otro o inicia sesion", email, sizeof email, 0)) goto cleanup;
            if (!auth_valid_email(email)) { ui_message("CORREO NO VALIDO", "Ingresa un correo valido."); continue; }
            continue;
        }
        show_result(r, 1);
        break;
    }
cleanup:
    sodium_memzero(password, sizeof password);
    sodium_memzero(confirm, sizeof confirm);
}

int main(void)
{
    int option;
    if (sodium_init() < 0) { fputs("Error al inicializar libsodium.\n", stderr); return 1; }
    ui_init();
    do {
        option = ui_menu();
        if (option == 1) login_flow();
        else if (option == 2) registration_flow();
    } while (option != 0);
    ui_finish();
    return 0;
}
