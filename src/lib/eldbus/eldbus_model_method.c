#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "eldbus_model_arguments_private.h"
#include "eldbus_model_method_private.h"
#include "eldbus_model_private.h"
#include "eldbus_proxy.h"

#include <Eina.h>

#include <stdbool.h>

#define MY_CLASS ELDBUS_MODEL_METHOD_CLASS
#define MY_CLASS_NAME "Eldbus_Model_Method"

static void _eldbus_model_method_call_cb(void *, const Eldbus_Message *, Eldbus_Pending *);

static Eo_Base*
_eldbus_model_method_eo_base_constructor(Eo *obj, Eldbus_Model_Method_Data *pd)
{
   DBG("(%p)", obj);
   obj = eo_do_super_ret(obj, MY_CLASS, obj, eo_constructor());

   pd->obj = obj;
   pd->method = NULL;
   return obj;
}

static void
_eldbus_model_method_constructor(Eo *obj EINA_UNUSED,
                                 Eldbus_Model_Method_Data *pd,
                                 Eldbus_Proxy *proxy,
                                 const Eldbus_Introspection_Method *method)
{
   DBG("(%p)", obj);
   EINA_SAFETY_ON_NULL_RETURN(proxy);
   EINA_SAFETY_ON_NULL_RETURN(method);
   eo_do_super(obj, MY_CLASS, eldbus_model_arguments_constructor(proxy, method->name, method->arguments));

   pd->method = method;
}

static Efl_Model_Load_Status
_eldbus_model_method_call(Eo *obj EINA_UNUSED, Eldbus_Model_Method_Data *pd EINA_UNUSED)
{
   DBG("(%p)", obj);
   Eldbus_Model_Arguments_Data *data = eo_data_scope_get(obj, ELDBUS_MODEL_ARGUMENTS_CLASS);
   EINA_SAFETY_ON_NULL_RETURN_VAL(data, EFL_MODEL_LOAD_STATUS_ERROR);

   Eldbus_Message *msg = eldbus_proxy_method_call_new(data->proxy, data->name);
   Eldbus_Message_Iter *iter = eldbus_message_iter_get(msg);

   unsigned int i = 0;
   const Eina_List *it;
   const Eldbus_Introspection_Argument *argument;
   EINA_LIST_FOREACH(data->arguments, it, argument)
     {
        if (ELDBUS_INTROSPECTION_ARGUMENT_DIRECTION_IN != argument->direction)
          continue;

        const Eina_Stringshare *name = eina_array_data_get(data->properties_array, i);
        EINA_SAFETY_ON_NULL_GOTO(name, on_error);

        const Eina_Value *value = eina_hash_find(data->properties_hash, name);
        EINA_SAFETY_ON_NULL_GOTO(value, on_error);

        Eina_Bool ret;
        const char *signature = argument->type;
        if (dbus_type_is_basic(signature[0]))
          ret = _message_iter_from_eina_value(signature, iter, value);
        else
          ret = _message_iter_from_eina_value_struct(signature, iter, value);

        EINA_SAFETY_ON_FALSE_GOTO(ret, on_error);

        ++i;
     }

   Eldbus_Pending *pending = eldbus_proxy_send(data->proxy, msg, _eldbus_model_method_call_cb, pd, -1);
   data->pending_list = eina_list_append(data->pending_list, pending);

   return data->load.status;

on_error:
   eldbus_message_unref(msg);
   return EFL_MODEL_LOAD_STATUS_ERROR;
}

static void
_eldbus_model_method_call_cb(void *data, const Eldbus_Message *msg, Eldbus_Pending *pending)
{
   Eldbus_Model_Method_Data *pd = (Eldbus_Model_Method_Data*)data;
   DBG("(%p)", pd->obj);

   Eldbus_Model_Arguments_Data *args_data = eo_data_scope_get(pd->obj, ELDBUS_MODEL_ARGUMENTS_CLASS);
   EINA_SAFETY_ON_NULL_RETURN(args_data);

   if (eldbus_model_arguments_process_arguments(args_data, msg, pending))
     eo_do(pd->obj, eo_event_callback_call(ELDBUS_MODEL_METHOD_EVENT_SUCCESSFUL_CALL, NULL));
}

#include "eldbus_model_method.eo.c"
