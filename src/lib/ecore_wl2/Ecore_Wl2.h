#ifndef _ECORE_WL2_H_
# define _ECORE_WL2_H_

# include <Eina.h>
# include <Ecore.h>
# include <wayland-client.h>
# include <wayland-cursor.h>
# include <xkbcommon/xkbcommon.h>

# define WL_HIDE_DEPRECATED
# include <wayland-server.h>

# ifdef EAPI
#  undef EAPI
# endif

# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif

/* # ifdef __cplusplus */
/* extern "C" { */
/* # endif */

# ifndef _ECORE_WAYLAND_WINDOW_PREDEF
typedef struct _Ecore_Wl_Window Ecore_Wl_Window;
# endif

typedef struct _Ecore_Wl2_Display Ecore_Wl2_Display;

typedef struct _Ecore_Wl2_Global
{
   const char *name;
   unsigned int id, version;
} Ecore_Wl2_Global;

EAPI extern int ECORE_WL2_EVENT_GLOBAL_ADDED;
EAPI extern int ECORE_WL2_EVENT_GLOBAL_REMOVED;

/**
 * @file
 * @brief Ecore functions for dealing with the Wayland display protocol
 *
 * @defgroup Ecore_Wl2_Group Ecore_Wl2 - Wayland integration
 * @ingroup Ecore
 *
 * Ecore_Wl2 provides a wrapper and convenience functions for using the
 * Wayland protocol in implementing a window system. Function groups for
 * this part of the library include the following:
 *
 * @li @ref Ecore_Wl2_Init_Group
 * @li @ref Ecore_Wl2_Display_Group
 */

/**
 * @defgroup Ecore_Wl2_Init_Group Wayland Library Init and Shutdown Functions
 * @ingroup Ecore_Wl2_Group
 *
 * Functions that start and shutdown the Ecore Wl2 Library.
 */

/**
 * Initialize the Ecore_Wl2 library
 *
 * @return  The number of times the library has been initialized without being
 *          shutdown. 0 is returned if an error occurs.
 *
 * @ingroup Ecore_Wl2_Init_Group
 */
EAPI int ecore_wl2_init(void);

/**
 * Shutdown the Ecore_Wl2 Library
 *
 * @return  The number of times the library has been initialized without
 *          being shutdown.
 *
 * @ingroup Ecore_Wl2_Init_Group
 */
EAPI int ecore_wl2_shutdown(void);

/**
 * @defgroup Ecore_Wl2_Display_Group Wayland Library Display Functions
 * @ingroup Ecore_Wl2_Group
 *
 * Functions that deal with creating, connecting, or interacting with
 * Wayland displays
 */

/**
 * Create a new Wayland display
 *
 * @brief This function is typically used to create a new display for
 * use with compositors, or to create a new display for use in nested
 * compositors.
 *
 * @return The newly created Ecore_Wl2_Display
 *
 * @ingroup Ecore_Wl2_Display_Group
 */
EAPI Ecore_Wl2_Display *ecore_wl2_display_create(void);

/**
 * Connect to an existing Wayland display
 *
 * @brief This function is typically used by clients to connect to an
 * existing Wayland display.
 *
 * @param name The display target name to connect to. If @c NULL, the default
 *             display is assumed.
 *
 * @return The Ecore_Wl2_Display which was connected to
 *
 * @ingroup Ecore_Wl2_Display_Group
 */
EAPI Ecore_Wl2_Display *ecore_wl2_display_connect(const char *name);

/* # ifdef __cplusplus */
/* } */
/* # endif */

# undef EAPI
# define EAPI

#endif
