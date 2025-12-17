// process_board.c
#include <stdint.h>
#include <stdlib.h>

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

static inline int idx(int z, int y, int x, int dimY, int dimX)
{
    return z * dimY * dimX + y * dimX + x;
}

static inline int8_t abs8(int8_t v)
{
    return v < 0 ? -v : v;
}

static int in_bounds(int z, int y, int x, int dimZ, int dimY, int dimX)
{
    return (z >= 0 && z < dimZ &&
            y >= 0 && y < dimY &&
            x >= 0 && x < dimX);
}

static void flood_fill(int8_t *board,
                       int z, int y, int x,
                       int dimZ, int dimY, int dimX)
{
    if (!in_bounds(z, y, x, dimZ, dimY, dimX))
        return;

    int index = idx(z, y, x, dimY, dimX);

    if (board[index] <= 0)
        board[index] = -board[index];

    if (board[index] != 1)
        return;

    for (int dz = -1; dz <= 1; dz++)
        for (int dy = -1; dy <= 1; dy++)
            for (int dx = -1; dx <= 1; dx++)
            {
                if (dz == 0 && dy == 0 && dx == 0)
                    continue;

                int nz = z + dz;
                int ny = y + dy;
                int nx = x + dx;

                if (!in_bounds(nz, ny, nx, dimZ, dimY, dimX))
                    continue;

                int nindex = idx(nz, ny, nx, dimY, dimX);
                int8_t v = board[nindex];

                if (v < 0 && abs8(v) != 28)
                {
                    board[nindex] = -v;
                    if (board[nindex] == 1)
                        flood_fill(board, nz, ny, nx, dimZ, dimY, dimX);
                }
            }
}

static void process_board(int8_t *board, int dimZ, int dimY, int dimX)
{
    // first pass
    for (int z = 0; z < dimZ; z++)
        for (int y = 0; y < dimY; y++)
            for (int x = 0; x < dimX; x++)
            {
                int index = idx(z, y, x, dimY, dimX);
                if (board[index] == 1)
                {
                    for (int dz = -1; dz <= 1; dz++)
                        for (int dy = -1; dy <= 1; dy++)
                            for (int dx = -1; dx <= 1; dx++)
                            {
                                if (dz == 0 && dy == 0 && dx == 0)
                                    continue;

                                int nz = z + dz;
                                int ny = y + dy;
                                int nx = x + dx;

                                if (in_bounds(nz, ny, nx, dimZ, dimY, dimX))
                                {
                                    int nindex = idx(nz, ny, nx, dimY, dimX);
                                    if (board[nindex] == -1)
                                        board[nindex] = 1;
                                }
                            }
                }
            }

    // flood fill pass
    for (int z = 0; z < dimZ; z++)
        for (int y = 0; y < dimY; y++)
            for (int x = 0; x < dimX; x++)
            {
                int index = idx(z, y, x, dimY, dimX);
                if (board[index] == 1)
                    flood_fill(board, z, y, x, dimZ, dimY, dimX);
            }
}

DLL_EXPORT void process_board_dll(int8_t *board,
                                  int dimZ,
                                  int dimY,
                                  int dimX)
{
    process_board(board, dimZ, dimY, dimX);
}
