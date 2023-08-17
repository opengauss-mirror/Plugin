select plvstr.normalize('Just   do    it!');

select plvstr.instr('TestABC', 'ABC');
select plvstr.instr('TestABC', 'ABC', 1);
select plvstr.instr('TestABC', 'ABC', 6);
select plvstr.instr('TestABC', 'ABC', 1, 1);
select plvstr.instr('TestABC', 'ABC', 6, 1);
select plvstr.instr('TestABC', 'ABC', -1);
select plvstr.instr('TestABC', 'ABC', -6);
select plvstr.instr('TestABC', 'ABC', -1, 1);
select plvstr.instr('TestABC', 'ABC', -6, 1);

select plvstr.is_prefix('TestABC', 'Test');
select plvstr.is_prefix('TestABC', 'ABC');
select plvstr.is_prefix('TestABC', 'Test', true);
select plvstr.is_prefix('TestABC', 'Test', false);
select plvstr.is_prefix('TestABC', 'ABC', true);
select plvstr.is_prefix('TestABC', 'ABC', false);
select plvstr.is_prefix(123456, 123);
select plvstr.is_prefix(123456, 456);
select plvstr.is_prefix(123456::int8, 123::int8);
select plvstr.is_prefix(123456::int8, 456::int8);

select plvstr.substr('abcdefg', 2);
select plvstr.substr('abcdefg', 2, 3);

select plvstr.lpart('ABC.DEF', '.');
select plvstr.rpart('ABC.DEF', '.');
select plvstr.lpart('ABC.DEF.GHI.JKL.MNO', '.', 5);
select plvstr.rpart('ABC.DEF.GHI.JKL.MNO', '.', 5);
select plvstr.lpart('ABC.DEF.GHI.JKL.MNO', '.', 5, 2);
select plvstr.rpart('ABC.DEF.GHI.JKL.MNO', '.', 5, 2);
select plvstr.lpart('ABCDEFGHIJKLMNO', '.', 5, 2, true);
select plvstr.rpart('ABCDEFGHIJKLMNO', '.', 5, 2, true);
select plvstr.lpart('ABCDEFGHIJKLMNO', '.', 5, 2, false);
select plvstr.rpart('ABCDEFGHIJKLMNO', '.', 5, 2, false);

select plvstr.left('Příliš žluťoučký kůň',4) = pg_catalog.substr('Příl', 1, 4);
select plvstr.right('Příliš žluťoučký kůň',4) = pg_catalog.substr('Příl', 17, 4);

select plvchr.first('ABCDEFG');
select plvchr.last('ABCDEFG');

-- kind: 1 blank, 2 digit, 3 quote, 4 other, 5 letter
select plvchr._is_kind(' ', 1);
select plvchr._is_kind('1', 2);
select plvchr._is_kind(E'\'', 3);
select plvchr._is_kind('%', 4);
select plvchr._is_kind('a', 5);
select plvchr._is_kind(ascii(' '), 1);
select plvchr._is_kind(ascii('1'), 2);
select plvchr._is_kind(ascii(E'\''), 3);
select plvchr._is_kind(ascii('%'), 4);
select plvchr._is_kind(ascii('a'), 5);

select plvstr.lstrip('abc1abc2abc', 'abc');
select plvstr.rstrip('abc1abc2abc', 'abc');
select plvstr.lstrip('abc1abc2abc', 'abc', 2);
select plvstr.rstrip('abc1abc2abc', 'abc', 2);

select plvstr.swap('abcdef', 'ghi');
select plvstr.swap('abcdef', 'ghi', 3);
select plvstr.swap('abcdef', 'ghi', 3, 3);
select plvstr.swap('abcdef', 'ghi', 3, 4);
select plvstr.swap('abcdef', 'ghi', 3, 5);
select plvstr.swap('abcdefghijklmnopq', 'rst', 3, 8);

-- test about SQL_ASCII
DROP DATABASE IF EXISTS instr_test;
CREATE DATABASE instr_test WITH TEMPLATE = template0 ENCODING='SQL_ASCII' LC_COLLATE='C' LC_CTYPE='C';
\c instr_test
CREATE EXTENSION whale;
select plvstr.instr('TestABC', 'ABC');
select plvstr.instr('TestABC', 'ABC', 1);
select plvstr.instr('TestABC', 'ABC', 6);
select plvstr.instr('TestABC', 'ABC', 1, 1);
select plvstr.instr('TestABC', 'ABC', 6, 1);
select plvstr.instr('TestABC', 'ABC', -1);
select plvstr.instr('TestABC', 'ABC', -6);
select plvstr.instr('TestABC', 'ABC', -1, 1);
select plvstr.instr('TestABC', 'ABC', -6, 1);
