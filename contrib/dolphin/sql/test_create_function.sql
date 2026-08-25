drop function if exists db_replace;
truncate table DBE_PLDEVELOPER.gs_source;
select owner, nspid, name, type, status, src from DBE_PLDEVELOPER.gs_source limit 1;
set check_function_bodies=off;
delimiter //
-- expected success
CREATE PROCEDURE `db_replace`(
    IN `search` varchar(100),
    IN `replacement` varchar(100),
    IN `db_name` varchar(100)
)
BEGIN
    DECLARE t_name VARCHAR(50);
    DECLARE done int default 0;
    DECLARE cur CURSOR FOR SELECT DISTINCT table_name as name FROM INFORMATION_SCHEMA.tables WHERE table_schema=db_name;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = 1;
    OPEN cur;
    FETCH cur INTO t_name;
        WHILE (done<>1) DO
            call tbl_replace(search,replacement,db_name,t_name);
            FETCH cur INTO t_name;
        END WHILE;
END
//

delimiter ;
select owner, nspid, name, type, status, src from DBE_PLDEVELOPER.gs_source limit 1;
select count(*) from DBE_PLDEVELOPER.gs_source;

drop function if exists db_replace;
truncate table DBE_PLDEVELOPER.gs_source;
select owner, nspid, name, type, status, src from DBE_PLDEVELOPER.gs_source limit 1;
set check_function_bodies=on;
show check_function_bodies;
delimiter //
-- expected failure
CREATE OR REPLACE PROCEDURE `db_replace`(
    IN `search` varchar(100),
    IN `replacement` varchar(100),
    IN `db_name` varchar(100)
)
BEGIN
    DECLARE t_name VARCHAR(50);
    DECLARE done int default 0;
    DECLARE cur CURSOR FOR SELECT DISTINCT table_name as name FROM INFORMATION_SCHEMA.tables WHERE table_schema=db_name;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = 1;
    OPEN cur;
    FETCH cur INTO t_name;
        WHILE (done<>1) DO
            call tbl_replace(search,replacement,db_name,t_name);
            FETCH cur INTO t_name;
        END WHILE;
END
//
delimiter ;
select owner, nspid, name, type, status, src from DBE_PLDEVELOPER.gs_source limit 1;

truncate table DBE_PLDEVELOPER.gs_source;
select owner, nspid, name, type, status, src from DBE_PLDEVELOPER.gs_source limit 1;
drop function if exists db_replace;
set check_function_bodies=off;
delimiter //
-- expected success
CREATE OR REPLACE PROCEDURE `db_replace`(
    IN `search` varchar(100),
    IN `replacement` varchar(100),
    IN `db_name` varchar(100)
)
BEGIN
    DECLARE t_name VARCHAR(50);
    DECLARE done int default 0;
    DECLARE cur CURSOR FOR SELECT DISTINCT table_name as name FROM INFORMATION_SCHEMA.tables WHERE table_schema=db_name;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = 1;
    OPEN cur;
    FETCH cur INTO t_name;
        WHILE (done<>1) DO
            call tbl_replace(search,replacement,db_name,t_name);
            FETCH cur INTO t_name;
        END WHILE;
END
//

delimiter ;
select owner, nspid, name, type, status, src from DBE_PLDEVELOPER.gs_source limit 1;

-- verify by checking if gs_source has records after procedure creation
drop function if exists db_replace;
truncate table DBE_PLDEVELOPER.gs_source;
select owner, nspid, name, type, status, src from DBE_PLDEVELOPER.gs_source limit 1;
set check_function_bodies=off;
delimiter //
-- expected success
CREATE PROCEDURE `db_replace`(
    IN `search` varchar(100),
    IN `replacement` varchar(100),
    IN `db_name` varchar(100)
)
BEGIN
    DECLARE t_name VARCHAR(50);
    DECLARE done int default 0;
    DECLARE cur CURSOR FOR SELECT DISTINCT table_name as name FROM INFORMATION_SCHEMA.tables WHERE table_schema=db_name;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = 1;
    OPEN cur;
    FETCH cur INTO t_name;
        WHILE (done<>1) DO
            call tbl_replace(search,replacement,db_name,t_name);
            FETCH cur INTO t_name;
        END WHILE;
END
//

delimiter ;
select owner, nspid, name, type, status, src from DBE_PLDEVELOPER.gs_source limit 1;
select count(*) from DBE_PLDEVELOPER.gs_source;


drop function if exists db_replace;
truncate table DBE_PLDEVELOPER.gs_source;
select owner, nspid, name, type, status, src from DBE_PLDEVELOPER.gs_source limit 1;
set check_function_bodies=on;
delimiter //
-- expected failure
CREATE PROCEDURE `db_replace`(
    IN `search` varchar(50),
    IN `replacement` varchar(50),
    IN `db_name` varchar(50)
)
BEGIN
    DECLARE t_name VARCHAR(50);
    DECLARE done int default 0;
    DECLARE cur CURSOR FOR SELECT DISTINCT table_name as name FROM INFORMATION_SCHEMA.tables WHERE table_schema=db_name;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = 1;
    OPEN cur;
    FETCH cur INTO t_name;
        WHILE (done<>1) DO
            call tbl_replace(search,replacement,db_name,t_name);
            FETCH cur INTO t_name;
        END WHILE;
END
//

delimiter ;
select owner, nspid, name, type, status, src from DBE_PLDEVELOPER.gs_source limit 1;
select count(*) from DBE_PLDEVELOPER.gs_source;


-- test calling subprocedure from main procedure fails
drop function if exists sp_Parent;
drop function if exists sp_Child;
set check_function_bodies=off;
-- just create the parent procedure
-- expected success

DELIMITER //

CREATE PROCEDURE sp_Parent()
BEGIN
    SELECT 'Main procedure started' AS 'Status';
    CALL sp_Child();  -- call the child procedure
    SELECT 'Main procedure ended' AS 'Status';
END //

DELIMITER ;

-- directly call the parent procedure, expected failure since child procedure is not yet created
call sp_Parent();

-- create the child procedure
DELIMITER //

CREATE PROCEDURE sp_Child()
BEGIN
    SELECT 'Child procedure called!' AS 'Message';
END //

DELIMITER ;
-- call the parent procedure again, now expected success since child procedure exists
call sp_Parent();