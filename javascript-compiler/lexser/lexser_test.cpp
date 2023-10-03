#include <gtest/gtest.h>
#include <iostream>
#include "lexser.h"

using namespace simple_lexser;

TEST(SingleQuatePairSpaceShouldSuccess, SingleQuateTest) {
    const char *src  = "' '";
    Lexser lexser;

    lexser.lex(src);
    EXPECT_STREQ(lexser.token_list[0].value.c_str(), " ");
}

TEST(SingleQuatePairEscapeQuateShouldSuccess, SingleQuateTest) {
    const char *src  = "'\\''"; // '\''
    Lexser lexser;

    lexser.lex(src);
    EXPECT_STREQ(lexser.token_list[0].value.c_str(), "\\'");
}

TEST(SingleQuatePairEscapeBackSlashShouldFail, SingleQuateTest) {
    const char *src  = "'\\'"; // '\'
    Lexser lexser;

    EXPECT_DEATH(lexser.lex(src), "");
}

TEST(SingleQuate1QuateShouldFail, SingleQuateTest) {
    const char *src  = "'";
    Lexser lexser;

    ASSERT_DEATH(lexser.lex(src), "");
}

TEST(SingleQuateEmpty2QuateShouldFail, SingleQuateTest) {
    const char *src  = "''";
    Lexser lexser;

    ASSERT_DEATH(lexser.lex(src), "");
}

TEST(SingleQuate3QuateShouldFail, SingleQuateTest) {
    const char *src  = "'''";
    Lexser lexser;

    ASSERT_DEATH(lexser.lex(src), "");
}

TEST(QuateEmpty1QuateShouldFail, QuateTest) {
    const char *src  = "\""; // "
    Lexser lexser;

    ASSERT_DEATH(lexser.lex(src), "");
}

TEST(QuateEmpty2QuateShouldSuccess, QuateTest) {
    const char *src  = "\"\""; // ""
    Lexser lexser;
    lexser.lex(src);
    EXPECT_STREQ(lexser.token_list[0].value.c_str(), "");
}

TEST(QuateEmpty3QuateShouldFail, QuateTest) {
    const char *src  = "\"\"\""; // """
    Lexser lexser;
    ASSERT_DEATH(lexser.lex(src), "");
}

TEST(QuateEmpty3BackSlashShouldFail, QuateTest) {
    const char *src  = "\"\\\\\\\""; // "\\\"
    Lexser lexser;
    lexser.lex(src);
    ASSERT_DEATH(lexser.lex(src);, "");
}