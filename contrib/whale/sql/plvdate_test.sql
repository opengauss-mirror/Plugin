select plvdate.add_bizdays('2022-11-11', 3);
select plvdate.add_bizdays('2022-11-11', 5);
select plvdate.add_bizdays('2022-11-11', 10);

select plvdate.nearest_bizday('2022-11-11');
select plvdate.nearest_bizday('2022-11-15');
select plvdate.nearest_bizday('2022-11-17');
select plvdate.nearest_bizday('2022-12-17');

select plvdate.next_bizday('2022-11-11');
select plvdate.next_bizday('2022-11-15');
select plvdate.next_bizday('2022-11-17');
select plvdate.next_bizday('2022-12-17');

select plvdate.bizdays_between('2022-11-11', '2022-12-17');

select plvdate.prev_bizday('2022-11-11');
select plvdate.prev_bizday('2022-11-15');
select plvdate.prev_bizday('2022-11-17');
select plvdate.prev_bizday('2022-12-17');

select plvdate.isbizday('2022-11-11');
select plvdate.isbizday('2022-11-15');
select plvdate.isbizday('2022-11-17');
select plvdate.isbizday('2022-12-17');
select plvdate.isbizday('2022-11-14');
select plvdate.isbizday('2022-11-16');
select plvdate.isbizday('2022-11-21');

select plvdate.set_nonbizday('Sunday');
select plvdate.set_nonbizday('Monday');
select plvdate.unset_nonbizday('Sunday');
select plvdate.unset_nonbizday('Monday');

select plvdate.set_nonbizday('2022-12-11'::"date");
select plvdate.set_nonbizday('2022-12-12'::"date");
select plvdate.set_nonbizday('2022-12-13'::"date", false);
select plvdate.set_nonbizday('2022-12-14'::"date", true);
select plvdate.unset_nonbizday('2022-12-11'::"date");
select plvdate.unset_nonbizday('2022-12-14'::"date", false);
select plvdate.unset_nonbizday('2022-12-14'::"date", true);
select plvdate.add_bizdays('2022-12-12'::"date");

select plvdate.use_easter();
select plvdate.using_easter();
select plvdate.use_easter(true);
select plvdate.using_easter();
select plvdate.use_easter(false);
select plvdate.using_easter();
select plvdate.unuse_easter();
select plvdate.using_easter();

select plvdate.use_great_friday();
select plvdate.using_great_friday();
select plvdate.use_great_friday(true);
select plvdate.using_great_friday();
select plvdate.use_great_friday(false);
select plvdate.using_great_friday();
select plvdate.unuse_great_friday();
select plvdate.using_great_friday();



