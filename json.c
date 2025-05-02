#include "./includes/types32.c"
#include "./includes/limits32.c"


I32 cmp8len(const U8 src[], const U8 dst[], const U32 len) {
  U32 j;

  for (j = 0; j < len && src[j] == dst[j]; j++);

  return j == len;
}


void strhex2char(const U8 str[], U8 dst[]) {
  U32 j;
  U8  table[103];

  for (j = '0'; j <= '9'; j++)
    table[j] = j - '0';
  for (j = 'A'; j <= 'F'; j++)
    table[j] = j - 'A' + 10;
  for (j = 'a'; j <= 'f'; j++)
    table[j] = j - 'a' + 10;

  for (j = 0; j < 4; j++)
    assert('0' <= str[j] && str[j] <= '9' || 'A' <= str[j] && str[j] <= 'F' || 'a' <= str[j] && str[j] <= 'f', "strhex2char: character '%c' in string '%.20s' is not valid hex digit", str[j], str);

  dst[0] = 0 | table[str[0]] << 4 | table[str[1]];
  dst[1] = 0 | table[str[2]] << 4 | table[str[3]];
}


void strhex2short(const U8 str[], U16 dst[]) {
  U32 j;
  U8  table[103];

  for (j = '0'; j <= '9'; j++)
    table[j] = j - '0';
  for (j = 'A'; j <= 'F'; j++)
    table[j] = j - 'A' + 10;
  for (j = 'a'; j <= 'f'; j++)
    table[j] = j - 'a' + 10;

  for (j = 0; j < 4; j++)
    assert('0' <= str[j] && str[j] <= '9' || 'A' <= str[j] && str[j] <= 'F' || 'a' <= str[j] && str[j] <= 'f', "strhex2char: character '%c' in string '%.20s' is not valid hex digit", str[j], str);

  dst[0] = 0 | table[str[0]] << 12 | table[str[1]] << 8 | table[str[2]] << 4 | table[str[3]];
}


U32 jparse_string(const U8 src[], const U32 srclen, U16 dst[], const U32 dstlen) {
  U32 j, k;

  assert(src[0] == '"', "jparse_string: string should start from a '\"' character. But it start with %.20s", src);

  for (j = 1, k = 0; j < srclen && src[j] && src[j] != '"'; j++, k++) {
    assert(k < dstlen, "String \"%.20s...\" is too big", src);
    if (src[j] != '\\') {
      dst[k] = src[j];
      continue;
    }
    switch (src[j + 1]) {
    case '"':
      dst[k] = '"';
      break;
    case '\\':
      dst[k] = '\\';
      break;
    case '/':
      dst[k] = '/';
      break;
    case 'b':
      dst[k] = '\b';
      break;
    case 'f':
      dst[k] = '\f';
      break;
    case 'n':
      dst[k] = '\n';
      break;
    case 'r':
      dst[k] = '\r';
      break;
    case 't':
      dst[k] = '\t';
      break;
    case 'u':
      assert(k + 1 < dstlen, "jparse_string: String %.20s is too long", src);
      strhex2short(src + j + 2, dst + k);
      j += 4;
      break;
    default:
      j--;
    }
    j++;
  }
  assert(k < dstlen, "There are not enough space in buffer");
  dst[k] = 0;

  assert(src[j] == '"', "jparse_string: String should end on the double quote \" character");

  return j + 1;
}


I32 is_whitespace(const U8 c) {
  return c == ' ' || c == '\t' || c == '\n';
}


U32 jparse_int(const U8 src[], I32 *res) {
        U32 j;
        I32 sign;
  const I32 RES_LIMIT = I32_MAX / 10;

  j    = 0;
  sign = 1;

  if (src[0] == '-') {
    sign = -1;
    j    =  1;
  }

  for (*res = 0; '0' <= src[j] && src[j] <= '9'; j++) {
    assert(*res < RES_LIMIT && *res * 10 < I32_MAX - (src[j] - '0'), "jparse_int: The number is too big for I32. The first parsed part is %d", *res);
    *res = *res * 10 + src[j] - '0';
  }

  *res *= sign;

  return j;
}


U32 jparse_boolean(const U8 src[], const U32 srclen, I32 *res) {
  if (srclen > 3 && cmp8len(src, "true", 4)) {
    *res = 1;
    return 4;
  }
  if (srclen > 4 && cmp8len(src, "false", 5)) {
    *res = 0;
    return 5;
  }

  assert(0, "jparse_boolean: %.4s is not a valid boolean", src);
}


U32 jparse_float(const U8 src[], const U32 srclen, F32 *res) {
  U32 j;
  F32 sign, fraction, divider;
  U32 expsign, exp;

  j    = 0;
  sign = 1.0;

  if (src[j] == '-') {
    sign = -1.0;
    j    =  1;
  }

  for (*res = 0; '0' <= src[j] && src[j] <= '9'; j++) {
    *res = *res * 10.0 + (F32)(src[j] - '0');
  }

  if (src[j] == '.') {
    fraction     = 0.0;
    divider = 1.0;
    j++;
    for (; '0' <= src[j] && src[j] <= '9'; j++) {
      fraction = fraction * 10.0 + (F32)(src[j] - '0');
      divider *= 10.0;
    }
    *res += fraction / divider;
  }
  
  if (src[j] == 'e') {
    expsign  = 1;
    exp      = 0;

    if (j + 1 < srclen && src[j + 1] == '-') {
      expsign = 0;
      j++;
    }

    for (j++; j < srclen && '0' <= src[j] && src[j] <= '9'; j++) {
      exp = exp * 10 + src[j] - '0';
    }

    if (expsign) {
      for (; exp > 0; exp--)
        *res *= 10.0;
    } else {
      for (; exp > 0; exp--)
        *res /= 10.0;
    }
  }

  *res *= sign;

  return j;
}


U32 jparse_null(const U8 src[], const U32 srclen, I32 *res) {
  U32 j;

  *res = 1;

  for (j = 0; j < srclen && is_whitespace(src[j]); j++);

  assert(srclen > 4 + j, "jparse_null: not enough characters for null value");

  assert(src[j] == 'n' && src[j+1] == 'u' && src[j+2] == 'l' && src[j+3] == 'l', "jparse_null: expected null, but got %.20s", src + j);

  *res = 0;

  return j + 4;
}
