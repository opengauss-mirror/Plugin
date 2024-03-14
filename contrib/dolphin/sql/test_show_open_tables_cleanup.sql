-- cleanup
\c contrib_regression
set dolphin.b_compatibility_mode to off;
drop database show_open_tables_b;
drop database show_open_tables_nb;
drop user user1;
drop user user2;