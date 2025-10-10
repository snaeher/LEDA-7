
// basic graphics operations used in the LEDA window & panel classes
//
// Macro V:   "virtual" or empty
// Macro XXX: "= 0" or empty 

#undef V
#define V

#undef XXX
#define XXX


// display

V int  x_open_display(void) XXX;
V void x_close_display(void) XXX;
V void x_flush_display(void) XXX;

V const char* x_display_info(int& width, int& height, int& dpi) XXX;

V void x_do_not_open_display(bool) XXX;

V char* x_root_pixrect(int,int,int,int) XXX;

// cloud (websocket)

V void  x_send_text(const char*) XXX;


// windows

V int x_create_window(void* inf, int width, int height, int bg_col, 
                    const char* label, const char* icon_label, int parent, 
                    void (*redraw)(void*,int,int,int,int,int)) XXX;

V void x_open_window(int win, int x, int y, int w, int h, int p_win=0, 
                                                       bool hidden = false) XXX;
V void x_set_topmost(int win) XXX;
V void x_resize_window(int win, int x, int y, int width, int height, int p) XXX;
V void x_clear_window(int win, int x0, int y0, int x1, int y1, int xorig=0,
                                                             int yorig=0) XXX;
V void x_close_window(int win) XXX;
V void x_minimize_window(int win) XXX;
V void x_maximize_window(int win) XXX;
V void x_destroy_window(int win) XXX;

V void* x_window_handle(int win) XXX;

V int   x_window_opened(int win) XXX;
V int   x_window_bits_saved(int win ) XXX;

V void* x_window_inf(int win) XXX;
V int   x_window_height(int win) XXX;
V int   x_window_width(int win) XXX;
V void  x_window_frame(int win, int&, int&, int&, int&) XXX;

V int   x_window_minimized(int win) XXX;

V int  x_set_cursor(int win, int cursor_id) XXX;
V void x_set_label(int win, const char* label) XXX;
V void x_set_icon_label(int win, const char* label) XXX;
V void x_set_border_width(int win, int w) XXX;
V void x_set_border_color(int win, int c) XXX;

V int  x_get_border_color(int win) XXX;
V int  x_get_border_width(int win) XXX;
V int  x_get_cursor(int win) XXX;

V int  x_get_resizing(int win) XXX;

V void x_set_special_event_handler(int win,
             void (*fun)(void*,const char*,const char*,int,int),
             unsigned long data) XXX;

V void x_set_icon_pixmap(int win,char*) XXX;


// buffering

V int  x_create_buffer(int) XXX;
V int  x_create_buffer(int,int,int) XXX;
V void x_delete_buffer(int) XXX;
V void x_flush_buffer(int,int,int,int,int) XXX;
V void x_flush_buffer(int,int,int,int,int,int,int) XXX;
V int  x_start_buffering(int) XXX;
V int  x_start_buffering(int,int,int) XXX;
V void x_set_buffer(int,char*) XXX;
V void x_stop_buffering(int) XXX;
V void x_stop_buffering(int, char**) XXX;
V int  x_test_buffer(int) XXX;


// timer

V void x_start_timer(int win, int msec) XXX;
V void x_stop_timer(int win) XXX;


// clipping

V void x_set_clip_rectangle(int win,int,int,int,int) XXX;
V void x_clip_mask_rectangle(int w, int x0, int y0, int x1, int y1,int m) XXX;
V void x_clip_mask_polygon(int w, int n, int *xcoord, int *ycoord, int m) XXX;
V void x_clip_mask_ellipse(int w, int x, int y, int r1, int r2, int m) XXX;
V void x_clip_mask_chord(int w, int x0, int y0, int x1, int y1, int cx, int cy, int r, int mode) XXX;


// drawing

V void x_pixel(int win, int x, int y) XXX;
V void x_pixels(int win, int n, int* x, int* y) XXX;

V int  x_get_pixel(int win, int x, int y) XXX;

V void x_line0(int win, int x1, int y1, int x2, int y2) XXX;
V void x_line(int win, int x1, int y1, int x2, int y2) XXX;
V void x_lines(int win, int n, int* x1, int* y1, int* x2, int* y2) XXX;

V void x_rect(int win, int x1, int y1, int x2, int y2) XXX;
V void x_box(int win, int x1, int y1, int x2, int y2) XXX;
V void x_polyline(int win, int n, int* xcoord, int* ycoord, int adjust=0) XXX;
V void x_polyline(int win, int n, int* xcoord, int* ycoord, int* clrs) XXX;
V void x_arc(int win,int x0,int y0,int r1,int r2,double start,double angle) XXX;
V void x_ellipse(int win, int x0, int y0, int r1, int r2) XXX;

V void x_fill_polygon(int win, int n, int* xcoord, int* ycoord) XXX;
V void x_fill_arc(int win,int x0,int y0,int r1,int r2,double start,double angle) XXX;
V void x_fill_ellipse(int win, int x0, int y0, int r1, int r2) XXX;

V void x_text(int win, int x, int y, const char* s) XXX;
V void x_text(int win, int x, int y, const char* s, int l) XXX;
V void x_ctext(int win, int x, int y, const char* s) XXX;

V void x_text_underline(int win,int x,int y,const char* s,int l,int r) XXX;
V void x_ctext_underline(int win,int x,int y,const char* s,int l,int r) XXX;


// bitmaps

V char* x_create_bitmap(int win, int width, int height, unsigned char* data,int f) XXX;
V void  x_insert_bitmap(int win, int x, int y, char* bmap) XXX;
V void  x_delete_bitmap(char* bmap) XXX;


// pixrects

V char* x_create_pixrect_from_bits(int win,int w,int h,unsigned char* data,
                                                     int fg, int bg) XXX;

V char* x_create_pixrect_from_bgra(int win,int w,int h,unsigned char* data, 
                                                     float scale_f=1) XXX;

V char* x_create_pixrect(int win, int x1, int y1, int x2, int y2) XXX;

V void  x_insert_pixrect(int win, char* prect) XXX;
V void  x_insert_pixrect(int win, int x, int y, char* prect) XXX;
V void  x_insert_pixrect(int win, int x, int y, char*,int,int,int,int) XXX;
V void  x_insert_pixrect(int win, int x, int y, int w, int h, char*) XXX;

V unsigned char* x_pixrect_to_bgra(int win, char* prect) XXX;

V void  x_delete_pixrect(char* prect) XXX;

V void  x_set_pixrect_alpha(char* prect, int a) XXX;
V void  x_set_pixrect_phi(char* prect, float phi) XXX;
V void  x_set_pixrect_anchor(char* prect, int x, int y) XXX;

V void  x_get_pixrect_size(char*,int&,int&) XXX;
V void  x_get_pixrect_anchor(char*,int&,int&) XXX;


// fonts and text

V int  x_set_font(int win, const char* fname) XXX;

V void x_set_text_font(int win) XXX;
V void x_set_italic_font(int win) XXX;
V void x_set_bold_font(int win) XXX;
V void x_set_fixed_font(int win) XXX;
V void x_set_button_font(int win) XXX;

V void x_save_font(int win) XXX;
V void x_restore_font(int win) XXX;

V int x_text_width(int win, const char* s) XXX;
V int x_text_width(int win, const char* s, int l) XXX;
V int x_text_height(int win, const char* s) XXX;


// drawing parameters

V int           x_set_color(int win, int col) XXX;
V int           x_set_bg_color(int win, int col) XXX;
V char*         x_set_bg_pixmap(int win, char* p) XXX;
V void          x_set_bg_origin(int win, int x, int y) XXX;
V int           x_set_line_width(int win, int w) XXX;
V int           x_set_join_style(int win, int s) XXX;
V drawing_mode  x_set_mode(int win, drawing_mode m) XXX;
V text_mode     x_set_text_mode(int win, text_mode tm) XXX;
V line_style    x_set_line_style(int win, line_style ls) XXX;
V void          x_set_stipple(int win, char* bits, int s) XXX;

V int           x_get_color(int win) XXX;
V int           x_get_line_width(int win) XXX;
V drawing_mode  x_get_mode(int win) XXX;
V text_mode     x_get_text_mode(int win) XXX;
V line_style    x_get_line_style(int win) XXX;

V void x_set_read_gc(int win) XXX;
V void x_reset_gc(int win) XXX;


// events

V int x_get_next_event(int& w,int& x,int& y,int& val1,int& val2,
                                                    unsigned long& t,
                                                    int msec=0) XXX;
V void x_put_back_event(void) XXX;


// mouse

V void x_grab_pointer(int w) XXX;
V void x_move_pointer(int w, int x, int y) XXX;
V void x_set_focus(int w) XXX;


// tooltips

V void  x_set_tooltip(int w, int id, int x0,int y0,int x1,int y1, const char*) XXX;
V void  x_del_tooltip(int w, int id) XXX;

//misc

V void x_window_to_screen(int w, int& x, int& y) XXX;
V void x_screen_to_window(int w, int& x, int& y) XXX;

V void  x_text_to_clipboard(int w, const char* txt) XXX;
V char* x_text_from_clipboard(int w) XXX;


//------------------------------------------------------------------------------
// mswin
//------------------------------------------------------------------------------

V void x_set_alpha(int win, int a) XXX;

// dialogs
V int x_choose_file(int w,int mode,const char* title, const char* filt,
                                                    const char* def_ext,
                                                    char* dname, 
                                                    char* fname) XXX;
V int x_choose_color(int w, int col) XXX;

// clipboard
V void  x_pixrect_to_clipboard(int w, char* prect) XXX;
V char* x_pixrect_from_clipboard(int w) XXX;

// metafile
V void  x_open_metafile(int w, const char* fname) XXX;
V void  x_close_metafile(int w) XXX;
V void  x_load_metafile(int w,int,int,int,int,const char* fname) XXX;
V void  x_metafile_to_clipboard(int w) XXX;

