set dolphin.b_compatibility_mode = on;

select * from information_schema.attributes limit 1;
select * from information_schema.columns limit 1;
select * from information_schema.domains limit 1;
select * from information_schema.element_types limit 1;
select * from information_schema.key_column_usage limit 1;
select * from information_schema.parameters limit 1;

reset dolphin.b_compatibility_mode;
