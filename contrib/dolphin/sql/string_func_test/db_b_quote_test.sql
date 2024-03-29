create schema db_b_quote_test;
set current_schema to 'db_b_quote_test';

SELECT QUOTE(E'Don\'t!');
SELECT QUOTE(E'O\'hello');
SELECT QUOTE('O\hello');

SELECT QUOTE(char(89));
SELECT QUOTE(char(224));

drop schema db_b_quote_test cascade;
reset current_schema;