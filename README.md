# Balatro-DSi (Port / Demake para Nintendo DSi XL)

Reimplementación nativa de **Balatro** en lenguaje C puro, diseñada específicamente para el hardware de la **Nintendo DSi XL** (CPU ARM9 a 133 MHz, 16 MB de RAM y dos pantallas de 256×192 píxeles).

---

## 🎮 Características Implementadas

- **Evaluador de Poker Completo (12 Manos):**
  - High Card, Pair, Two Pair, Three of a Kind, Straight (incluyendo As bajo A-2-3-4-5), Flush, Full House, Four of a Kind, Straight Flush, Five of a Kind, Flush House y Flush Five.
  - Escalado por nivel idéntico al código fuente original (`s_chips`, `l_chips`, `s_mult`, `l_mult`).
- **Motor Matemático de Puntuación:**
  - Pipeline de cálculo ordenado: Fichas base $\rightarrow$ Cartas anotadas $\rightarrow$ Sellos/Ediciones $\rightarrow$ Cartas en mano $\rightarrow$ Jokers $\rightarrow$ Plasma Deck.
  - Soporte de cartas de piedra (*Stone*), cristal (*Glass*), acero (*Steel*), bonus, mult y comodines (*Wild*).
- **Más de 30 Jokers Oficiales Implementados:**
  - Traje (Greedy, Lusty, Wrathful, Gluttonous).
  - Manos (Jolly, Zany, Mad, Crazy, Droll, Sly, Wily, Clever, Devious, Crafty, Half).
  - Efectos exponenciales y condicionales: Cavendish (x3), Baron (x1.5 por Rey), Banner (+30 fichas por descarte), Mystic Summit, Even Steven, Odd Todd (+31 fichas), Scholar, Fibonacci, Scary Face, Smiley Face, Abstract Joker, Blue Joker, Joker Stencil, Ice Cream (-5 fichas por mano) y Popcorn.
- **Pipeline de Conversión Gráfica (`tools/asset_pipeline.py`):**
  - Mapeo y reescalado de spritesheets originales a celdas de 24×32 píxeles compatibles con tilesets de hardware NDS.
  - Cuantización a paletas indexadas y exportación de cabeceras C en formato de color NDS BGR555 (`include/gfx/`).
- **Soporte Doble Pantalla y Táctil para Nintendo DSi (`src/platform/ds/main_ds.c`):**
  - **Pantalla Superior:** Ciega, Ante, Objetivo, Puntuación acumulada, desglose animado de Fichas $\times$ Mult y Jokers activos.
  - **Pantalla Inferior (Táctil):** Cartas en mano seleccionables con el Stylus, botones táctiles de *Jugar Mano* y *Descartar*.
- **Simulador Interactivo de Escritorio (`balatro_cli.exe`):**
  - Permite jugar partidas reales completas directamente en la terminal de Windows.

---

## 📂 Estructura del Proyecto

```text
Port/
├── include/
│   ├── core/
│   │   ├── card.h            # Estructura de cartas, palos, rangos, sellos, mejoras y ediciones
│   │   ├── poker_hand.h      # Algoritmo de detección de manos
│   │   ├── scoring.h         # Motor aritmético de puntuación
│   │   ├── jokers.h          # Sistema de callbacks y eventos de Jokers
│   │   ├── deck.h            # Barajado Fisher-Yates (xorshift32) y robo
│   │   └── game_state.h      # Ciclo de juego, antes, ciegas y recompensas
│   ├── platform/
│   │   └── hal.h             # Capa de abstracción de hardware (pantalla táctil, botones, video)
│   └── gfx/                  # Previews y datos de texturas NDS (RGB555)
├── src/
│   ├── core/                 # Lógica portable en C99/C11
│   ├── platform/
│   │   ├── ds/
│   │   │   └── main_ds.c     # Frontend nativo Nintendo DSi con libnds
│   │   └── desktop/
│   │       ├── interactive_play.c # Juego interactivo por consola
│   │       └── test_runner.c      # Suite de 30 tests unitarios automatizados
├── tools/
│   ├── extract_balatro.py    # Extractor del archivo .love original
│   └── asset_pipeline.py     # Conversor de sprites a resolución DSi
├── Makefile.ds               # Makefile para devkitARM
└── README.md
```

---

## 🚀 Cómo Jugar en PC (Simulador CLI)

El proyecto incluye un simulador jugable por terminal compilado con MSVC:

```powershell
.\balatro_cli.exe
```

### Controles de la terminal:
- `p 1 3` : Juega las cartas 1 y 3 (por ejemplo, una Pareja).
- `d 2 4 5` : Descarta las cartas seleccionadas y roba nuevas del mazo.
- `q` : Salir del juego.

---

## 🧪 Ejecución de Pruebas Unitarias

Para verificar toda la lógica de cartas, manos, Jokers y puntuación:

```powershell
.\test_runner.exe
```

Salida esperada:
```text
=========================================================
  30 / 30 TESTS PASSED
  ALL TESTS PASSED SUCCESSFULLY!
=========================================================
```

---

## 🛠️ Compilación para Nintendo DSi (`.nds`)

Para generar la ROM lista para flashcards (R4, Acekard, Twilight Menu++ en tarjeta SD):

1. Instala **devkitPro** con el componente `devkitARM` y `libnds`.
2. Asegúrate de tener configurada la variable de entorno:
   ```bash
   export DEVKITPRO=/opt/devkitpro
   export DEVKITARM=$DEVKITPRO/devkitARM
   ```
3. Ejecuta la compilación:
   ```bash
   make -f Makefile.ds
   ```
4. Se generará el archivo `balatro_dsi.nds`.
