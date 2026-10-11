# JDM E-TICKETS — práctica de autenticación en C con ncurses

Interfaz de terminal JDM E-TICKETS con navegación por flechas, campos simulados y botones azules. La autenticación local usa libsodium/Argon2id. En el registro se aplican las reglas de formato; durante el login se informa si el usuario no existe o si la contraseña es incorrecta.

## Dependencias (Debian, Ubuntu o Linux Mint)

```bash
sudo apt update
sudo apt install build-essential libncurses-dev libsodium-dev
```

## Compilar y ejecutar

```bash
make clean
make
./acceso
```

Usa una terminal de al menos 58 columnas por 18 filas para visualizar correctamente la interfaz.

## Navegación

- Flechas arriba/abajo: mover la selección.
- ENTER: seleccionar o confirmar.
- TAB no es necesario para navegar: los formularios se capturan uno por pantalla.
- BACKSPACE: borrar.
- ESC: cancelar o volver.
- No se requieren opciones numéricas ni respuestas S/N.

## Reglas de validación

- Username único, de 5 a 24 caracteres; sólo letras ASCII, números, guion bajo o guion.
- Correo con formato básico `usuario@dominio.tld`; se rechaza si ya está registrado.
- Contraseña de 8 a 128 caracteres imprimibles ASCII, con al menos una mayúscula, una minúscula, un número y un símbolo.
- La confirmación de contraseña debe coincidir exactamente.
- Si un dato del registro no es válido, se vuelve a mostrar ese mismo campo conservando lo escrito.
- En login, si la contraseña es incorrecta se conserva el username y se limpia la contraseña; si el usuario no existe, se limpian ambos campos.
- Las contraseñas se almacenan como hashes Argon2id; nunca en texto plano.
- `usuarios.txt` se crea con permisos 0600.

## Formato de persistencia

Los registros nuevos usan `username:email:hash`. Los registros antiguos `username:hash` siguen permitiendo iniciar sesión, aunque no tienen correo asociado. Si quieres un archivo limpio para pruebas, respalda y elimina `usuarios.txt` antes de ejecutar.

## Advertencias

Ignorar errores sugeridos de ciertos editores de codigo.