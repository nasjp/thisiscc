#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./thisiscc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 'return 0;'
assert 42 'return 42;'
assert 21 'return 5 + 20 -4;'
assert 41 'return 12 + 34 - 5;'
assert 4 'return (3 + 5) / 2;'
assert 10 'return -10 + 20;'
assert 10 'return - - + 10;'

assert 0 'return 0 == 1;'
assert 1 'return 42 == 42;'
assert 1 'return 0 != 1;'
assert 0 'return 42 != 42;'

assert 1 'return 0 < 1;'
assert 0 'return 1 < 1;'
assert 0 'return 2 < 1;'
assert 1 'return 0 <= 1;'
assert 1 'return 1 <= 1;'
assert 0 'return 2 <= 1;'

assert 1 'return 1 > 0;'
assert 0 'return 1 > 1;'
assert 0 'return 1 > 2;'
assert 1 'return 1 >= 0;'
assert 1 'return 1 >= 1;'
assert 0 'return 1 >= 2;'

assert 6 'foo = 1; bar = 2 + 3; return foo + bar;'
assert 1 'return 1; return 2;'

assert 3 'if (0) return 2; return 3;'
assert 2 'if (3 > 2) return 2; else return 3;'
assert 3 'if (3 < 2) return 2; else return 3;'
assert 4 'if (3 < 2) return 2; else if (2 < 1) return 3; else return 4;'
assert 4 'if (3 < 2) return 2; else if (2 < 1) return 3; else return 4;'

assert 10 'i = 0; while (i < 10) i = i + 1; return i;'

assert 55 'j = 0; for (i = 0; i <= 10; i = i + 1) j = i + j; return j;'
assert 3 'for (;;) return 3; return 5;'

assert 3 '{1; {2;} return 3;}'
assert 55 'j = 0; for (i = 0; i <= 10; i = i + 1) {tmp = i + j; j = tmp;} return j;'


echo OK
