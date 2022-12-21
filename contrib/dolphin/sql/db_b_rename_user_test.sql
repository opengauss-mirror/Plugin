create schema db_b_rename_user_test;
set current_schema to 'db_b_rename_user_test';

CREATE USER user1 WITH ENCRYPTED PASSWORD 'user1@1234';
CREATE USER user2 WITH ENCRYPTED PASSWORD 'user2@1234';
CREATE USER user3 WITH ENCRYPTED PASSWORD 'user3@1234';

select usename from pg_catalog.pg_user 
    where usename='user1' or usename='user2' or usename='user3';

-- rename to other name
rename user 
    user1 to user4, 
    user2 to user5,
    user3 to user6;

select usename from pg_catalog.pg_user 
    where usename='user1' or usename='user2' or usename='user3';

select usename from pg_catalog.pg_user 
    where usename='user4' or usename='user5' or usename='user6';

-- rename rollback
rename user 
    user4 to user1, 
    user5 to user2,
    user7 to user3;

select usename from pg_catalog.pg_user 
    where usename='user4' or usename='user5' or usename='user6';


drop user user4;
drop user user5;
drop user user6;

drop schema db_b_rename_user_test cascade;
reset current_schema;