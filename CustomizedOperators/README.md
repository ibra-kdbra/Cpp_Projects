# CustomizedOperators

implements custom operators for C++.

## System requirements

- a C++17 compiler
- The meson build system

I tried to adhere to the C++17 standard, but have only tested if it works on a
x86 64-bit Debian Linux with gcc, so it might not work on other systems.

## Installation

Should work like any other meson project. See the meson documentation for
further information.

## How it works

In C++, one cannot create new operators such as `<>` or `|&`. However, you can create a variable named `x` of a newly created type with an irrelevant name and overload the operators `<` and `>` so that you can write `< x >`, creating new operators of sorts.

## Features

### Creating new operators

You can create new operators like so:

    #include "mage-hand/mage-hand.h

    constexpr struct X : mage_hand::Operator {} x;

    template<> struct mage_hand::Operator::Slash<int, X, int> {
        auto operator()(int a, int b) {
            return a * b;
        }
    };

This will create a the new operator `/x/` that multiplies two integers. For a
complete list of operator shapes see the mage-hand.h header file.

### Overloading existing operators

You can overload existing operators like so:

    #include "mage-hand/mage-hand.h"
    #include "mage-hand/math.h"

    template<> struct mage_hand::Operator::Slash<BigInt, mage_hand::Mod, BigInt> {
        auto operator()(const BigInt& a, const BigInt& b) {
            return a.mod(b);
        }
    };

    template<> struct mage_hand::Operator::Slash<BigInt, mage_hand::Mod, int> {
        auto operator()(const BigInt& a, int b) {
            return a.mod(b);
        }
    };

As you can see in this example, the type deduction is done without references
and const volatile qualifiers.

### Accessing the operator object

You can access the operator object by providing a constructor that takes the
operator object as its only argument.

    #include "mage-hand/mage-hand.h"

    struct X : mage_hand::Operator {
        int i;
    };

    template<>
    class mage_hand::Operator::Slash<long, X, long> {
        long i;
    public:
        Slash(const X& x) : i(x.i) {}
        auto operator()(long a, long b) {
            return a * i + b;
        }
    };

    template<>
    struct mage_hand::Operator::Slash<int, X, int> {
        X x;
        auto operator()(int a, int b) {
            return a * x.i + b;
        }
    };

    assert(2 /X{3}/ 4 == 10);

### Preimplemented operators

This library implements some generally useful custom operators itself:

#### Power operator for literals

Uses the Unicode characters ⁰, ¹, ², ..., ⁹. Works only integer literals and
only on exponents up to and including ⁶³.

Example:

    #include "mage-hand/literals.h"

    using namespace mage_hand::literals;

    static_assert(3² == 9);

#### Power operator using `/xx/`

Works on floating point and integer numbers.

Example:

    #include "mage-hand/math.h"

    using mage_hand::xx;

    assert(3 /xx/ 2 == 9);

#### `/mod/`

Implements the modulus function on floating point and integer numbers. Unlike
the `%` operator this implements the mathematically correct modulus function.

Examples:

    #include "mage-hand/math.h"

    using mage_hand::mod;

    assert(17 /mod/ 10 == 7);
    assert(-7 /mod/ 10 == 3);

#### `/div/`

When dividing integer numbers in C++ one gets integer numbers out. This however
loses information. This library introduces the `/fdiv/`, `/div/` and `/ldiv/`
operators which perform division using floating numbers even if given integer
numbers.

Examples:

    #include "mage-hand/math.h"

    using mage_hand::div;

    assert(std::is_same_v<double, decltype(1 /div/ 3)>);
    assert(0 < 1 /div/ 3);
    assert(1 /div/ 3 < 1);

#### The comparison operators

In standard C++ `0 <= x < 2` doesn't do what one would expect, neither does
`x == 0 or 1`. This library creates the new operators `<lt>`, `<lt>=`, `<le>`,
`<less>`, `<less>=`, `<is>`, `<is_not>`, `<more>`, `<more>=`, `<ge>`, `<gt>=`,
`<gt>`, which can do both of these.

Examples:

    #include "mage-hand/comparisons.h"

    using mage_hand::less;
    using mage_hand::is;

    int x = 0;
    assert(0 <less>= x <less> 2)
    assert(x <is> 0 or 1)

#### Fuzzy comparison operators

When comparing floating point numbers, one shouldn't use == and != directly.
Instead one should compare floating point numbers with an error margin. This
library introduces the `<near>` (also known as `<~is>`) and `<far>` (also known
as `<!~is>)` operators to deal with this problem.  The default error margin is
0.001, using a custom error margin is possible using
`<Near(errorMargin)>`/`<Far(errorMargin)>`.  If `rhs != 0.0`, numbers are
considered equal if `lhs` is between `rhs / (1.0 + errorMargin)` and
`rhs * (1.0 + errorMargin)`, where `lhs` refers to the left hand side of the
expression and `rhs` to the right side.  If `rhs == 0.0` the numbers are
considered equal if `lhs` is between `-errorMargin` and `errorMargin`.
If `rhs != 0.0`, `rhs / errorMargin` and `rhs * errorMargin` are considered
equal to `lhs`.  If `rhs == 0.0`, `-errorMargin` and `errorMargin` are
considered equal to `lhs`.

Examples:

    #include "mage-hand/fuzzy-comparisons.h"

    using mage_hand::is;
    using mage_hand::near;
    using mage_hand::far;
    using mage_hand::Near;

    assert(1.0 / 3.0 <near> 0.333333);
    assert(1.0 / 3.0 <~is> 0.333333);
    assert(!(1.0 / 3.0 <far> 0.333333));
    assert(1.0 / 3.0 <Near(0.1)> 0.33);

#### `<in>` operator

Test if a container contains an element. Works on any container that either has
a `find` method that works like `std::string`, or a `find` method like
`set::set` or that can be searched with `std::find`. Can be chained and negated
just like the `<is>` operator.

Examples:

    #include "mage-hand/containers.h"

    using mage_hand::in;
    using mage_hand::not_in;

    int array[] = {1, 2, 3};
    auto list = {1, 2, 3}; // produces a std::initializer_list

    assert(1 <in> array);
    assert(0 <!in> array);
    assert(0 <not_in> array); // alternative way to negate this operator

    assert(1 <in> list);
    assert('s' <in> "string");
    assert('e' <in> "needle" <in> "more needles"sv);
    assert('e' <in> "needle" or "string");

#### `match` statement

The builtin C++ `switch` statement has 2 main annoyances: It can only match
integer types and one can't use it as an expression. The `match` statement can
do both of these things.

The `match` statement begins with a call to the `match` function followed by
zero or more conditions. A condition is one or more `|when|` statements followed
by a `|then|` statement. `|then|` statements take a function to execute if any
of the corresponding `|when|` statements matched.  After all conditions an
`|otherwise|` statement can follow whose corresponding function is executed if
none of the preceding conditions matched.

If an `|otherwise|` statement is present the return type of the statement is the
common type of all `|then|` and `|otherwise|` statements. If no `|otherwise|`
statement is present the return type is a `MatchResult` which has
`hasResult()`/`operator bool()` and `result()`/`operator*()`/`operator->()`
methods and works similar to `std::optional`. `MatchResult` works with
references, using `std::reference_wrapper` is not necessary.

Examples:
```cpp
    #include <iostream>
    #include "mage-hand/match.h"

    using mage_hand::match;
    using mage_hand::when;
    using mage_hand::then;
    using mage_hand::otherwise;

    // otherwise is optional, but you need to check if the result contains
    // anything if you don't use an otherwise statement
    assert(!(match(0) |when| 1 |then| [&] { return "one"; }));

    // if one doesn't use an otherwise statement the result type must be
    // dereferenced explicitly
    assert("zero"s == *(match(0) |when| 0 |then| [&] { return "zero"; }));

    // if an otherwise statement is present no dereferencing is necessary
    assert(""s == (match(0)
                   |when| 1 |then| [&] { return "one"; }
                   |otherwise| [&] { return ""; }));

    // multiple when statements can follow each other
    assert("small"s == (match(0)
                        |when| 0
                        |when| 1
                        |when| 2 |then| [&] { return "small"; }
                        |otherwise| [&] { return "large"; }));

    // Functions returning void are possible. Types do not need to match
    // exactly, it's enough if they can be compared with operator==().
    std::string x = "x";
    match(x)
        |when| "x" |then| [&] { std::cout << ":-)" << std::endl; }
        |otherwise| [&] { std::cout << ":-(" << std::endl; };
```
## Caveats

- The left hand side and right hand side will have the precedence of the
  corresponding operator and might even differ if different operators are used.
- Most of this won't play nicely together with automatic code formatters
  (clang-format and similar)
- The compiler might warn you about doing weird stuff
- Compiler errors probably won't be readable
