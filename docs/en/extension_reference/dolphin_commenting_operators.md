# Comment Operators

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-07-30T06:20:32.749Z pushedAt=2026-09-11T12:10:30.551Z -->

Compared with the original openGauss, the main modifications to comment operators in dolphin are as follows:

1. Added support for using ```#``` to indicate the start of a single-line comment. For example, ```# select a from t1``` represents a comment.

- Single-line comments starting with #

  Description: Treats the content from # to the newline character as comment information.

  Note: This operator is intended for B-compatible databases and has different semantics from the original # operator of openGauss. To use this semantics, create a B-compatibility mode database, enable the MySQL-compatible SQL engine plugin, and set dolphin.b_compatibility_mode to TRUE.

  Example:

  ```
  openGauss=# SELECT lower('TOM') #1234;
  openGauss=# ;
  lower
  -------
  tom
  ```