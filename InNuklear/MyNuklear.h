#pragma once
extern struct gdi_param {
    HBITMAP bitmap;
    HDC window_dc;
    HDC memory_dc;
    unsigned int width;
    unsigned int height;
    struct nk_context ctx;
} gdi;
