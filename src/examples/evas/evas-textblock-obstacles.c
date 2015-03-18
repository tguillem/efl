/**
 * Evas textblock example for obstacles feature
 *
 * You'll need at least one engine built for it (excluding the buffer
 * one). See stdout/stderr for output.
 *
 * @verbatim
 * gcc -o evas-textblock-obstacles evas-textblock-obstacles.c `pkg-config --libs --cflags evas ecore ecore-evas`
 * @endverbatim
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define PACKAGE_EXAMPLES_DIR "."
#endif

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <stdio.h>
#include <errno.h>
#include "evas-common.h"

#define WIDTH  (320)
#define HEIGHT (240)

#define GREY {190, 190, 190, 255}
#define BLACK {0, 0, 0, 255}
#define WHITE {255, 255, 255, 255}
#define RED   {255, 0, 0, 255}
#define GREEN {0, 255, 0, 255}
#define BLUE  {0, 0, 255, 255}

#define POINTER_CYCLE(_ptr, _array)                             \
  do                                                            \
    {                                                           \
       if ((unsigned int)(((unsigned char *)(_ptr)) - ((unsigned char *)(_array))) >= \
           sizeof(_array))                                      \
         _ptr = _array;                                         \
    }                                                           \
  while(0)

static const char *commands = \
  "commands are:\n"
  "\ts - change obstacle's size\n"
  "\tp - change obstacle's position\n"
  "\tt - change obstacle's type (rectangle/image)\n"
  "\tf - change obstacle's format\n"
  "\th - print help\n";

static const char *obs_img_path = PACKAGE_EXAMPLES_DIR EVAS_IMAGE_FOLDER "/kitty.png";

struct color_tuple
{
   int r, g, b, a;
};

struct text_preset_data
{
   const char        **font_ptr;
   const char         *font[3];

   Evas_Coord         *obs_size_ptr;
   Evas_Coord          obs_size[3];

   const Evas_Textblock_Obstacle_Format *obs_fmt_ptr;
   Evas_Textblock_Obstacle_Format obs_fmt[3];

   Eo **obs_ptr; /* pointer to the currently controlled obstacle object */
   Eo *obs[2];
};

struct test_data
{
   Ecore_Evas             *ee;
   Evas                   *evas;
   struct text_preset_data t_data;
   Evas_Object            *text, *bg, *border;
   Evas_Coord             w, h;
};

static struct test_data d = {0};

static void
_on_destroy(Ecore_Evas *ee EINA_UNUSED)
{
   ecore_main_loop_quit();
}

static void
_canvas_resize_cb(Ecore_Evas *ee)
{
   int w, h;

   ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
   evas_object_resize(d.bg, w, h);
   evas_object_resize(d.text, w, h);
   d.w = w;
   d.h = h;
}

static unsigned int
_getrand(unsigned int low, unsigned int high)
{
   return (rand() % (high - low)) + low;
}

static void
_on_keydown(void        *data EINA_UNUSED,
            Evas        *evas EINA_UNUSED,
            Evas_Object *o EINA_UNUSED,
            void        *einfo)
{
   Evas_Event_Key_Down *ev = einfo;

   if (strcmp(ev->key, "h") == 0) /* print help */
     {
        fprintf(stdout, commands);
        return;
     }

   if (strcmp(ev->key, "t") == 0) /* change obstacle type */
     {
        (d.t_data.obs_ptr)++;
        POINTER_CYCLE(d.t_data.obs_ptr, d.t_data.obs);

        fprintf(stdout, "Now controlling obstacle: %p\n", *d.t_data.obs_ptr);

        return;
     }

   if (strcmp(ev->key, "v") == 0) /* change obstacle visibility */
     {
        Evas_Coord x, y;
        Eo *obj = *d.t_data.obs_ptr;
        Eina_Bool is_v;
        eo_do(obj, is_v = evas_obj_visibility_get());
        if (is_v)
           evas_object_hide(obj);
        else
           evas_object_show(obj);
        fprintf(stdout, "Show/hide toggle for obstacle %p\n",
              *d.t_data.obs_ptr);

        return;
     }

   if (strcmp(ev->key, "f") == 0) /* change obstacle format */
     {
        (d.t_data.obs_fmt_ptr)++;
        POINTER_CYCLE(d.t_data.obs_fmt_ptr, d.t_data.obs_fmt);

        evas_object_textblock_obstacle_register(d.text,
                                               *d.t_data.obs_ptr,
                                               *d.t_data.obs_fmt_ptr);

        fprintf(stdout, "Changing obstacle format to: %d\n", *d.t_data.obs_fmt_ptr);

        return;
     }

   if (strcmp(ev->key, "s") == 0) /* change obstacle size */
     {
        (d.t_data.obs_size_ptr)++;
        POINTER_CYCLE(d.t_data.obs_size_ptr, d.t_data.obs_size);

        evas_object_resize(*d.t_data.obs_ptr,
              *d.t_data.obs_size_ptr,
              *d.t_data.obs_size_ptr);

        fprintf(stdout, "Changing obstacle size to: %d,%d\n", *d.t_data.obs_size_ptr, *d.t_data.obs_size_ptr);

        return;
     }
   if (strcmp(ev->key, "p") == 0) /* change obstacle position */
     {
        Evas_Coord x, y;
        x = _getrand(0, d.w);
        y = _getrand(0, d.h);
        evas_object_move(*d.t_data.obs_ptr, x, y);

        fprintf(stdout, "Changing obstacle position to: %d,%d\n", x, y);

        return;
     }
}

static void
_obs_init(Evas_Object *obj)
{
   evas_object_resize(obj, 50, 50);
}

static void
_text_init(Evas_Object *obj, Evas_Textblock_Style **st)
{
   char buf[2000];
   const char *wrapt = "word";
   snprintf(buf,
         2000,
         "DEFAULT='font=Sans font_size=16 color=#000 wrap=%s text_class=entry'"
         "br='\n'"
         "ps='ps'"
         "tab='\t'",
         wrapt);

   *st = evas_textblock_style_new();
   evas_textblock_style_set(*st, buf);
   evas_object_textblock_style_set(obj, *st);

   evas_object_textblock_text_markup_set(d.text,
         "This is an example text to demonstrate the textblock object"
         " with obstacle objects support."
         " Any evas object can register itself as an obstacle to the textblock"
         " object. Upon registring, it affects the layout of the text in"
         " certain situations. Usually, when the obstacle shows above the text"
         " area, it will cause the layout of the text to split and move"
         " parts of it, so that all text area is apparent.");
}

int
main(void)
{
   Evas_Textblock_Style *st;

   if (!ecore_evas_init())
     return EXIT_FAILURE;

   /* example obstacles types */
   Evas_Object *rect, *img;

   /* init values one is going to cycle through while running this
    * example */
   struct text_preset_data init_data =
   {
      .font = {"DejaVu", "Courier", "Utopia"},
      .obs_size = {50, 70, 100},
      .obs_fmt = {EVAS_TEXTBLOCK_OBSTACLE_DISABLED,
                  EVAS_TEXTBLOCK_OBSTACLE_LINE,
                  EVAS_TEXTBLOCK_OBSTACLE_FLOAT},
      .obs = {NULL, NULL},
   };

   d.t_data = init_data;
   d.t_data.font_ptr = d.t_data.font;
   d.t_data.obs_size_ptr = d.t_data.obs_size;
   d.t_data.obs_ptr = d.t_data.obs;
   d.t_data.obs_fmt_ptr = d.t_data.obs_fmt;

   /* this will give you a window with an Evas canvas under the first
    * engine available */
   d.ee = ecore_evas_new(NULL, 0, 0, WIDTH, HEIGHT, NULL);
   if (!d.ee)
     goto error;

   //ecore_evas_callback_destroy_set(d.ee, _on_destroy);
   ecore_evas_callback_delete_request_set(d.ee, _on_destroy);
   ecore_evas_callback_resize_set(d.ee, _canvas_resize_cb);
   ecore_evas_show(d.ee);

   d.evas = ecore_evas_get(d.ee);

   d.bg = evas_object_rectangle_add(d.evas);
   evas_object_color_set(d.bg, 255, 255, 255, 255); /* white bg */
   evas_object_move(d.bg, 0, 0); /* at canvas' origin */
   evas_object_resize(d.bg, WIDTH, HEIGHT); /* covers full canvas */
   evas_object_show(d.bg);

   evas_object_focus_set(d.bg, EINA_TRUE);
   evas_object_event_callback_add(
     d.bg, EVAS_CALLBACK_KEY_DOWN, _on_keydown, NULL);

   d.text = evas_object_textblock_add(d.evas);
   /* Remember to free the style */
   _text_init(d.text, &st);
   evas_object_resize(d.text, WIDTH, HEIGHT);
   evas_object_move(d.text, 0, 0);
   evas_object_show(d.text);
   d.w = WIDTH;
   d.h = HEIGHT;

   /* init obstacles */
   rect = evas_object_rectangle_add(d.evas);
   d.t_data.obs[0] = rect;
   evas_object_color_set(rect, 255, 0, 0, 255);
   _obs_init(rect);
   evas_object_show(rect);
   img = evas_object_image_add(d.evas);
   d.t_data.obs[1] = img;
   evas_object_image_file_set(img, obs_img_path, NULL);
   printf("Loading image path: %s\n", obs_img_path);
   evas_object_image_filled_set(img, EINA_TRUE);
   _obs_init(img);
   evas_object_show(rect);
   evas_object_show(img);

   evas_object_textblock_obstacle_register(d.text, rect, EVAS_TEXTBLOCK_OBSTACLE_FLOAT);
   evas_object_textblock_obstacle_register(d.text, img, EVAS_TEXTBLOCK_OBSTACLE_FLOAT);

   fprintf(stdout, commands);
   ecore_main_loop_begin();

   evas_textblock_style_free(st);
   ecore_evas_free(d.ee);
   ecore_evas_shutdown();

   return 0;

error:
   fprintf(stderr, "you got to have at least one evas engine built and linked"
                   " up to ecore-evas for this example to run properly.\n");
   ecore_evas_shutdown();
   return -1;
}

