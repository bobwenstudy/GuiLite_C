
#pragma once

#include <stdbool.h>
#include <windows.h>

typedef struct tagMYBITMAPINFO
{
    BITMAPINFOHEADER bmiHeader;
    DWORD biRedMask;
    DWORD biGreenMask;
    DWORD biBlueMask;
} MYBITMAPINFO;

typedef struct
{
    int m_color_bytes;
    RECT m_block_rect;
    int m_ui_width;
    int m_ui_height;

    MYBITMAPINFO m_ui_bm_info;
    HDC m_memDC;
    HBITMAP m_blockBmp;

    bool m_is_mouse_down;
} CUiBlock;

void CUiBlock_init(CUiBlock *self, int index, int color_bytes);
void CUiBlock_renderUI(CUiBlock *self, RECT rect, HDC hDC);
void CUiBlock_OnLButtonDown(CUiBlock *self, int x, int y);
void CUiBlock_OnLButtonUp(CUiBlock *self, int x, int y);
void CUiBlock_OnMouseMove(CUiBlock *self, int x, int y);
void CUiBlock_pointMFC2GuiLite(CUiBlock *self, int *x, int *y);
void CUiBlock_OnKeyUp(CUiBlock *self, unsigned int nChar);
