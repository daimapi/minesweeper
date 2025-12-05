// boarddll.c
#include <stdint.h>
#include <stdlib.h>

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

// int8 abs
static inline int8_t abs8(int8_t v)
{
    return v < 0 ? -v : v;
}

// 檢查座標是否在範圍內
static int in_bounds(int z, int y, int x, int dimZ, int dimY, int dimX)
{
    return (z >= 0 && z < dimZ &&
            y >= 0 && y < dimY &&
            x >= 0 && x < dimX);
}

// flood fill
static void flood_fill(int8_t *board, int z, int y, int x, int dimZ, int dimY, int dimX)
{
    int dz, dy, dx;

    if (!in_bounds(z, y, x, dimZ, dimY, dimX))
        return;

    int idx = z * dimY * dimX + y * dimX + x;

    if (board[idx] <= 0)
        board[idx] = -board[idx];

    if (board[idx] != 1)
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

                if (in_bounds(nz, ny, nx, dimZ, dimY, dimX))
                {
                    int nidx = nz * dimY * dimX + ny * dimX + nx;
                    int8_t v = board[nidx];

                    if (v < 0 && abs8(v) != 28)
                    {
                        board[nidx] = -v;
                        if (board[nidx] == 1)
                            flood_fill(board, nz, ny, nx, dimZ, dimY, dimX);
                    }
                }
            }
        }
    }
}

// 處理 board
static void process_board(int8_t *board, int dimZ, int dimY, int dimX)
{
    int z, y, x, dz, dy, dx;

    // 第一階段
    for (z = 0; z < dimZ; z++)
    {
        for (y = 0; y < dimY; y++)
        {
            for (x = 0; x < dimX; x++)
            {
                int idx = z * dimY * dimX + y * dimX + x;

                if (board[idx] == 1)
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

                                if (in_bounds(nz, ny, nx, dimZ, dimY, dimX))
                                {
                                    int nidx = nz * dimY * dimX + ny * dimX + nx;
                                    if (board[nidx] == -1)
                                        board[nidx] = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // 第二階段 flood fill
    for (z = 0; z < dimZ; z++)
    {
        for (y = 0; y < dimY; y++)
        {
            for (x = 0; x < dimX; x++)
            {
                int idx = z * dimY * dimX + y * dimX + x;
                if (board[idx] == 1)
                    flood_fill(board, z, y, x, dimZ, dimY, dimX);
            }
        }
    }
}

// ---------------------- DLL Export ------------------------
// board: 1D pointer, dimZ/dimY/dimX: board dimensions
DLL_EXPORT void process_board_dll(int8_t *board, int dimZ, int dimY, int dimX)
{
    process_board(board, dimZ, dimY, dimX);
}
