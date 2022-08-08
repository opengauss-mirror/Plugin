--
-- MERGE INTO: test compatiblity 
--

-- initial
CREATE SCHEMA mergeinto_compatible;
SET current_schema = mergeinto_compatible;

--The following example uses the bonuses TABLE in the sample schema oe with a default bonus of 100.
--It then inserts into the bonuses TABLE all employees who made sales, based on the sales_rep_id
--column of the oe.orders TABLE. Finally, the human resources manager decides that employees with
--a salary of $8000 or less should receive a bonus. Those who have not made sales get a bonus of 1%
--of their salary. Those who already made sales get an increase in their bonus equal to 1% of their
--salary. The MERGE statement implements these changes in one step:

DROP FOREIGN TABLE if exists employees;
CREATE FOREIGN TABLE employees
(
  employee_id int,
  salary      int,
  department_id int
) SERVER mot_server ;
insert into employees values(1,6000,80); --100+6000*1% = 160
insert into employees values(2,7000,81); --departement = 81, bonuses=100
insert into employees values(3,9000,80); --salay > 8000, no money at all
insert into employees values(4,8000,80); --have no sales, 1%*salary = 80

DROP FOREIGN TABLE if exists orders;
CREATE FOREIGN TABLE orders
(
  sales_rep_id int,
  amount       int
) SERVER mot_server ;

insert into orders values(1,100);
insert into orders values(2,200);
insert into orders values(3,300);

DROP FOREIGN TABLE if exists bonuses;
CREATE FOREIGN TABLE bonuses (employee_id int, bonus int DEFAULT 100) SERVER mot_server ;

INSERT INTO bonuses(employee_id)
   (SELECT e.employee_id FROM employees e, orders o
   WHERE e.employee_id = o.sales_rep_id
   GROUP BY e.employee_id); 
   
SELECT * FROM bonuses ORDER BY employee_id;

--the original query
MERGE INTO bonuses D
   USING (SELECT employee_id, salary, department_id FROM employees
   WHERE department_id = 80) S
   ON (D.employee_id = S.employee_id)
   WHEN MATCHED THEN UPDATE SET D.bonus = D.bonus + S.salary*.01
     DELETE WHERE (S.salary > 8000) --delete not supported yet
   WHEN NOT MATCHED THEN INSERT (D.employee_id, D.bonus)
     VALUES (S.employee_id, S.salary*.01)
     WHERE (S.salary <= 8000);

--delete not supported yet
MERGE INTO bonuses D
   USING (SELECT employee_id, salary, department_id FROM employees
   WHERE department_id = 80) S
   ON (D.employee_id = S.employee_id)
   WHEN MATCHED THEN UPDATE SET D.bonus = D.bonus + S.salary*.01
     DELETE WHERE (S.salary > 8000); --delete not supported yet

--support alias in insert targetlist
MERGE INTO bonuses D
   USING (SELECT employee_id, salary, department_id FROM employees
   WHERE department_id = 80) S
   ON (D.employee_id = S.employee_id)
   WHEN MATCHED THEN UPDATE SET D.bonus = D.bonus + S.salary*.01
   WHEN NOT MATCHED THEN INSERT (D.employee_id, D.bonus)
     VALUES (S.employee_id, S.salary*.01);
SELECT * FROM bonuses ORDER BY employee_id;

--the rewrited compatible query
TRUNCATE bonuses;
INSERT INTO bonuses(employee_id)
   (SELECT e.employee_id FROM employees e, orders o
   WHERE e.employee_id = o.sales_rep_id
   GROUP BY e.employee_id);

MERGE INTO bonuses D
   USING (SELECT employee_id, salary, department_id FROM employees
   WHERE department_id = 80) S
   ON (D.employee_id = S.employee_id)
   WHEN MATCHED THEN UPDATE SET D.bonus = D.bonus + S.salary*.01
   WHEN NOT MATCHED THEN INSERT (employee_id, bonus)
     VALUES (S.employee_id, S.salary*.01)
     WHERE (S.salary <= 8000);

DELETE FROM bonuses b where b.employee_id = any (select employee_id from employees where salary > 8000);

SELECT * FROM bonuses ORDER BY employee_id;

--correct answer:

--EMPLOYEE_ID      BONUS
------------- ----------
--          1        160
--          2        100
--          4         80


--postgres some complex joins on the source sourceIDe
CREATE FOREIGN TABLE cj_target (targetID integer, balance float, val varchar(100)) SERVER mot_server ;
CREATE FOREIGN TABLE cj_source1 (sourceID1 integer, scat integer, delta integer) SERVER mot_server ;
CREATE FOREIGN TABLE cj_source2 (sourceID2 integer, sval varchar(100)) SERVER mot_server ;
INSERT INTO cj_source1 VALUES (1, 10, 100);
INSERT INTO cj_source1 VALUES (1, 20, 200);
INSERT INTO cj_source1 VALUES (2, 20, 300);
INSERT INTO cj_source1 VALUES (3, 10, 400);
INSERT INTO cj_source2 VALUES (1, 'initial source2');
INSERT INTO cj_source2 VALUES (2, 'initial source2');
INSERT INTO cj_source2 VALUES (3, 'initial source2');

--error: source relation is an unalised join
MERGE INTO cj_target t
USING cj_source1 s1
    INNER JOIN cj_source2 s2 ON sourceID1 = sourceID2
ON t.targetID = sourceID1
WHEN NOT MATCHED THEN
    INSERT VALUES (sourceID1, delta, sval);

DROP FOREIGN TABLE cj_source2, cj_source1, cj_target;
-- clean up
DROP SCHEMA mergeinto_compatible CASCADE;
