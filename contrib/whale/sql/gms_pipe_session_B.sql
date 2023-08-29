\set ECHO none
\set VERBOSITY terse
--Wait for 'pipe_test_owner' created notification to be sent by session A
SELECT gms_pipe.receive_message('pipe_test_owner_created_notifier');

-- create new connection under the userid of 'pipe_test_owner'
SET SESSION AUTHORIZATION pipe_test_owner PASSWORD 'Test@123';

/* Tests receive_message(text,integer), next_item_type() and all versions of
 *  unpack_message_<type>() and  purge(text)
 */

CREATE OR REPLACE FUNCTION receiveFrom(pipename text) RETURNS void AS $$
DECLARE
        typ INTEGER;
BEGIN
         WHILE true LOOP
                PERFORM gms_pipe.receive_message(pipename,2);
                SELECT gms_pipe.next_item_type() INTO typ;
                IF typ = 0 THEN EXIT;
                ELSIF typ=9 THEN RAISE NOTICE 'RECEIVE %: %', typ, gms_pipe.unpack_message_number();
                ELSIF typ=11 THEN RAISE NOTICE 'RECEIVE %: %', typ, gms_pipe.unpack_message_text();
                ELSIF typ=12 THEN RAISE NOTICE 'RECEIVE %: %', typ, gms_pipe.unpack_message_date();
                ELSIF typ=13 THEN RAISE NOTICE 'RECEIVE %: %', typ, gms_pipe.unpack_message_timestamp();
                ELSIF typ=23 THEN RAISE NOTICE 'RECEIVE %: %', typ, encode(gms_pipe.unpack_message_bytea(),'escape');
                ELSIF typ=24 THEN RAISE NOTICE 'RECEIVE %: %', typ, gms_pipe.unpack_message_record();
                END IF;
        END LOOP;
        PERFORM gms_pipe.purge(pipename);
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION bulkReceive() RETURNS void AS $$
DECLARE
        typ INTEGER;
BEGIN
	IF gms_pipe.receive_message('named_pipe_2',2) = 1 THEN
        	RAISE NOTICE 'Timeout';
        	PERFORM pg_sleep(2);
		PERFORM gms_pipe.receive_message('named_pipe_2',2);
        END IF;
        WHILE true LOOP
                SELECT gms_pipe.next_item_type() INTO typ;
                IF typ = 0 THEN EXIT;
                ELSIF typ=9 THEN RAISE NOTICE 'RECEIVE %: %', typ, gms_pipe.unpack_message_number();
                ELSIF typ=11 THEN RAISE NOTICE 'RECEIVE %: %', typ, gms_pipe.unpack_message_text();
                ELSIF typ=12 THEN RAISE NOTICE 'RECEIVE %: %', typ, gms_pipe.unpack_message_date();
                ELSIF typ=13 THEN RAISE NOTICE 'RECEIVE %: %', typ, gms_pipe.unpack_message_timestamp();
                ELSIF typ=23 THEN RAISE NOTICE 'RECEIVE %: %', typ, encode(gms_pipe.unpack_message_bytea()::bytea,'escape');
                ELSIF typ=24 THEN RAISE NOTICE 'RECEIVE %: %', typ, gms_pipe.unpack_message_record();
                END IF;
        END LOOP;
        PERFORM gms_pipe.purge('named_pipe_2');
END;
$$ LANGUAGE plpgsql;

-- Tests receive_message(text)

CREATE OR REPLACE FUNCTION checkReceive1(pipename text) RETURNS void AS $$
BEGIN
	PERFORM gms_pipe.receive_message(pipename);
	RAISE NOTICE 'RECEIVE %',gms_pipe.unpack_message_text();
END; $$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION dropTempTable() RETURNS void AS $$
BEGIN
	WHILE gms_pipe.receive_message('pipe_name_3') <> 0 LOOP
		CONTINUE;
	END LOOP;
        DROP TABLE TEMP;
END; $$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION checkUniqueSessionNameB() RETURNS bool AS $$
DECLARE
	result bool;
BEGIN
        PERFORM gms_pipe.receive_message('pipe_name_4');
        SELECT gms_pipe.unpack_message_text() = gms_pipe.unique_session_name() INTO result;
	RETURN result;
END; $$ LANGUAGE plpgsql;

\set ECHO all

-- Receives messages sent via an implicit pipe
SELECT receiveFrom('named_pipe');

-- Bulk receive messages
SELECT bulkReceive();

-- Receives messages sent via an explicit private pipe under the same user
-- 'pipe_test_owner'
SELECT gms_pipe.receive_message('recv_private1_notifier');
SELECT receiveFrom('private_pipe_1');

-- Switch user to 'pipe_test_other'
DROP USER IF EXISTS pipe_test_other;
CREATE USER pipe_test_other PASSWORD 'Test@123';
SET SESSION AUTHORIZATION pipe_test_other PASSWORD 'Test@123';

-- Try to receive messages sent via an explicit private pipe under the user
-- 'pipe_test_other' who is not the owner of pipe.
-- insufficient privileges in case of 'private_pipe_2'.

SELECT gms_pipe.receive_message('recv_private2_notifier');
SELECT receiveFrom('private_pipe_2');

-- These are explicit private pipes created using create_pipe(text,integer)
-- and create_pipe(text)
SELECT gms_pipe.receive_message('recv_public1_notifier');
SELECT receiveFrom('public_pipe_3');

SELECT gms_pipe.receive_message('recv_public2_notifier');
SELECT receiveFrom('public_pipe_4');

-- Switch back to user 'pipe_test_owner'
SET SESSION AUTHORIZATION pipe_test_owner PASSWORD 'Test@123';
DROP USER pipe_test_other;

-- Tests receive_message(text)
SELECT checkReceive1('pipe_name_1');
SELECT checkReceive1('pipe_name_2');

-- Tests gms_pipe.db_pipes view
SELECT name, items, "limit", private, owner
FROM gms_pipe.db_pipes
WHERE name LIKE 'private%'
ORDER BY name;

-- Tests gms_pipe.__list_pipes(); attribute size is not included
-- since it can be different across runs.
SELECT name, items, "limit", private, owner
FROM gms_pipe.__list_pipes()  AS  (name varchar, items int4, siz int4, "limit" int4, private bool, owner varchar)
WHERE name <> 'pipe_name_4'
ORDER BY 1;

-- Tests remove_pipe(text)
SELECT gms_pipe.remove_pipe('private_pipe_1');
SELECT gms_pipe.remove_pipe('private_pipe_2');
SELECT gms_pipe.remove_pipe('public_pipe_3');
SELECT gms_pipe.remove_pipe('public_pipe_4');
SELECT gms_pipe.purge('pipe_name_1');
SELECT gms_pipe.purge('pipe_name_2');

-- Receives drop table notification from session A via 'pipe_name_3'
SELECT  dropTempTable();
SELECT gms_pipe.purge('pipe_name_3');


-- tests unique_session_name() (uses 'pipe_name_4')
SELECT checkUniqueSessionNameB();
SELECT gms_pipe.purge('pipe_name_4');

DROP FUNCTION receiveFrom(text);
DROP FUNCTION checkReceive1(text);
DROP FUNCTION checkUniqueSessionNameB();
DROP FUNCTION bulkReceive();
DROP FUNCTION dropTempTable();

-- Perform a recieve on removed pipe resulting on timeout
SELECT gms_pipe.receive_message('public_pipe_4',2);
SELECT gms_pipe.purge('public_pipe_4');

SET SESSION AUTHORIZATION DEFAULT;
DROP USER pipe_test_owner;
