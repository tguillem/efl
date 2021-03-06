
#ifndef EOLIAN_CXX_TYPE_LOOKUP_HH
#define EOLIAN_CXX_TYPE_LOOKUP_HH

#include <algorithm>
#include <string>
#include <vector>
#include <cctype>
#include <iterator>
#include <stdexcept>
#include <cassert>
#include <cstddef>

#include <Eolian.h>
#include <eolian_database.h>

#include <Eina.hh>

#include "eo_types.hh"
#include "safe_strings.hh"

namespace eolian_cxx {

typedef std::vector<efl::eolian::eolian_type> lookup_table_type;
extern const lookup_table_type type_lookup_table;

inline std::string
class_format_cxx(std::string const& fullname)
{
   std::string s = fullname;
   auto found = s.find(".");
   while (found != std::string::npos)
     {
        s.replace(found, 1, "::");
        found = s.find(".");
     }
   return s;
}

inline bool
type_is_complex(Eolian_Type const& type)
{
   return ::eolian_type_type_get(&type) == EOLIAN_TYPE_COMPLEX;
}

inline efl::eolian::eolian_type
type_from_eolian(Eolian_Type const& type)
{
   efl::eolian::eolian_type x;

   Eolian_Type_Type tpt = ::eolian_type_type_get(&type);
   if (tpt == EOLIAN_TYPE_POINTER || tpt == EOLIAN_TYPE_ALIAS || tpt == EOLIAN_TYPE_REGULAR)
     {
        Eolian_Type const* base_type = ::eolian_type_base_type_get(&type);
        if (base_type && ::eolian_type_type_get(base_type) == EOLIAN_TYPE_CLASS)
          {
             Eolian_Class const* klass = ::eolian_type_class_get(base_type);
             if (klass)
               {
                  x.category = efl::eolian::eolian_type::simple_;
                  x.is_class = true;
                  x.binding_requires_optional = false;
                  x.binding = "::" + class_format_cxx(safe_lower(safe_str(::eolian_class_full_name_get(klass))));

                  Eina_Stringshare* klass_file = ::eolian_class_file_get(klass);
                  if (klass_file)
                    x.includes = {safe_str(klass_file) + ".hh"};
               }
          }
     }

   x.native = normalize_spaces(safe_str(::eolian_type_c_type_get(&type)));
   x.is_own = ::eolian_type_is_own(&type);
   x.is_const = ::eolian_type_is_const(&type);
   return x;
}

template <typename Iterator>
inline const efl::eolian::eolian_type&
type_find(Iterator first, Iterator last, efl::eolian::eolian_type const& type)
{
   auto res = std::find_if
     (first, last,
      [&type] (efl::eolian::eolian_type const& x)
      {
        return (x.native == type.native && x.is_own == type.is_own);
      });
   return (res != last) ? *res : type;
}

inline efl::eolian::eolian_type_instance
type_lookup(const Eolian_Type* type,
            lookup_table_type const& lut = type_lookup_table)
{
   if (type == NULL) return { efl::eolian::void_type }; // XXX shouldn't
   // assert(type != NULL);

   std::vector<Eolian_Type const*> types;
   types.push_back(type);

   if (::eolian_type_type_get(type) == EOLIAN_TYPE_POINTER && type_is_complex(*eolian_type_base_type_get(type)))
     {
        efl::eina::iterator<Eolian_Type const> end;
        efl::eina::iterator<Eolian_Type const> it
          (::eolian_type_subtypes_get(eolian_type_base_type_get(type)));
        while(it != end)
          {
             if(Eolian_Type const* t = &*it)
               types.push_back(t), ++it;
          }
     }

   efl::eolian::eolian_type_instance v(types.size());
   for (std::size_t i = 0; i != types.size(); ++i)
     {
        v.parts[i] = type_find(lut.begin(), lut.end(), type_from_eolian(*types[i]));
     }

   // Let's degrade to opaque classes when not enough information
   // is available for complex types
   if(v.parts.size() == 1 && type_is_complex(v.front()))
     {
       efl::eolian::eolian_type tmp = v.front();
       return {efl::eolian::type_to_native(tmp)};
     }

   return v;
}

} // namespace eolian_cxx {

#endif // EOLIAN_CXX_TYPE_LOOKUP_HH
