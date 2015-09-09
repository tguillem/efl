#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "ecore_wl2_private.h"

static void
_cb_global_add(void *data, struct wl_registry *registry, unsigned int id, const char *interface, unsigned int version)
{
   Ecore_Wl2_Display *ewd;

   ewd = data;

   if (!eina_hash_find(ewd->globals, id))
     {
        Ecore_Wl2_Global *global;

        global = calloc(1, sizeof(Ecore_Wl2_Global));
        if (!global) return;

        global->id = id;
        global->interface = eina_stringshare_add(interface);
        global->version = version;

        if (!eina_hash_direct_add(ewd->globals, global->id, global))
          {
             eina_stringshare_del(global->interface);
             free(global);
          }
     }
}

static void
_cb_global_remove(void *data, struct wl_registry *registry EINA_UNUSED, unsigned int id)
{
   Ecore_Wl2_Display *ewd;

   ewd = data;

   eina_hash_del_by_key(ewd->globals, id);
}

static const struct wl_registry_listener _registry_listener =
{
   _cb_global_add,
   _cb_global_remove
};

static Eina_Bool
_cb_create_data(void *data, Ecore_Fd_Handler *hdl)
{
   Ecore_Wl2_Display *ewd;
   struct wl_event_loop *loop;

   ewd = data;

   if (ecore_main_fd_handler_active_get(hdl, ECORE_FD_ERROR))
     {
        /* TODO: handle error case */
        return ECORE_CALLBACK_CANCEL;
     }

   loop = wl_display_get_event_loop(ewd->wl.display);
   wl_event_loop_dispatch(loop, -1);
   wl_display_flush_clients(ewd->wl.display);

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_cb_connect_data(void *data, Ecore_Fd_Handler *hdl)
{
   Ecore_Wl2_Display *ewd;
   int ret = 0;

   ewd = data;

   if (ecore_main_fd_handler_active_get(hdl, ECORE_FD_ERROR))
     {
        /* TODO: handle error case */
        return ECORE_CALLBACK_CANCEL;
     }

   /* if (ecore_main_fd_handler_active_get(hdl, ECORE_FD_READ)) */
   ret = wl_display_dispatch(ewd->wl.display);

   if ((ret < 0) && ((errno != EAGAIN) && (errno != EINVAL)))
     {
        /* TODO: handle error case */
        return ECORE_CALLBACK_CANCEL;
     }

   return ECORE_CALLBACK_RENEW;
}

static void
_cb_globals_hash_del(void *data)
{
   Ecore_Wl2_Global *global;

   global = data;

   eina_stringshare_del(global->name);

   free(global);
}

EAPI Ecore_Wl2_Display *
ecore_wl2_display_create(void)
{
   Ecore_Wl2_Display *ewd;
   struct wl_event_loop *loop;

   /* allocate space for display structure */
   ewd = calloc(1, sizeof(Ecore_Wl2_Display));
   if (!ewd) return NULL;

   /* try to create new wayland display */
   ewd->wl.display = wl_display_create();
   if (!ewd->wl.display)
     {
        ERR("Could not create wayland display: %m");
        goto create_err;
     }

   ewd->name = wl_display_add_socket_auto(ewd->wl.display);
   if (!ewd->name)
     {
        ERR("Failed to add display socket: %m");
        goto socket_err;
     }

   loop = wl_display_get_event_loop(ewd->wl.display);

   ewd->fd_hdl =
     ecore_main_fd_handler_add(wl_event_loop_get_fd(loop),
                               ECORE_FD_READ | ECORE_FD_ERROR,
                               _cb_create_data, ewd, NULL, NULL);

   return ewd;

socket_err:
   wl_display_destroy(ewd->wl.display);

create_err:
   free(ewd);
   return NULL;
}

EAPI Ecore_Wl2_Display *
ecore_wl2_display_connect(const char *name)
{
   Ecore_Wl2_Display *ewd;

   /* allocate space for display structure */
   ewd = calloc(1, sizeof(Ecore_Wl2_Display));
   if (!ewd) return NULL;

   ewd->globals = eina_hash_int32_new(_cb_globals_hash_del);

   /* try to connect to wayland display with this name */
   ewd->wl.display = wl_display_connect(name);
   if (!ewd->wl.display)
     {
        ERR("Could not connect to display %s: %m", name);
        goto err;
     }

   ewd->fd_hdl =
     ecore_main_fd_handler_add(wl_display_get_fd(ewd->wl.display),
                               ECORE_FD_READ | ECORE_FD_ERROR,
                               _cb_connect_data, ewd, NULL, NULL);

   wl_registry_add_listener(wl_display_get_registry(ewd->wl.display),
                            &_registry_listener, ewd);

   return ewd;

err:
   eina_hash_free(ewd->globals);
   free(ewd);
   return NULL;
}
