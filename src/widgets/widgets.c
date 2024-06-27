#include "widgets.h"

DIALOG_ARRAY c_dialog_ms_the_dialogs[SURFACE_CNT_MAX];
c_keyboard c_edit_s_keyboard;

static c_keyboard_button s_key_0, s_key_1, s_key_2, s_key_3, s_key_4, s_key_5, s_key_6, s_key_7, s_key_8, s_key_9;
static c_keyboard_button s_key_A, s_key_B, s_key_C, s_key_D, s_key_E, s_key_F, s_key_G, s_key_H, s_key_I, s_key_J;
static c_keyboard_button s_key_K, s_key_L, s_key_M, s_key_N, s_key_O, s_key_P, s_key_Q, s_key_R, s_key_S, s_key_T;
static c_keyboard_button s_key_U, s_key_V, s_key_W, s_key_X, s_key_Y, s_key_Z;
static c_keyboard_button s_key_dot, s_key_caps, s_key_space, s_key_enter, s_key_del, s_key_esc, s_key_num_switch;

WND_TREE g_key_board_children[] = {
        // Row 1
        {(c_wnd *)&s_key_Q, 'Q', 0, POS_X(0), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_W, 'W', 0, POS_X(1), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_E, 'E', 0, POS_X(2), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_R, 'R', 0, POS_X(3), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_T, 'T', 0, POS_X(4), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_Y, 'Y', 0, POS_X(5), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_U, 'U', 0, POS_X(6), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_I, 'I', 0, POS_X(7), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_O, 'O', 0, POS_X(8), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_P, 'P', 0, POS_X(9), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
        // Row 2
        {(c_wnd *)&s_key_A, 'A', 0, ((KEY_WIDTH / 2) + POS_X(0)), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_S, 'S', 0, ((KEY_WIDTH / 2) + POS_X(1)), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_D, 'D', 0, ((KEY_WIDTH / 2) + POS_X(2)), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_F, 'F', 0, ((KEY_WIDTH / 2) + POS_X(3)), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_G, 'G', 0, ((KEY_WIDTH / 2) + POS_X(4)), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_H, 'H', 0, ((KEY_WIDTH / 2) + POS_X(5)), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_J, 'J', 0, ((KEY_WIDTH / 2) + POS_X(6)), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_K, 'K', 0, ((KEY_WIDTH / 2) + POS_X(7)), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_L, 'L', 0, ((KEY_WIDTH / 2) + POS_X(8)), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
        // Row 3
        {(c_wnd *)&s_key_caps, 0x14, 0, POS_X(0), POS_Y(2), CAPS_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_Z, 'Z', 0, ((KEY_WIDTH / 2) + POS_X(1)), POS_Y(2), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_X, 'X', 0, ((KEY_WIDTH / 2) + POS_X(2)), POS_Y(2), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_C, 'C', 0, ((KEY_WIDTH / 2) + POS_X(3)), POS_Y(2), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_V, 'V', 0, ((KEY_WIDTH / 2) + POS_X(4)), POS_Y(2), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_B, 'B', 0, ((KEY_WIDTH / 2) + POS_X(5)), POS_Y(2), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_N, 'N', 0, ((KEY_WIDTH / 2) + POS_X(6)), POS_Y(2), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_M, 'M', 0, ((KEY_WIDTH / 2) + POS_X(7)), POS_Y(2), KEY_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_del, 0x7F, 0, ((KEY_WIDTH / 2) + POS_X(8)), POS_Y(2), DEL_WIDTH, KEY_HEIGHT},
        // Row 4
        {(c_wnd *)&s_key_esc, 0x1B, 0, POS_X(0), POS_Y(3), ESC_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_num_switch, 0x90, 0, POS_X(2), POS_Y(3), SWITCH_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_space, ' ', 0, ((KEY_WIDTH / 2) + POS_X(3)), POS_Y(3), SPACE_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_dot, '.', 0, ((KEY_WIDTH / 2) + POS_X(6)), POS_Y(3), DOT_WIDTH, KEY_HEIGHT},
        {(c_wnd *)&s_key_enter, '\n', 0, POS_X(8), POS_Y(3), ENTER_WIDTH, KEY_HEIGHT},
        {0, 0, 0, 0, 0, 0, 0}};

WND_TREE g_number_board_children[] = {{(c_wnd *)&s_key_1, '1', 0, POS_X(0), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
                                      {(c_wnd *)&s_key_2, '2', 0, POS_X(1), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},
                                      {(c_wnd *)&s_key_3, '3', 0, POS_X(2), POS_Y(0), KEY_WIDTH, KEY_HEIGHT},

                                      {(c_wnd *)&s_key_4, '4', 0, POS_X(0), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
                                      {(c_wnd *)&s_key_5, '5', 0, POS_X(1), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},
                                      {(c_wnd *)&s_key_6, '6', 0, POS_X(2), POS_Y(1), KEY_WIDTH, KEY_HEIGHT},

                                      {(c_wnd *)&s_key_7, '7', 0, POS_X(0), POS_Y(2), KEY_WIDTH, KEY_HEIGHT},
                                      {(c_wnd *)&s_key_8, '8', 0, POS_X(1), POS_Y(2), KEY_WIDTH, KEY_HEIGHT},
                                      {(c_wnd *)&s_key_9, '9', 0, POS_X(2), POS_Y(2), KEY_WIDTH, KEY_HEIGHT},

                                      {(c_wnd *)&s_key_esc, 0x1B, 0, POS_X(0), POS_Y(3), KEY_WIDTH, KEY_HEIGHT},
                                      {(c_wnd *)&s_key_0, '0', 0, POS_X(1), POS_Y(3), KEY_WIDTH, KEY_HEIGHT},
                                      {(c_wnd *)&s_key_dot, '.', 0, POS_X(2), POS_Y(3), KEY_WIDTH, KEY_HEIGHT},

                                      {(c_wnd *)&s_key_del, 0x7F, 0, POS_X(3), POS_Y(0), KEY_WIDTH, KEY_HEIGHT * 2 + 2},
                                      {(c_wnd *)&s_key_enter, '\n', 0, POS_X(3), POS_Y(2), KEY_WIDTH, KEY_HEIGHT * 2 + 2},
                                      {0, 0, 0, 0, 0, 0, 0}};

void c_widgets_init(void)
{
    c_keyboard_init(&c_edit_s_keyboard);

    c_keyboard_button_init(&s_key_0);
    c_keyboard_button_init(&s_key_1);
    c_keyboard_button_init(&s_key_2);
    c_keyboard_button_init(&s_key_3);
    c_keyboard_button_init(&s_key_4);
    c_keyboard_button_init(&s_key_5);
    c_keyboard_button_init(&s_key_6);
    c_keyboard_button_init(&s_key_7);
    c_keyboard_button_init(&s_key_8);
    c_keyboard_button_init(&s_key_9);

    c_keyboard_button_init(&s_key_A);
    c_keyboard_button_init(&s_key_B);
    c_keyboard_button_init(&s_key_C);
    c_keyboard_button_init(&s_key_D);
    c_keyboard_button_init(&s_key_E);
    c_keyboard_button_init(&s_key_F);
    c_keyboard_button_init(&s_key_G);
    c_keyboard_button_init(&s_key_H);
    c_keyboard_button_init(&s_key_I);
    c_keyboard_button_init(&s_key_J);
    c_keyboard_button_init(&s_key_K);
    c_keyboard_button_init(&s_key_L);
    c_keyboard_button_init(&s_key_M);
    c_keyboard_button_init(&s_key_N);
    c_keyboard_button_init(&s_key_O);
    c_keyboard_button_init(&s_key_P);
    c_keyboard_button_init(&s_key_Q);
    c_keyboard_button_init(&s_key_R);
    c_keyboard_button_init(&s_key_S);
    c_keyboard_button_init(&s_key_T);
    c_keyboard_button_init(&s_key_U);
    c_keyboard_button_init(&s_key_V);
    c_keyboard_button_init(&s_key_W);
    c_keyboard_button_init(&s_key_X);
    c_keyboard_button_init(&s_key_Y);
    c_keyboard_button_init(&s_key_Z);

    c_keyboard_button_init(&s_key_dot);
    c_keyboard_button_init(&s_key_caps);
    c_keyboard_button_init(&s_key_space);
    c_keyboard_button_init(&s_key_enter);
    c_keyboard_button_init(&s_key_del);
    c_keyboard_button_init(&s_key_esc);
    c_keyboard_button_init(&s_key_num_switch);
}
