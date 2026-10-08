# dolphin-Operator Precedence

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-07-30T06:20:52.849Z pushedAt=2026-09-12T03:37:39.545Z -->

Operators are listed from highest to lowest precedence as follows (operators on the same line have the same precedence):

```
COLLATE
OR_OR_SYM
^
*, /, %
+, -
IS
ISNULL
NOTNULL
Op, OPERATOR '@'
REGEXP, RLIKE
DIV, MOD
IN_P, NOT_IN
BETWEEN, NOT_BETWEEN
LIKE, NOT_LIKE
BINARY
'=', '<', '>', CmpOp ('!=', '<>', '<=', '>=', '^=')
CmpNullOp ('<=>'), COLON_EQUALS (':=')
NOT
AND
XOR
OR
```

- When ```dolphin.b_compatibility_mode``` is ```TRUE```, ```PIPES_AS_CONCAT``` is enabled by default, and ```||``` becomes ```OR_OR_SYM```, serving as string concatenation; when ```PIPES_AS_CONCAT``` is disabled, ```||``` becomes the logical ```OR``` operator.