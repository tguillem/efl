#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "eldbus_model_object_private.h"
#include "eldbus_model_private.h"

#include <Eina.h>

#define MY_CLASS ELDBUS_MODEL_OBJECT_CLASS
#define MY_CLASS_NAME "Eldbus_Model_Object"

#define UNIQUE_NAME_PROPERTY "unique_name"

static void _eldbus_model_object_efl_model_base_properties_load(Eo *, Eldbus_Model_Object_Data *);
static void _eldbus_model_object_efl_model_base_children_load(Eo *, Eldbus_Model_Object_Data *);
static bool _eldbus_model_object_introspect(Eldbus_Model_Object_Data *, const char *, const char *);
static void _eldbus_model_object_introspect_cb(void *, const Eldbus_Message *, Eldbus_Pending *);
static void _eldbus_model_object_connect(Eldbus_Model_Object_Data *);
static void _eldbus_model_object_disconnect(Eldbus_Model_Object_Data *);
static void _eldbus_model_object_clear(Eldbus_Model_Object_Data *);
static void _eldbus_model_object_introspect_nodes(Eldbus_Model_Object_Data *, const char *, Eina_List *);
static char *_eldbus_model_object_concatenate_path(const char *, const char *);
static void _eldbus_model_object_create_children(Eldbus_Model_Object_Data *, Eldbus_Object *, Eina_List *);

static Eo_Base*
_eldbus_model_object_eo_base_constructor(Eo *obj, Eldbus_Model_Object_Data *pd)
{
   DBG("(%p)", obj);
   obj = eo_do_super_ret(obj, MY_CLASS, obj, eo_constructor());

   pd->obj = obj;
   pd->load.status = EFL_MODEL_LOAD_STATUS_UNLOADED;
   pd->connection = NULL;
   pd->object_list = NULL;
   pd->properties_array = NULL;
   pd->children_list = NULL;
   pd->type = ELDBUS_CONNECTION_TYPE_UNKNOWN;
   pd->address = NULL;
   pd->private = false;
   pd->bus = NULL;
   pd->path = NULL;
   pd->unique_name = NULL;
   pd->pending_list = NULL;
   pd->introspection = NULL;

   return obj;
}

static void
_eldbus_model_object_constructor(Eo *obj EINA_UNUSED,
                                 Eldbus_Model_Object_Data *pd,
                                 int type,
                                 const char* address,
                                 Eina_Bool private,
                                 const char* bus,
                                 const char* path)
{
   DBG("(%p)", obj);
   EINA_SAFETY_ON_NULL_RETURN(bus);
   EINA_SAFETY_ON_NULL_RETURN(path);

   pd->type = type;
   pd->address = eina_stringshare_add(address);
   pd->private = private;
   pd->bus = eina_stringshare_add(bus);
   pd->path = eina_stringshare_add(path);
}

static void
_eldbus_model_object_connection_constructor(Eo *obj EINA_UNUSED,
                                            Eldbus_Model_Object_Data *pd,
                                            Eldbus_Connection *connection,
                                            const char* bus,
                                            const char* path)
{
   DBG("(%p)", obj);
   EINA_SAFETY_ON_NULL_RETURN(connection);
   EINA_SAFETY_ON_NULL_RETURN(bus);
   EINA_SAFETY_ON_NULL_RETURN(path);

   pd->connection = eldbus_connection_ref(connection);
   pd->bus = eina_stringshare_add(bus);
   pd->path = eina_stringshare_add(path);
}

static void
_eldbus_model_object_eo_base_destructor(Eo *obj, Eldbus_Model_Object_Data *pd)
{
   DBG("(%p)", obj);

   eina_stringshare_del(pd->address);
   eina_stringshare_del(pd->bus);
   eina_stringshare_del(pd->path);

   _eldbus_model_object_clear(pd);

   eo_do_super(obj, MY_CLASS, eo_destructor());
}

static Efl_Model_Load_Status
_eldbus_model_object_efl_model_base_properties_get(Eo *obj EINA_UNUSED,
                                                Eldbus_Model_Object_Data *pd,
                                                Eina_Array * const* properties_array)
{
   DBG("(%p)", obj);
   EINA_SAFETY_ON_NULL_RETURN_VAL(pd, EFL_MODEL_LOAD_STATUS_ERROR);
   EINA_SAFETY_ON_NULL_RETURN_VAL(pd->obj, EFL_MODEL_LOAD_STATUS_ERROR);

   if (NULL == pd->properties_array)
     {
        pd->properties_array = eina_array_new(1);
        EINA_SAFETY_ON_NULL_RETURN_VAL(pd->properties_array, EFL_MODEL_LOAD_STATUS_ERROR);

        Eina_Bool ret = eina_array_push(pd->properties_array, UNIQUE_NAME_PROPERTY);
        EINA_SAFETY_ON_FALSE_RETURN_VAL(ret, EFL_MODEL_LOAD_STATUS_ERROR);
    }

   *(Eina_Array**)properties_array = pd->properties_array;
   return pd->load.status;
}

static void
_eldbus_model_object_efl_model_base_properties_load(Eo *obj, Eldbus_Model_Object_Data *pd)
{
   DBG("(%p)", obj);

   if (pd->load.status & EFL_MODEL_LOAD_STATUS_LOADED_PROPERTIES)
     return;

   if (!pd->connection)
     _eldbus_model_object_connect(pd);

   pd->unique_name = eina_value_new(EINA_VALUE_TYPE_STRING);
   EINA_SAFETY_ON_NULL_RETURN(pd->unique_name);

   const char *unique_name = eldbus_connection_unique_name_get(pd->connection);
   Eina_Bool ret = eina_value_set(pd->unique_name, unique_name);
   EINA_SAFETY_ON_FALSE_RETURN(ret);

   efl_model_load_set(pd->obj, &pd->load, EFL_MODEL_LOAD_STATUS_LOADED_PROPERTIES);
}

static Efl_Model_Load_Status
_eldbus_model_object_efl_model_base_property_set(Eo *obj EINA_UNUSED,
                                         Eldbus_Model_Object_Data *pd EINA_UNUSED,
                                         const char *property EINA_UNUSED,
                                         Eina_Value const* value EINA_UNUSED)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(property, EFL_MODEL_LOAD_STATUS_ERROR);
   DBG("(%p): property=%s", obj, property);
   return EFL_MODEL_LOAD_STATUS_ERROR;
}

static Efl_Model_Load_Status
_eldbus_model_object_efl_model_base_property_get(Eo *obj EINA_UNUSED,
                                         Eldbus_Model_Object_Data *pd,
                                         const char *property,
                                         Eina_Value const**value)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(property, EFL_MODEL_LOAD_STATUS_ERROR);
   EINA_SAFETY_ON_NULL_RETURN_VAL(value, EFL_MODEL_LOAD_STATUS_ERROR);
   DBG("(%p): property=%s", obj, property);

   if (!(pd->load.status & EFL_MODEL_LOAD_STATUS_LOADED_PROPERTIES))
     return EFL_MODEL_LOAD_STATUS_ERROR;

   if (strcmp(property, UNIQUE_NAME_PROPERTY) != 0)
     return EFL_MODEL_LOAD_STATUS_ERROR;

   *value = pd->unique_name;
   return pd->load.status;
}

static void
_eldbus_model_object_efl_model_base_load(Eo *obj EINA_UNUSED, Eldbus_Model_Object_Data *pd)
{
   DBG("(%p)", obj);

   if (!pd->connection)
     _eldbus_model_object_connect(pd);

   _eldbus_model_object_efl_model_base_properties_load(obj, pd);
   _eldbus_model_object_efl_model_base_children_load(obj, pd);
}

static Efl_Model_Load_Status
_eldbus_model_object_efl_model_base_load_status_get(Eo *obj EINA_UNUSED, Eldbus_Model_Object_Data *pd)
{
   DBG("(%p)", obj);
   return pd->load.status;
}

static void
_eldbus_model_object_efl_model_base_unload(Eo *obj EINA_UNUSED, Eldbus_Model_Object_Data *pd)
{
   DBG("(%p)", obj);

   _eldbus_model_object_clear(pd);

   efl_model_load_set(pd->obj, &pd->load, EFL_MODEL_LOAD_STATUS_UNLOADED);
}

Eo *
_eldbus_model_object_efl_model_base_child_add(Eo *obj EINA_UNUSED, Eldbus_Model_Object_Data *pd EINA_UNUSED)
{
   DBG("(%p)", obj);
   return NULL;
}

static Efl_Model_Load_Status
_eldbus_model_object_efl_model_base_child_del(Eo *obj EINA_UNUSED,
                                    Eldbus_Model_Object_Data *pd EINA_UNUSED,
                                    Eo *child EINA_UNUSED)
{
   DBG("(%p)", obj);
   return EFL_MODEL_LOAD_STATUS_ERROR;
}

static Efl_Model_Load_Status
_eldbus_model_object_efl_model_base_children_slice_get(Eo *obj EINA_UNUSED,
                                               Eldbus_Model_Object_Data *pd,
                                               unsigned start,
                                               unsigned count,
                                               Eina_Accessor **children_accessor)
{
   fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);
   DBG("(%p)", obj);
   fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);

   if (!(pd->load.status & EFL_MODEL_LOAD_STATUS_LOADED_CHILDREN))
     {
        WRN("(%p): Children not loaded", obj);
        *children_accessor = NULL;
        return pd->load.status;
     }
   else
     WRN("(%p): Children already loaded", obj);

   fprintf(stderr, "%p\n", pd->children_list);

   *children_accessor = efl_model_list_slice(pd->children_list, start, count);
   return pd->load.status;
}

static Efl_Model_Load_Status
_eldbus_model_object_efl_model_base_children_count_get(Eo *obj EINA_UNUSED,
                                               Eldbus_Model_Object_Data *pd,
                                               unsigned *children_count)
{
   DBG("(%p)", obj);
   *children_count = eina_list_count(pd->children_list);
   return pd->load.status;
}

static void
_eldbus_model_object_efl_model_base_children_load(Eo *obj, Eldbus_Model_Object_Data *pd)
{
   DBG("(%p)", obj);

   if (pd->load.status & EFL_MODEL_LOAD_STATUS_LOADED_CHILDREN)
     return;

   if (!pd->connection)
     _eldbus_model_object_connect(pd);

   if (!_eldbus_model_object_introspect(pd, pd->bus, pd->path))
     return;

   efl_model_load_set(pd->obj, &pd->load, EFL_MODEL_LOAD_STATUS_LOADING_CHILDREN);
}

static const char *
_eldbus_model_object_address_get(Eo *obj EINA_UNUSED, Eldbus_Model_Object_Data *pd)
{
   return pd->address;
}

static void
_eldbus_model_object_address_set(Eo *obj EINA_UNUSED, Eldbus_Model_Object_Data *pd, const char *value)
{
   eina_stringshare_del(pd->address);
   pd->address = eina_stringshare_add(value);
}

static Eina_Bool
_eldbus_model_object_private_get(Eo *obj EINA_UNUSED, Eldbus_Model_Object_Data *pd)
{
   return pd->private;
}

static void
_eldbus_model_object_private_set(Eo *obj EINA_UNUSED, Eldbus_Model_Object_Data *pd, Eina_Bool value)
{
   pd->private = value;
}

static int
_eldbus_model_object_type_get(Eo *obj EINA_UNUSED, Eldbus_Model_Object_Data *pd)
{
   return pd->type;
}

static void
_eldbus_model_object_type_set(Eo *obj EINA_UNUSED, Eldbus_Model_Object_Data *pd, int value)
{
   pd->type = value;
}

static const char *
_eldbus_model_object_bus_get(Eo *obj EINA_UNUSED, Eldbus_Model_Object_Data *pd)
{
   return pd->bus;
}

static void
_eldbus_model_object_bus_set(Eo *obj EINA_UNUSED, Eldbus_Model_Object_Data *pd, const char *value)
{
   eina_stringshare_del(pd->bus);
   pd->bus = eina_stringshare_add(value);
}

static const char *
_eldbus_model_object_path_get(Eo *obj EINA_UNUSED, Eldbus_Model_Object_Data *pd)
{
   return pd->path;
}

static void
_eldbus_model_object_path_set(Eo *obj EINA_UNUSED, Eldbus_Model_Object_Data *pd, const char *value)
{
   eina_stringshare_del(pd->path);
   pd->path = eina_stringshare_add(value);
}

static void
_eldbus_model_object_connect(Eldbus_Model_Object_Data *pd)
{
   EINA_SAFETY_ON_NULL_RETURN(pd);

   if (ELDBUS_CONNECTION_TYPE_ADDRESS == pd->type)
     {
        if (pd->private)
          pd->connection = eldbus_address_connection_get(pd->address);
        else
          pd->connection = eldbus_private_address_connection_get(pd->address);
     }
   else
     {
        if (pd->private)
          pd->connection = eldbus_private_connection_get(pd->type);
        else
          pd->connection = eldbus_connection_get(pd->type);
     }

   // TODO: Register for disconnection event

   EINA_SAFETY_ON_FALSE_RETURN(NULL != pd->connection);
}

static void
_eldbus_model_object_disconnect(Eldbus_Model_Object_Data *pd)
{
   EINA_SAFETY_ON_NULL_RETURN(pd);
   eldbus_connection_unref(pd->connection);
   pd->connection = NULL;
}

static void
_eldbus_model_object_clear(Eldbus_Model_Object_Data *pd)
{
   EINA_SAFETY_ON_NULL_RETURN(pd);
   if (!pd->connection)
     return;

   eina_value_free(pd->unique_name);
   pd->unique_name = NULL;

   Eo *child;
   EINA_LIST_FREE(pd->children_list, child)
     eo_unref(child);

   Eldbus_Pending *pending;
   EINA_LIST_FREE(pd->pending_list, pending)
     eldbus_pending_cancel(pending);

   if (pd->properties_array)
     {
        eina_array_free(pd->properties_array);
        pd->properties_array = NULL;
     }

   Eldbus_Object *object;
   EINA_LIST_FREE(pd->object_list, object)
     eldbus_object_unref(object);

   if (pd->introspection)
     {
        eldbus_introspection_node_free(pd->introspection);
        pd->introspection = NULL;
     }

   _eldbus_model_object_disconnect(pd);
}


static bool
_eldbus_model_object_introspect(Eldbus_Model_Object_Data *pd,
                                const char *bus,
                                const char *path)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(bus, false);
   EINA_SAFETY_ON_NULL_RETURN_VAL(path, false);

   DBG("(%p) Introspecting: bus = %s, path = %s", pd->obj, bus, path);

   Eldbus_Object *object = eldbus_object_get(pd->connection, bus, path);
   if (!object)
     {
        ERR("(%p): Cannot get object: bus=%s, path=%s", pd->obj, bus, path);
        return false;
     }
   pd->object_list = eina_list_append(pd->object_list, object);

   // TODO: Register for interface added/removed event

   Eldbus_Pending *pending = eldbus_object_introspect(object, &_eldbus_model_object_introspect_cb, pd);
   eldbus_pending_data_set(pending, "object", object);
   pd->pending_list = eina_list_append(pd->pending_list, pending);
   return true;
}

static void
_eldbus_model_object_introspect_cb(void *data,
                                   const Eldbus_Message *msg,
                                   Eldbus_Pending *pending)
{
   Eldbus_Model_Object_Data *pd = (Eldbus_Model_Object_Data*)data;
   DBG("(%p)", pd->obj);

   pd->pending_list = eina_list_remove(pd->pending_list, pending);
   Eldbus_Object *object = eldbus_pending_data_get(pending, "object");

   const char *error_name, *error_text;
   if (eldbus_message_error_get(msg, &error_name, &error_text))
     {
        ERR("%s: %s", error_name, error_text);
        efl_model_error_notify(pd->obj);
        return;
     }

   const char *xml = NULL;
   if (!eldbus_message_arguments_get(msg, "s", &xml))
     {
        ERR("Error getting arguments.");
        return;
     }
   EINA_SAFETY_ON_NULL_RETURN(xml);

   const char *current_path = eldbus_object_path_get(object);
   EINA_SAFETY_ON_NULL_RETURN(current_path);

   DBG("(%p): introspect of bus = %s, path = %s =>\n%s", pd->obj, pd->bus, current_path, xml);

   pd->introspection = eldbus_introspection_parse(xml);
   EINA_SAFETY_ON_NULL_RETURN(pd->introspection);

   _eldbus_model_object_introspect_nodes(pd, current_path, pd->introspection->nodes);
   _eldbus_model_object_create_children(pd, object, pd->introspection->interfaces);

   if (eina_list_count(pd->pending_list) == 0)
     {
        efl_model_load_set(pd->obj, &pd->load, EFL_MODEL_LOAD_STATUS_LOADED_CHILDREN);

        unsigned int count = eina_list_count(pd->children_list);
        if (count)
          eo_do(pd->obj, eo_event_callback_call(EFL_MODEL_BASE_EVENT_CHILDREN_COUNT_CHANGED, &count));
     }
}

static void
_eldbus_model_object_introspect_nodes(Eldbus_Model_Object_Data *pd, const char *current_path, Eina_List *nodes)
{
   EINA_SAFETY_ON_NULL_RETURN(pd);
   EINA_SAFETY_ON_NULL_RETURN(current_path);

   Eina_List *it;
   Eldbus_Introspection_Node *node;
   EINA_LIST_FOREACH(nodes, it, node)
     {
        const char *relative_path = node->name;
        EINA_SAFETY_ON_NULL_RETURN(relative_path);

        char *absolute_path = _eldbus_model_object_concatenate_path(current_path, relative_path);
        EINA_SAFETY_ON_NULL_RETURN(absolute_path);

        _eldbus_model_object_introspect(pd, pd->bus, absolute_path);

        free(absolute_path);
     }
}

static char *
_eldbus_model_object_concatenate_path(const char *root_path, const char *relative_path)
{
   Eina_Strbuf *buffer = eina_strbuf_new();
   EINA_SAFETY_ON_NULL_RETURN_VAL(buffer, NULL);

   Eina_Bool ret = eina_strbuf_append(buffer, root_path);
   if (strcmp(root_path, "/") != 0)
     ret = ret && eina_strbuf_append_char(buffer, '/');
   ret = ret && eina_strbuf_append(buffer, relative_path);
   EINA_SAFETY_ON_FALSE_GOTO(ret, free_buffer);

   char *absolute_path = eina_strbuf_string_steal(buffer);

   eina_strbuf_free(buffer);

   return absolute_path;

free_buffer:
   eina_strbuf_free(buffer);
   return NULL;
}

static void
_eldbus_model_object_create_children(Eldbus_Model_Object_Data *pd, Eldbus_Object *object, Eina_List *interfaces)
{
   const char *current_path = eldbus_object_path_get(object);
   EINA_SAFETY_ON_NULL_RETURN(current_path);

   Eina_List *it;
   Eldbus_Introspection_Interface *interface;
   EINA_LIST_FOREACH(interfaces, it, interface)
     {
        WRN("(%p) Creating child: bus = %s, path = %s, interface = %s", pd->obj, pd->bus, current_path, interface->name);

        // TODO: increment reference to keep 'interface' in memory
        Eo *child = eo_add_ref(ELDBUS_MODEL_PROXY_CLASS, NULL,
                               eldbus_model_proxy_constructor(object, interface));

        pd->children_list = eina_list_append(pd->children_list, child);
     }
}

#include "eldbus_model_object.eo.c"
