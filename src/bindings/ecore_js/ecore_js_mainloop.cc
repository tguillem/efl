#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ecore_js_mainloop.hh>
#include <Ecore.h>

namespace efl { namespace ecore { namespace js {

struct persistent_with_isolate_t
{
    template<class S>
    persistent_with_isolate_t(v8::Isolate *isolate, v8::Handle<S> that)
        : isolate(isolate)
        , persistent(isolate, that)
    {}

    v8::Isolate *isolate;
    v8::Persistent<v8::Value> persistent;
};

EAPI
void register_callback_cancel(v8::Isolate *isolate,
                              v8::Handle<v8::Object> global,
                              v8::Handle<v8::String> name)
{
    using v8::Boolean;
    global->Set(name, compatibility_new<Boolean>(isolate,
                                                 bool{ECORE_CALLBACK_CANCEL}));
}

EAPI
void register_callback_renew(v8::Isolate *isolate,
                             v8::Handle<v8::Object> global,
                             v8::Handle<v8::String> name)
{
    using v8::Boolean;
    global->Set(name, compatibility_new<Boolean>(isolate,
                                                 bool{ECORE_CALLBACK_RENEW}));
}

EAPI
void register_callback_pass_on(v8::Isolate *isolate,
                               v8::Handle<v8::Object> global,
                               v8::Handle<v8::String> name)
{
    using v8::Boolean;
    global->Set(name, compatibility_new<Boolean>(isolate,
                                                 bool{ECORE_CALLBACK_PASS_ON}));
}

EAPI
void register_callback_done(v8::Isolate *isolate, v8::Handle<v8::Object> global,
                            v8::Handle<v8::String> name)
{
    using v8::Boolean;
    global->Set(name, compatibility_new<Boolean>(isolate,
                                                 bool{ECORE_CALLBACK_DONE}));
}

EAPI
void register_mainloop_iterate(v8::Isolate *isolate,
                               v8::Handle<v8::Object> global,
                               v8::Handle<v8::String> name)
{
    using v8::Integer;
    using v8::FunctionTemplate;

    auto f = [](compatibility_callback_info_type args)
        -> compatibility_return_type {
        if (args.Length() != 0)
            return compatibility_return();

        ecore_main_loop_iterate();
        return compatibility_return();
    };

    global->Set(name,
                compatibility_new<FunctionTemplate>(isolate, f)->GetFunction());
}

EAPI
void register_mainloop_iterate_may_block(v8::Isolate *isolate,
                                         v8::Handle<v8::Object> global,
                                         v8::Handle<v8::String> name)
{
    using v8::Integer;
    using v8::FunctionTemplate;

    auto f = [](compatibility_callback_info_type args)
        -> compatibility_return_type {
        if (args.Length() != 1 || !args[0]->IsNumber())
            return compatibility_return();

        auto ret = ecore_main_loop_iterate_may_block(args[0]->NumberValue());
        return compatibility_return(compatibility_new<Integer>
                                    (args.GetIsolate(), ret), args);
    };

    global->Set(name,
                compatibility_new<FunctionTemplate>(isolate, f)->GetFunction());
}

EAPI
void register_mainloop_begin(v8::Isolate *isolate,
                             v8::Handle<v8::Object> global,
                             v8::Handle<v8::String> name)
{
    using v8::Integer;
    using v8::FunctionTemplate;

    auto f = [](compatibility_callback_info_type args)
        -> compatibility_return_type {
        if (args.Length() != 0)
            return compatibility_return();

        ecore_main_loop_begin();
        return compatibility_return();
    };

    global->Set(name,
                compatibility_new<FunctionTemplate>(isolate, f)->GetFunction());
}

EAPI
void register_mainloop_quit(v8::Isolate *isolate, v8::Handle<v8::Object> global,
                            v8::Handle<v8::String> name)
{
    using v8::Integer;
    using v8::FunctionTemplate;

    auto f = [](compatibility_callback_info_type args)
        -> compatibility_return_type {
        if (args.Length() != 0)
            return compatibility_return();

        ecore_main_loop_quit();
        return compatibility_return();
    };

    global->Set(name,
                compatibility_new<FunctionTemplate>(isolate, f)->GetFunction());
}

EAPI
void register_mainloop_animator_ticked_get(v8::Isolate *isolate,
                                           v8::Handle<v8::Object> global,
                                           v8::Handle<v8::String> name)
{
    using v8::Boolean;
    using v8::FunctionTemplate;

    auto f = [](compatibility_callback_info_type args)
        -> compatibility_return_type {
        if (args.Length() != 0)
            return compatibility_return();

        auto ret = ecore_main_loop_animator_ticked_get();
        return compatibility_return(compatibility_new<Boolean>
                                    (args.GetIsolate(), ret), args);
    };

    global->Set(name,
                compatibility_new<FunctionTemplate>(isolate, f)->GetFunction());
}

EAPI
void register_mainloop_nested_get(v8::Isolate *isolate,
                                  v8::Handle<v8::Object> global,
                                  v8::Handle<v8::String> name)
{
    using v8::Boolean;
    using v8::FunctionTemplate;

    auto f = [](compatibility_callback_info_type args)
        -> compatibility_return_type {
        if (args.Length() != 0)
            return compatibility_return();

        auto ret = ecore_main_loop_nested_get();
        return compatibility_return(compatibility_new<Boolean>
                                    (args.GetIsolate(), ret), args);
    };

    global->Set(name,
                compatibility_new<FunctionTemplate>(isolate, f)->GetFunction());
}

EAPI
void register_mainloop_thread_safe_call_async(v8::Isolate *isolate,
                                              v8::Handle<v8::Object> global,
                                              v8::Handle<v8::String> name)
{
    using v8::Local;
    using v8::Value;
    using v8::Undefined;
    using v8::Function;
    using v8::FunctionTemplate;

    auto f = [](compatibility_callback_info_type args)
        -> compatibility_return_type {
        if (args.Length() != 1 || !args[0]->IsFunction())
            return compatibility_return();

        persistent_with_isolate_t *f
            = new persistent_with_isolate_t(args.GetIsolate(), args[0]);
        ecore_main_loop_thread_safe_call_async([](void *data) {
                persistent_with_isolate_t *persistent
                    = reinterpret_cast<persistent_with_isolate_t *>(data);
                auto value = Local<Value>::New(persistent->isolate,
                                               persistent->persistent);
                auto closure = Function::Cast(*value);

                closure->Call(Undefined(persistent->isolate), 0, NULL);

                delete persistent;
            }, f);
        return compatibility_return();
    };

    global->Set(name,
                compatibility_new<FunctionTemplate>(isolate, f)->GetFunction());
}

EAPI
void register_mainloop_thread_safe_call_sync(v8::Isolate *isolate,
                                             v8::Handle<v8::Object> global,
                                             v8::Handle<v8::String> name)
{
    using v8::Local;
    using v8::Value;
    using v8::Undefined;
    using v8::Function;
    using v8::FunctionTemplate;

    auto f = [](compatibility_callback_info_type args)
        -> compatibility_return_type {
        if (args.Length() != 1 || !args[0]->IsFunction())
            return compatibility_return();

        auto f = new persistent_with_isolate_t(args.GetIsolate(), args[0]);
        void *data = ecore_main_loop_thread_safe_call_sync([](void *data) {
                persistent_with_isolate_t *persistent
                    = reinterpret_cast<persistent_with_isolate_t*>(data);
                auto value = Local<Value>::New(persistent->isolate,
                                                       persistent->persistent);
                auto closure = Function::Cast(*value);
                auto res = closure->Call(Undefined(persistent->isolate), 0,
                                         NULL);
                void *ret = new persistent_with_isolate_t(persistent->isolate,
                                                          res);

                delete persistent;

                return ret;
            }, f);

        auto ret = reinterpret_cast<persistent_with_isolate_t*>(data);
        auto value = Local<Value>::New(ret->isolate, ret->persistent);

        delete ret;

        return compatibility_return(value, args);
    };

    global->Set(name,
                compatibility_new<FunctionTemplate>(isolate, f)->GetFunction());
}

} } } // namespace efl { namespace js {