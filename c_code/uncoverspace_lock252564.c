// example.c
#include <stdint.h>
#include <stdlib.h>

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

#define X 25
#define Y 25
#define Z 64

// int8 abs
static inline int8_t abs8(int8_t v)
{
    return v < 0 ? -v : v;
}

int in_bounds(int z, int y, int x)
{
    return (z >= 0 && z < Z &&
            y >= 0 && y < Y &&
            x >= 0 && x < X);
}

static inline int is_uncovered_number(int8_t v)
{
    return (v >= 1 && v <= 27);
}

static inline int is_unknown(int8_t v)
{
    return (v >= -28 && v <= -1);
}

static inline int is_flagged_mine(int8_t v)
{
    return (v == 78);
}

void flood_fill(int8_t board[Z][Y][X], int z, int y, int x)
{
    int dz, dy, dx;

    if (!in_bounds(z, y, x))
        return;

    if (board[z][y][x] <= 0)
        board[z][y][x] = -board[z][y][x];

    if (board[z][y][x] != 1)
        return;

    for (dz = -1; dz <= 1; dz++)
    {
        for (dy = -1; dy <= 1; dy++)
        {
            for (dx = -1; dx <= 1; dx++)
            {
                if (dz == 0 && dy == 0 && dx == 0)
                    continue;

                int nz = z + dz;
                int ny = y + dy;
                int nx = x + dx;

                if (in_bounds(nz, ny, nx))
                {
                    int8_t v = board[nz][ny][nx];

                    if (v < 0 && abs8(v) != 28)
                    {
                        board[nz][ny][nx] = -v;
                        if (board[nz][ny][nx] == 1)
                            flood_fill(board, nz, ny, nx);
                    }
                }
            }
        }
    }
}

void process_board(int8_t board[Z][Y][X])
{
    int z, y, x, dz, dy, dx;

    // 第一階段
    for (z = 0; z < Z; z++)
    {
        for (y = 0; y < Y; y++)
        {
            for (x = 0; x < X; x++)
            {
                if (board[z][y][x] == 1)
                {
                    for (dz = -1; dz <= 1; dz++)
                    {
                        for (dy = -1; dy <= 1; dy++)
                        {
                            for (dx = -1; dx <= 1; dx++)
                            {
                                if (dz == 0 && dy == 0 && dx == 0)
                                    continue;

                                int nz = z + dz;
                                int ny = y + dy;
                                int nx = x + dx;

                                if (in_bounds(nz, ny, nx))
                                {
                                    if (board[nz][ny][nx] == -1)
                                        board[nz][ny][nx] = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // 第二階段 flood fill
    for (z = 0; z < Z; z++)
    {
        for (y = 0; y < Y; y++)
        {
            for (x = 0; x < X; x++)
            {
                if (board[z][y][x] == 1)
                    flood_fill(board, z, y, x);
            }
        }
    }
}

void checkboard(int8_t board[Z][Y][X], int *win)
{
    *win = 1;
    for (int z = 0; z < Z; z++)
    {
        for (int y = 0; y < Y; y++)
        {
            for (int x = 0; x < X; x++)
            {
                if (board[z][y][x] <= -1 && board[z][y][x] >= -27)
                {
                    *win = 0;
                    return;
                }
                else if (board[z][y][x] <= 77 && board[z][y][x] >= 51)
                {
                    *win = 0;
                    return;
                }
                else if (board[z][y][x] == 80)
                {
                    *win = 0;
                    return;
                }
            }
        }
    }
}

void find_next_deduction(
    int8_t board[Z][Y][X],
    int *out_z,
    int *out_y,
    int *out_x,
    int *out_action)
{
    int z, y, x, dz, dy, dx;
    *out_action = 0; // 預設無動作
    for (z = 0; z < Z; z++)
    {
        for (y = 0; y < Y; y++)
        {
            for (x = 0; x < X; x++)
            {
                int8_t center = board[z][y][x];

                /* 只對已翻開的數字格推理 */
                if (!is_uncovered_number(center))
                    continue;

                int flagged_mines = 0;
                int unknown_count = 0;

                int uz = 0, uy = 0, ux = 0;

                for (dz = -1; dz <= 1; dz++)
                {
                    for (dy = -1; dy <= 1; dy++)
                    {
                        for (dx = -1; dx <= 1; dx++)
                        {
                            if (dz == 1 && dy == 1 && dx == 1)
                                continue;

                            int nz = z + dz;
                            int ny = y + dy;
                            int nx = x + dx;

                            if (!in_bounds(nz, ny, nx))
                                continue;

                            int8_t v = board[nz][ny][nx];

                            if (is_flagged_mine(v))
                            {
                                flagged_mines++;
                            }
                            else if (is_unknown(v))
                            {
                                unknown_count++;
                                uz = nz;
                                uy = ny;
                                ux = nx;
                            }
                        }
                    }
                }

                /* 規則 1：flagged == number → 必然可按 */
                if (flagged_mines == center - 1 && unknown_count > 0)
                {
                    *out_z = uz;
                    *out_y = uy;
                    *out_x = ux;
                    *out_action = 1;
                    return;
                }

                /* 規則 2：flagged + unknown == number → 必然是雷 */
                if (flagged_mines + unknown_count == center - 1 && unknown_count > 0)
                {
                    *out_z = uz;
                    *out_y = uy;
                    *out_x = ux;
                    *out_action = 3;
                    return;
                }
                else
                {
                    /* 無法推理，繼續下一格 */
                }
            }
        }
    }
}
// ---------------------- DLL Export ------------------------

DLL_EXPORT void process_board_dll(int8_t board[Z][Y][X])
{
    process_board(board);
}

DLL_EXPORT void checkboard_dll(int8_t board[Z][Y][X], int *win)
{
    checkboard(board, win);
}

DLL_EXPORT void ai(int8_t board[Z][Y][X],
                   int *out_z,
                   int *out_y,
                   int *out_x,
                   int *out_action)
{
    find_next_deduction(
        board,
        out_z,
        out_y,
        out_x,
        out_action);
}
