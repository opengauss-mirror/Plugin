select '101111101'::datetime;
select '2010117.5678'::datetime;
select '2010117.1234'::datetime;

select '101111101'::timestamp;
select '2010117.5678'::timestamp;
select '2010117.1234'::timestamp;

set dolphin.b_compatibility_mode to on;

select '2022-01-01'::bit(64)::datetime;
select '2022-01-01'::bit(64)::timestamp;
select 8385958.999999::bit(64)::time;