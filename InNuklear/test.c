#include <stdio.h>
#include <string.h>

#include "nuklear.h"

#define WIDTH 1200
#define HEIDTH 800

struct nk_canvas {
   struct nk_command_buffer *painter;
   struct nk_vec2 item_spacing;
   struct nk_vec2 panel_padding;
   struct nk_style_item window_background;
};

static void
canvas_begin(struct nk_context *ctx, struct nk_canvas *canvas, nk_flags flags,
             int x, int y, int width, int height, struct nk_color background_color)
{
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
canvas_end(struct nk_context *ctx, struct nk_canvas *canvas)
{
   nk_end(ctx);
   ctx->style.window.spacing = canvas->panel_padding;
   ctx->style.window.padding = canvas->item_spacing;
   ctx->style.window.fixed_background = canvas->window_background;
}

int main(void)
{
   struct nk_canvas canvas;
   struct nk_context *ctx;
   int running = 1;

   ctx = nk_gdi_init(WIDTH ,HEIDTH ,"Окно Window");

   while (running)
   {
      nk_gdi_dispatch(ctx, &running);
      

      {
         canvas_begin(ctx, &canvas, 0, 0, 0, WIDTH, HEIDTH, nk_rgb(250,250,250));
         nk_fill_rect(canvas.painter, nk_rect(0.25*WIDTH,0.25*HEIDTH,0.75*WIDTH,0.75*HEIDTH), 1, nk_rgb(188, 174, 118));
         canvas_end(ctx, &canvas);
      }


      if (nk_begin(ctx, "Пример", nk_rect(50, 50, 200, 200),
                   NK_WINDOW_BORDER))
      {
         enum {EASY, HARD};
         static int op = EASY;
         static int property = 20;
         static int text_len=0;
         static char text[64];

         nk_layout_row_static(ctx, 30, 80, 1);
         nk_label(ctx, "Текст:", NK_TEXT_LEFT);
         nk_edit_string(ctx, NK_EDIT_SIMPLE, text, &text_len, 64, nk_filter_default);
         if (nk_button_label(ctx, "Кнопка"))
            fprintf(stdout, "button pressed %s\n",text);
         nk_layout_row_dynamic(ctx, 30, 2);
         if (nk_option_label(ctx, "простой", op == EASY)) op = EASY;
         if (nk_option_label(ctx, "Сложный", op == HARD)) op = HARD;
         nk_layout_row_dynamic(ctx, 22, 1);
         nk_property_int(ctx, "Сжатие:", 0, &property, 100, 10, 1);


         struct nk_command_buffer *painter = nk_window_get_canvas(ctx);
         struct nk_rect space;
         nk_widget(&space, ctx);
         nk_stroke_line(painter, space.x, space.y, space.x+100, space.y+100, 4.0f, nk_rgb(189,200,75));
      }
      nk_end(ctx);

      nk_gdi_render(nk_rgb(120,120,120));
   }
   nk_gdi_release();

   return 0;
}