drop database if exists b_databaselock;
create database b_databaselock dbcompatibility 'b';
\c b_databaselock

select get_lock('βάση δεδομένων');
select get_lock('Η αστυνομία');
select get_lock('Η ενσάρκωση του φωτός');
select get_lock('Χυμός μπακαλάρου θάλασσας');
select get_lock('Δαιμονισμένος από ψυχές δράκων');
select lockname from get_all_locks() order by lockname;

select is_free_lock('βάση δεδομένων');
select is_free_lock('Η αστυνομία');
select is_free_lock('Η ενσάρκωση του φωτός');
select is_free_lock('Χυμός μπακαλάρου θάλασσας');
select is_free_lock('Δαιμονισμένος από ψυχές δράκων');

select release_lock('βάση δεδομένων');
select release_lock('Η αστυνομία');
select release_lock('Η ενσάρκωση του φωτός');
select release_lock('Χυμός μπακαλάρου θάλασσας');
select release_lock('Δαιμονισμένος από ψυχές δράκων');
\c postgres
drop database if exists b_databaselock;
