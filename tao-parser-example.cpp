#include <iostream>
#include <string>

#include <tao/pegtl.hpp>

namespace pegtl = tao::TAO_PEGTL_NAMESPACE;

namespace hello
{
   // clang-format off
   /* The `:` is the start of inheritance, apparently. I guess that
    * the inheritance here is something of a type.
    */
   //struct prefix : pegtl::string< 'H', 'e', 'l', 'l', 'o', ',', ' ' > {};
   /* This seems to work fine, too. The inheritance seems to just be
    * a... more elegant form of typedef? Maybe I'd only need to use a
    * struct explicitly if I had to: define a sort of "custom rule"
    * that would work with some sort of "input rule", where the input
    * rule is a type, or if I had to add some extra information on the
    * type, using members of a struct.
    */
   typedef pegtl::string< 'H', 'e', 'l', 'l', 'o', ',', ' ' > prefix;
   struct name : pegtl::plus< pegtl::alpha > {};
   struct grammar : pegtl::must< prefix, name, pegtl::one< '!' >, pegtl::eof > {};
   // clang-format on

   template< typename Rule >
   struct action
      : pegtl::nothing< Rule >
   {
   };

   /* I think that this is like a template class, but using a struct.
    * However... what the fuck is going on with the no template
    * parameters here? Why is it even necessary?
    *
    * It seems that the `typename` is always necessary next to the
    * name of a type.
    *
    * There was a struct type named action created. Then, here, we
    * specialize that struct type named action.
    *
    * A related set of structs named `action` were created. For every
    * different `...` placed in `action<...>`, we get a different 'kind'
    * of action. A different type. Why is the original struct
    * necessary, though?
    *
    * In general, an action inherits from pegtl::nothing< Rule >.
    * However, for the specific action intended for the type `name`,
    * we have a struct with a single static member function, `apply`.
    *
    * That member function is itself a template function which uses
    * the template type `Input`. However, I don't understand what the
    * apply function actually does. What is `v`?
    */
   template<>
   struct action< name >
   {
      template< typename Input >
      static void apply( const Input& in, std::string& v )
      {
         v = in.string();
      }
   };

}  // namespace hello

int main( int argc, char** argv )
{
   if( argc > 1 ) {
      std::string name;

      /* Why are the `<>` necessary here if they're empty? */
      pegtl::argv_input<> in( argv, 1 );
      pegtl::parse< hello::grammar, hello::action >( in, name );

      std::cout << "Good bye, " << name << "!" << std::endl;
   }
}
