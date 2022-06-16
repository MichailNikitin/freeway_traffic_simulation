






#define WIDTH 1000
#define HEIDTH 800

#define WIDTH_BUTTON 100
#define HEIDTH_BUTTON 80
#define HEIDTH_TEXT 100

int running = 1;
int FreeSpaceInStart = 1;
int Collision = 0;
int CarsCount = 0;

struct nk_canvas canvas;
struct nk_context *ctx;

enum page {
   MENU, ABOUT, MODEL
} selected_page = MENU;

struct Car {
   double want_v;
   double v;
   int x;
   double t;
} cars[30];

typedef struct traffic {
   int head;
   int tail;
   int size;
   struct Car car;
} traffic;

void nk_MyGdi_dispatch() {

   nk_input_begin(ctx);
   MSG msg;

   while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT)
         running = 0;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
   nk_input_end(ctx);
}

struct nk_canvas {
   struct nk_command_buffer *painter;
   struct nk_vec2 item_spacing;
   struct nk_vec2 panel_padding;
   struct nk_style_item window_background;
};
/*__________convas function___________*/
static void
canvas_begin(struct nk_context *ctx, struct nk_canvas *canvas, nk_flags flags,
             int x, int y, int width, int height, struct nk_color background_color) {
   /* save style properties which will be overwritten */
   canvas->panel_padding = ctx->style.window.padding;
   canvas->item_spacing = ctx->style.window.spacing;
   canvas->window_background = ctx->style.window.fixed_background;

   /* use the complete window space and set background */
   ctx->style.window.spacing = nk_vec2(0,0);
   ctx->style.window.padding = nk_vec2(0,0);
   ctx->style.window.fixed_background = nk_style_item_color(background_color);

   /* create/update window and set position + size */
   flags = flags & ~NK_WINDOW_DYNAMIC;
   nk_window_set_bounds(ctx, "Window", nk_rect(x, y, width, height));
   nk_begin(ctx, "Window", nk_rect(x, y, width, height), NK_WINDOW_NO_SCROLLBAR|flags);

   /* allocate the complete window space for drawing */
   {  struct nk_rect total_space;
      total_space = nk_window_get_content_region(ctx);
      nk_layout_row_dynamic(ctx, total_space.h, 1);
      nk_widget(&total_space, ctx);
      canvas->painter = nk_window_get_canvas(ctx);
   }
}

static void
canvas_end(struct nk_context *ctx, struct nk_canvas *canvas) {
   nk_end(ctx);
   ctx->style.window.spacing = canvas->panel_padding;
   ctx->style.window.padding = canvas->item_spacing;
   ctx->style.window.fixed_background = canvas->window_background;
}

/*________window:Start-Menu__________*/
void menu() {
   if (nk_begin(ctx, "Меню", nk_rect(0, 0, gdi.width, gdi.height),
                NK_WINDOW_BORDER))
   {
      nk_layout_row_dynamic(ctx, HEIDTH_TEXT, 1);
      nk_label(ctx, "Программа моделирования движения на автостраде", NK_TEXT_CENTERED);
      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 1);
      if (nk_button_label(ctx, "Моделирование")) {
         selected_page = MODEL;
      }
      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 1);
      if (nk_button_label(ctx, "О программе")) {
         selected_page = ABOUT;
      }
      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 1);
      if (nk_button_label(ctx, "Закрыть")) {
         running = 0;
      }
   }
   nk_end(ctx);
}

/*______window:Program-description______*/
void about() {
   if (nk_begin(ctx, "О программе", nk_rect(0, 0, gdi.width, gdi.height),
                NK_WINDOW_BORDER))
   {
      nk_layout_row_dynamic(ctx, HEIDTH_TEXT, 1);
      nk_label(ctx, "Это Программа моделирования движения на автостраде", NK_TEXT_CENTERED);

      nk_layout_row_dynamic(ctx, 250, 1);
      nk_label(ctx, "Программа по моделированию движения автомобилей на автостраде, сдаланная бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-бла-", NK_TEXT_CENTERED);

      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 2);
      if (nk_button_label(ctx, "Назад")) {
         selected_page = MENU;
      }
      if (nk_button_label(ctx, "Закрыть")) {
         running = 0;
      }
   }
   nk_end(ctx);

}

/*______window:Modeling______*/
void model() {
   static int is_start = 0;

   static double MaxSpeed = 5.0;
   static int SafeDist = 10;
   static double Accel = 2;

   int roadH = 100;
   int roadW = gdi.width;

   time_t t;
   srand((unsigned) time(&t));

   static int motion_X = 10;
   /*__drow-road&cars__*/
   {
      canvas_begin(ctx, &canvas, 0, 0, 0, roadW, roadH+20, nk_rgb(250,20,20));
      nk_fill_rect(canvas.painter, nk_rect(0, 10, roadW,roadH), 1, nk_rgb(200, 200, 200));

      /*_motion-cars_*/
      if (is_start) {
         int i = 0;
         int v = _abracadabra_cast(cars[i]);