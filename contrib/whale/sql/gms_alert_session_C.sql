\set ECHO all

/* Register alerts */
SELECT gms_alert.register('a1');
SELECT gms_alert.register('a2');

/* Test: multisession waitone */
SELECT gms_alert.waitone('a1',20);

/* Test: multisession waitany */
SELECT gms_alert.waitany(10);

/* cleanup */
SELECT gms_alert.removeall();

