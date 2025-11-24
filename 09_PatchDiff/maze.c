#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

static void generate_maze(int size, char passage, char wall) {
    if (size <= 0) {
        fprintf(stderr, "Invalid maze size: %d\n", size);
        exit(EXIT_FAILURE);
    }

    int rows = 2 * size + 1;
    int cols = rows;

    char *grid = (char *)malloc((size_t)rows * (size_t)cols);
    if (!grid) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Fill with walls
    for (int i = 0; i < rows * cols; ++i) {
        grid[i] = wall;
    }

    // Place room centers
    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            int rr = 2 * r + 1;
            int cc = 2 * c + 1;
            grid[rr * cols + cc] = passage;
        }
    }

    // Perfect maze generation via randomized DFS on room grid size x size
    bool *visited = (bool *)calloc((size_t)size * (size_t)size, sizeof(bool));
    if (!visited) {
        perror("calloc");
        free(grid);
        exit(EXIT_FAILURE);
    }

    int max_cells = size * size;
    int *stack_r = (int *)malloc(sizeof(int) * (size_t)max_cells);
    int *stack_c = (int *)malloc(sizeof(int) * (size_t)max_cells);
    if (!stack_r || !stack_c) {
        perror("malloc");
        free(grid);
        free(visited);
        free(stack_r);
        free(stack_c);
        exit(EXIT_FAILURE);
    }

    int top = 0;
    stack_r[top] = 0;
    stack_c[top] = 0;
    visited[0] = true;

    while (top >= 0) {
        int r = stack_r[top];
        int c = stack_c[top];

        int dirs[4][2];
        int n_dirs = 0;

        // Collect unvisited neighbours
        const int OFFS[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
        for (int k = 0; k < 4; ++k) {
            int nr = r + OFFS[k][0];
            int nc = c + OFFS[k][1];
            if (nr < 0 || nr >= size || nc < 0 || nc >= size) continue;
            if (!visited[nr * size + nc]) {
                dirs[n_dirs][0] = nr;
                dirs[n_dirs][1] = nc;
                ++n_dirs;
            }
        }

        if (n_dirs == 0) {
            // backtrack
            --top;
            continue;
        }

        // choose random neighbour
        int pick = rand() % n_dirs;
        int nr = dirs[pick][0];
        int nc = dirs[pick][1];

        // carve passage between (r,c) and (nr,nc)
        int cr = 2 * r + 1;
        int cc = 2 * c + 1;
        int ncr = 2 * nr + 1;
        int ncc = 2 * nc + 1;
        int wr = (cr + ncr) / 2;
        int wc = (cc + ncc) / 2;
        grid[wr * cols + wc] = passage;

        visited[nr * size + nc] = true;
        ++top;
        stack_r[top] = nr;
        stack_c[top] = nc;
    }

    // Check wall percentage (must be >= 15%)
    int total = rows * cols;
    int wall_count = 0;
    for (int i = 0; i < total; ++i) {
        if (grid[i] == wall) ++wall_count;
    }
    double wall_ratio = (double)wall_count / (double)total;
    if (wall_ratio < 0.15) {
        fprintf(stderr, "Internal error: wall ratio %.3f < 0.15\n", wall_ratio);
        free(grid);
        free(visited);
        free(stack_r);
        free(stack_c);
        exit(EXIT_FAILURE);
    }

    // Print maze
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            putchar(grid[r * cols + c]);
        }
        putchar('\n');
    }

    free(grid);
    free(visited);
    free(stack_r);
    free(stack_c);
}

int main(void) {
    int size = 6;
    char passage = '.';
    char wall = '#';

    srand((unsigned int)time(NULL));
    generate_maze(size, passage, wall);
    return 0;
}
