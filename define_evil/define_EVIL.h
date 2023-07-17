/* MIT License
 *
 * Copyright (c) 2021 ibra-kdbra
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#ifndef EVIL
#define EVIL

// macros auto-generated from generateor.sh
// should all be prefixed with _GEN_
#include "generated_EVIL.h"

// # General Utils

// convert input to string literal, can be used with EXPAND_CALL
// depends on: none
#define EVIL_TO_STRING(...) #__VA_ARGS__

// used internally by other macros
// depends on: none
#define EVIL_ORDER_FORWARD(a, b) a b
#define EVIL_ORDER_BACKWARD(a, b) b a

// expands the arguments and concatenates them
// depends on: none
#define EVIL_EXPAND_CAT(a, ...) _EVIL_EXPAND_CAT(a, __VA_ARGS__)
#define _EVIL_EXPAND_CAT(a, ...) a##__VA_ARGS__

// expands the arguments and then call macro with the expanded arguments
// depends on: none
#define EVIL_EXPAND_CALL(macro, ...) macro(__VA_ARGS__)

// even I'm not completely sure why this is needed, but it is
// depends on: none
#define EVIL_EXPAND(...) __VA_ARGS__

// toggleable expansion
// depends on: none
#define EVIL_EXPAND_TRUE(...) __VA_ARGS__
#define EVIL_EXPAND_FALSE(...)

// removes commas in a list of arguments, leaves a space between each one
// depends on: _EVIL_GEN_REMOVE_COMMAS
#define EVIL_REMOVE_COMMAS(...) _EVIL_GEN_REMOVE_COMMAS(__VA_ARGS__)

// # Boolean operations
// these expand to the result of boolean logic (not, or, and, xor)
// should only be given inputs TRUE and FALSE, other inputs will yeild unwanted results

// if given TRUE or FALSE, will expand to the oposite
// depends on: EVIL_EXPAND_CAT
#define EVIL_NOT(...) _EVIL_NOT(__VA_ARGS__)
#define _EVIL_NOT(a) EVIL_EXPAND_CAT(_EVIL_BOOL_NOT_, a)
#define _EVIL_BOOL_NOT_TRUE FALSE
#define _EVIL_BOOL_NOT_FALSE TRUE

// expands to TRUE if either inputs are TRUE
// depends on: EVIL_EXPAND_CAT
#define EVIL_OR(...) _EVIL_OR(__VA_ARGS__)
#define _EVIL_OR(a, b) EVIL_EXPAND_CAT(_EVIL_BOOL_OR_, EVIL_EXPAND_CAT(a, EVIL_EXPAND_CAT(_, b)))
#define _EVIL_BOOL_OR_FALSE_FALSE FALSE
#define _EVIL_BOOL_OR_TRUE_FALSE TRUE
#define _EVIL_BOOL_OR_FALSE_TRUE TRUE
#define _EVIL_BOOL_OR_TRUE_TRUE TRUE

// expands to TRUE if both inputs are TRUE
// depends on: EVIL_EXPAND_CAT
#define EVIL_AND(...) _EVIL_AND(__VA_ARGS__)
#define _EVIL_AND(a, b) EVIL_EXPAND_CAT(_EVIL_BOOL_AND_, EVIL_EXPAND_CAT(a, EVIL_EXPAND_CAT(_, b)))
#define _EVIL_BOOL_AND_FALSE_FALSE FALSE
#define _EVIL_BOOL_AND_TRUE_FALSE FALSE
#define _EVIL_BOOL_AND_FALSE_TRUE FALSE
#define _EVIL_BOOL_AND_TRUE_TRUE TRUE

// expands to true if exactly one input is TRUE
// depends on: EVIL_EXPAND_CAT
#define EVIL_XOR(...) _EVIL_XOR(__VA_ARGS__)
#define _EVIL_XOR(a, b) EVIL_EXPAND_CAT(_EVIL_BOOL_XOR_, EVIL_EXPAND_CAT(a, EVIL_EXPAND_CAT(_, b)))
#define _EVIL_BOOL_XOR_FALSE_FALSE FALSE
#define _EVIL_BOOL_XOR_TRUE_FALSE TRUE
#define _EVIL_BOOL_XOR_FALSE_TRUE TRUE
#define _EVIL_BOOL_XOR_TRUE_TRUE FALSE

// # Conditionals
// should only be given inputs TRUE and FALSE, other inputs will yeild unwanted results

// expands to the thing in the second set of parentheses only if the thing in the first set is TRUE
// EVIL_IF(TRUE)(abc)    -> abc
// EVIL_IF(FALSE)(abc)   -> [empty]
// depends on: EVIL_NOT, EVIL_EXPAND_CAT, EVIL_EXPAND_TRUE, EVIL_EXPAND_FALSE
#define EVIL_IF(cond) EVIL_EXPAND_CAT(_EVIL_IF_COND_WAS_, cond)
#define _EVIL_IF_COND_WAS_TRUE(...) __VA_ARGS__
#define _EVIL_IF_COND_WAS_FALSE(...)

// expands to the thing in the second set of parentheses only if the thing in the first set is FALSE
// EVIL_IF_NOT(TRUE)(abc)    -> [empty]
// EVIL_IF_NOT(FALSE)(abc)   -> abc
// depends on: EVIL_NOT, EVIL_IF, EVIL_EXPAND_CAT, EVIL_EXPAND_TRUE, EVIL_EXPAND_FALSE
#define EVIL_IF_NOT(cond) EVIL_IF(EVIL_NOT(cond))

// if the thing in the first set of parentheses is true expands to the thing in the second set
// if it's false expands to the thing in the third set
// EVIL_IF_ELSE(TRUE)(abc)(xyz)  -> abc
// EVIL_IF_ELSE(FALSE)(abc)(xyz) -> xyz
// depends on: EVIL_EXPAND_CAT, EVIL_EXPAND_TRUE, EVIL_EXPAND_FALSE
#define EVIL_IF_ELSE(cond) EVIL_EXPAND_CAT(_EVIL_IF_ELSE_, cond)
#define _EVIL_IF_ELSE_TRUE(...) __VA_ARGS__ EVIL_EXPAND_FALSE
#define _EVIL_IF_ELSE_FALSE(...) EVIL_EXPAND_TRUE

// # Equality

// expand to TRUE if two tokens are known to be equal, FALSE otherwise
// only works if there is an ENABLE_EQ_*_* defined for the tokens
// below EVIL_ENABLE_EQ is defined for booleans, void and empty values, generated.h has it defined for small numbers.
#define EVIL_EQ(a_val, b_val) EVIL_IF_ELSE \
    (EVIL_IS_THING(EVIL_EXPAND_CAT(EVIL_ENABLE_EQ_, EVIL_EXPAND_CAT(b_val, EVIL_EXPAND_CAT(_, b_val))))) \
    (EVIL_EXPAND_CALL(EVIL_TO_STRING, You must define \
        EVIL_EXPAND_CAT( \
            EVIL_ENABLE_EQ_, EVIL_EXPAND_CAT( \
                b_val, EVIL_EXPAND_CAT(_, b_val))) \
        to use EVIL_EQ on b_val)) \
    (EVIL_NOT(EVIL_IS_THING(EVIL_EXPAND_CAT(EVIL_ENABLE_EQ_, EVIL_EXPAND_CAT(a_val, EVIL_EXPAND_CAT(_, b_val))))))

// expands to TRUE if the two tokens are not equal (see EVIL_EQ)
#define EVIL_NE(a, b) EVIL_NOT(EVIL_EQ(a, b))

// enable equality check for various common values, you can add more elsewhere
#define EVIL_ENABLE_EQ_TRUE_TRUE
#define EVIL_ENABLE_EQ_FALSE_FALSE
#define EVIL_ENABLE_EQ_void_void
#define EVIL_ENABLE_EQ__

// # Info Getters

// NOTE: the final arg can not be the name of a function-like macro! (see below)
// a common way to use is to make two macros (ex. EXAMPLE_TRUE and EXAMPLE_FALSE) and concat your prefix with the
// result of this macro, so you can do different things depending on the thingyness
// the last argument must not be a function-like macro
// IS_THING(one_or_more, args)  -> TRUE
// IS_THING()                   -> FALSE
// IS_THING(FN_MACRO)           -> <fails to compile>
// IS_THING(arg, FN_MACRO)      -> <fails to compile>
// I realize how much of a clusterfuck this is. If you can make it cleaner without failing any tests, plz submit PR
#define EVIL_IS_THING(...) _EVIL_IS_THING_A(EVIL_EXPAND_CAT(_, EVIL_EXPAND(_EVIL_IS_THING_LOOK_FOR_PEREN EVIL_REMOVE_COMMAS(__VA_ARGS__))))
#define _EVIL_IS_THING_A(a) EVIL_EXPAND_CALL(_EVIL_IS_THING_C, _EVIL_IS_THING_B a (), TRUE)
#define _EVIL_IS_THING_B() was_not_thing, FALSE
#define _EVIL_IS_THING_C(a, b, ...) b
#define _EVIL_IS_THING_LOOK_FOR_PEREN(...) was_thing_with_peren
#define __EVIL_IS_THING_LOOK_FOR_PEREN

// checks if the argument(s) are completely surrounded by parenthesis
// NOTE: If it starts with parentheses, it can not end with a function-like macro! (see below)
// EVIL_HAS_PEREN()                 -> FALSE
// EVIL_HAS_PEREN("a")              -> FALSE
// EVIL_HAS_PEREN(6, "a")           -> FALSE
// EVIL_HAS_PEREN((), a, 6, "a")    -> FALSE
// EVIL_HAS_PEREN(())               -> TRUE
// EVIL_HAS_PEREN((a, 6, "a"))      -> TRUE
// EVIL_HAS_PEREN((a) (b))          -> FALSE
// EVIL_HAS_PEREN(FN_MACRO)         -> FALSE
// EVIL_HAS_PEREN((FN_MACRO))       -> TRUE
// EVIL_HAS_PEREN((a) FN_MACRO)     -> <fails to compile>
#define EVIL_HAS_PEREN(...) _EVIL_HAS_PEREN_C(EVIL_EXPAND(_EVIL_HAS_PEREN_A __VA_ARGS__))
#define _EVIL_HAS_PEREN_A(...) _EVIL_HAS_PEREN_B
#define _EVIL_OTHER_EVIL_HAS_PEREN_A _EVIL_NO_PEREN(
#define _EVIL_OTHER_EVIL_HAS_PEREN_B _EVIL_YES_PEREN(
#define _EVIL_HAS_PEREN_C(...) EVIL_EXPAND_CAT(_EVIL_OTHER, __VA_ARGS__) )
#define _EVIL_NO_PEREN(...) FALSE
#define _EVIL_YES_PEREN(...) EVIL_NOT(EVIL_IS_THING(__VA_ARGS__))

// same as EVIL_COUNT, except always reports at least 1 item and is not bothered by function-like macros at the end
#define EVIL_COUNT_AT_LEAST_1(...) EVIL_EXPAND_CALL(_EVIL_GEN_COUNT, __VA_ARGS__, _EVIL_GEN_COUNT_NUMBERS)

// expands to the number of arguments; empty arguments are counted; zero arguments is handeled correctly
// NOTE: can not end with a function-like macro!
#define EVIL_COUNT(...) EVIL_IF_ELSE \
    (EVIL_IS_THING(__VA_ARGS__)) \
    (EVIL_COUNT_AT_LEAST_1(__VA_ARGS__)) \
    (0)

// a closure (sometimes called a lamda expression) is a chunk of functionality that can be passed around like data
// what we're calling a closure here is a function-like macro be bundled with zero or more captured values
// EVIL_CLOSURE_INVOKE() expands the contined macro with the captured values as well as zero or more additional arguments
// the closure macro receives first the captured values, then the arguments supplied to EVIL_CLOSURE_INVOKE()
// the closure macro and all captured values need to be surounded by parentheses
// EVIL_CLOSURE_INVOKE((MACRO))             -> MACRO()
// EVIL_CLOSURE_INVOKE((MACRO, a))          -> MACRO(a)
// EVIL_CLOSURE_INVOKE((MACRO), x)          -> MACRO(x)
// EVIL_CLOSURE_INVOKE((MACRO, a, b), x, y) -> MACRO(a, b, x, y)
#define EVIL_CLOSURE_INVOKE(...) \
    /* check that the closure is enclosed by parentheses */ \
    /* (not doing this for closures with no captured values is an easy mistake) */ \
    EVIL_IF_ELSE(EVIL_HAS_PEREN(_EVIL_CLOSURE_INVOKE_EXTRACT_FIRST(__VA_ARGS__))) \
    ( \
        _EVIL_CLOSURE_INVOKE_EXPAND_CALL ( \
            /* extract the closure, and from the closure extract the function-like macro at the start */ \
            _EVIL_CLOSURE_INVOKE_EXTRACT_FIRST _EVIL_CLOSURE_INVOKE_EXTRACT_FIRST(__VA_ARGS__), \
            EVIL_EXPAND( \
                /* extract the closure, and from the closure extract it's captured values */ \
                _EVIL_CLOSURE_INVOKE_EXTRACT_ARGS _EVIL_CLOSURE_INVOKE_EXTRACT_FIRST(__VA_ARGS__) \
                /* if there are both captured values and arguments, we need a comma between them */ \
                EVIL_IF_NOT(EVIL_OR( \
                    EVIL_EQ(EVIL_COUNT_AT_LEAST_1(__VA_ARGS__), 1), \
                    EVIL_EQ(EVIL_COUNT_AT_LEAST_1 _EVIL_CLOSURE_INVOKE_EXTRACT_FIRST(__VA_ARGS__), 1))) \
                (,) \
                /* extract the arguments */ \
                _EVIL_CLOSURE_INVOKE_EXTRACT_ARGS(__VA_ARGS__) \
            ) \
        ) \
    ) \
    ("Closures need to be enclosed by parentheses, dumbass")
#define _EVIL_CLOSURE_INVOKE_EXTRACT_FIRST(first, ...) first
#define _EVIL_CLOSURE_INVOKE_EXTRACT_ARGS(macro, ...) __VA_ARGS__
// everything blows up if we use the normal EXPAND_CALL here. We're probably somehow calling it recursivly
#define _EVIL_CLOSURE_INVOKE_EXPAND_CALL(macro, ...) macro(__VA_ARGS__)

// repeats closure count number of times (see EVIL_CLOSURE_INVOKE)
// closure is invoked with the current index as it's argument (in addition to any captured values)
// EVIL_REPEAT((FOO), 3)    -> FOO(0) FOO(1) FOO(2)
// EVIL_REPEAT((FOO), 0)    -> [empty]
#define EVIL_REPEAT(closure, count) EVIL_EXPAND_CAT(_EVIL_GEN_MAP_, count) \
    ((_EVIL_REPEAT_FUNC, closure), EVIL_ORDER_FORWARD, 0, EVIL_INC_,)
// we have to duplicate EVIL_CLOSURE_INVOKE here because no recursion
#define _EVIL_REPEAT_FUNC(closure, unused_item, index) \
    EVIL_IF_ELSE(EVIL_HAS_PEREN(closure)) \
    ( \
        _EVIL_REPEAT_FUNC_EXPAND_CALL ( \
            _EVIL_CLOSURE_INVOKE_EXTRACT_FIRST closure, \
            EVIL_EXPAND( \
                _EVIL_CLOSURE_INVOKE_EXTRACT_ARGS closure \
                EVIL_IF_NOT(EVIL_EQ(EVIL_COUNT_AT_LEAST_1 closure, 1)) (,) \
                index \
            ) \
        ) \
    ) \
    ("Closures need to be enclosed by parentheses, dumbass")
#define _EVIL_REPEAT_FUNC_EXPAND_CALL(macro, ...) macro(__VA_ARGS__)

// same as EVIL_REPEAT, but in reverse order
// EVIL_REPEAT_DOWN((FOO), 3)    -> FOO(2) FOO(1) FOO(0)
#define EVIL_REPEAT_DOWN(closure, count) EVIL_EXPAND_CAT(_EVIL_GEN_MAP_, count) \
    ((_EVIL_REPEAT_FUNC, closure), EVIL_ORDER_FORWARD, EVIL_EXPAND_CAT(EVIL_DEC_, count), EVIL_DEC_,)

// applies the given closure to all additional arguments
// closure is given the item and the index, see EVIL_CLOSURE_INVOKE for details
// EVIL_MAP((FOO), a, b, c)   -> FOO(a, 0) FOO(b, 1) FOO(c, 2)
#define EVIL_MAP(closure, ...) EVIL_EXPAND_CAT(_EVIL_GEN_MAP_, EVIL_COUNT(__VA_ARGS__)) \
    (closure, EVIL_ORDER_FORWARD, 0, EVIL_INC_, __VA_ARGS__)

// same as EVIL_MAP, but indexes count down instead of up
// EVIL_MAP_DOWN((FOO), a, b, c)   -> FOO(a, 2) FOO(b, 1) FOO(c, 0)
#define EVIL_MAP_DOWN(closure, ...) EVIL_EXPAND_CAT(_EVIL_GEN_MAP_, EVIL_COUNT(__VA_ARGS__)) \
    (closure, EVIL_ORDER_FORWARD, EVIL_EXPAND_CAT(EVIL_DEC_, EVIL_COUNT(__VA_ARGS__)), EVIL_DEC_, __VA_ARGS__)

// same as EVIL_MAP but reverses the order of the items
// EVIL_MAP_REVERSE((FOO), a, b, c)   -> FOO(c, 0) FOO(b, 1) FOO(a, 2)
#define EVIL_MAP_REVERSE(closure, ...) EVIL_EXPAND_CAT(_EVIL_GEN_MAP_, EVIL_COUNT(__VA_ARGS__)) \
    (closure, EVIL_ORDER_BACKWARD, EVIL_EXPAND_CAT(EVIL_DEC_, EVIL_COUNT(__VA_ARGS__)), EVIL_DEC_, __VA_ARGS__)

// same as EVIL_MAP but reverses the order of items and counts down
// pairs the same indexes with the same items as EVIL_MAP()
// EVIL_MAP_REVERSE_DOWN((FOO), a, b, c)   -> FOO(c, 2) FOO(b, 1) FOO(a, 0)
#define EVIL_MAP_REVERSE_DOWN(closure, ...) EVIL_EXPAND_CAT(_EVIL_GEN_MAP_, EVIL_COUNT(__VA_ARGS__)) \
    (closure, EVIL_ORDER_BACKWARD, 0, EVIL_INC_, __VA_ARGS__)

#endif // EVIL
