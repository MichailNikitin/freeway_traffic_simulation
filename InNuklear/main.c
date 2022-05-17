#include <stdio.h>
#include <string.h>
#include "nuklear.h"

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

/*______window:Program-description______*/
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

/*______window:Modeling______*/
void model(struct nk_context *ctx,struct nk_canvas canvas, int *is_menu,int *is_model,int *running) {
   static int is_start = 0;

   static double MaxSpeed = 5.0;
   static int SafeDist = 10;
   static double Accel = 2;
   //
   int roadH = 100;
   int roadW = WIDTH;

   static int motion_X = 10;
   /*__drow-road&cars__*/
   {
      canvas_begin(ctx, &canvas, 0, 0, 0, WIDTH, roadH+20, nk_rgb(250,20,20));
      nk_fill_rect(canvas.painter, nk_rect(0, 10, roadW,roadH), 1, nk_rgb(200, 200, 200));
      /*_motion-cars_*/
      if (is_start){
      motion_X = (int)(motion_X + MaxSpeed/20) % roadW;
      nk_stroke_line(canvas.painter, motion_X, roadH/2, motion_X+2, roadH/2, 5, nk_rgb(150,150,255));
      }
      canvas_end(ctx, &canvas);
   }
   /*___input parameters___*/
   if (nk_begin(ctx, "Парамметры модели", nk_rect(0, roadH+20, WIDTH, HEIDTH),
                NK_WINDOW_BORDER))
   {

      nk_layout_row_dynamic(ctx, HEIDTH/16, 1);
      nk_label(ctx, "Введите значения значения параметров", NK_TEXT_CENTERED);

      nk_layout_row_dynamic(ctx, HEIDTH/16, 1);
      nk_property_double(ctx,"Максимальная скорость", 1.0, &MaxSpeed, 80.0, 0.5, 0.1);

      nk_layout_row_dynamic(ctx, HEIDTH/16, 1);
      nk_property_int(ctx,"Безопасная дистанция", 1, &SafeDist, 50, 1, 1);

      nk_layout_row_dynamic(ctx, HEIDTH/16, 1);
      nk_property_double(ctx,"Величина ускорения/торможения", 0.05, &Accel, 10.0, 0.1, 0.05);

      nk_layout_row_dynamic(ctx, HEIDTH/16, 3);
      if (nk_button_label(ctx, "Начало")) {
         printf("MaxSpeed - %.2lf\n", MaxSpeed);
         printf("SafeDist - %d\n", SafeDist);
         printf("Accel - %.3lf\n", Accel);
         printf("motion_X - %d\n", motion_X);
         is_start = 1;
      }
      if (nk_button_label(ctx, "Назад")) {
         printf("button mod pressed\n");
         *is_model = 0;
         *is_menu = 1;
      }
      if (nk_button_label(ctx, "Закрыть")) {
         printf("button mod pressed\n");
         *is_model = 0;
         *running = 0;
      }
   }
   nk_end(ctx);

}

int main(void)
{
   struct nk_canvas canvas;
   struct nk_context *ctx;

   int running = 1;
   int is_menu = 1;
   int is_about = 0;
   int is_model = 0;

   ctx = nk_gdi_init(WIDTH,HEIDTH,"Freeway traffic simulation");

   while (running)
   {

      nk_gdi_dispatch(ctx, &running);
      if (is_menu)
         menu(ctx, &is_menu, &is_about, &is_model, &running);
      if (is_about)
         about(ctx, &is_menu, &is_about, &running);
      if (is_model)
         model(ctx, canvas, &is_menu, &is_model, &running);

      nk_gdi_render(nk_rgb(120,120,120));
   }
   nk_gdi_release();

   return 0;
}