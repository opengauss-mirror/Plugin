CREATE TABLE fetch_float_1 ( id SERIAL PRIMARY KEY, name text, score float, group_id INT );
INSERT INTO fetch_float_1 (name, score, group_id) VALUES ('Alice', 95.5, 1), ('Bob', 95.5, 1), ('Charlie', 90.1, 2), ('David', 85.5, 2), ('Eve', 80.19999, 3), ('Frank', 80.19999, 3), ('Grace', 75, 4), ('Heidi', 70.908, 4), ('Ivan', 65.1, 5), ('Judy', 60.1, 5),('海绵',null,6),('星星**',null,6);

--0 12 2 3 4 6 6 7 2 1 3 3
SELECT * FROM fetch_float_1 ORDER BY score FETCH NEXT 0 PERCENT rows WITH TIES;

SELECT * FROM fetch_float_1 ORDER BY score FETCH NEXT 100 PERCENT rows WITH TIES;
SELECT * FROM fetch_float_1 ORDER BY score FETCH NEXT 16.6 PERCENT rows WITH TIES;
SELECT * FROM fetch_float_1 ORDER BY score FETCH NEXT 16.7 PERCENT rows WITH TIES;

SELECT * FROM fetch_float_1 ORDER BY score FETCH NEXT 33.3 PERCENT rows WITH TIES;
SELECT * FROM fetch_float_1 ORDER BY score FETCH NEXT 33.34 PERCENT rows WITH TIES;
SELECT * FROM fetch_float_1 ORDER BY score FETCH NEXT 50 PERCENT rows WITH TIES;
SELECT * FROM fetch_float_1 ORDER BY score FETCH NEXT 50.1 PERCENT rows WITH TIES;

SELECT * FROM fetch_float_1 ORDER BY score desc FETCH first 3 PERCENT rows WITH TIES;
SELECT * FROM fetch_float_1 ORDER BY score desc,name FETCH NEXT 3 PERCENT rows WITH TIES;
SELECT * FROM fetch_float_1 ORDER BY score DESC, name FETCH NEXT 3 rows WITH TIES;
SELECT * FROM fetch_float_1 FETCH NEXT 3 rows WITH TIES;
DROP TABLE fetch_float_1;
