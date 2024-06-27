#pragma once

// BITMAP
typedef struct struct_bitmap_info
{
    unsigned short width;
    unsigned short height;
    unsigned short color_bits; // support 16 bits only
    const unsigned short *pixel_color_array;
} BITMAP_INFO;

// FONT
typedef struct struct_lattice
{
    unsigned int utf8_code;
    unsigned char width;
    const unsigned char *pixel_buffer;
} LATTICE;

typedef struct struct_lattice_font_info
{
    unsigned char height;
    unsigned int count;
    const LATTICE *lattice_array;
} LATTICE_FONT_INFO;
