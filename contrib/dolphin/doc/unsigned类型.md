# UNSIGNED整型

UNSIGNED整型即无符号整型，与普通整形相比，其最高位是数字位而非符号位。要实现兼容mysql的UNSIGNED整型，首先保证数据库是'B'类型：`CREATE DATABASE test_db with dbcompatibility='B';`

此外，还兼容了mysql的zerofill属性，只是语法上的支持，实际并没有填充零的效果。与UNSIGNED的作用等价。

### 创建和使用UNSIGNED整型

* 实现了tinyint unsigned、smallint unsigned、 int unsigned、 bigint unsigned四种类型，分别也可以用uin1、uint2、uint4、uint8表示。例如，您可以创建一个具有UNSIGNED整型列的表，如下所示：
  
  ```sql
  CREATE TABLE uint_test (
    col1 tinyint unsigned,
    col2 uint4
  );
  (1 row)
  
* 支持+ - * \ % ~ @ > < >= <= = <> >> << & | #操作符。

* 支持avg，sum、min、max、var_samp、var_pop、variance、stddev_pop、stddev_samp、stddev、bit_and、bit_or、listagg聚合函数。

### 数值范围

* TINYINT UNSIGNED 
0 - 255 

* SMALLINT UNSIGNED 
0 - 65535 

* INT UNSIGNED 
0 - 4294967295 

* BIGINT UNSIGNED 
0 - 18446744073709551615 

### 类型转换

* 支持多种类型之间的转换，同时对于数字类型的隐式转换和显式转换对于负数会有差异，隐式转换时负数均会报错，显式转换时整形和float类型的负数会依据其对应的二进制将最高位替换成数值位计算结果，例如(-1)::uint4 = 16777215, 而numeric类型负数会转为0。

