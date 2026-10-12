# Type Conversion

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-07-30T06:21:52.402Z pushedAt=2026-09-24T09:47:11.902Z -->

Compared with the original openGauss, the dolphin plugin supplements the following rules for mutual conversion between data types, which include:

- BIT/TINYINT/SMALLINT/INT/BIGINT
- UINT1/UINT2/UINT4/UINT8
- NUMERIC/FLOAT/DOUBLE
- DATE/DATETIME/TIMESTAMP/TIME/YEAR
- CHAR/VARCHAR/TEXT/BINARY/VARBINARY/TINYBLOB/MEDIUMBLOB/BLOB/LONGBLOB
- ENUM/SET/JSON

In addition, conversion rules from DATETIME/TIMESTAMP, YEAR, BINARY/VARBINARY, TINYBLOB/MEDIUMBLOB/BLOB/LONGBLOB, ENUM, and SET to BOOL have been added. The conversion rules between two types have three levels: Implicit (IMPLICIT, i), Assignment (ASSIGNMENT, a), and Explicit (EXPLICIT, e), which can be viewed through the castcontext field of the system table pg_cast. The applicable scopes of the three conversion levels are as follows:

- 'e': Indicates that only explicit conversion is supported (using CAST or :: syntax).
- 'a': Indicates that conversion is supported when assigning a value to a target column, and explicit conversion is also supported.
- 'i': Indicates that implicit conversion in expressions (functions, operators, etc.) is supported, and assignment and explicit conversions are also supported.

Added assignment conversion rules from UINT4, BOOL, CHAR, VARCHAR, and TEXT to YEAR.

## Conversion Rules

The following conversion rules require the GUC parameter dolphin.b_compatibility_mode to be set to on.

### Source Type: BIT

|Target Type|Conversion Rule Description|Remarks|
|--|--|--|
|TINYINT/SMALLINT/INT/BIGINT<br>UINT1/UINT2/UINT4/UINT8<br>NUMERIC/FLOAT/DOUBLE|BIT conversion to numeric types can be regarded as BIT converted to UINT8 first, then converted to the target type. If the value exceeds the target type range, overflow occurs. In Strict Mode, an error is reported; in Non-strict Mode, the maximum value of the target type is used.|BIT to UINT8 conversion has no negative numbers. Therefore, for example, b'11111111'::tinyint is not -1, but 255::tinyint, which overflows.<br>Conversions to UINT4/UINT8/NUMERIC/FLOAT/DOUBLE are Implicit; others are Assignment.|
|DATE/DATETIME/TIMESTAMP/TIME|BIT conversion to date/time types: the BIT binary encoding is first converted to a string according to the current character set, and then converted to the date/time type.|The conversion rule is BIT to number to date type.<br>Conversion Level: Assignment|
|YEAR|After converting the binary value to decimal, the same rule as numeric type to YEAR applies.|The value range of the YEAR type is 0, 1901-2155.<br>Conversion Level: Assignment|
|CHAR/VARCHAR/TEXT|The BIT binary encoding is converted to a string according to the current character set.|Conversion Level: Assignment|
|BINARY/VARBINARY|Convert the binary value to decimal, and then convert each digit character by character according to the character set.|For example, b'11111'::binary: binary 11111 converted to decimal is 31, 3 encoded as /x33, 1 encoded as /x31, resulting in /x3331.<br>Conversion Level: Assignment|
|TINYBLOB/MEDIUMBLOB/BLOB/LONGBLOB|BIT binary corresponds to BLOB hexadecimal.|For example, b'11111111'::blob is /xff.<br>Conversion Level: Assignment|
|ENUM|Conversion is performed by binary value, corresponding to the ENUM index.|Conversion Level: Assignment|
|SET|Conversion is performed by binary value, corresponding to the SET index.|The SET index order is 1, 2, 4, 8..., corresponding to each binary bit, and can be combined. For example, b'101' is converted to the set of the first and third items.<br>Conversion Level: Assignment|
|JSON|Displayed as "base64:type16:xxx", where xxx is the base64 encoding of the BIT binary converted to a hexadecimal "0x" format string.|For example, b'1111'::json results in "base64:type16:Dw==", where Dw== decoded by base64 is 0x0F.<br>Conversion Level: Explicit|

### Source Type: Numeric Type

#### TINYINT/SMALLINT/INT/BIGINT

#### UINT1/UINT2/UINT4/UINT8

#### NUMERIC/FLOAT/DOUBLE

|Target Type|Conversion Rule Description|Remarks|
|--|--|--|
|BIT|When converting a numeric type to BIT, it can be treated as converting the numeric type to BIT(64) first, and then converting to the target BIT(n). If the length exceeds the limit, an error is reported in strict mode, and the maximum value (all 1s) is used in non-strict mode. Rounding applies to decimals.|The highest bit of BIT(64) can be the sign bit, so -1::bit(8) is not '11111111' but 64 bits of 1, causing overflow.<br>-1 and UINT8_MAX are both 64 bits of 1.<br>Conversion Level: Assignment|
|TINYINT/SMALLINT/INT/BIGINT<br>UINT1/UINT2/UINT4/UINT8<br>NUMERIC/FLOAT/DOUBLE|Mutual conversion between numeric types may involve rounding, precision loss, and overflow. Overflow occurs when the value exceeds the representation range of the target type. The integer overflow rules are as follows: Conversion Level: (1) Implicit/Assignment: An error is reported in strict mode, and the value is truncated to the minimum/maximum value of the target type in non-strict mode. (2) Explicit: First, truncate to the maximum/minimum value within the bit representation range of the target type (for example, UINT8 has 64 bits; negative numbers exceeding the 64-bit representation range are truncated to the minimum value of INT8, and positive numbers are truncated to the maximum value of UINT8). When converting to unsigned integer, overflowed negative numbers are converted to their corresponding positive complement. When converting to signed integer, overflowed positive numbers are converted to their corresponding negative integer. (3) Special cases: The explicit rule applies to implicit/assignment conversion from UINT8 to INT8. The implicit rule applies to explicit conversion when the target type is TINYINT/SMALLINT/INT. When the source type is a floating-point type, the implicit rule applies to explicit conversion to signed numbers, and the explicit rule applies to conversion to unsigned numbers. Difference from MySQL: In openGauss, numbers exceeding the INT8 representation range are of the NUMERIC type. When the source type is NUMERIC, the same rule applies to all three conversion levels: when converting to unsigned integer, negative numbers are truncated to 0. When converting to signed integer, positive numbers are truncated to the maximum value of the target type.|Conversion Level: Implicit|
|DATE|Three digits: 2000-0x-xx<br>Four digits: 2000-xx-xx<br>Five digits: 200x-xx-xx<br>Six digits: 20xx-xx-xx<br>Seven digits: 0xxx-xx-xx<br>Eight digits: xxxx-xx-xx<br>The fractional part is ignored, with no rounding.|Conversion from TINYINT/SMALLINT/INT to DATE is at the implicit level, from UINT4 to DATE is at the explicit level, and others are at the assignment level.|
|DATETIME/TIMESTAMP|For eight digits or fewer, the date part is the same as DATE, and the time is 00:00:00. For nine digits or more, time takes priority:<br>Nine digits: 2000-0x-xx xx:xx:xx<br>Ten digits: 200x-xx-xx xx:xx:xx<br>Eleven digits: 20xx-xx-xx xx:xx:xx<br>Twelve digits: 0xxx-xx-xx xx:xx:xx<br>Thirteen digits: xxxx-xx-xx xx:xx:xx<br>The fractional part is ignored, with no rounding.<br>|Conversion from TINYINT/SMALLINT/INT/BIGINT/NUMERIC to DATETIME/TIMESTAMP is at the implicit level, from UINT4/UINT8 is at the explicit level, and others are at the assignment level.|
|TIME|Bitwise correspondence starting from seconds.|The value range of the TIME type under B compatibility is -838:59:59 to 838:59:59.<br>'12:34'::time results in 12:34:00, which is more consistent with daily usage habits. However, 1234::time results in 00:12:34. Note the distinction. For more conversion rules about character types, see the detailed introduction.<br>Conversion from TINYINT/SMALLINT/INT/BIGINT/NUMERIC to TIME is at the implicit level, from UINT4 is at the explicit level, and others are at the assignment level.|
|YEAR|0 is 0.<br>One digit: 200x<br>Two digits: 20xx<br>Four digits: xxxx<br>Rounding applies to decimals.|The value range of the YEAR type is 0, 1901 to 2155.<br>Conversion from TINYINT/SMALLINT/INT to YEAR is at the implicit level, and others are at the assignment level.|
|CHAR/VARCHAR/TEXT|Convert by numeric value to string.|Conversion to TEXT is at the implicit level.<br>Conversion from UINT1/UINT2/UINT4/UINT8 to CHAR is at the assignment level, and others are at the implicit level.|
|BINARY/VARBINARY<br>TINYBLOB/MEDIUMBLOB/BLOB/LONGBLOB|Convert by numeric value to string, displayed in /x format hexadecimal current character set encoding.|Conversion Level: Assignment|
|ENUM|Convert by corresponding ENUM index based on numeric value.<br>The fractional part is ignored, with no rounding.|Conversion Level: Assignment|
|SET|Convert by corresponding SET index based on numeric value.<br>The fractional part is ignored, with no rounding.|The SET index order is 1, 2, 4, 8..., and they can be combined. For example, 5 is converted to the set of the first and third items of SET.<br>Conversion from SMALLINT/INT/BIGINT/NUMERIC/FLOAT/DOUBLE to SET is implicit, and others are at the assignment level.|
|JSON|Convert by numeric value, similar to string.|Conversion Level: Explicit|

### Source Type: DATE

|Target Type|Conversion Rule Description|Remarks|
|--|--|--|
|BIT|Bitwise conversion by literal value to decimal value, then convert to binary value. Refer to the numeric type to BIT conversion rule|Conversion Level: Assignment|
|TINYINT/SMALLINT/INT/BIGINT<br>UINT1/UINT2/UINT4/UINT8<br>NUMERIC/FLOAT/DOUBLE|Bitwise conversion by literal value to decimal value|Conversion Level: Assignment for TINYINT/SMALLINT/UINT1/UINT2, Implicit for others|
|DATETIME/TIMESTAMP|Date remains unchanged, time is 00:00:00|Conversion Level: Implicit|
|TIME|00:00:00|Conversion Level: Assignment|
|YEAR|Extract the year part from the DATE type as YEAR|Value Range of Type: 0, 1901–2155<br>Conversion Level: Assignment|
|CHAR/VARCHAR/TEXT|Convert to string by corresponding display format|Conversion Level: Implicit|
|BINARY/VARBINARY<br>TINYBLOB/MEDIUMBLOB/BLOB/LONGBLOB|Convert to string by corresponding display format, displayed in /x format hexadecimal current character set encoding|Conversion Level: Assignment|
|ENUM|Convert by corresponding display format string, convert to the item corresponding to the ENUM label|Conversion Level: Assignment|
|SET|Convert by corresponding display format string, convert to the item corresponding to the SET label|Conversion Level: Assignment|
|JSON|Convert by corresponding display format string, wrapped in double quotes|Conversion Level: Explicit|

### Source Type: DATETIME/TIMESTAMP

|Target Type|Conversion Rule|Remarks|
|--|--|--|
|BIT|Convert each digit literally to a decimal value, and then convert the result to a binary value. For details, see the BIT rules in Numeric Type Conversion.|Conversion Level: Assignment|
|TINYINT/SMALLINT/INT/BIGINT<br>UINT1/UINT2/UINT4/UINT8<br>NUMERIC/FLOAT/DOUBLE|Bitwise conversion by literal value to decimal value.|Conversion Level: Assignment for TINYINT/SMALLINT/INT/UINT1/UINT2/UINT4, and Implicit for BIGINT/UINT8/NUMERIC/FLOAT/DOUBLE.|
|DATE|Retain the date part.|Conversion Level: Assignment|
|TIMESTAMP/DATETIME|The difference between DATETIME and TIMESTAMP is that the former does not carry timezone information, while the latter does.|Conversion Level: Implicit for DATETIME to TIMESTAMP, and Assignment for TIMESTAMP to DATETIME.|
|TIME|Retain the time part.|Conversion Level: Implicit for DATETIME to TIME, and Assignment for TIMESTAMP to TIME.|
|YEAR|Extract the year part from DATETIME and TIMESTAMP types as YEAR.|Value Range of Type YEAR: 0, 1901–2155.<br>Conversion Level: Assignment|
|CHAR/VARCHAR/TEXT|Convert by corresponding display format to string.|Conversion Level: Implicit|
|BINARY/VARBINARY<br>TINYBLOB/MEDIUMBLOB/BLOB/LONGBLOB|Convert by corresponding display format to string, displayed in /x format hexadecimal encoding of the current character set.|Conversion Level: Assignment|
|ENUM|Convert by corresponding display format string, converting to the item corresponding to the ENUM label.|Conversion Level: Assignment|
|SET|Convert by corresponding display format string, converting to the item corresponding to the SET label.|Conversion Level: Assignment|
|JSON|Convert by corresponding display format string, wrapped in double quotes.|Conversion Level: Explicit|
|BOOL|A valid non-zero time is TRUE, but an all-zero time is FALSE.|Conversion Level: Assignment|

### Source Type: TIME

|Target Type|Conversion Rule Description|Notes|
|--|--|--|
|BIT|Convert each digit literally to a decimal value, and then convert the result to a binary value. For details, see the BIT rules in Numeric Type Conversion.|Conversion Level: Assignment|
|TINYINT/SMALLINT/INT/BIGINT<br>UINT1/UINT2/UINT4/UINT8<br>NUMERIC/FLOAT/DOUBLE|Bitwise conversion by literal value to decimal value.|Conversion Level: Assignment for TINYINT/SMALLINT/UINT1/UINT2, Implicit for others.|
|DATE|The date after adding or subtracting the current date (TIME type supports negative time).|Conversion Level: Assignment|
|DATETIME/TIMESTAMP|The difference between DATETIME and TIMESTAMP is that the former does not carry timezone information, while the latter does.|Conversion Level: Assignment|
|YEAR|Convert by literal value. Same rule as numeric type to YEAR conversion.|Value Range of Type: 0, 1901–2155.<br>Conversion Level: Assignment|
|CHAR/VARCHAR/TEXT|Convert to string by corresponding display format.|Conversion Level: Implicit|
|BINARY/VARBINARY<br>TINYBLOB/MEDIUMBLOB/BLOB/LONGBLOB|Convert to string by corresponding display format, displayed in /x format hexadecimal current character set encoding.|Conversion Level: Assignment|
|ENUM|Convert by corresponding display format string, converting to the item with the corresponding label in ENUM.|Conversion Level: Assignment|
|SET|Convert by corresponding display format string, converting to the item with the corresponding label in SET.|Conversion Level: Assignment|
|JSON|Convert by corresponding display format string, wrapped in double quotes.|Conversion Level: Explicit|

### Source Type: YEAR

|Target Type|Conversion Rule Description|Remarks|
|--|--|--|
|BIT|After converting to a decimal value bitwise by literal value, then convert to a binary value. Refer to the BIT conversion rule for numeric types|Conversion Level: Assignment|
|TINYINT/SMALLINT/INT/BIGINT<br>UINT1/UINT2/UINT4/UINT8<br>NUMERIC/FLOAT/DOUBLE|Convert by Numeric Value|Conversion Level: Implicit to INT, Conversion Level: Explicit to UINT4/UINT8/NUMERIC/DOUBLE, Conversion Level: Assignment to others|
|DATE|Same Rule as numeric type to DATE conversion|Conversion Level: Assignment|
|DATETIME/TIMESTAMP|Same Rule as numeric type to DATETIME/TIMESTAMP conversion|Conversion Level: Assignment|
|TIME|Same Rule as numeric type to TIME conversion|Conversion Level: Assignment|
|CHAR/VARCHAR/TEXT|Convert to a string by the corresponding display format|Conversion Level: Assignment|
|BINARY/VARBINARY<br>TINYBLOB/MEDIUMBLOB/BLOB/LONGBLOB|Convert to a string by the corresponding display format, displayed in /x format hexadecimal encoding of the current character set|Conversion Level: Assignment|
|ENUM|Convert by corresponding the numeric value to the ENUM index|Conversion Level: Assignment|
|SET|Convert by corresponding the numeric value to the SET index|Conversion Level: Assignment|
|JSON|Convert by Numeric Value, similar to string conversion|Conversion Level: Explicit|
|BOOLEAN|Convert to TEXT type first, then convert to Boolean type|Conversion Level: Implicit|

### Source Type: CHAR/VARCHAR/TEXT

|Target Type|Conversion Rule Description|Remarks|
|--|--|--|
|BIT|Convert each byte to the corresponding binary encoding.|Conversion Level: Assignment|
|TINYINT/SMALLINT/INT/BIGINT<br>UINT1/UINT2/UINT4/UINT8<br>NUMERIC/FLOAT/DOUBLE|Convert by the literal numeric value of the string. Overflow occurs if the value exceeds the representation range of the target type. The integer overflow rule is as follows: In Strict Mode, an error is reported. In Non-strict Mode, when the conversion level is (1) Implicit/Assignment: truncate to the minimum/maximum value of the target type. (2) Explicit: first truncate to the maximum/minimum value within the bit representation range of the target type; when converting to an unsigned integer, an overflowed negative number is converted to the corresponding positive complement; when converting to a signed integer, an overflowed positive number is converted to the corresponding negative integer.|Conversion Level: Implicit|
|DATE/DATETIME/TIMESTAMP/TIME|Convert by the corresponding format. There are many format rules.|Conversion Level: Explicit for TEXT to TIMESTAMP/TIME; Implicit for others.|
|YEAR|First convert to the BIGINT type, then convert to the YEAR type.|Value Range of Type YEAR: 0, 1901–2155.<br>Conversion Level: Assignment|
|CHAR/VARCHAR/TEXT|Mutual conversion between string types.|Conversion Level: Implicit|
|BINARY/VARBINARY<br>TINYBLOB/MEDIUMBLOB/BLOB/LONGBLOB|Displayed as hexadecimal encoding of the current character set in /x format.|Conversion Level: Assignment|
|ENUM|Convert the string to the corresponding ENUM label item.|Conversion Level: Assignment|
|SET|Convert the string to the corresponding SET label item.|Conversion Level: Implicit|
|JSON|Convert by a valid JSON string format.|Conversion Level: Explicit|

- When the TEXT type is used in a numeric context, such as participating in numeric operations, being compared with numeric values, or being passed as an argument to a function whose input parameter type is numeric, the above scenarios involve conversion from the TEXT type to a numeric type. MySQL truncates illegal formats without generating a warning. However, when TEXT type data is inserted into a numeric type column, MySQL generates a warning or error. openGauss adopts stricter warning/error handling for all the above scenarios.

### Source Type: BINARY/VARBINARY<br>TINYBLOB/MEDIUMBLOB/BLOB/LONGBLOB

|Target Type|Conversion Rule Description|Remarks|
|--|--|--|
|BIT|Converts the hexadecimal encoding of the corresponding character set to binary encoding|Conversion Level: Assignment|
|TINYINT/SMALLINT/INT/BIGINT<br>UINT1/UINT2/UINT4/UINT8<br>NUMERIC/FLOAT/DOUBLE|After decoding with the corresponding character set, converts by the literal numeric value of the string|Conversion Level: Implicit for conversion to DOUBLE, Assignment for others<br>When the decoded string cannot be fully converted to a numeric value, a warning is generated,<br>which differs from MySQL where no warning is generated when TINYBLOB/MEDIUMBLOB/BLOB/LONGBLOB are compared with TINYINT/SMALLINT/INT/BIGINT/UINT1/UINT2/UINT4/UINT8/NUMERIC/FLOAT/DOUBLE using operators such as =/<>/!=/</<=/>/>=.|
|DATE/DATETIME/TIMESTAMP/TIME|After decoding with the corresponding character set, converts by the corresponding format; the format rules are numerous|Conversion Level: Assignment|
|YEAR|After decoding with the corresponding character set, converts by the literal numeric value of the string; the rule is the same as that for converting numeric types to YEAR|Value Range of Type YEAR: 0, 1901–2155<br>Conversion Level: Assignment|
|CHAR/VARCHAR/TEXT|Decodes with the corresponding character set|Conversion Level: Implicit for BINARY/VARBINARY to TEXT, Assignment for others|
|BINARY/VARBINARY<br>TINYBLOB/MEDIUMBLOB/BLOB/LONGBLOB|Mutual conversion between similar types|Conversion Level: Implicit for conversions between BINARY and VARBINARY, Implicit for conversions among TINYBLOB/MEDIUMBLOB/BLOB/LONGBLOB, Assignment for others|
|ENUM|After decoding with the corresponding character set, converts the string to the item corresponding to the ENUM label|Conversion Level: Assignment|
|SET|After decoding with the corresponding character set, converts the string to the item corresponding to the SET label|Conversion Level: Assignment|
|JSON|BINARY to JSON is displayed as "base64:type254:xxx"<br>VARBINARY to JSON is displayed as "base64:type15:xxx"<br>TINYBLOB to JSON is displayed as "base64:type249:xxx"<br>MEDIUMBLOB to JSON is displayed as "base64:type250:xxx"<br>BLOB to JSON is displayed as "base64:type252:xxx"<br>LONGBLOB to JSON is displayed as "base64:type251:xxx"<br>where xxx is the base64 encoding of a string in "0x" format|For example, x'5C'::blob::json yields "base64:type252:XA==", where XA== decoded by base64 is 0x5C<br>Conversion Level: Explicit|
|BOOLEAN|BINARY is first converted to CHAR type, VARBINARY is first converted to VARCHAR type, and others are first converted to TEXT type, then converted to Boolean type|Conversion Level: Assignment|

### Source Type: ENUM

|Target Type|Conversion Rule Description|Remarks|
|--|--|--|
|BIT|Convert the index value of the corresponding item to binary|For example, if test_enum(a enum('a', 'b', 'c')) exists, the second item converted to BIT(4) results in b'0010'<br>Conversion Level: Assignment|
|TINYINT/SMALLINT/INT/BIGINT<br>UINT1/UINT2/UINT4/UINT8<br>NUMERIC/FLOAT/DOUBLE|Convert by the index value of the corresponding item|Conversion Level: Implicit for ENUM to DOUBLE, Assignment for others|
|DATE/DATETIME/TIMESTAMP/TIME|Take the Label string of the corresponding item and convert according to the corresponding format; there are many format rules|Conversion Level: Assignment|
|YEAR|Convert by the index value of the corresponding item, with the same rule as Numeric Type to YEAR|Value Range of Type YEAR: 0, 1901–2155<br>Conversion Level: Assignment|
|CHAR/VARCHAR/TEXT|The Label string of the corresponding item|Conversion Level: Assignment|
|BINARY/VARBINARY<br>TINYBLOB/MEDIUMBLOB/BLOB/LONGBLOB|Take the Label string of the corresponding item and display it in /x format hexadecimal encoding of the current character set|Conversion Level: Assignment|
|SET|Convert to the item with the same Label string|Conversion Level: Assignment|
|JSON|Take the Label string of the corresponding item and convert according to the legal JSON string format|Conversion Level: Explicit|
|BOOLEAN|Convert to INT type first, then convert to Boolean type|Conversion Level: Assignment|

### Source Type: SET

|Target Type|Conversion Rule Description|Notes|
|--|--|--|
|BIT|Convert the numeric value of the corresponding item number to binary|For example, if test_set(a set('a', 'b', 'c')) exists, the set containing the first and third items is converted to BIT(4) as b'0101'<br>Conversion Level: Assignment|
|TINYINT/SMALLINT/INT/BIGINT<br>UINT1/UINT2/UINT4/UINT8<br>NUMERIC/FLOAT/DOUBLE|Convert by the numeric value of the corresponding item number|Conversion Level: Implicit for SMALLINT/INT/BIGINT/NUMERIC/FLOAT/DOUBLE, and Conversion Level: Assignment for TINYINT/UINT1/UINT2/UINT4/UINT8|
|DATE/DATETIME/TIMESTAMP/TIME|Take the Label string of the corresponding item and convert it according to the corresponding format. There are many format rules|Conversion Level: Assignment|
|YEAR|Follow the numeric value of the corresponding item number, with the same rule as converting from numeric types to YEAR|Value Range of Type YEAR: 0, 1901–2155<br>Conversion Level: Assignment|
|CHAR/VARCHAR/TEXT|The Label string of the corresponding item|Conversion Level: Implicit|
|BINARY/VARBINARY<br>TINYBLOB/MEDIUMBLOB/BLOB/LONGBLOB|Take the Label string of the corresponding item and display it in /x format hexadecimal encoding of the current character set|Conversion Level: Assignment|
|ENUM|Convert to the item with the same Label string|Conversion Level: Assignment|
|JSON|Take the Label string of the corresponding item and convert it in a valid JSON string format|Conversion Level: Explicit|
|BOOLEAN|Convert to INT type first, then convert to Boolean type|Conversion Level: Assignment|

### Source Type: JSON

|Target Type|Conversion Rule Description|Remarks|
|--|--|--|
|BIT|Convert the JSON string to the corresponding character set hexadecimal encoding, then convert to binary encoding|Conversion Level: Assignment|
|TINYINT/SMALLINT/INT/BIGINT<br>UINT1/UINT2/UINT4/UINT8<br>NUMERIC/FLOAT/DOUBLE|Convert by the literal numeric value of the JSON string|Conversion Level: Assignment|
|DATE/DATETIME/TIMESTAMP/TIME|The JSON string is converted according to the corresponding format; the format rules are numerous|Conversion Level: Assignment|
|YEAR|Convert by the literal numeric value of the JSON string; the rule is the same as that for converting numeric types to YEAR|Value Range of Type YEAR: 0, 1901–2155<br>Conversion Level: Assignment|
|CHAR/VARCHAR/TEXT|Convert the JSON string to a string|Conversion Level: Assignment|
|BINARY/VARBINARY<br>TINYBLOB/MEDIUMBLOB/BLOB/LONGBLOB|Displayed in /x format hexadecimal current character set encoding|Conversion Level: Explicit|
|ENUM|Convert by string to the corresponding Label item of ENUM|Conversion Level: Assignment|
|SET|Convert by string to the corresponding Label item of SET|Conversion Level: Implicit|

### Source Type: BOOL

|Target Type|Conversion Rule Description|Remarks|
|--|--|--|
|YEAR|First convert to BIGINT type, then convert to YEAR type|Conversion Level: Assignment|

### Source Type: INT16

|Target Type|Conversion Rule Description|Remarks|
|--|--|--|
|TEXT|Converts the 16-byte numeric type into text.|Conversion Level: Implicit|