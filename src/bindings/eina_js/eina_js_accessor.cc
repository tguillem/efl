#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <Eina.hh>

#include EINA_STRINGIZE(V8_INCLUDE_HEADER)
#include <eina_js_accessor.hh>
#include <eina_js_compatibility.hh>

#include <iostream>

namespace efl { namespace eina { namespace js {

EAPI void register_destroy_accessor(v8::Isolate *isolate,
                                    v8::Handle<v8::Object> global,
                                    v8::Handle<v8::String> name)
{
  std::cerr << "register_destroy_accessor" << std::endl;
    typedef void (*deleter_t)(void*);

    auto f = [](compatibility_callback_info_type info) -> compatibility_return_type
      {
        if (info.Length() != 1 || !info[0]->IsObject())
          return compatibility_return();

        v8::Handle<v8::Object> o = info[0]->ToObject();

        deleter_t deleter = compatibility_get_pointer_internal_field<deleter_t>(o, 1);
        deleter(compatibility_get_pointer_internal_field<>(o, 0));
        return compatibility_return();
      };

    global->Set(name, compatibility_new<v8::FunctionTemplate>(isolate, f)->GetFunction());
}

} } } // namespace efl { namespace js {