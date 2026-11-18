# XML Functions

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-07-30T06:21:37.112Z pushedAt=2026-09-24T10:48:18.127Z -->

## New XML Functions

- extractvalue(xml_frag, xpath_expr)

  Description: Extracts and returns the value of the XML string corresponding to the XPath expression. It accepts two parameters: the XML value parameter xml_frag and the XPath expression xpath_expr, and outputs a TEXT string.

  Return type: text

  Example:

  ```sql
  opengauss=# select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a');
    extractvalue 
  --------------
   a1 a2
  (1 row)
  ```

- updatexml(xml_target, xpath_expr, new_xml)

  Description: Updates the value of an XML expression and returns the updated XML expression after replacement. The function takes three XML input parameters. It replaces part or all of the content of the first XML value input parameter xml_target with the third input parameter new_xml. The scope of replacement (partial or full) depends on the second input parameter xpath_expr. The function returns the complete XML after replacement. If xpath_expr does not match anything in xml_target, the original xml_target content is returned directly. If any of the three input parameters is NULL, the result is empty.

  Return type: text

  Example:

  ```sql
  opengauss=# select UpdateXML('<a>a1<b>b1<c>c1</c>b2</b>a2</a>','/a/b/c','+++++++++');
    updatexml            
  ---------------------------------
   <a>a1<b>b1+++++++++b2</b>a2</a>
  (1 row)

  opengauss=# select UpdateXML('<a>a1<b>b1<c>c1</c>b2</b>a2</a>','/a','+++++++++');
   updatexml
  -----------
   +++++++++
  (1 row)

  opengauss=# select UpdateXML('<a>a1<b>b1<c>c1</c>b2</b>a2</a>','/a/d','+++++++++');
    updatexml
  ---------------------------------
   <a>a1<b>b1<c>c1</c>b2</b>a2</a>
  (1 row)

  opengauss=# select UpdateXML('<a>a1<b>b1<c>c1</c>b2</b>a2</a>',NULL,'+++++++++');
   updatexml
  -----------
  
  (1 row)
  ```

## Notes

- 1. The parsing of path_expr supports functions

- (1) Node-set functions

- last()

  Example:

  ```sql
  select extractvalue('<a>A<b>B1</b><b>B2</b></a>','/a/b[last()]');
   extractvalue
  --------------
   B2
  (1 row)
  
  select extractvalue('<a>A<b>B1</b><b>B2</b></a>','/a/b[last()-1]');
   extractvalue
  --------------
   B1
  (1 row)
  
  select extractvalue('<a>A<b>B1</b><b>B2</b></a>','/a/b[last()=1]');
   extractvalue
  --------------
  
  (1 row)
  
  select extractvalue('<a>A<b>B1</b><b>B2</b></a>','/a/b[last()=2]');
   extractvalue
  --------------
   B1 B2
  (1 row)
  ```

- position()

  Example:

  ```sql
  select extractvalue('<a>A<b>B1</b><b>B2</b></a>','/a/b[position()]');
   extractvalue
  --------------
   B1 B2
  (1 row)
  ```

- count()

  Example:

  ```sql
  select extractValue('<a>a<b>a1<c>c1</c></b><b>a2</b></a>','/a/b[count(c)=0]');
   extractvalue
  --------------
   a2
  (1 row)
  
  select extractValue('<a>a<b>a1<c>c1</c></b><b>a2</b></a>','/a/b[count(c)=1]');
   extractvalue
  --------------
   a1
  (1 row)
  ```

- (2) String functions
- contains(string, string)

  Example:

  ```sql
  select extractValue('<a>ab</a>','/a[contains("abc","b")]');
   extractvalue
  --------------
   ab
  (1 row)
  
  select extractValue('<a>ab</a>','/a[contains(.,"a")]');
   extractvalue
  --------------
   ab
  (1 row)
  
  select extractValue('<a>ab</a>','/a[contains(.,"b")]');
   extractvalue
  --------------
   ab
  (1 row)
  
  select extractValue('<a>ab</a>','/a[contains(.,"c")]');
   extractvalue
  --------------
  
  (1 row)
  ```

- substring(string, number, number)

  Example:

  ```sql
  select extractValue('<a b="123">ab</a>', '/a/@b[substring(.,1,1)="1"]');
   extractvalue
  --------------
   123
  (1 row)
  
  select extractValue('<a b="123">ab</a>', '/a/@b[substring(.,2,1)="1"]');
   extractvalue
  --------------
  
  (1 row)
  
  select extractValue('<a b="123">ab</a>', '/a/@b[substring(.,2,1)="2"]');
   extractvalue
  --------------
   123
  (1 row)
  ```

- string-length(string)

  Example:

  ```sql
  select extractValue('<a><b>b1</b><b>b2</b></a>', '/a/b[string-length("x")=1]');
   extractvalue
  --------------
   b1 b2
  (1 row)
  
  select extractValue('<a><b>b1</b><b>b2</b></a>', '/a/b[string-length("xx")=2]');
   extractvalue
  --------------
   b1 b2
  (1 row)
  ```

- (3) Boolean functions

  Example:

  - and

  ```sql
  select extractValue('<a>a</a>', '/a[true() and true()]');
   extractvalue
  --------------
   a
  (1 row)
  
  select extractValue('<a>a</a>', '/a[true() and false()]');
   extractvalue
  --------------
  
  (1 row)
  ```

- or
  ```sql
  select extractValue('<a>a</a>', '/a[true() or true()]');
   extractvalue
  --------------
   a
  (1 row)
  
  select extractValue('<a>a</a>', '/a[true() or false()]');
   extractvalue
  --------------
   a
  (1 row)
  ```

- not


- true()
  ```sql
  select extractValue('<a>a</a>', '/a[not(false())]');
   extractvalue
  --------------
   a
  (1 row)
  
  select extractValue('<a>a</a>', '/a[not(true())]');
   extractvalue
  --------------
  
  (1 row)
  ```

- false()
  ```sql
  select extractValue('<a>a</a>', '/a[false()and false()]');
   extractvalue
  --------------
  
  (1 row)
  
  select extractValue('<a>a</a>', '/a[false()and true()]');
   extractvalue
  --------------
  
  (1 row)
  ```

- (4) Numeric functions

  Example:

- sum()
  ```sql
  select extractValue('<a>a1<b ba1="1" ba2="2">b1</b>4</a>','/a/b[sum(@ba1)=1]');
  ```
- floor(number)
  ```sql
  select extractValue('<a>a</a>', '/a[floor(3.1)=3]');
   extractvalue
  --------------
   a
  (1 row)
  
  select extractValue('<a>a</a>', '/a[round(3.1)=3]');
   extractvalue
  --------------
   a
  (1 row)
  
  select extractValue('<a>a</a>', '/a[round(3.8)=4]');
   extractvalue
  --------------
   a
  (1 row)
  ```

- ceiling(number)
  ```sql
  select extractValue('<a>a</a>', '/a[ceiling(3.1)=4]');
   extractvalue
  --------------
   a
  (1 row)
  ```

- round(number)
  ```sql
  select extractValue('<a>a</a>', '/a[round(3.1)=3]');
   extractvalue
  --------------
   a
  (1 row)
  
  select extractValue('<a>a</a>', '/a[round(3.8)=4]');
   extractvalue
  --------------
   a
  (1 row)
  ```

- 2. Duplicate element name scenarios do not conform to the XML format standard and will directly report an error.

  Execute SQL:

  `select extractValue('<a><b x="xb1" x="xb2"/><c x="xc1" x="xc2"/></a>','/a//@x');`

  The result is as follows:

  ERROR:  Failed to parse XML
  CONTEXT:  referenced column: extractvalue

- 3. Input parameter length limit

  The input parameter length is limited to a maximum of `100*1024*1024` bytes, i.e., 100 MB.


## XML Input Parameter Standards — Following libxml2 Library Standards

- 1. Document Structure Requirements

- (1) There must be exactly one root element.

  Correct format example:

  `<root><a>text</a></root>`

  Incorrect format example: Multiple root elements

  `<a>text</a><b>text</b>`

- (2) Non-empty elements must have matching start and end tags.

  Correct format example:

   `<title>Hello World</title>`

  Incorrect format example: The tag is not closed.

  `<title>Hello World`

- (3) Empty elements must be properly closed with a self-closing tag or a start/end tag pair.

  Correct format example:

  `<br/> or <br></br>`

  Incorrect format example: 

  Using HTML-style self-closing tags in strict mode (libxml2 will report an error)

  `<br>`

- (4) All elements must be properly nested, and child elements must be fully closed before the parent element ends.

  Correct format example:

  `<i><b>text</b></i>`

  Incorrect Format: Tags are cross-nested.

  `<i><b>text</i></b>`

- 2. Syntax and character requirements

- (1) Tag names can only start with a letter, underscore, or colon. Subsequent characters may include letters, digits, hyphens, underscores, and colons.

  Correct Format examples:

  `<book>, <_item>, <ns:title>`

  Incorrect format examples: starting with a digit or containing illegal characters

  `<1tag>, <tag!>`

- (2) Attribute values must be fully enclosed in single or double quotation marks

  Correct format example:

  `<div id="main">` or `<div id='main'>`

  Incorrect format example: The attribute value is not enclosed in quotation marks.

  `<div id=main>`

- (3) The five predefined special characters must be escaped using entity references.

  Correct format example:

  AT&amp;T, x &lt; 5

  Incorrect format example:

  Directly using unescaped &amp; or &lt; characters

  AT&T, x < 5

- (4) Comment content must not contain double hyphens --

  Correct format example:

  <!-- This is a comment -->

  Incorrect format example: comment contains -- internally

  <!-- Incorrect--comment -->

- (5) Processing instructions must have the correct `<?target data?>` format.

  Correct format example:

  `<?xml-stylesheet type="text/css" href="style.css"?>`

  Incorrect format example: missing the closing ?>

  `<?xml-stylesheet type="text/css" href="style.css"`

- 3. Namespace requirements

- (1) Any namespace prefix used must be declared within the scope of the current element or its parent element.

  Correct format example:

  `<ns:book xmlns:ns="http://example.com/ns"></ns:book>`

  Incorrect format example: An undeclared prefix is used.

  `<ns:book></ns:book>`

- (2) The URI declared for a namespace must not be an empty string.

  Correct format example:

  xmlns:ns="http://example.com"

  Incorrect format example: using an empty string as the URI

  xmlns:ns=""

- 4. Other important constraints

- (1) An element cannot have two attributes with the same name.

  Correct format example:

  `<src="test1" alt="test2"/>`

  Incorrect format example: The src attribute is defined repeatedly.

  `<src="test1" src="test2"/>`


- (2) Case sensitivity:

  XML tags and attribute names are case-sensitive. `<Book></Book>` and `<book></book>` are treated as different elements.

## References

- 1. Official documentation for XML Path Language (XPath)

  `https://www.w3.org/TR/xpath-10/`