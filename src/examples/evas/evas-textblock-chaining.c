/**
 * Simple Evas example illustrating textblock chaining
 *
 * @verbatim
 * gcc -o evas-textblock-chaining evas-textblock-chaining.c `pkg-config --libs --cflags evas ecore ecore-evas`
 * @endverbatim
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define PACKAGE_EXAMPLES_DIR "."
#endif

#define EFL_EO_API_SUPPORT
#define EFL_BETA_API_SUPPORT

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <stdio.h>
#include <errno.h>

#define WIDTH  (640)
#define HEIGHT (480)

#define BLACK {0, 0, 0, 255}
#define WHITE {255, 255, 255, 255}

#define POINTER_CYCLE(_ptr, _array)                             \
  do                                                            \
    {                                                           \
       if ((unsigned int)(((unsigned char *)(_ptr)) - ((unsigned char *)(_array))) >= \
           sizeof(_array))                                      \
         _ptr = _array;                                         \
    }                                                           \
  while(0)

#define POINTER_ADD(_type, _arr_size, _name) \
   _type _name[_arr_size]; \
   _type* _name##_ptr

#define PTRINIT(_name) d.t_data._name##_ptr = d.t_data._name
#define PTRINC(_name) (d.t_data._name##_ptr)++
#define PTRDATA(_name) *d.t_data._name##_ptr
#define PTRCYCLE(_name) POINTER_CYCLE(d.t_data._name##_ptr, d.t_data._name)

static const char *commands =
  "commands are:\n"
  "\ta - attach extension object\n"
  "\tq - change main textblock's size\n"
  "\tw - change extension's size\n"
  "\th - print help\n";

struct color_tuple
{
   int r, g, b, a;
};

struct text_preset_data
{

   POINTER_ADD(const char *, 3, text);
   POINTER_ADD(Evas_Coord, 3, size);

};

struct test_data
{
   Ecore_Evas             *ee;
   Evas                   *evas;
   struct text_preset_data t_data;
   Evas_Object            *text, *ext, *bg;
   Evas_Textblock_Style   *style;
   Eina_Bool               ext_attached : 1;
};

static struct test_data d = {0};

static void
_on_destroy(Ecore_Evas *ee EINA_UNUSED)
{
   ecore_main_loop_quit();
}

/* here just to keep our example's window size and background image's
 * size in synchrony */
static void
_canvas_resize_cb(Ecore_Evas *ee)
{
   int w, h;

   ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
   evas_object_resize(d.bg, w, h);
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

   if (strncmp(ev->key, "t", 1) == 0) /* text */
     {
        PTRINC(text);
        PTRCYCLE(text);
        evas_object_textblock_text_markup_set(d.text, PTRDATA(text));
        return;
     }
   if (strncmp(ev->key, "s", 1) == 0) /* size */
     {
        PTRINC(size);
        PTRCYCLE(size);
        printf("Changing size to (%d, %d)\n", PTRDATA(size), PTRDATA(size));
        evas_object_resize(d.text, PTRDATA(size), PTRDATA(size));
        return;
     }

   if (strncmp(ev->key, "a", 1) == 0) /* size */
     {
        Eina_Bool attached = d.ext_attached;
        d.ext_attached = !d.ext_attached;
        if (attached)
          {
             eo_do(d.text, evas_obj_textblock_extension_remove(d.ext));
          }
        else
          {
             eo_do(d.text, evas_obj_textblock_extension_append(d.ext));
          }
        evas_object_resize(d.text, PTRDATA(size), PTRDATA(size));
        return;
     }
}

int
main(void)
{
   int size;
   const char *font;

   if (!ecore_evas_init())
     return EXIT_FAILURE;

   const char *text1 = "<wrap=word>This is a small paragraph example";
   const char *text2 =
      "<wrap=word>This is a bigger paragraph example,"
      " which is hopefully flow into the extension evas object"
      " so we can actually see how the textblock chaining mechanism"
      " works on real examples";
   char *stbuf =
      "DEFAULT='font=Sans font_size=12 color=#000 wrap=word"
      "text_class=entry'";

   /* init values one is going to cycle through while running this
    * example */
   struct text_preset_data init_data =
   {
      .size = {100, 200, 300},
      .text = {text1, text2, text2},
   };

   d.t_data = init_data;
   /* init cyclic pointers */
   PTRINIT(text);
   PTRINIT(size);

   /* this will give you a window with an Evas canvas under the first
    * engine available */
   d.ee = ecore_evas_new(NULL, 10, 10, WIDTH, HEIGHT, NULL);
   if (!d.ee)
     goto error;

   ecore_evas_callback_destroy_set(d.ee, _on_destroy);
   ecore_evas_callback_resize_set(d.ee, _canvas_resize_cb);
   ecore_evas_show(d.ee);

   /* the canvas pointer, de facto */
   d.evas = ecore_evas_get(d.ee);

   d.bg = evas_object_rectangle_add(d.evas);
   evas_object_color_set(d.bg, 255, 255, 255, 255); /* white bg */
   evas_object_move(d.bg, 0, 0); /* at canvas' origin */
   evas_object_resize(d.bg, WIDTH, HEIGHT); /* covers full canvas */
   evas_object_show(d.bg);

   evas_object_focus_set(d.bg, EINA_TRUE);
   evas_object_event_callback_add(
     d.bg, EVAS_CALLBACK_KEY_DOWN, _on_keydown, NULL);

   d.text = eo_add(EVAS_TEXTBLOCK_CLASS, d.evas);
   d.ext = eo_add(EVAS_TEXTBLOCK_EXTENSION_CLASS, d.evas);

   d.style = evas_textblock_style_new();
   evas_textblock_style_set(d.style, stbuf);
   evas_object_textblock_style_set(d.text, d.style);

   evas_object_textblock_text_markup_set(d.text, PTRDATA(text));

   evas_object_resize(d.text, PTRDATA(size), PTRDATA(size));
   evas_object_resize(d.ext, PTRDATA(size), PTRDATA(size));
   evas_object_move(d.text, 0, 0);
   evas_object_move(d.ext, PTRDATA(size) + 20, 0);
   evas_object_show(d.text);
   evas_object_show(d.ext);

   fprintf(stdout, commands);
   ecore_main_loop_begin();

   ecore_evas_free(d.ee);
   ecore_evas_shutdown();
   return 0;

error:
   fprintf(stderr, "you got to have at least one evas engine built and linked"
                   " up to ecore-evas for this example to run properly.\n");
   ecore_evas_shutdown();
   return -1;
}

