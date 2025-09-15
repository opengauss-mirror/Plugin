CREATE TABLE sales (
    id SERIAL,
    sale_date DATE NOT NULL,
    product_id INTEGER,
    amount DECIMAL(10,2))PARTITION BY RANGE (sale_date) (
    PARTITION sales_2023_q1 VALUES LESS THAN ('2023-04-01'),
    PARTITION sales_2023_q2 VALUES LESS THAN ('2023-07-01'),
    PARTITION sales_2023_q3 VALUES LESS THAN ('2023-10-01'),
    PARTITION sales_2023_q4 VALUES LESS THAN ('2024-01-01'),
    PARTITION sales_max VALUES LESS THAN (MAXVALUE)
);

\d sales

SELECT create_distributed_table('sales', 'id');
DROP TABLE sales;
select count(1) from pg_class where relname = 'sales';

CREATE TABLE sales_multi_level (
    id SERIAL,
    sale_date DATE NOT NULL,
    region VARCHAR(20) NOT NULL,
    product_id INTEGER,
    amount DECIMAL(10,2))
PARTITION BY RANGE (sale_date)
SUBPARTITION BY LIST (region) (
    PARTITION sales_2023 VALUES LESS THAN ('2024-01-01') (
        SUBPARTITION sales_2023_east VALUES ('NY', 'NJ', 'PA', 'MA'),
        SUBPARTITION sales_2023_west VALUES ('CA', 'OR', 'WA', 'NV'),
        SUBPARTITION sales_2023_other VALUES (DEFAULT)
    ),
    PARTITION sales_2024 VALUES LESS THAN ('2025-01-01') (
        SUBPARTITION sales_2024_east VALUES ('NY', 'NJ', 'PA', 'MA'),
        SUBPARTITION sales_2024_west VALUES ('CA', 'OR', 'WA', 'NV'),
        SUBPARTITION sales_2024_other VALUES (DEFAULT)
    ),
    PARTITION sales_future VALUES LESS THAN (MAXVALUE) (
        SUBPARTITION sales_future_all VALUES (DEFAULT)
    )
);

SELECT create_distributed_table('sales_multi_level', 'id');
DROP TABLE sales_multi_level;
select count(1) from pg_class where relname = 'sales_multi_level';