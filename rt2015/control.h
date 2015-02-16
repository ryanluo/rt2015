#ifndef CONTROL_H
#define CONTROL_H


#include "common.h"


// init calls
int make_menu();


// glut callbacks
void menu_func (int value);
void keyboard_func (unsigned char key, int x, int y);
void mouse_click_func (int button, int state, int x, int y);
void mouse_move_func (int x, int y);


// menu calls
void menu_help ();

void menu_file_open ();
void menu_file_reload ();
void menu_file_save_image ();
void menu_file_save_rayfile ();

void menu_render_raytrace ();
void menu_render_jitter ();
void menu_render_shadows ();
void menu_render_rdepth ();
void menu_render_climit ();
void menu_render_set_size ();


// mouse calls
void mouse_rotate_camera (int dx, int dy);
void mouse_translate_camera (int dx, int dy);


#endif // CONTROL_H
