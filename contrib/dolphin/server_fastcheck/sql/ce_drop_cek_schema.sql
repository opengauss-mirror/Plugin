\! gs_ktool -d all
\! gs_ktool -g
\! gs_ktool -g

CREATE SCHEMA drop_cmk_test;
CREATE CLIENT MASTER KEY test_drop_cmk1 WITH ( KEY_STORE = gs_ktool , KEY_PATH = "gs_ktool/1" , ALGORITHM = AES_256_CBC);
CREATE CLIENT MASTER KEY drop_cmk_test.test_drop_cmk2 WITH ( KEY_STORE = gs_ktool , KEY_PATH = "gs_ktool/2" , ALGORITHM = AES_256_CBC);
CREATE COLUMN ENCRYPTION KEY test_drop_cek1 WITH VALUES (CLIENT_MASTER_KEY = test_drop_cmk1, ALGORITHM = AEAD_AES_256_CBC_HMAC_SHA256);
CREATE COLUMN ENCRYPTION KEY drop_cmk_test.test_drop_cek2 WITH VALUES (CLIENT_MASTER_KEY = test_drop_cmk1, ALGORITHM = AEAD_AES_256_CBC_HMAC_SHA256);
DROP COLUMN ENCRYPTION KEY test_drop_cek1;
SELECT COUNT(*) FROM gs_column_keys;
DROP COLUMN ENCRYPTION KEY test_drop_cek2;
SELECT COUNT(*) FROM gs_column_keys;
DROP COLUMN ENCRYPTION KEY drop_cmk_test.test_drop_cek2;
SELECT COUNT(*) FROM gs_column_keys;
DROP CLIENT MASTER KEY test_drop_cmk1;
SELECT COUNT(*) FROM gs_client_global_keys;
DROP CLIENT MASTER KEY test_drop_cmk2;
SELECT COUNT(*) FROM gs_client_global_keys;
DROP CLIENT MASTER KEY drop_cmk_test.test_drop_cmk2;
SELECT COUNT(*) FROM gs_client_global_keys;
DROP SCHEMA drop_cmk_test;

\! gs_ktool -d all
