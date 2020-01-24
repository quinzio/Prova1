/*=============================================================================
    This example was taken fom the issue list on Github
    for the project Boost Wave.
    https://github.com/boostorg/wave/issues
=============================================================================*/

// code to reproduce TRAC 10733

#include <iostream>
#include <string>
#include <stdint.h>

#include <boost/wave.hpp>
#include <boost/wave/token_ids.hpp>
#include <boost/wave/preprocessing_hooks.hpp>
#include <boost/wave/cpplexer/cpp_lex_token.hpp>
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp>

#include <boost/wave/cpplexer/re2clex/cpp_re2c_lexer.hpp>

std::string hookRes;

// custom preprocessor hooks

struct pp_hooks : boost::wave::context_policies::default_preprocessing_hooks
{
    template <typename ContextT, typename TokenT, typename ContainerT>
    bool expanding_object_like_macro(
        ContextT const&, TokenT const& macro,
        ContainerT const&, TokenT const&)
    {
        //std::cout << "expanding object macro " << macro.get_value() << "\n";
        hookRes += "expanding object macro " << macro.get_value() << "\n";
        return false;   // do not skip expansion
    }

    template <typename ContextT, typename ContainerT>
    void expanded_macro(ContextT const&,
        ContainerT const& result)
    {
        std::cout << "macro expanded to |";
        for (auto const& tok : result)
        {
            //std::cout << tok.get_value();
            hookRes += tok.get_value();
        }
        //std::cout << "|\n";
        hookRes += "\n";
    }
};

typedef boost::wave::cpplexer::lex_token<> cpplexer_token_t;
typedef boost::wave::cpplexer::lex_iterator<cpplexer_token_t> cpplexer_iterator_t;

int main()
{
    std::string cppstr =
        R"###(
#include "inc.h"
#define AOO 25
#define FOO 25
#define A 1
#define B 2
#define BOO  A * B
#define KOO  BOO + FOO

int main() {
    int foo = KOO;
    int a = AOO;
    a = KOO;
    int thisline = __LINE__;
}
)###";

    std::ifstream instream("./Sources/MCN1_acmotor.c");

    if (!instream.is_open()) {
        std::cerr << "Could not open input file: " << "Text.txt" << std::endl;
        return -2;
    }
    instream.unsetf(std::ios::skipws);
    cppstr = std::string(std::istreambuf_iterator<char>(instream.rdbuf()),
        std::istreambuf_iterator<char>());

    auto cbeg = cppstr.begin();
    auto cend = cppstr.end();
    using namespace boost::wave;

    using context_type = context<std::string::const_iterator, cpplexer_iterator_t,
        iteration_context_policies::load_file_to_string,
        pp_hooks>;

    context_type ctx(cbeg, cend, "<Unknown>");
    ctx.add_macro_definition("__HCS12__");
    ctx.add_macro_definition("__HC12__");
    ctx.add_macro_definition("_CANTATA_");
    ctx.add_include_path("./Sources");
    ctx.add_include_path("./Sources/sysfiles");
    ctx.add_sysinclude_path("./Sources");
    ctx.add_sysinclude_path("./Sources/sysfiles");

    for (cpplexer_token_t const& tok : ctx) { 
        std::string temp;
        (void)tok;
        temp = tok.get_value().c_str();
        if (temp.replace()
        //std::cout << tok.get_position() << "\n";
        std::cout << tok.get_value();
        //std::cout << "\n";
        
    }

    return 0;
}

