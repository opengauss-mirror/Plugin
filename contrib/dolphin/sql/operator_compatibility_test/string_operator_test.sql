drop schema if exists string_operator_test_schema cascade;
create schema string_operator_test_schema;
set current_schema to 'string_operator_test_schema';
---------- head ----------
set dolphin.b_compatibility_mode to on;
drop table if exists test_string_table;
CREATE TABLE test_string_table
(
    `char` char(100),
    `varchar` varchar(100),
    `binary` binary(100),
    `varbinary` varbinary(100),
    `tinyblob` tinyblob,
    `blob` blob,
    `mediumblob` mediumblob,
    `longblob` longblob,
    `text` text,
    `enum_t` enum('a', 'b', 'c'),
    `set_t` set('a', 'b', 'c'),
    `json` json
);

-- type test
drop table if exists test_string_type;
CREATE TABLE test_string_type AS SELECT
`char` + `char` AS `char+char`,
`char` - `char` AS `char-char`,
`char` * `char` AS `char*char`,
`char` / `char` AS `char/char`,
`char` + `varchar` AS `char+varchar`,
`char` - `varchar` AS `char-varchar`,
`char` * `varchar` AS `char*varchar`,
`char` / `varchar` AS `char/varchar`,
`char` + `binary` AS `char+binary`,
`char` - `binary` AS `char-binary`,
`char` * `binary` AS `char*binary`,
`char` / `binary` AS `char/binary`,
`char` + `varbinary` AS `char+varbinary`,
`char` - `varbinary` AS `char-varbinary`,
`char` * `varbinary` AS `char*varbinary`,
`char` / `varbinary` AS `char/varbinary`,
`char` + `tinyblob` AS `char+tinyblob`,
`char` - `tinyblob` AS `char-tinyblob`,
`char` * `tinyblob` AS `char*tinyblob`,
`char` / `tinyblob` AS `char/tinyblob`,
`char` + `mediumblob` AS `char+mediumblob`,
`char` - `mediumblob` AS `char-mediumblob`,
`char` * `mediumblob` AS `char*mediumblob`,
`char` / `mediumblob` AS `char/mediumblob`,
`char` + `longblob` AS `char+longblob`,
`char` - `longblob` AS `char-longblob`,
`char` * `longblob` AS `char*longblob`,
`char` / `longblob` AS `char/longblob`,
`char` + `text` AS `char+text`,
`char` - `text` AS `char-text`,
`char` * `text` AS `char*text`,
`char` / `text` AS `char/text`,
`char` + `enum_t` AS `char+enum_t`,
`char` - `enum_t` AS `char-enum_t`,
`char` * `enum_t` AS `char*enum_t`,
`char` / `enum_t` AS `char/enum_t`,
`char` + `set_t` AS `char+set_t`,
`char` - `set_t` AS `char-set_t`,
`char` * `set_t` AS `char*set_t`,
`char` / `set_t` AS `char/set_t`,
`char` + `json` AS `char+json`,
`char` - `json` AS `char-json`,
`char` * `json` AS `char*json`,
`char` / `json` AS `char/json`,
`varchar` + `char` AS `varchar+char`,
`varchar` - `char` AS `varchar-char`,
`varchar` * `char` AS `varchar*char`,
`varchar` / `char` AS `varchar/char`,
`varchar` + `varchar` AS `varchar+varchar`,
`varchar` - `varchar` AS `varchar-varchar`,
`varchar` * `varchar` AS `varchar*varchar`,
`varchar` / `varchar` AS `varchar/varchar`,
`varchar` + `binary` AS `varchar+binary`,
`varchar` - `binary` AS `varchar-binary`,
`varchar` * `binary` AS `varchar*binary`,
`varchar` / `binary` AS `varchar/binary`,
`varchar` + `varbinary` AS `varchar+varbinary`,
`varchar` - `varbinary` AS `varchar-varbinary`,
`varchar` * `varbinary` AS `varchar*varbinary`,
`varchar` / `varbinary` AS `varchar/varbinary`,
`varchar` + `tinyblob` AS `varchar+tinyblob`,
`varchar` - `tinyblob` AS `varchar-tinyblob`,
`varchar` * `tinyblob` AS `varchar*tinyblob`,
`varchar` / `tinyblob` AS `varchar/tinyblob`,
`varchar` + `mediumblob` AS `varchar+mediumblob`,
`varchar` - `mediumblob` AS `varchar-mediumblob`,
`varchar` * `mediumblob` AS `varchar*mediumblob`,
`varchar` / `mediumblob` AS `varchar/mediumblob`,
`varchar` + `longblob` AS `varchar+longblob`,
`varchar` - `longblob` AS `varchar-longblob`,
`varchar` * `longblob` AS `varchar*longblob`,
`varchar` / `longblob` AS `varchar/longblob`,
`varchar` + `text` AS `varchar+text`,
`varchar` - `text` AS `varchar-text`,
`varchar` * `text` AS `varchar*text`,
`varchar` / `text` AS `varchar/text`,
`varchar` + `enum_t` AS `varchar+enum_t`,
`varchar` - `enum_t` AS `varchar-enum_t`,
`varchar` * `enum_t` AS `varchar*enum_t`,
`varchar` / `enum_t` AS `varchar/enum_t`,
`varchar` + `set_t` AS `varchar+set_t`,
`varchar` - `set_t` AS `varchar-set_t`,
`varchar` * `set_t` AS `varchar*set_t`,
`varchar` / `set_t` AS `varchar/set_t`,
`varchar` + `json` AS `varchar+json`,
`varchar` - `json` AS `varchar-json`,
`varchar` * `json` AS `varchar*json`,
`varchar` / `json` AS `varchar/json`,
`binary` + `char` AS `binary+char`,
`binary` - `char` AS `binary-char`,
`binary` * `char` AS `binary*char`,
`binary` / `char` AS `binary/char`,
`binary` + `varchar` AS `binary+varchar`,
`binary` - `varchar` AS `binary-varchar`,
`binary` * `varchar` AS `binary*varchar`,
`binary` / `varchar` AS `binary/varchar`,
`binary` + `binary` AS `binary+binary`,
`binary` - `binary` AS `binary-binary`,
`binary` * `binary` AS `binary*binary`,
`binary` / `binary` AS `binary/binary`,
`binary` + `varbinary` AS `binary+varbinary`,
`binary` - `varbinary` AS `binary-varbinary`,
`binary` * `varbinary` AS `binary*varbinary`,
`binary` / `varbinary` AS `binary/varbinary`,
`binary` + `tinyblob` AS `binary+tinyblob`,
`binary` - `tinyblob` AS `binary-tinyblob`,
`binary` * `tinyblob` AS `binary*tinyblob`,
`binary` / `tinyblob` AS `binary/tinyblob`,
`binary` + `mediumblob` AS `binary+mediumblob`,
`binary` - `mediumblob` AS `binary-mediumblob`,
`binary` * `mediumblob` AS `binary*mediumblob`,
`binary` / `mediumblob` AS `binary/mediumblob`,
`binary` + `longblob` AS `binary+longblob`,
`binary` - `longblob` AS `binary-longblob`,
`binary` * `longblob` AS `binary*longblob`,
`binary` / `longblob` AS `binary/longblob`,
`binary` + `text` AS `binary+text`,
`binary` - `text` AS `binary-text`,
`binary` * `text` AS `binary*text`,
`binary` / `text` AS `binary/text`,
`binary` + `enum_t` AS `binary+enum_t`,
`binary` - `enum_t` AS `binary-enum_t`,
`binary` * `enum_t` AS `binary*enum_t`,
`binary` / `enum_t` AS `binary/enum_t`,
`binary` + `set_t` AS `binary+set_t`,
`binary` - `set_t` AS `binary-set_t`,
`binary` * `set_t` AS `binary*set_t`,
`binary` / `set_t` AS `binary/set_t`,
`binary` + `json` AS `binary+json`,
`binary` - `json` AS `binary-json`,
`binary` * `json` AS `binary*json`,
`binary` / `json` AS `binary/json`,
`varbinary` + `char` AS `varbinary+char`,
`varbinary` - `char` AS `varbinary-char`,
`varbinary` * `char` AS `varbinary*char`,
`varbinary` / `char` AS `varbinary/char`,
`varbinary` + `varchar` AS `varbinary+varchar`,
`varbinary` - `varchar` AS `varbinary-varchar`,
`varbinary` * `varchar` AS `varbinary*varchar`,
`varbinary` / `varchar` AS `varbinary/varchar`,
`varbinary` + `binary` AS `varbinary+binary`,
`varbinary` - `binary` AS `varbinary-binary`,
`varbinary` * `binary` AS `varbinary*binary`,
`varbinary` / `binary` AS `varbinary/binary`,
`varbinary` + `varbinary` AS `varbinary+varbinary`,
`varbinary` - `varbinary` AS `varbinary-varbinary`,
`varbinary` * `varbinary` AS `varbinary*varbinary`,
`varbinary` / `varbinary` AS `varbinary/varbinary`,
`varbinary` + `tinyblob` AS `varbinary+tinyblob`,
`varbinary` - `tinyblob` AS `varbinary-tinyblob`,
`varbinary` * `tinyblob` AS `varbinary*tinyblob`,
`varbinary` / `tinyblob` AS `varbinary/tinyblob`,
`varbinary` + `mediumblob` AS `varbinary+mediumblob`,
`varbinary` - `mediumblob` AS `varbinary-mediumblob`,
`varbinary` * `mediumblob` AS `varbinary*mediumblob`,
`varbinary` / `mediumblob` AS `varbinary/mediumblob`,
`varbinary` + `longblob` AS `varbinary+longblob`,
`varbinary` - `longblob` AS `varbinary-longblob`,
`varbinary` * `longblob` AS `varbinary*longblob`,
`varbinary` / `longblob` AS `varbinary/longblob`,
`varbinary` + `text` AS `varbinary+text`,
`varbinary` - `text` AS `varbinary-text`,
`varbinary` * `text` AS `varbinary*text`,
`varbinary` / `text` AS `varbinary/text`,
`varbinary` + `enum_t` AS `varbinary+enum_t`,
`varbinary` - `enum_t` AS `varbinary-enum_t`,
`varbinary` * `enum_t` AS `varbinary*enum_t`,
`varbinary` / `enum_t` AS `varbinary/enum_t`,
`varbinary` + `set_t` AS `varbinary+set_t`,
`varbinary` - `set_t` AS `varbinary-set_t`,
`varbinary` * `set_t` AS `varbinary*set_t`,
`varbinary` / `set_t` AS `varbinary/set_t`,
`varbinary` + `json` AS `varbinary+json`,
`varbinary` - `json` AS `varbinary-json`,
`varbinary` * `json` AS `varbinary*json`,
`varbinary` / `json` AS `varbinary/json`,
`tinyblob` + `char` AS `tinyblob+char`,
`tinyblob` - `char` AS `tinyblob-char`,
`tinyblob` * `char` AS `tinyblob*char`,
`tinyblob` / `char` AS `tinyblob/char`,
`tinyblob` + `varchar` AS `tinyblob+varchar`,
`tinyblob` - `varchar` AS `tinyblob-varchar`,
`tinyblob` * `varchar` AS `tinyblob*varchar`,
`tinyblob` / `varchar` AS `tinyblob/varchar`,
`tinyblob` + `binary` AS `tinyblob+binary`,
`tinyblob` - `binary` AS `tinyblob-binary`,
`tinyblob` * `binary` AS `tinyblob*binary`,
`tinyblob` / `binary` AS `tinyblob/binary`,
`tinyblob` + `varbinary` AS `tinyblob+varbinary`,
`tinyblob` - `varbinary` AS `tinyblob-varbinary`,
`tinyblob` * `varbinary` AS `tinyblob*varbinary`,
`tinyblob` / `varbinary` AS `tinyblob/varbinary`,
`tinyblob` + `tinyblob` AS `tinyblob+tinyblob`,
`tinyblob` - `tinyblob` AS `tinyblob-tinyblob`,
`tinyblob` * `tinyblob` AS `tinyblob*tinyblob`,
`tinyblob` / `tinyblob` AS `tinyblob/tinyblob`,
`tinyblob` + `mediumblob` AS `tinyblob+mediumblob`,
`tinyblob` - `mediumblob` AS `tinyblob-mediumblob`,
`tinyblob` * `mediumblob` AS `tinyblob*mediumblob`,
`tinyblob` / `mediumblob` AS `tinyblob/mediumblob`,
`tinyblob` + `longblob` AS `tinyblob+longblob`,
`tinyblob` - `longblob` AS `tinyblob-longblob`,
`tinyblob` * `longblob` AS `tinyblob*longblob`,
`tinyblob` / `longblob` AS `tinyblob/longblob`,
`tinyblob` + `text` AS `tinyblob+text`,
`tinyblob` - `text` AS `tinyblob-text`,
`tinyblob` * `text` AS `tinyblob*text`,
`tinyblob` / `text` AS `tinyblob/text`,
`tinyblob` + `enum_t` AS `tinyblob+enum_t`,
`tinyblob` - `enum_t` AS `tinyblob-enum_t`,
`tinyblob` * `enum_t` AS `tinyblob*enum_t`,
`tinyblob` / `enum_t` AS `tinyblob/enum_t`,
`tinyblob` + `set_t` AS `tinyblob+set_t`,
`tinyblob` - `set_t` AS `tinyblob-set_t`,
`tinyblob` * `set_t` AS `tinyblob*set_t`,
`tinyblob` / `set_t` AS `tinyblob/set_t`,
`tinyblob` + `json` AS `tinyblob+json`,
`tinyblob` - `json` AS `tinyblob-json`,
`tinyblob` * `json` AS `tinyblob*json`,
`tinyblob` / `json` AS `tinyblob/json`,
`mediumblob` + `char` AS `mediumblob+char`,
`mediumblob` - `char` AS `mediumblob-char`,
`mediumblob` * `char` AS `mediumblob*char`,
`mediumblob` / `char` AS `mediumblob/char`,
`mediumblob` + `varchar` AS `mediumblob+varchar`,
`mediumblob` - `varchar` AS `mediumblob-varchar`,
`mediumblob` * `varchar` AS `mediumblob*varchar`,
`mediumblob` / `varchar` AS `mediumblob/varchar`,
`mediumblob` + `binary` AS `mediumblob+binary`,
`mediumblob` - `binary` AS `mediumblob-binary`,
`mediumblob` * `binary` AS `mediumblob*binary`,
`mediumblob` / `binary` AS `mediumblob/binary`,
`mediumblob` + `varbinary` AS `mediumblob+varbinary`,
`mediumblob` - `varbinary` AS `mediumblob-varbinary`,
`mediumblob` * `varbinary` AS `mediumblob*varbinary`,
`mediumblob` / `varbinary` AS `mediumblob/varbinary`,
`mediumblob` + `tinyblob` AS `mediumblob+tinyblob`,
`mediumblob` - `tinyblob` AS `mediumblob-tinyblob`,
`mediumblob` * `tinyblob` AS `mediumblob*tinyblob`,
`mediumblob` / `tinyblob` AS `mediumblob/tinyblob`,
`mediumblob` + `mediumblob` AS `mediumblob+mediumblob`,
`mediumblob` - `mediumblob` AS `mediumblob-mediumblob`,
`mediumblob` * `mediumblob` AS `mediumblob*mediumblob`,
`mediumblob` / `mediumblob` AS `mediumblob/mediumblob`,
`mediumblob` + `longblob` AS `mediumblob+longblob`,
`mediumblob` - `longblob` AS `mediumblob-longblob`,
`mediumblob` * `longblob` AS `mediumblob*longblob`,
`mediumblob` / `longblob` AS `mediumblob/longblob`,
`mediumblob` + `text` AS `mediumblob+text`,
`mediumblob` - `text` AS `mediumblob-text`,
`mediumblob` * `text` AS `mediumblob*text`,
`mediumblob` / `text` AS `mediumblob/text`,
`mediumblob` + `enum_t` AS `mediumblob+enum_t`,
`mediumblob` - `enum_t` AS `mediumblob-enum_t`,
`mediumblob` * `enum_t` AS `mediumblob*enum_t`,
`mediumblob` / `enum_t` AS `mediumblob/enum_t`,
`mediumblob` + `set_t` AS `mediumblob+set_t`,
`mediumblob` - `set_t` AS `mediumblob-set_t`,
`mediumblob` * `set_t` AS `mediumblob*set_t`,
`mediumblob` / `set_t` AS `mediumblob/set_t`,
`mediumblob` + `json` AS `mediumblob+json`,
`mediumblob` - `json` AS `mediumblob-json`,
`mediumblob` * `json` AS `mediumblob*json`,
`mediumblob` / `json` AS `mediumblob/json`,
`longblob` + `char` AS `longblob+char`,
`longblob` - `char` AS `longblob-char`,
`longblob` * `char` AS `longblob*char`,
`longblob` / `char` AS `longblob/char`,
`longblob` + `varchar` AS `longblob+varchar`,
`longblob` - `varchar` AS `longblob-varchar`,
`longblob` * `varchar` AS `longblob*varchar`,
`longblob` / `varchar` AS `longblob/varchar`,
`longblob` + `binary` AS `longblob+binary`,
`longblob` - `binary` AS `longblob-binary`,
`longblob` * `binary` AS `longblob*binary`,
`longblob` / `binary` AS `longblob/binary`,
`longblob` + `varbinary` AS `longblob+varbinary`,
`longblob` - `varbinary` AS `longblob-varbinary`,
`longblob` * `varbinary` AS `longblob*varbinary`,
`longblob` / `varbinary` AS `longblob/varbinary`,
`longblob` + `tinyblob` AS `longblob+tinyblob`,
`longblob` - `tinyblob` AS `longblob-tinyblob`,
`longblob` * `tinyblob` AS `longblob*tinyblob`,
`longblob` / `tinyblob` AS `longblob/tinyblob`,
`longblob` + `mediumblob` AS `longblob+mediumblob`,
`longblob` - `mediumblob` AS `longblob-mediumblob`,
`longblob` * `mediumblob` AS `longblob*mediumblob`,
`longblob` / `mediumblob` AS `longblob/mediumblob`,
`longblob` + `longblob` AS `longblob+longblob`,
`longblob` - `longblob` AS `longblob-longblob`,
`longblob` * `longblob` AS `longblob*longblob`,
`longblob` / `longblob` AS `longblob/longblob`,
`longblob` + `text` AS `longblob+text`,
`longblob` - `text` AS `longblob-text`,
`longblob` * `text` AS `longblob*text`,
`longblob` / `text` AS `longblob/text`,
`longblob` + `enum_t` AS `longblob+enum_t`,
`longblob` - `enum_t` AS `longblob-enum_t`,
`longblob` * `enum_t` AS `longblob*enum_t`,
`longblob` / `enum_t` AS `longblob/enum_t`,
`longblob` + `set_t` AS `longblob+set_t`,
`longblob` - `set_t` AS `longblob-set_t`,
`longblob` * `set_t` AS `longblob*set_t`,
`longblob` / `set_t` AS `longblob/set_t`,
`longblob` + `json` AS `longblob+json`,
`longblob` - `json` AS `longblob-json`,
`longblob` * `json` AS `longblob*json`,
`longblob` / `json` AS `longblob/json`,
`text` + `char` AS `text+char`,
`text` - `char` AS `text-char`,
`text` * `char` AS `text*char`,
`text` / `char` AS `text/char`,
`text` + `varchar` AS `text+varchar`,
`text` - `varchar` AS `text-varchar`,
`text` * `varchar` AS `text*varchar`,
`text` / `varchar` AS `text/varchar`,
`text` + `binary` AS `text+binary`,
`text` - `binary` AS `text-binary`,
`text` * `binary` AS `text*binary`,
`text` / `binary` AS `text/binary`,
`text` + `varbinary` AS `text+varbinary`,
`text` - `varbinary` AS `text-varbinary`,
`text` * `varbinary` AS `text*varbinary`,
`text` / `varbinary` AS `text/varbinary`,
`text` + `tinyblob` AS `text+tinyblob`,
`text` - `tinyblob` AS `text-tinyblob`,
`text` * `tinyblob` AS `text*tinyblob`,
`text` / `tinyblob` AS `text/tinyblob`,
`text` + `mediumblob` AS `text+mediumblob`,
`text` - `mediumblob` AS `text-mediumblob`,
`text` * `mediumblob` AS `text*mediumblob`,
`text` / `mediumblob` AS `text/mediumblob`,
`text` + `longblob` AS `text+longblob`,
`text` - `longblob` AS `text-longblob`,
`text` * `longblob` AS `text*longblob`,
`text` / `longblob` AS `text/longblob`,
`text` + `text` AS `text+text`,
`text` - `text` AS `text-text`,
`text` * `text` AS `text*text`,
`text` / `text` AS `text/text`,
`text` + `enum_t` AS `text+enum_t`,
`text` - `enum_t` AS `text-enum_t`,
`text` * `enum_t` AS `text*enum_t`,
`text` / `enum_t` AS `text/enum_t`,
`text` + `set_t` AS `text+set_t`,
`text` - `set_t` AS `text-set_t`,
`text` * `set_t` AS `text*set_t`,
`text` / `set_t` AS `text/set_t`,
`text` + `json` AS `text+json`,
`text` - `json` AS `text-json`,
`text` * `json` AS `text*json`,
`text` / `json` AS `text/json`,
`enum_t` + `char` AS `enum_t+char`,
`enum_t` - `char` AS `enum_t-char`,
`enum_t` * `char` AS `enum_t*char`,
`enum_t` / `char` AS `enum_t/char`,
`enum_t` + `varchar` AS `enum_t+varchar`,
`enum_t` - `varchar` AS `enum_t-varchar`,
`enum_t` * `varchar` AS `enum_t*varchar`,
`enum_t` / `varchar` AS `enum_t/varchar`,
`enum_t` + `binary` AS `enum_t+binary`,
`enum_t` - `binary` AS `enum_t-binary`,
`enum_t` * `binary` AS `enum_t*binary`,
`enum_t` / `binary` AS `enum_t/binary`,
`enum_t` + `varbinary` AS `enum_t+varbinary`,
`enum_t` - `varbinary` AS `enum_t-varbinary`,
`enum_t` * `varbinary` AS `enum_t*varbinary`,
`enum_t` / `varbinary` AS `enum_t/varbinary`,
`enum_t` + `tinyblob` AS `enum_t+tinyblob`,
`enum_t` - `tinyblob` AS `enum_t-tinyblob`,
`enum_t` * `tinyblob` AS `enum_t*tinyblob`,
`enum_t` / `tinyblob` AS `enum_t/tinyblob`,
`enum_t` + `mediumblob` AS `enum_t+mediumblob`,
`enum_t` - `mediumblob` AS `enum_t-mediumblob`,
`enum_t` * `mediumblob` AS `enum_t*mediumblob`,
`enum_t` / `mediumblob` AS `enum_t/mediumblob`,
`enum_t` + `longblob` AS `enum_t+longblob`,
`enum_t` - `longblob` AS `enum_t-longblob`,
`enum_t` * `longblob` AS `enum_t*longblob`,
`enum_t` / `longblob` AS `enum_t/longblob`,
`enum_t` + `text` AS `enum_t+text`,
`enum_t` - `text` AS `enum_t-text`,
`enum_t` * `text` AS `enum_t*text`,
`enum_t` / `text` AS `enum_t/text`,
`enum_t` + `enum_t` AS `enum_t+enum_t`,
`enum_t` - `enum_t` AS `enum_t-enum_t`,
`enum_t` * `enum_t` AS `enum_t*enum_t`,
`enum_t` / `enum_t` AS `enum_t/enum_t`,
`enum_t` + `set_t` AS `enum_t+set_t`,
`enum_t` - `set_t` AS `enum_t-set_t`,
`enum_t` * `set_t` AS `enum_t*set_t`,
`enum_t` / `set_t` AS `enum_t/set_t`,
`enum_t` + `json` AS `enum_t+json`,
`enum_t` - `json` AS `enum_t-json`,
`enum_t` * `json` AS `enum_t*json`,
`enum_t` / `json` AS `enum_t/json`,
`set_t` + `char` AS `set_t+char`,
`set_t` - `char` AS `set_t-char`,
`set_t` * `char` AS `set_t*char`,
`set_t` / `char` AS `set_t/char`,
`set_t` + `varchar` AS `set_t+varchar`,
`set_t` - `varchar` AS `set_t-varchar`,
`set_t` * `varchar` AS `set_t*varchar`,
`set_t` / `varchar` AS `set_t/varchar`,
`set_t` + `binary` AS `set_t+binary`,
`set_t` - `binary` AS `set_t-binary`,
`set_t` * `binary` AS `set_t*binary`,
`set_t` / `binary` AS `set_t/binary`,
`set_t` + `varbinary` AS `set_t+varbinary`,
`set_t` - `varbinary` AS `set_t-varbinary`,
`set_t` * `varbinary` AS `set_t*varbinary`,
`set_t` / `varbinary` AS `set_t/varbinary`,
`set_t` + `tinyblob` AS `set_t+tinyblob`,
`set_t` - `tinyblob` AS `set_t-tinyblob`,
`set_t` * `tinyblob` AS `set_t*tinyblob`,
`set_t` / `tinyblob` AS `set_t/tinyblob`,
`set_t` + `mediumblob` AS `set_t+mediumblob`,
`set_t` - `mediumblob` AS `set_t-mediumblob`,
`set_t` * `mediumblob` AS `set_t*mediumblob`,
`set_t` / `mediumblob` AS `set_t/mediumblob`,
`set_t` + `longblob` AS `set_t+longblob`,
`set_t` - `longblob` AS `set_t-longblob`,
`set_t` * `longblob` AS `set_t*longblob`,
`set_t` / `longblob` AS `set_t/longblob`,
`set_t` + `text` AS `set_t+text`,
`set_t` - `text` AS `set_t-text`,
`set_t` * `text` AS `set_t*text`,
`set_t` / `text` AS `set_t/text`,
`set_t` + `enum_t` AS `set_t+enum_t`,
`set_t` - `enum_t` AS `set_t-enum_t`,
`set_t` * `enum_t` AS `set_t*enum_t`,
`set_t` / `enum_t` AS `set_t/enum_t`,
`set_t` + `set_t` AS `set_t+set_t`,
`set_t` - `set_t` AS `set_t-set_t`,
`set_t` * `set_t` AS `set_t*set_t`,
`set_t` / `set_t` AS `set_t/set_t`,
`set_t` + `json` AS `set_t+json`,
`set_t` - `json` AS `set_t-json`,
`set_t` * `json` AS `set_t*json`,
`set_t` / `json` AS `set_t/json`,
`json` + `char` AS `json+char`,
`json` - `char` AS `json-char`,
`json` * `char` AS `json*char`,
`json` / `char` AS `json/char`,
`json` + `varchar` AS `json+varchar`,
`json` - `varchar` AS `json-varchar`,
`json` * `varchar` AS `json*varchar`,
`json` / `varchar` AS `json/varchar`,
`json` + `binary` AS `json+binary`,
`json` - `binary` AS `json-binary`,
`json` * `binary` AS `json*binary`,
`json` / `binary` AS `json/binary`,
`json` + `varbinary` AS `json+varbinary`,
`json` - `varbinary` AS `json-varbinary`,
`json` * `varbinary` AS `json*varbinary`,
`json` / `varbinary` AS `json/varbinary`,
`json` + `tinyblob` AS `json+tinyblob`,
`json` - `tinyblob` AS `json-tinyblob`,
`json` * `tinyblob` AS `json*tinyblob`,
`json` / `tinyblob` AS `json/tinyblob`,
`json` + `mediumblob` AS `json+mediumblob`,
`json` - `mediumblob` AS `json-mediumblob`,
`json` * `mediumblob` AS `json*mediumblob`,
`json` / `mediumblob` AS `json/mediumblob`,
`json` + `longblob` AS `json+longblob`,
`json` - `longblob` AS `json-longblob`,
`json` * `longblob` AS `json*longblob`,
`json` / `longblob` AS `json/longblob`,
`json` + `text` AS `json+text`,
`json` - `text` AS `json-text`,
`json` * `text` AS `json*text`,
`json` / `text` AS `json/text`,
`json` + `enum_t` AS `json+enum_t`,
`json` - `enum_t` AS `json-enum_t`,
`json` * `enum_t` AS `json*enum_t`,
`json` / `enum_t` AS `json/enum_t`,
`json` + `set_t` AS `json+set_t`,
`json` - `set_t` AS `json-set_t`,
`json` * `set_t` AS `json*set_t`,
`json` / `set_t` AS `json/set_t`,
`json` + `json` AS `json+json`,
`json` - `json` AS `json-json`,
`json` * `json` AS `json*json`,
`json` / `json` AS `json/json`
from test_string_table;
SHOW COLUMNS FROM test_string_type;

-- test value
set dolphin.sql_mode to 'sql_mode_full_group,pipes_as_concat,ansi_quotes,no_zero_date';
delete from test_string_type;
delete from test_string_table;
insert into test_string_type values(null);
insert into test_string_table values('1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', '1.23a', 'a', 'a,c', json_object('a', 1, 'b', 2));

UPDATE test_string_type, test_string_table SET test_string_type.`char+char` = test_string_table.`char`+test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`char-char` = test_string_table.`char`-test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`char*char` = test_string_table.`char`*test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`char/char` = test_string_table.`char`/test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`char+varchar` = test_string_table.`char`+test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`char-varchar` = test_string_table.`char`-test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`char*varchar` = test_string_table.`char`*test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`char/varchar` = test_string_table.`char`/test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`char+binary` = test_string_table.`char`+test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`char-binary` = test_string_table.`char`-test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`char*binary` = test_string_table.`char`*test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`char/binary` = test_string_table.`char`/test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`char+varbinary` = test_string_table.`char`+test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`char-varbinary` = test_string_table.`char`-test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`char*varbinary` = test_string_table.`char`*test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`char/varbinary` = test_string_table.`char`/test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`char+tinyblob` = test_string_table.`char`+test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`char-tinyblob` = test_string_table.`char`-test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`char*tinyblob` = test_string_table.`char`*test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`char/tinyblob` = test_string_table.`char`/test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`char+mediumblob` = test_string_table.`char`+test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`char-mediumblob` = test_string_table.`char`-test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`char*mediumblob` = test_string_table.`char`*test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`char/mediumblob` = test_string_table.`char`/test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`char+longblob` = test_string_table.`char`+test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`char-longblob` = test_string_table.`char`-test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`char*longblob` = test_string_table.`char`*test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`char/longblob` = test_string_table.`char`/test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`char+text` = test_string_table.`char`+test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`char-text` = test_string_table.`char`-test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`char*text` = test_string_table.`char`*test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`char/text` = test_string_table.`char`/test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`char+enum_t` = test_string_table.`char`+test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`char-enum_t` = test_string_table.`char`-test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`char*enum_t` = test_string_table.`char`*test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`char/enum_t` = test_string_table.`char`/test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`char+set_t` = test_string_table.`char`+test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`char-set_t` = test_string_table.`char`-test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`char*set_t` = test_string_table.`char`*test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`char/set_t` = test_string_table.`char`/test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`char+json` = test_string_table.`char`+test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`char-json` = test_string_table.`char`-test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`char*json` = test_string_table.`char`*test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`char/json` = test_string_table.`char`/test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar+char` = test_string_table.`varchar`+test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar-char` = test_string_table.`varchar`-test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar*char` = test_string_table.`varchar`*test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar/char` = test_string_table.`varchar`/test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar+varchar` = test_string_table.`varchar`+test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar-varchar` = test_string_table.`varchar`-test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar*varchar` = test_string_table.`varchar`*test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar/varchar` = test_string_table.`varchar`/test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar+binary` = test_string_table.`varchar`+test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar-binary` = test_string_table.`varchar`-test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar*binary` = test_string_table.`varchar`*test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar/binary` = test_string_table.`varchar`/test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar+varbinary` = test_string_table.`varchar`+test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar-varbinary` = test_string_table.`varchar`-test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar*varbinary` = test_string_table.`varchar`*test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar/varbinary` = test_string_table.`varchar`/test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar+tinyblob` = test_string_table.`varchar`+test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar-tinyblob` = test_string_table.`varchar`-test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar*tinyblob` = test_string_table.`varchar`*test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar/tinyblob` = test_string_table.`varchar`/test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar+mediumblob` = test_string_table.`varchar`+test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar-mediumblob` = test_string_table.`varchar`-test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar*mediumblob` = test_string_table.`varchar`*test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar/mediumblob` = test_string_table.`varchar`/test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar+longblob` = test_string_table.`varchar`+test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar-longblob` = test_string_table.`varchar`-test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar*longblob` = test_string_table.`varchar`*test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar/longblob` = test_string_table.`varchar`/test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar+text` = test_string_table.`varchar`+test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar-text` = test_string_table.`varchar`-test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar*text` = test_string_table.`varchar`*test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar/text` = test_string_table.`varchar`/test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar+enum_t` = test_string_table.`varchar`+test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar-enum_t` = test_string_table.`varchar`-test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar*enum_t` = test_string_table.`varchar`*test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar/enum_t` = test_string_table.`varchar`/test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar+set_t` = test_string_table.`varchar`+test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar-set_t` = test_string_table.`varchar`-test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar*set_t` = test_string_table.`varchar`*test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar/set_t` = test_string_table.`varchar`/test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar+json` = test_string_table.`varchar`+test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar-json` = test_string_table.`varchar`-test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar*json` = test_string_table.`varchar`*test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`varchar/json` = test_string_table.`varchar`/test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary+char` = test_string_table.`binary`+test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary-char` = test_string_table.`binary`-test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary*char` = test_string_table.`binary`*test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary/char` = test_string_table.`binary`/test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary+varchar` = test_string_table.`binary`+test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary-varchar` = test_string_table.`binary`-test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary*varchar` = test_string_table.`binary`*test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary/varchar` = test_string_table.`binary`/test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary+binary` = test_string_table.`binary`+test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary-binary` = test_string_table.`binary`-test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary*binary` = test_string_table.`binary`*test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary/binary` = test_string_table.`binary`/test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary+varbinary` = test_string_table.`binary`+test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary-varbinary` = test_string_table.`binary`-test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary*varbinary` = test_string_table.`binary`*test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary/varbinary` = test_string_table.`binary`/test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary+tinyblob` = test_string_table.`binary`+test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary-tinyblob` = test_string_table.`binary`-test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary*tinyblob` = test_string_table.`binary`*test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary/tinyblob` = test_string_table.`binary`/test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary+mediumblob` = test_string_table.`binary`+test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary-mediumblob` = test_string_table.`binary`-test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary*mediumblob` = test_string_table.`binary`*test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary/mediumblob` = test_string_table.`binary`/test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary+longblob` = test_string_table.`binary`+test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary-longblob` = test_string_table.`binary`-test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary*longblob` = test_string_table.`binary`*test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary/longblob` = test_string_table.`binary`/test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary+text` = test_string_table.`binary`+test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary-text` = test_string_table.`binary`-test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary*text` = test_string_table.`binary`*test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary/text` = test_string_table.`binary`/test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary+enum_t` = test_string_table.`binary`+test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary-enum_t` = test_string_table.`binary`-test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary*enum_t` = test_string_table.`binary`*test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary/enum_t` = test_string_table.`binary`/test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary+set_t` = test_string_table.`binary`+test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary-set_t` = test_string_table.`binary`-test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary*set_t` = test_string_table.`binary`*test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary/set_t` = test_string_table.`binary`/test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary+json` = test_string_table.`binary`+test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary-json` = test_string_table.`binary`-test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary*json` = test_string_table.`binary`*test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`binary/json` = test_string_table.`binary`/test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary+char` = test_string_table.`varbinary`+test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary-char` = test_string_table.`varbinary`-test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary*char` = test_string_table.`varbinary`*test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary/char` = test_string_table.`varbinary`/test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary+varchar` = test_string_table.`varbinary`+test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary-varchar` = test_string_table.`varbinary`-test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary*varchar` = test_string_table.`varbinary`*test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary/varchar` = test_string_table.`varbinary`/test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary+binary` = test_string_table.`varbinary`+test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary-binary` = test_string_table.`varbinary`-test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary*binary` = test_string_table.`varbinary`*test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary/binary` = test_string_table.`varbinary`/test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary+varbinary` = test_string_table.`varbinary`+test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary-varbinary` = test_string_table.`varbinary`-test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary*varbinary` = test_string_table.`varbinary`*test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary/varbinary` = test_string_table.`varbinary`/test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary+tinyblob` = test_string_table.`varbinary`+test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary-tinyblob` = test_string_table.`varbinary`-test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary*tinyblob` = test_string_table.`varbinary`*test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary/tinyblob` = test_string_table.`varbinary`/test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary+mediumblob` = test_string_table.`varbinary`+test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary-mediumblob` = test_string_table.`varbinary`-test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary*mediumblob` = test_string_table.`varbinary`*test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary/mediumblob` = test_string_table.`varbinary`/test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary+longblob` = test_string_table.`varbinary`+test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary-longblob` = test_string_table.`varbinary`-test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary*longblob` = test_string_table.`varbinary`*test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary/longblob` = test_string_table.`varbinary`/test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary+text` = test_string_table.`varbinary`+test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary-text` = test_string_table.`varbinary`-test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary*text` = test_string_table.`varbinary`*test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary/text` = test_string_table.`varbinary`/test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary+enum_t` = test_string_table.`varbinary`+test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary-enum_t` = test_string_table.`varbinary`-test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary*enum_t` = test_string_table.`varbinary`*test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary/enum_t` = test_string_table.`varbinary`/test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary+set_t` = test_string_table.`varbinary`+test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary-set_t` = test_string_table.`varbinary`-test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary*set_t` = test_string_table.`varbinary`*test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary/set_t` = test_string_table.`varbinary`/test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary+json` = test_string_table.`varbinary`+test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary-json` = test_string_table.`varbinary`-test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary*json` = test_string_table.`varbinary`*test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`varbinary/json` = test_string_table.`varbinary`/test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob+char` = test_string_table.`tinyblob`+test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob-char` = test_string_table.`tinyblob`-test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob*char` = test_string_table.`tinyblob`*test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob/char` = test_string_table.`tinyblob`/test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob+varchar` = test_string_table.`tinyblob`+test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob-varchar` = test_string_table.`tinyblob`-test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob*varchar` = test_string_table.`tinyblob`*test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob/varchar` = test_string_table.`tinyblob`/test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob+binary` = test_string_table.`tinyblob`+test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob-binary` = test_string_table.`tinyblob`-test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob*binary` = test_string_table.`tinyblob`*test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob/binary` = test_string_table.`tinyblob`/test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob+varbinary` = test_string_table.`tinyblob`+test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob-varbinary` = test_string_table.`tinyblob`-test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob*varbinary` = test_string_table.`tinyblob`*test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob/varbinary` = test_string_table.`tinyblob`/test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob+tinyblob` = test_string_table.`tinyblob`+test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob-tinyblob` = test_string_table.`tinyblob`-test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob*tinyblob` = test_string_table.`tinyblob`*test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob/tinyblob` = test_string_table.`tinyblob`/test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob+mediumblob` = test_string_table.`tinyblob`+test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob-mediumblob` = test_string_table.`tinyblob`-test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob*mediumblob` = test_string_table.`tinyblob`*test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob/mediumblob` = test_string_table.`tinyblob`/test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob+longblob` = test_string_table.`tinyblob`+test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob-longblob` = test_string_table.`tinyblob`-test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob*longblob` = test_string_table.`tinyblob`*test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob/longblob` = test_string_table.`tinyblob`/test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob+text` = test_string_table.`tinyblob`+test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob-text` = test_string_table.`tinyblob`-test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob*text` = test_string_table.`tinyblob`*test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob/text` = test_string_table.`tinyblob`/test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob+enum_t` = test_string_table.`tinyblob`+test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob-enum_t` = test_string_table.`tinyblob`-test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob*enum_t` = test_string_table.`tinyblob`*test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob/enum_t` = test_string_table.`tinyblob`/test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob+set_t` = test_string_table.`tinyblob`+test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob-set_t` = test_string_table.`tinyblob`-test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob*set_t` = test_string_table.`tinyblob`*test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob/set_t` = test_string_table.`tinyblob`/test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob+json` = test_string_table.`tinyblob`+test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob-json` = test_string_table.`tinyblob`-test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob*json` = test_string_table.`tinyblob`*test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`tinyblob/json` = test_string_table.`tinyblob`/test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob+char` = test_string_table.`mediumblob`+test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob-char` = test_string_table.`mediumblob`-test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob*char` = test_string_table.`mediumblob`*test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob/char` = test_string_table.`mediumblob`/test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob+varchar` = test_string_table.`mediumblob`+test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob-varchar` = test_string_table.`mediumblob`-test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob*varchar` = test_string_table.`mediumblob`*test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob/varchar` = test_string_table.`mediumblob`/test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob+binary` = test_string_table.`mediumblob`+test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob-binary` = test_string_table.`mediumblob`-test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob*binary` = test_string_table.`mediumblob`*test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob/binary` = test_string_table.`mediumblob`/test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob+varbinary` = test_string_table.`mediumblob`+test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob-varbinary` = test_string_table.`mediumblob`-test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob*varbinary` = test_string_table.`mediumblob`*test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob/varbinary` = test_string_table.`mediumblob`/test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob+tinyblob` = test_string_table.`mediumblob`+test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob-tinyblob` = test_string_table.`mediumblob`-test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob*tinyblob` = test_string_table.`mediumblob`*test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob/tinyblob` = test_string_table.`mediumblob`/test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob+mediumblob` = test_string_table.`mediumblob`+test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob-mediumblob` = test_string_table.`mediumblob`-test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob*mediumblob` = test_string_table.`mediumblob`*test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob/mediumblob` = test_string_table.`mediumblob`/test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob+longblob` = test_string_table.`mediumblob`+test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob-longblob` = test_string_table.`mediumblob`-test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob*longblob` = test_string_table.`mediumblob`*test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob/longblob` = test_string_table.`mediumblob`/test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob+text` = test_string_table.`mediumblob`+test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob-text` = test_string_table.`mediumblob`-test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob*text` = test_string_table.`mediumblob`*test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob/text` = test_string_table.`mediumblob`/test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob+enum_t` = test_string_table.`mediumblob`+test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob-enum_t` = test_string_table.`mediumblob`-test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob*enum_t` = test_string_table.`mediumblob`*test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob/enum_t` = test_string_table.`mediumblob`/test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob+set_t` = test_string_table.`mediumblob`+test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob-set_t` = test_string_table.`mediumblob`-test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob*set_t` = test_string_table.`mediumblob`*test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob/set_t` = test_string_table.`mediumblob`/test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob+json` = test_string_table.`mediumblob`+test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob-json` = test_string_table.`mediumblob`-test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob*json` = test_string_table.`mediumblob`*test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`mediumblob/json` = test_string_table.`mediumblob`/test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob+char` = test_string_table.`longblob`+test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob-char` = test_string_table.`longblob`-test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob*char` = test_string_table.`longblob`*test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob/char` = test_string_table.`longblob`/test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob+varchar` = test_string_table.`longblob`+test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob-varchar` = test_string_table.`longblob`-test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob*varchar` = test_string_table.`longblob`*test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob/varchar` = test_string_table.`longblob`/test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob+binary` = test_string_table.`longblob`+test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob-binary` = test_string_table.`longblob`-test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob*binary` = test_string_table.`longblob`*test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob/binary` = test_string_table.`longblob`/test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob+varbinary` = test_string_table.`longblob`+test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob-varbinary` = test_string_table.`longblob`-test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob*varbinary` = test_string_table.`longblob`*test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob/varbinary` = test_string_table.`longblob`/test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob+tinyblob` = test_string_table.`longblob`+test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob-tinyblob` = test_string_table.`longblob`-test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob*tinyblob` = test_string_table.`longblob`*test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob/tinyblob` = test_string_table.`longblob`/test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob+mediumblob` = test_string_table.`longblob`+test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob-mediumblob` = test_string_table.`longblob`-test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob*mediumblob` = test_string_table.`longblob`*test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob/mediumblob` = test_string_table.`longblob`/test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob+longblob` = test_string_table.`longblob`+test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob-longblob` = test_string_table.`longblob`-test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob*longblob` = test_string_table.`longblob`*test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob/longblob` = test_string_table.`longblob`/test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob+text` = test_string_table.`longblob`+test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob-text` = test_string_table.`longblob`-test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob*text` = test_string_table.`longblob`*test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob/text` = test_string_table.`longblob`/test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob+enum_t` = test_string_table.`longblob`+test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob-enum_t` = test_string_table.`longblob`-test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob*enum_t` = test_string_table.`longblob`*test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob/enum_t` = test_string_table.`longblob`/test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob+set_t` = test_string_table.`longblob`+test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob-set_t` = test_string_table.`longblob`-test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob*set_t` = test_string_table.`longblob`*test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob/set_t` = test_string_table.`longblob`/test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob+json` = test_string_table.`longblob`+test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob-json` = test_string_table.`longblob`-test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob*json` = test_string_table.`longblob`*test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`longblob/json` = test_string_table.`longblob`/test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`text+char` = test_string_table.`text`+test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`text-char` = test_string_table.`text`-test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`text*char` = test_string_table.`text`*test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`text/char` = test_string_table.`text`/test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`text+varchar` = test_string_table.`text`+test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`text-varchar` = test_string_table.`text`-test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`text*varchar` = test_string_table.`text`*test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`text/varchar` = test_string_table.`text`/test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`text+binary` = test_string_table.`text`+test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`text-binary` = test_string_table.`text`-test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`text*binary` = test_string_table.`text`*test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`text/binary` = test_string_table.`text`/test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`text+varbinary` = test_string_table.`text`+test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`text-varbinary` = test_string_table.`text`-test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`text*varbinary` = test_string_table.`text`*test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`text/varbinary` = test_string_table.`text`/test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`text+tinyblob` = test_string_table.`text`+test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`text-tinyblob` = test_string_table.`text`-test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`text*tinyblob` = test_string_table.`text`*test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`text/tinyblob` = test_string_table.`text`/test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`text+mediumblob` = test_string_table.`text`+test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`text-mediumblob` = test_string_table.`text`-test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`text*mediumblob` = test_string_table.`text`*test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`text/mediumblob` = test_string_table.`text`/test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`text+longblob` = test_string_table.`text`+test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`text-longblob` = test_string_table.`text`-test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`text*longblob` = test_string_table.`text`*test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`text/longblob` = test_string_table.`text`/test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`text+text` = test_string_table.`text`+test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`text-text` = test_string_table.`text`-test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`text*text` = test_string_table.`text`*test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`text/text` = test_string_table.`text`/test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`text+enum_t` = test_string_table.`text`+test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`text-enum_t` = test_string_table.`text`-test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`text*enum_t` = test_string_table.`text`*test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`text/enum_t` = test_string_table.`text`/test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`text+set_t` = test_string_table.`text`+test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`text-set_t` = test_string_table.`text`-test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`text*set_t` = test_string_table.`text`*test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`text/set_t` = test_string_table.`text`/test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`text+json` = test_string_table.`text`+test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`text-json` = test_string_table.`text`-test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`text*json` = test_string_table.`text`*test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`text/json` = test_string_table.`text`/test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t+char` = test_string_table.`enum_t`+test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t-char` = test_string_table.`enum_t`-test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t*char` = test_string_table.`enum_t`*test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t/char` = test_string_table.`enum_t`/test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t+varchar` = test_string_table.`enum_t`+test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t-varchar` = test_string_table.`enum_t`-test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t*varchar` = test_string_table.`enum_t`*test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t/varchar` = test_string_table.`enum_t`/test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t+binary` = test_string_table.`enum_t`+test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t-binary` = test_string_table.`enum_t`-test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t*binary` = test_string_table.`enum_t`*test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t/binary` = test_string_table.`enum_t`/test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t+varbinary` = test_string_table.`enum_t`+test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t-varbinary` = test_string_table.`enum_t`-test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t*varbinary` = test_string_table.`enum_t`*test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t/varbinary` = test_string_table.`enum_t`/test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t+tinyblob` = test_string_table.`enum_t`+test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t-tinyblob` = test_string_table.`enum_t`-test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t*tinyblob` = test_string_table.`enum_t`*test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t/tinyblob` = test_string_table.`enum_t`/test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t+mediumblob` = test_string_table.`enum_t`+test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t-mediumblob` = test_string_table.`enum_t`-test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t*mediumblob` = test_string_table.`enum_t`*test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t/mediumblob` = test_string_table.`enum_t`/test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t+longblob` = test_string_table.`enum_t`+test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t-longblob` = test_string_table.`enum_t`-test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t*longblob` = test_string_table.`enum_t`*test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t/longblob` = test_string_table.`enum_t`/test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t+text` = test_string_table.`enum_t`+test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t-text` = test_string_table.`enum_t`-test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t*text` = test_string_table.`enum_t`*test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t/text` = test_string_table.`enum_t`/test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t+enum_t` = test_string_table.`enum_t`+test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t-enum_t` = test_string_table.`enum_t`-test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t*enum_t` = test_string_table.`enum_t`*test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t/enum_t` = test_string_table.`enum_t`/test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t+set_t` = test_string_table.`enum_t`+test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t-set_t` = test_string_table.`enum_t`-test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t*set_t` = test_string_table.`enum_t`*test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t/set_t` = test_string_table.`enum_t`/test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t+json` = test_string_table.`enum_t`+test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t-json` = test_string_table.`enum_t`-test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t*json` = test_string_table.`enum_t`*test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`enum_t/json` = test_string_table.`enum_t`/test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t+char` = test_string_table.`set_t`+test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t-char` = test_string_table.`set_t`-test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t*char` = test_string_table.`set_t`*test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t/char` = test_string_table.`set_t`/test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t+varchar` = test_string_table.`set_t`+test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t-varchar` = test_string_table.`set_t`-test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t*varchar` = test_string_table.`set_t`*test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t/varchar` = test_string_table.`set_t`/test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t+binary` = test_string_table.`set_t`+test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t-binary` = test_string_table.`set_t`-test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t*binary` = test_string_table.`set_t`*test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t/binary` = test_string_table.`set_t`/test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t+varbinary` = test_string_table.`set_t`+test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t-varbinary` = test_string_table.`set_t`-test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t*varbinary` = test_string_table.`set_t`*test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t/varbinary` = test_string_table.`set_t`/test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t+tinyblob` = test_string_table.`set_t`+test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t-tinyblob` = test_string_table.`set_t`-test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t*tinyblob` = test_string_table.`set_t`*test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t/tinyblob` = test_string_table.`set_t`/test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t+mediumblob` = test_string_table.`set_t`+test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t-mediumblob` = test_string_table.`set_t`-test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t*mediumblob` = test_string_table.`set_t`*test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t/mediumblob` = test_string_table.`set_t`/test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t+longblob` = test_string_table.`set_t`+test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t-longblob` = test_string_table.`set_t`-test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t*longblob` = test_string_table.`set_t`*test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t/longblob` = test_string_table.`set_t`/test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t+text` = test_string_table.`set_t`+test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t-text` = test_string_table.`set_t`-test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t*text` = test_string_table.`set_t`*test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t/text` = test_string_table.`set_t`/test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t+enum_t` = test_string_table.`set_t`+test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t-enum_t` = test_string_table.`set_t`-test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t*enum_t` = test_string_table.`set_t`*test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t/enum_t` = test_string_table.`set_t`/test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t+set_t` = test_string_table.`set_t`+test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t-set_t` = test_string_table.`set_t`-test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t*set_t` = test_string_table.`set_t`*test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t/set_t` = test_string_table.`set_t`/test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t+json` = test_string_table.`set_t`+test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t-json` = test_string_table.`set_t`-test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t*json` = test_string_table.`set_t`*test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`set_t/json` = test_string_table.`set_t`/test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`json+char` = test_string_table.`json`+test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`json-char` = test_string_table.`json`-test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`json*char` = test_string_table.`json`*test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`json/char` = test_string_table.`json`/test_string_table.`char`;
UPDATE test_string_type, test_string_table SET test_string_type.`json+varchar` = test_string_table.`json`+test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`json-varchar` = test_string_table.`json`-test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`json*varchar` = test_string_table.`json`*test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`json/varchar` = test_string_table.`json`/test_string_table.`varchar`;
UPDATE test_string_type, test_string_table SET test_string_type.`json+binary` = test_string_table.`json`+test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`json-binary` = test_string_table.`json`-test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`json*binary` = test_string_table.`json`*test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`json/binary` = test_string_table.`json`/test_string_table.`binary`;
UPDATE test_string_type, test_string_table SET test_string_type.`json+varbinary` = test_string_table.`json`+test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`json-varbinary` = test_string_table.`json`-test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`json*varbinary` = test_string_table.`json`*test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`json/varbinary` = test_string_table.`json`/test_string_table.`varbinary`;
UPDATE test_string_type, test_string_table SET test_string_type.`json+tinyblob` = test_string_table.`json`+test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`json-tinyblob` = test_string_table.`json`-test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`json*tinyblob` = test_string_table.`json`*test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`json/tinyblob` = test_string_table.`json`/test_string_table.`tinyblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`json+mediumblob` = test_string_table.`json`+test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`json-mediumblob` = test_string_table.`json`-test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`json*mediumblob` = test_string_table.`json`*test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`json/mediumblob` = test_string_table.`json`/test_string_table.`mediumblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`json+longblob` = test_string_table.`json`+test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`json-longblob` = test_string_table.`json`-test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`json*longblob` = test_string_table.`json`*test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`json/longblob` = test_string_table.`json`/test_string_table.`longblob`;
UPDATE test_string_type, test_string_table SET test_string_type.`json+text` = test_string_table.`json`+test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`json-text` = test_string_table.`json`-test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`json*text` = test_string_table.`json`*test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`json/text` = test_string_table.`json`/test_string_table.`text`;
UPDATE test_string_type, test_string_table SET test_string_type.`json+enum_t` = test_string_table.`json`+test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`json-enum_t` = test_string_table.`json`-test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`json*enum_t` = test_string_table.`json`*test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`json/enum_t` = test_string_table.`json`/test_string_table.`enum_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`json+set_t` = test_string_table.`json`+test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`json-set_t` = test_string_table.`json`-test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`json*set_t` = test_string_table.`json`*test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`json/set_t` = test_string_table.`json`/test_string_table.`set_t`;
UPDATE test_string_type, test_string_table SET test_string_type.`json+json` = test_string_table.`json`+test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`json-json` = test_string_table.`json`-test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`json*json` = test_string_table.`json`*test_string_table.`json`;
UPDATE test_string_type, test_string_table SET test_string_type.`json/json` = test_string_table.`json`/test_string_table.`json`;
SELECT * FROM test_string_type;
---------- tail ----------
drop schema string_operator_test_schema cascade;
reset current_schema;
