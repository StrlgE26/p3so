# Práctica: Inicio de sesión e interfaces basadas en texto (ncurses)

Este repositorio contiene el código fuente y el reporte de la práctica de Sistemas Operativos. Si les gusta trabajar por aca, modificaremos el repo para trabajar todas nuestras tareas por aca.

## Estructura del Repositorio

*   **/login:** Contiene el código fuente en C, los headers, el Makefile y el `README.md` técnico para el profesor.
*   **/reporte:** Contiene los módulos en LaTeX (`.tex`) y las imágenes.
*   `.gitignore`: Archivo que evita subir archivos temporales de compilación de C (`.o`) y LaTeX (`.aux`, `.log`, etc.).

---

## 🛠️ Guía para el trabajo en equipo

Para evitar conflictos al unir nuestro código, debemos seguir estas reglas estrictas:

1. **No trabajar en la rama `main`:** Cada miembro debe crear una rama para su parte (Ejemplo: `reporte/marco-teorico` o `login/cifrado`). Al terminar, deben hacer *push* de su rama y abrir un Pull Request siguiendo el tutorial de abajo.
2. **Regla de Oro para LaTeX:** En los archivos `.tex`, **escriban una sola oración por línea de código**. Git rastrea los cambios por línea; si escriben párrafos gigantes en una sola línea, será imposible unir el trabajo si dos personas editan el mismo archivo. Para cambiar de párrafo en LaTeX, simplemente presionen dos veces `Enter` (dejen una línea en blanco).

---

### Tutorial: Paso a paso para cada tarea

Sigan este flujo de trabajo para cada tarea que realicen en el repositorio:

#### 1. Clonar el repositorio (Solo la primera vez)
Abre tu consola donde quieras guardar el proyecto y ejecuta:
```cmd
git clone [https://github.com/StrlgE26/p3so.git](https://github.com/StrlgE26/p3so.git)
cd p3so
```
### 2. Actualizar tu repositorio local (Siempre antes de empezar)
Asegúrate de tener la última versión del código antes de hacer cualquier cambio:
```cmd
git checkout main
git pull origin main
```
### 3. Crear una rama para tu tarea
Crea una rama descriptiva sobre lo que vas a hacer (ej: `reporte/marco-teorico`, `login/cifrado`, `login/interfaz`).
```cmd
git checkout -b nombre-de-tu-rama
```
### 4. Trabajar en el código y guardar cambios
Cuando termines de programar o escribir, prepara y guarda tus cambios:
```cmd
git add .
git commit -m "Descripción clara de lo que hiciste"
```
### 5. Subir tu rama a GitHub
Empuja tu nueva rama al repositorio en la nube:
```cmd
git push -u origin nombre-de-tu-rama
```
### 6. Crear un Pull Request y hacer Merge (En la página web)
1. Ve a la página del repositorio en GitHub: `https://github.com/StrlgE26/p3so`.
2. Verás un botón verde que dice **"Compare & pull request"**. Haz clic en él.
3. Agrega un título y una breve descripción de lo que terminaste. Haz clic en **"Create pull request"**.
4. Pide a un compañero que revise tu código. Si todo está bien, haz clic en el botón verde **"Merge pull request"** y luego en **"Confirm merge"**.
5. ¡Listo! Tu código ya forma parte de la rama `main`.
### 7. Borrar la rama y continuar
Una vez que el código esté en main, limpia tu entorno local y prepárate para la siguiente tarea:
```cmd
git checkout main
git pull origin main
git branch -d nombre-de-tu-rama
```

---

## 📄 Cómo compilar el Reporte (PDF) en Windows

Es necesario tener el compilador local de LaTeX (MiKTeX). Si no lo tienes, instálalo ejecutando en PowerShell como administrador: `winget install MiKTeX.MiKTeX`.

1.  Abre una consola (CMD o PowerShell) dentro de la carpeta `reporte/`.
2.  Ejecuta el compilador dos veces seguidas para generar el índice correctamente:
    ```cmd
    pdflatex p3so.tex
    pdflatex p3so.tex
    ```
3.  Abre el documento generado con:
    ```cmd
    start p3so.pdf
    ```

---

## 💻 Cómo compilar el Código C (Linux / WSL)

El código utiliza `ncurses`, por lo que **debe compilarse en un entorno Linux**.

1.  Abre tu terminal de Linux y navega a la carpeta `login/`.
2.  Compila el programa usando el Makefile:
    ```bash
    make clean
    make
    ```
3.  Ejecuta el inicio de sesión:
    ```bash
    ./acceso
    ```