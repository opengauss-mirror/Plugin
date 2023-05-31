create schema b_signal_resignal;
set current_schema to 'b_signal_resignal';

create procedure p_resignal1() resignal;
call p_resignal1();
show warnings;

create procedure p_resignal2() resignal set MESSAGE_TEXT = 'this is resignal statement', MYSQL_ERRNO = 1234;
call p_resignal2();
show warnings;

create procedure p_resignal3() resignal sqlstate '42000' set MESSAGE_TEXT = 'this is resignal statement', MYSQL_ERRNO = 1234;
call p_resignal3();
show warnings;

create procedure p_resignal4(a text, b int) resignal SQLSTATE '42000' set MESSAGE_TEXT = a, MYSQL_ERRNO = b;
call p_resignal4('this is resignal error', 1234);
show warning;

create procedure p_resignal5() resignal sqlstate '42000' set MESSAGE_TEXT = 'this is resignal statement', MYSQL_ERRNO = 1234,
        CLASS_ORIGIN = 'ISO 9075', SUBCLASS_ORIGIN = 'ISO 9075', CONSTRAINT_NAME = 'constraint_name',
        CONSTRAINT_CATALOG = 'constraint_catalog', CONSTRAINT_SCHEMA = 'constraint_schema',
        CATALOG_NAME = 'catalog_name', SCHEMA_NAME = 'schema_name', TABLE_NAME = 'table_name',
        COLUMN_NAME = 'column name', CURSOR_NAME = 'cursor name';;
call p_resignal5();
show warnings;

create procedure p_signal1() signal SQLSTATE '42000';
call p_signal1();
show warnings;

create procedure p_signal2() signal SQLSTATE '42000' set MESSAGE_TEXT = 'this is signal statement', MYSQL_ERRNO = 1234;
call p_signal2();
show warnings;

create procedure p_signal3(a text, b int) resignal sqlstate '42000' set MESSAGE_TEXT = a, MYSQL_ERRNO = b;
call p_signal3('this is signal error', 1234);
show warnings;

create procedure p_signal4() resignal sqlstate '42000' set MESSAGE_TEXT = 'this is signal statement', MYSQL_ERRNO = 1234,
        CLASS_ORIGIN = 'ISO 9075', SUBCLASS_ORIGIN = 'ISO 9075', CONSTRAINT_NAME = 'constraint_name',
        CONSTRAINT_CATALOG = 'constraint_catalog', CONSTRAINT_SCHEMA = 'constraint_schema',
        CATALOG_NAME = 'catalog_name', SCHEMA_NAME = 'schema_name', TABLE_NAME = 'table_name',
        COLUMN_NAME = 'column name', CURSOR_NAME = 'cursor name';;
call p_signal4();
show warnings;

drop schema b_signal_resignal cascade;
reset current_schema;