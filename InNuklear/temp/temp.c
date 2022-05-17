



#define WIDTH 1200
#define HEIDTH 800

struct Car {
   double v;
   int x;
};

struct nk_canvas {
   struct nk_command_buffer *painter;
   struct nk_vec2 item_spacing;
   struct nk_vec2 panel_padding;
   struct nk_style_item window_background;
};

static void
canvas_begin(struct nk_context *ctx, struct nk_canvas *canvas, nk_flags flags,
             int x, int y, int width, int height, struct nk_color background_color){
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
canvas_end(struct nk_context *ctx, struct nk_canvas *canvas){
   nk_end(ctx);
   ctx->style.window.spacing = canvas->panel_padding;
   ctx->style.window.padding = canvas->item_spacing;
   ctx->style.window.fixed_background = canvas->window_background;
}


void menu(struct nk_context *ctx, int *is_menu, int *is_about,int *is_model,int *running) {
   if (nk_begin(ctx, "Меню", nk_rect(0, 0, WIDTH, HEIDTH),
                NK_WINDOW_BORDER))
   {
      nk_layout_row_dynamic(ctx, HEIDTH/6, 1);
      nk_label(ctx, "Программа моделирования движения на автостраде", NK_TEXT_CENTERED);
      nk_layout_row_dynamic(ctx, HEIDTH/6, 1);
      if (nk_button_label(ctx, "Моделирование")) {
         printf("button mod pressed\n");
         *is_model = 1;
         *is_menu = 0;
      }
      nk_layout_row_dynamic(ctx, HEIDTH/6, 1);
      if (nk_button_label(ctx, "О программе")) {
         printf("button mod pressed\n");
         *is_about = 1;
         *is_menu = 0;
      }
      nk_layout_row_dynamic(ctx, HEIDTH/6, 1);
      if (nk_button_label(ctx, "Закрыть")) {
         printf("button mod pressed\n");
         *running = 0;
         *is_menu = 0;
      }
   }
   nk_end(ctx);
}

void about(struct nk_context *ctx, int *is_menu, int *is_about,int *running) {
   if (nk_begin(ctx, "О программе", nk_rect(0, 0, WIDTH, HEIDTH),
                NK_WINDOW_BORDER))
   {
      nk_layout_row_static(ctx, HEIDTH/5, WIDTH/4, 1);
      nk_label(ctx, "Это Программа моделирования движения на автостраде", NK_TEXT_CENTERED);

      nk_layout_row_static(ctx, WIDTH/2, HEIDTH/16, 1);
      nk_label(ctx, "Описание происходящего", NK_TEXT_CENTERED);

      nk_layout_row_dynamic(ctx, HEIDTH/6, 2);
      if (nk_button_label(ctx, "Назад")) {
         printf("button mod pressed\n");
         *is_about = 0;
         *is_menu = 1;
      }
      if (nk_button_label(ctx, "Закрыть")) {
         printf("button mod pressed\n");
         *running = 0;
         *is_about = 0;
      }
   }
   nk_end(ctx);

}

void model(struct nk_context *ctx,struct nk_canvas canvas, int *is_menu,int *is_model,int *running) {
   int is_start = 0;
   
   static double MaxSpeed = 5.0;
   static int SafeDist = 10;
   static double Accel = 2;
   // 
   int roadH = 100;
   int roadW = WIDTH;

   static int motion_X = 0;
   
   {
      canvas_begin(ctx, &canvas, 0, 0, 0, WIDTH, roadH+20, nk_rgb(250,20,20));
      nk_fill_rect(canvas.painter, nk_rect(0, 10, roadW,roadH), 1, nk_rgb(200, 200, 200));
      static int 
      nk_stroke_line(_abracadabra_cast(canvas);