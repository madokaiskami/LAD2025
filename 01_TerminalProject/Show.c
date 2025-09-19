#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#define DX 3
#define DY 3

int main(int argc, char *argv[]) {
    WINDOW *win;
    FILE *file;
    int ch;
    int line_count = 0;
    char **lines = NULL;
    int max_win_lines, max_win_cols;
    int current_top_line = 0;
    int current_left_col = 0; 

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // --- Read the entire file into memory ---


    int c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            line_count++;
        }
    }

    if (ftell(file) > 0) {
        fseek(file, -1, SEEK_END);
        if (fgetc(file) != '\n') {
            line_count++;
        }
    }


    lines = (char **)malloc(line_count * sizeof(char *));
    if (lines == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return 1;
    }


    fseek(file, 0, SEEK_SET);
    char buffer[4096]; // Use a larger buffer for longer lines
    int current_line = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL && current_line < line_count) {

        int len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }


        lines[current_line] = (char *)malloc((len + 1) * sizeof(char));
        if (lines[current_line] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");

            for (int i = 0; i < current_line; i++) free(lines[i]);
            free(lines);
            fclose(file);
            return 1;
        }

        strcpy(lines[current_line], buffer);
        current_line++;
    }

    fclose(file);


    initscr();            // Start curses mode
    noecho();             // Don't echo() while we do getch
    cbreak();             // Line buffering disabled
    keypad(stdscr, TRUE); // Enable F-keys, arrows, etc.

    // Create a new window with a border
    win = newwin(LINES - 2 * DY, COLS - 2 * DX, DY, DX);
    keypad(win, TRUE); // Enable function keys for the new window as well

    max_win_lines = getmaxy(win) - 2; // -2 for the border
    max_win_cols = getmaxx(win) - 2;  // -2 for the border

    // --- Main application loop ---
    while (1) {
        werase(win); 
        box(win, 0, 0); 

        
        int title_len = strlen(argv[1]);
        int title_pos = (getmaxx(win) - title_len) / 2;
        if (title_pos < 1) title_pos = 1;
        mvwaddstr(win, 0, title_pos, argv[1]);

        
        for (int i = 0; i < max_win_lines; i++) {
            int line_index = current_top_line + i;
            if (line_index >= line_count) {
                break; 
            }

            
            char *line_ptr = lines[line_index];
            int line_len = strlen(line_ptr);

            
            if (current_left_col < line_len) {
                 line_ptr += current_left_col;
            } else {
                 line_ptr = ""; 
            }

            
            char display_line[max_win_cols + 1];
            strncpy(display_line, line_ptr, max_win_cols);
            display_line[max_win_cols] = '\0';

            mvwaddstr(win, i + 1, 1, display_line);
        }

        wrefresh(win); 

        ch = wgetch(win);

        switch (ch) {
            case ' ':
            case KEY_DOWN: 
                if (current_top_line + max_win_lines < line_count) {
                    current_top_line++;
                }
                break;

            case KEY_UP: 
                 if (current_top_line > 0) {
                    current_top_line--;
                }
                break;

            case KEY_NPAGE: // Page Down
                current_top_line += max_win_lines;
                
                if (current_top_line + max_win_lines > line_count) {
                    current_top_line = line_count - max_win_lines;
                    if (current_top_line < 0) current_top_line = 0;
                }
                break;

            case KEY_PPAGE: // Page Up
                current_top_line -= max_win_lines;
                if (current_top_line < 0) {
                    current_top_line = 0;
                }
                break;

            case KEY_RIGHT: // Scroll right one column
                current_left_col++;
                break;

            case KEY_LEFT: // Scroll left one column
                if (current_left_col > 0) {
                    current_left_col--;
                }
                break;

            case 27: // ESC key - exit the program
                goto cleanup;

            default:
                // Ignore all other keys
                break;
        }
    }

cleanup:
    // --- Deallocate memory and end ncurses mode ---
    for (int i = 0; i < line_count; i++) {
        free(lines[i]);
    }
    free(lines);
    delwin(win);
    endwin();

    return 0;
}
