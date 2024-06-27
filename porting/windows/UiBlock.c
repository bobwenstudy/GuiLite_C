
// Windows Header Files
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>

#include "UiBlock.h"
#include "UICode.h"

void CUiBlock_init(CUiBlock *self, int index, int color_bytes)
{
    self->m_color_bytes = color_bytes;
    self->m_is_mouse_down = false;
    self->m_ui_width = self->m_ui_height = 0;
}

void CUiBlock_renderUI(CUiBlock *self, RECT rect, HDC hDC)
{
    if (!EqualRect(&rect, &self->m_block_rect))
    {
        // printf("CUiBlock_renderUI: rect changed, color bytes: %d. rect: %ld,%ld,%ld,%ld.\n", self->m_color_bytes, rect.left, rect.top, rect.right,
        // rect.bottom);
        self->m_block_rect = rect;
        uicode_get_current_ui(&self->m_ui_width, &self->m_ui_height, false);
        memset(&self->m_ui_bm_info, 0, sizeof(self->m_ui_bm_info));
        self->m_ui_bm_info.bmiHeader.biSize = sizeof(MYBITMAPINFO);
        self->m_ui_bm_info.bmiHeader.biWidth = self->m_ui_width;
        self->m_ui_bm_info.bmiHeader.biHeight = (0 - self->m_ui_height); // fix upside down.
        self->m_ui_bm_info.bmiHeader.biBitCount = self->m_color_bytes * 8;
        self->m_ui_bm_info.bmiHeader.biPlanes = 1;
        self->m_ui_bm_info.bmiHeader.biSizeImage = self->m_ui_width * self->m_ui_height * self->m_color_bytes;

        switch (self->m_color_bytes)
        {
        case 2:
            self->m_ui_bm_info.bmiHeader.biCompression = BI_BITFIELDS;
            self->m_ui_bm_info.biRedMask = 0xF800;
            self->m_ui_bm_info.biGreenMask = 0x07E0;
            self->m_ui_bm_info.biBlueMask = 0x001F;
            break;
        case 4:
            self->m_ui_bm_info.bmiHeader.biCompression = BI_RGB;
            break;
        default:
            break;
        }
    }

    self->m_memDC = CreateCompatibleDC(hDC);
    self->m_blockBmp = CreateCompatibleBitmap(hDC, (rect.right - rect.left), (rect.bottom - rect.top));
    SelectObject(self->m_memDC, self->m_blockBmp);
    SetStretchBltMode(self->m_memDC, STRETCH_HALFTONE);

    int block_width = (rect.right - rect.left);
    int block_height = (rect.bottom - rect.top);

    // get ui data
    void *data = uicode_get_current_ui(&self->m_ui_width, &self->m_ui_height, true);
    // printf("CUiBlock_renderUI: block_width: %d, block_height: %d.\n", block_width, block_height);
    StretchDIBits(self->m_memDC, 0, 0, block_width, block_height, 0, 0, self->m_ui_width, self->m_ui_height, data, (BITMAPINFO *)&self->m_ui_bm_info,
                  DIB_RGB_COLORS, SRCCOPY);
    BitBlt(hDC, rect.left, rect.top, block_width, block_height, self->m_memDC, 0, 0, SRCCOPY);

    DeleteDC(self->m_memDC);
    DeleteObject(self->m_blockBmp);
}

void CUiBlock_OnLButtonDown(CUiBlock *self, int x, int y)
{
    self->m_is_mouse_down = TRUE;
    CUiBlock_pointMFC2GuiLite(self, &x, &y);
    if (x < 0 || y < 0)
    {
        return;
    }
    uicode_on_touch(x, y, true);
}

void CUiBlock_OnLButtonUp(CUiBlock *self, int x, int y)
{
    self->m_is_mouse_down = FALSE;
    CUiBlock_pointMFC2GuiLite(self, &x, &y);
    if (x < 0 || y < 0)
    {
        return;
    }
    uicode_on_touch(x, y, false);
}

void CUiBlock_OnMouseMove(CUiBlock *self, int x, int y)
{
    if (self->m_is_mouse_down == FALSE)
    {
        return;
    }
    CUiBlock_OnLButtonDown(self, x, y);
}

void CUiBlock_pointMFC2GuiLite(CUiBlock *self, int *x, int *y)
{
    if (*x > self->m_block_rect.right || *y > self->m_block_rect.bottom || *x < self->m_block_rect.left || *y < self->m_block_rect.top)
    {
        *x = *y = 0;
        return;
    }
    *x = self->m_ui_width * (*x - self->m_block_rect.left) / (self->m_block_rect.right - self->m_block_rect.left);
    *y = self->m_ui_height * (*y - self->m_block_rect.top) / (self->m_block_rect.bottom - self->m_block_rect.top);
}

void CUiBlock_OnKeyUp(CUiBlock *self, unsigned int nChar)
{
    unsigned int key = 2;
    switch (nChar)
    {
    case 68:
        key = 0;
        break;
    case 65:
        key = 1;
        break;
    }
    uicode_on_navigate(key);
}
