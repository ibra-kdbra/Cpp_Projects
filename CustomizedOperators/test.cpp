#include <cstdlib>
#include <iostream>
#include <limits>
#include <set>
#include "comparisons.h"
#include "containers.h"
#include "fuzzy-comparisons.h"
#include "literals.h"
#include "match.h"
#include "math.h"
#include <memory>

using namespace mage_hand;
using namespace std::string_literals;
using namespace std::string_view_literals;

#define verify(condition) \
    verify_function((condition), #condition, __FILE__, __LINE__, __func__)
#define compare(actual, expected) \
    compare_function((actual), (expected), #actual, #expected, __FILE__, __LINE__, __func__)

template<class T>
auto verify_function(T&& condition,
        const char* text,
        const char* file,
        int line,
        const char* function) //
{
    bool r = std::forward<T>(condition);
    if (!r) {
        std::cerr << file << ":" << line << ": " //
                  << "assertion (" << text << ") failed" << std::endl;
        std::cerr << file << ": " << function << " failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    return r;
}

template<class A, class E>
auto compare_function(A&& actual,
        E&& expected,
        const char* atext,
        const char* etext,
        const char* file,
        int line,
        const char* function) //
{
    bool r = std::forward<A>(actual) == std::forward<E>(expected);
    if (!r) {
        std::cerr << file << ":" << line << ": comparison "            //
                  << "(" << atext << ") == (" << etext << ") failed: " //
                  << "value is (" << actual << ") but should be "      //
                  << "(" << expected << ")" << std::endl;
        std::cerr << function << " failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    return actual;
}

void test_array() {
    int array[] = {1, 2, 3};
    verify(1 <in> array);
    verify(4 <!in> array);

    verify('s' <in> "string");
    verify('x' <!in> "string");
}

void test_initializer_list() {
    auto list = {1, 2, 3};
    verify(1 <in> list);
    verify(!(0 <in> list));
    verify(0 <!in> list);
    verify(0 <not_in> list);
    verify(0 <not in> list);
}

void test_string() {
    std::string string = "haystick with needle";
    verify('e' <in> string);
    verify('x' <!in> string);
    verify("needle" <in> string);
    verify("needle"s <in> string);
    verify("needle"sv <in> string);

    std::string_view view = "needles in a haystack";
    verify('e' <in> view);
    verify('x' <!in> view);
    verify("needle" <in> view);
    verify("needle"s <in> view);
    verify("needle"sv <in> view);
}

void test_set() {
    std::set<int> set{1, 2, 3};
    verify(1 <in> set);
    verify(0 <!in> set);
}

void test_container_chaining() {
    verify(!('e' <in> "haystack" or "string"));
    verify('e' <in> "haystack" or "needle");
    verify('e' <in> "needle" or "haystack");
    verify('e' <in> "needle" or "test");

    verify(!('e' <in> "haystack" and "string"));
    verify(!('e' <in> "haystack" and "needle"));
    verify(!('e' <in> "needle" and "haystack"));
    verify('e' <in> "needle" and "test");

    verify('e' <!in> "haystack" or "string");
    verify('e' <!in> "haystack" or "needle");
    verify('e' <!in> "needle" or "haystack");
    verify(!('e' <!in> "needle" or "test"));

    verify('e' <!in> "haystack" and "string");
    verify(!('e' <!in> "haystack" and "needle"));
    verify(!('e' <!in> "needle" and "haystack"));
    verify(!('e' <!in> "needle" and "test"));

    verify(!('e' <in> "string" <in> "haystack"sv));
    verify(!('e' <in> "needle" <in> "haystack"sv));
    verify(!('e' <in> "string" <in> "haystack with strings"sv));
    verify('e' <in> "needle" <in> "haystack with needles"sv);

    verify('e' <!in> "string" <!in> "haystack"sv);
    verify(!('e' <!in> "needle" <!in> "haystack"sv));
    verify(!('e' <!in> "string" <!in> "haystack with strings"sv));
    verify(!('e' <!in> "needle" <!in> "haystack with needles"sv));

    verify(!('e' <in> "string" <!in> "haystack"sv));
    verify('e' <in> "needle" <!in> "haystack"sv);
    verify(!('e' <in> "string" <!in> "haystack with strings"sv));
    verify(!('e' <in> "needle" <!in> "haystack with needles"sv));

    verify(!('e' <!in> "string" <in> "haystack"sv));
    verify(!('e' <!in> "needle" <in> "haystack"sv));
    verify('e' <!in> "string" <in> "haystack with strings"sv);
    verify(!('e' <!in> "needle" <in> "haystack with needles"sv));
}

void test_containers() {
    test_array();
    test_initializer_list();
    test_string();
    test_set();
    test_container_chaining();
}

void test_is() {
    int x = 1, y = 1;
    verify(x <is> 1);
    verify(!(x <is> 2));
    verify(x <!is> 2);
    verify(x <not is> 2);
    verify(x <is_not> 2);

    verify(x <is_not> 2 and 3);
    verify(!(x <is_not> 1 and 2));
    verify(!(x <is_not> 2 and 1));
    verify(!(x <is_not> 1 and 1));

    verify(x <is> 1 or 1);
    verify(x <is> 1 or 2);
    verify(x <is> 2 or 1);
    verify(!(x <is> 2 or 3));

    int a = 0, b = 1;
    verify(x <is> y <is> 1);
    verify(!(x <is> y <is> 0));
    verify(!(a <is> b <is> 1));
    verify(!(a <is> b <is> 2));

    verify(a <!is> b <!is> 2);
    verify(!(a <!is> b <!is> 1));
    verify(!(x <!is> y <!is> 0));
    verify(!(x <!is> y <!is> 1));

    verify(x <is> y == 1);
    verify(!(x <is> y == 0));
    verify(!(a <is> b == 1));
    verify(!(a <is> b == 2));

    verify(a <!is> b != 2);
    verify(!(a <!is> b != 1));
    verify(!(x <!is> y != 0));
    verify(!(x <!is> y != 1));

}

void test_less() {
    int x = 1;
    verify(!(x <less> 0));
    verify(!(x <less> 1));
    verify(x <less> 2);

    verify(!(x <less>= 0));
    verify(x <less>= 1);
    verify(x <less>= 2);

    verify(x <!less> 0);
    verify(x <!less> 1);
    verify(!(x <!less> 2));

    verify(x <!less>= 0);
    verify(!(x <!less>= 1));
    verify(!(x <!less>= 2));
}

void test_more() {
    int x = 1;
    verify(x <more> 0);
    verify(!(x <more> 1));
    verify(!(x <more> 2));

    verify(x <more>= 0);
    verify(x <more>= 1);
    verify(!(x <more>= 2));

    verify(!(x <!more> 0));
    verify(x <!more> 1);
    verify(x <!more> 2);

    verify(!(x <!more>= 0));
    verify(!(x <!more>= 1));
    verify(x <!more>= 2);
}

void test_comparison_chaining() {
    int x = 1;
    verify(0 <lt> x <lt> 2);
    verify(!(0 <lt> x <lt> 1));
    verify(!(1 <lt> x <lt> 2));
    verify(!(1 <lt> x <lt> 1));

    verify(x <lt> 2 and 3);
    verify(!(x <lt> 0 and 2));
    verify(!(x <lt> 2 and 0));
    verify(!(x <lt> 0 and 1));

    verify(x <lt> 2 or 3);
    verify(x <lt> 1 or 2);
    verify(x <lt> 2 or 1);
    verify(!(x <lt> 0 or 1));

    verify(0 <lt> x < 2);
    verify(!(0 <lt> x < 1));
    verify(!(1 <lt> x < 2));
    verify(!(1 <lt> x < 1));

    verify(0 <lt> x <gt> 0);
    verify(!(2 <lt> x <gt> 0));
    verify(!(0 <lt> x <gt> 2));
    verify(!(2 <lt> x <gt> 2));

    verify(2 <gt> x <lt> 2);
    verify(!(0 <gt> x <lt> 2));
    verify(!(2 <gt> x <lt> 0));
    verify(!(0 <gt> x <lt> 0));

    char alpha = 'c';
    char digit = '3';
    verify(alpha <is> '-' or '_' || 'a' <lt>= alpha <lt>= 'z');
    verify('a' <lt>= alpha <lt>= 'z' || (alpha <is> '-' or '_'));
    verify(!(digit <is> '-' or '_' || 'a' <lt>= digit <lt>= 'z'));
    verify(!('a' <lt>= digit <lt>= 'z' || (digit <is> '-' or '_' )));

    verify(alpha <is> '-' or '_' || (alpha <is> 'c' or 'd'));
    verify(!(digit <is> '-' or '_' || (digit <is> 'c' or 'd')));

    verify('a' <lt>= alpha <lt>= 'z' || 'A' <lt>= alpha <lt>= 'Z');
    verify(!('a' <lt>= digit <lt>= 'z' || 'A' <lt>= digit <lt>= 'Z'));
}

void test_fuzzy_comparisons() {
    verify(0.0 <near> 0.0);
    verify(0.0 <near> -0.0);

    using L = std::numeric_limits<double>;
    if (L::has_infinity)
        verify(L::infinity() <near> L::infinity());
    if (L::has_quiet_NaN)
        verify(!(L::quiet_NaN() <near> L::quiet_NaN()));

    verify(0.00042 <near> 0.0);
    verify(!(0.0042 <near> 0.0));

    verify(-0.00042 <near> 0.0);
    verify(!(-0.0042 <near> 0.0));

    double x = 4.2;
    verify(x <near> 4.2);
    verify(x <near> 4.199);
    verify(x <near> 4.201);
    verify(!(x <near> 4.19));
    verify(!(x <near> 4.21));

    verify(-x <near> -4.2);
    verify(-x <near> -4.199);
    verify(-x <near> -4.201);
    verify(!(-x <near> -4.19));
    verify(!(-x <near> -4.21));

    verify(!(x <far> 4.2));
    verify(!(x <far> 4.199));
    verify(!(x <far> 4.201));
    verify(x <far> 4.19);
    verify(x <far> 4.21);
}

void test_fuzzy_comparisons_with_custom_epsilon() {
    verify(0.0042 <Near{0.01}> 0.0);
    verify(!(0.042 <Near{0.01}> 0.0));

    verify(0.000042 <Near{0.0001}> 0.0);
    verify(!(0.00042 <Near{0.0001}> 0.0));

    double x = 4.2;
    verify(x <Near{0.01}> 4.2);
    verify(x <Near{0.01}> 4.199);
    verify(x <Near{0.01}> 4.201);
    verify(!(x <Near{0.01}> 4.1));
    verify(!(x <Near{0.01}> 4.3));

    verify(x <Near{0.0001}> 4.2);
    verify(x <Near{0.0001}> 4.1999);
    verify(x <Near{0.0001}> 4.2001);
    verify(!(x <Near{0.0001}> 4.199));
    verify(!(x <Near{0.0001}> 4.201));
}

void test_match_void() {
    std::string s = "string";
    std::string r;

    match(s) |when| "s" |then| [&] { r = "s"; };
    compare(r, "");
    r.clear();

    match(s) |otherwise| [&] { r = "otherwise"; };
    compare(r, "otherwise");
    r.clear();

    match(s)
        |when| "s" |then| [&] { r = "s"; }
        |otherwise| [&] { r = "otherwise"; };
    compare(r, "otherwise");
    r.clear();

    match(s) |when| "string" |then| [&] { r = "string"; };
    compare(r, "string");
    r.clear();

    match(s)
        |when| "string" |then| [&] { r = "string"; }
        |otherwise| [&] { r = "otherwise"; };
    compare(r, "string");
    r.clear();

    match(s)
        |when| "string" |then| [&] { r = "string"; }
        |when| "s" |then| [&] { r = "s"; }
        |otherwise| [&] { r = "otherwise"; };
    compare(r, "string");
    r.clear();

    match(s)
        |when| "string"
        |when| "s" |then| [&] { r = "string"; }
        |otherwise| [&] { r = "otherwise"; };
    compare(r, "string");
    r.clear();

    match(s)
        |when| "s"
        |when| "string" |then| [&] { r = "string"; }
        |otherwise| [&] { r = "otherwise"; };
    compare(r, "string");
    r.clear();

    match(s)
        |when| "string" |then| [&] { r = "string"; }
        |when| "string" |then| [&] { r = "string2"; }
        |otherwise| [&] { r = "otherwise"; };
    compare(r, "string");
}

void test_match_normal() {
    using std::literals::operator""s;
    std::string r;
    MatchResult<std::string> mr;

    r = match(0)
        |when| 0 |then| [] { return "0"; }
        |otherwise| [] { return ""; };
    compare(r, "0");

    r = match(0) |otherwise| [] { return ""; };
    compare(r, "");

    r = match(0)
        |when| 1 |then| [] { return "1"; }
        |otherwise| [] { return ""; };
    compare(r, "");

    mr = match(0) |when| 0 |then| [] { return "0"s; };
    verify(mr);
    compare(*mr, "0");

    mr = match(0) |when| 1 |then| [] { return "1"s; };
    verify(!mr);

    r = match(0)
        |when| 0 |then| [] { return "0"; }
        |when| 1 |then| [] { return "1"; }
        |otherwise| [] { return ""; };
    compare(r, "0");

    r = match(0)
        |when| 1 |then| [] { return "1"; }
        |when| 0 |then| [] { return "0"; }
        |otherwise| [] { return ""; };
    compare(r, "0");

    r = match(0)
        |when| 0
        |when| 1 |then| [] { return "0|1"; }
        |otherwise| [] { return ""; };
    compare(r, "0|1");

    r = match(0)
        |when| 1
        |when| 0 |then| [] { return "1|0"; }
        |otherwise| [] { return ""; };
    compare(r, "1|0");

    r = match(0)
        |when| 0 |then| [] { return "0.0"; }
        |when| 0 |then| [] { return "1.0"; }
        |otherwise| [] { return ""; };
    compare(r, "0.0");
}

void test_match_ref() {
    int o = 0;
    {
        int& r = match(0) |otherwise| [&]() -> int& { return o; };
        compare(&r, &o);
    }
    int i = 0;
    {
        int& r = match(0)
            |when| 0 |then| [&]() -> int& { return i; }
            |otherwise| [&]() -> int& { return o; };
        compare(&r, &i);
    }
    {
        int& r = match(0)
            |when| 1 |then| [&]() -> int& { return i; }
            |otherwise| [&]() -> int& { return o; };
        compare(&r, &o);
    }
    {
        auto r = match(0) |when| 0 |then| [&]() -> int& { return i; };
        verify(r.hasResult());
        compare(&r.result(), &i);
    }
    {
        auto r = match(0) |when| 1 |then| [&]() -> int& { return i; };
        verify(!r);
    }
}

void test_match() {
    test_match_void();
    test_match_normal();
    test_match_ref();
}

void test_comparisons() {
    test_is();
    test_less();
    test_more();
    test_comparison_chaining();
    test_fuzzy_comparisons();
    test_fuzzy_comparisons_with_custom_epsilon();
    test_match();
}

void test_mod() {
    compare(23 /mod/ 10, 3);
    compare(-23 /mod/ 10, 7);
    compare(7.0 /mod/ 4.0, 3.0);
    compare(-7.0 /mod/ 4.0, 1.0);
}

void test_div() {
    using mage_hand::fdiv;
    using mage_hand::div;
    using mage_hand::ldiv;

    verify((std::is_same_v<float, decltype(1 /fdiv/ 3)>));
    verify((std::is_same_v<double, decltype(1 /div/ 3)>));
    verify((std::is_same_v<long double, decltype(1 /ldiv/ 3)>));

    verify(1 /fdiv/ 3 <~is> 0.333333f);
    verify(1 /div/ 3 <~is> 0.333333);
    verify(1 /ldiv/ 3 <~is> 0.333333l);
}

void test_pow() {
    compare(2 /xx/ 0, 1);
    compare(2 /xx/ 1, 2);
    compare(2 /xx/ 2, 4);
    compare(2 /xx/ 3, 8);
    compare(2 /xx/ 4, 16);

    compare(3 /xx/ 0, 1);
    compare(3 /xx/ 1, 3);
    compare(3 /xx/ 2, 9);
    compare(3 /xx/ 3, 27);
    compare(3 /xx/ 4, 81);
    compare(3 /xx/ 5, 243);

    compare(2.0 /xx/ 0.0, 1.0);
    compare(2.0 /xx/ 1.0, 2.0);
    compare(2.0 /xx/ 2.0, 4.0);
    compare(2.0 /xx/ 3.0, 8.0);
    compare(2.0 /xx/ 4.0, 16.0);
    compare(2.0 /xx/ 5.0, 32.0);
}

void test_literals() {
    using namespace mage_hand::literals;

    compare(0², 0);
    compare(1², 1);
    compare(2², 4);
    compare(3², 9);

    compare(0³, 0);
    compare(1³, 1);
    compare(2³, 8);
    compare(3³, 27);
}

void test_math() {
    test_mod();
    test_div();
    test_pow();
    test_literals();
}

int main(int argc, char* argv[]) {
    test_containers();
    test_comparisons();
    test_math();
    return EXIT_SUCCESS;
}
