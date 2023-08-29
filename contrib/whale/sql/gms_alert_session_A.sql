\set ECHO all

SELECT pg_sleep(3);

/* 
 * GMS_ALERT is used for one-way communication of one session to other.
 *
 * This session mainly sends signals for testing the alert functionality in 
 * session B and C.
 *
 * The following alerts are used to ensure that signals are sent at correct 
 * times to session B for testing. These signals are sent from session B 
 * indicating completion of an event. 
 * After the signal is received, the next required signal for testing is sent
 * from this session. 
 */

SELECT gms_alert.register('b1');
SELECT gms_alert.register('b2');
SELECT gms_alert.register('b3');
SELECT gms_alert.register('b4');
SELECT gms_alert.register('b5');

SELECT gms_alert.signal('a1','Msg1 for a1');

SELECT gms_alert.signal('a2','Msg1 for a2');

/* 
 * Test: defered_signal 
 * The signal is received only when the signalling transaction commits.
 * To test this, an explict BEGIN-COMMIT block is used. 
 */ 
SELECT gms_alert.signal('tds','Begin defered_signal test');
BEGIN;
SELECT gms_alert.signal('tds','Testing defered_signal');
/* The signal is received while transaction is running */
SELECT gms_alert.waitone('b1',20);
COMMIT;
/* The signal is received after transaction completed.
 * After this the tds signal is received in session B indicating that the
 * signal is received only after commit.
 */
SELECT gms_alert.waitone('b1',20);

SELECT gms_alert.waitone('b2',20);
/* This signals a3 which is not registered in Session B */
SELECT gms_alert.signal('a3','Msg1 for a3');
/* alert a4 is signalled soon after a3 */
SELECT gms_alert.signal('a4','Test- Register after signal');

/* This signal indicates at remove() is called */
SELECT gms_alert.waitone('b3',20);
/* Send signal which is removed in session B */
SELECT gms_alert.signal('a1','Msg2 for a1');

SELECT gms_alert.waitone('b4',20);
/* Send signal which is registered in B and not removed */
SELECT gms_alert.signal('a4','Msg1 for a4');

/* This signal inidcates that removeall() is called */
SELECT gms_alert.waitone('b5',20);
/* Send a signal to test if session B receives it after removeall() */
SELECT gms_alert.signal('a2','Msg2 for a2');

/* cleanup */
SELECT gms_alert.removeall();
