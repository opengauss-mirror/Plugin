\set ECHO all

/* Register alerts */
SELECT gms_alert.register('a1');
SELECT gms_alert.register('a2');
SELECT gms_alert.register('tds');

/* Test: multisession waitone */
SELECT gms_alert.waitone('a1',20);

/* Test: multisession waitany */
SELECT gms_alert.waitany(10);

/* Test defered_signal */
/* This indicated that the transaction has begun */
SELECT gms_alert.waitone('tds',10);
/* The signal will not be received because the transaction is running */
SELECT gms_alert.waitone('tds',2);
SELECT gms_alert.signal('b1','Transaction still running');
SELECT gms_alert.signal('b1','Transaction committed');
/* Since the transaction has commited, the signal will be received */
SELECT gms_alert.waitone('tds',10);

/* Signal session A to send msg1 for a3 */
SELECT gms_alert.signal('b2','to check unregistered alert wait');
/* Test: wait for unregistered alert which is signaled*/
SELECT gms_alert.waitone('a3',2);

/* Test: Register after alert is signaled and wait */
SELECT gms_alert.register('a4');
SELECT gms_alert.waitone('a4',2);

/* Test: remove one */
SELECT gms_alert.remove('a1');
/* Signal session A to send msg2 for a1 */
SELECT gms_alert.signal('b3','remove(a1) called');
/* Test: wait for removed alert */
SELECT gms_alert.waitone('a1',2);
/* Signal session A to send msg1 for a4 */
SELECT gms_alert.signal('b4','to check unremoved alert');
/* Test: Check if unremoved alert is received */
SELECT gms_alert.waitone('a4',10);

/* Test removeall */
SELECT gms_alert.removeall();
/* Signal session A to send msg2 for a2 */
SELECT gms_alert.signal('b5','removeall called');
/* Test: Use waitany to see if any alert is received */
SELECT gms_alert.waitany(2);
