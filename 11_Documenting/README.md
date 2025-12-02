\
    # 11_Documenting — number-game

    Simple number guessing game (from previous homework) extended with:

    * Roman numeral mode with `-r` / `--roman` (range 1..100).
    * Two conversion functions:
      * `arabic_to_roman(int value)` — 1..100 → Roman string.
      * `roman_to_arabic(const char *s)` — Roman string → 1..100.
    * Doxygen documentation for functions, macros and global variables.
    * Bilingual `--help` (English + Russian).
    * Man page generated via `help2man`.
    * Doxygen main page filled with the same help information.

    Directory layout:

    * `number-game.c`  — main program and CLI handling.
    * `roman.c` / `roman.h` — Roman numeral conversion functions.
    * `Doxyfile`       — Doxygen configuration.
    * `doc_mainpage.dox` — Doxygen main page description.
    * `Makefile`       — build + doc + man targets.

    ## Build

    Requirements (typical Linux):

    * `gcc`
    * `make`
    * `doxygen`
    * `help2man` (for `man` target)

    Build the program:

    ```sh
    cd 11_Documenting
    make
    ```

    ## Running

    Arabic mode (default):

    ```sh
    ./number-game
    ```

    Roman mode:

    ```sh
    ./number-game -r
    ```

    Show help (bilingual):

    ```sh
    ./number-game --help
    ```

    Show help in Markdown/Doxygen-friendly format:

    ```sh
    ./number-game --help-md
    ```

    Show version:

    ```sh
    ./number-game --version
    ```

    ## Documentation

    Generate Doxygen HTML documentation (in `doxygen-doc/html`):

    ```sh
    make doc
    ```

    Then open (for example):

    ```sh
    xdg-open doxygen-doc/html/index.html
    ```

    or run a simple HTTP server, e.g.:

    ```sh
    python3 -m http.server -d doxygen-doc/html 8000
    ```

    and open `http://localhost:8000` in a browser.

    ## Man page

    Generate `number-game.1` man page using `help2man`:

    ```sh
    make man
    ```

    View it locally:

    ```sh
    man -l ./number-game.1
    ```

    ## Quick self-check

    Minimal set of commands to verify the homework requirements:

    ```sh
    cd 11_Documenting

    # 1) Build the program
    make

    # 2) Check --help (bilingual) and --help-md
    ./number-game --help
    ./number-game --help-md

    # 3) Check Roman mode
    ./number-game -r

    # 4) Generate Doxygen documentation
    make doc

    # 5) Generate man page from --help via help2man
    make man
    man -l ./number-game.1
    ```

    All project files for this homework are located under the `11_Documenting` directory,
    as required by the assignment.
