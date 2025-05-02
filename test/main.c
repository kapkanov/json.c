#include "../json.c"
#include <stdio.h>


U32 cmp16(const U16 src[], const U16 dst[]) {
  U32 j;

  for (j = 0; src[j] && dst[j] && src[j] == dst[j]; j++);

  return src[j] == '\0' && dst[j] == '\0';
}


U32 cmp16len(const U16 src[], const U16 dst[], const U32 len) {
  U32 j;

  for (j = 0; j < len && src[j] == dst[j]; j++);

  return j == len;
}


U32 cmp816len(const U8 src[], const U16 dst[], const U32 len) {
  U32 j;
  U16 tmp;

  for (j = 0; j < len && src[j] == dst[j]; j++);

  return j == len;
}

#define BUFLEN 100
const U32 CONST_BUFLEN = BUFLEN;

I32 main(void) {
  U8  buf[BUFLEN];
  U16 buf16[BUFLEN];
  I32 number;
  F32 fnumber;
  U16 cmpbuf[BUFLEN];

  assert(jparse_int("42", &number)  ==  2 && number == 42, "jparse_int(\"42\") failed");
  assert(jparse_int("-42", &number) == 3 && number == -42, "jparse_int(\"-42\") failed");
  assert(jparse_int("0", &number)   ==   1 && number == 0, "jparse_int(\"0\") failed");
  assert(jparse_int("-0", &number)  ==   2 && number == 0, "jparse_int(\"-0\") failed");

  assert(jparse_boolean("true",  5, &number) == 4 && number == 1, "jparse_boolean(\"true\") failed");
  assert(jparse_boolean("false", 6, &number) == 5 && number == 0, "jparse_boolean(\"false\") failed");

  assert(jparse_float("3.14", 5, &fnumber) == 4 && fnumber - 3.14 < 0.1, "jparse_float(\"3.14\") failed");
  assert(jparse_float("-3.14", 6, &fnumber) == 5 && fnumber + 3.14 < 0.1, "jparse_float(\"-3.14\") failed");
  assert(jparse_float("1e10", 5, &fnumber) == 4 && fnumber - 10000000000 < 0.1, "jparse_float(\"1e10\") failed");
  assert(jparse_float("-1e10", 6, &fnumber) == 5 && fnumber + 10000000000 < 0.1, "jparse_float(\"-1e10\") failed");
  assert(jparse_float("1.23e-4", 8, &fnumber) == 7 && fnumber - 0.000123 < 0.1, "jparse_float(\"1.23e-4\") failed");

  assert(jparse_string("\"hello\"", 8, buf16, CONST_BUFLEN) == 7 && cmp816len("hello", buf16, 5), "jparse_string(\"hello\") failed");
  assert(jparse_string("\"\"", 3, buf16, CONST_BUFLEN) == 2 && cmp816len("", buf16, 0), "jparse_string(\"\") failed");
  assert(jparse_string("\"\\\"\\/\\b\\f\\n\\r\\t\"", 17, buf16, CONST_BUFLEN) == 16 && cmp816len("\"/\b\f\n\r\t", buf16, 7), "jparse_string(\"\\\"\\/\\b\\f\\n\\r\\t\") failed");
  cmpbuf[0] = 0x41; cmpbuf[1] = 0x5a; cmpbuf[2] = 0;
  assert(jparse_string("\"\\u0041\\u005a\"", 15, buf16, CONST_BUFLEN) == 14 && cmp16len(cmpbuf, buf16, 3), "jparse_string(\"\\u0041\\u005a\") failed");
  cmpbuf[0] = 0xd83d; cmpbuf[1] = 0xde00; cmpbuf[2] = 0;
  assert(jparse_string("\"\\ud83d\\ude00\"", 15, buf16, CONST_BUFLEN) == 14 && cmp16len(cmpbuf, buf16, 3), "jparse_string(\"\\ud83d\\ude00\") failed");

  assert(jparse_null("null", 5, &number) == 4 && number == 0, "jparse_null(\"null\") failed");

  return 0;
}
