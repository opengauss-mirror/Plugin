
drop database if exists xml_test;
create database xml_test dbcompatibility 'b';
\c xml_test

SET enable_set_variable_b_format = 1;

select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a/b');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a/b/c');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a/@aa1');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a/@aa2');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a/@*');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','//@ba1');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','//a');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','//b');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','//c');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a//b');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a//c');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','//*');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a//*');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/./a');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a/b/.');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a/b/..');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a/b/../@aa1');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/*');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/*/*');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/*/*/*');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a/child::*');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a/self::*');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a/descendant::*');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a/descendant-or-self::*');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a/attribute::*');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a/b/c/parent::*');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a/b/c/ancestor::*');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/a/b/c/ancestor-or-self::*');
select extractValue('<a aa1="aa1" aa2="aa2">a1<b ba1="ba1">b1<c>c1</c>b2</b>a2</a>','/descendant-or-self::*');
select extractValue('<a>a11<b ba="ba11" ba="ba12">b11</b><b ba="ba21" ba="ba22">b21<c>c1</c>b22</b>a12</a>','/a/b/c/ancestor-or-self::*');
select extractValue('<a>a11<b ba="ba11" ba="ba12">b11</b><b ba="ba21" ba="ba22">b21<c>c1</c>b22</b>a12</a>','//@ba');
select extractValue('<a><b>b</b><c>c</c></a>','/a/b');
select extractValue('<a><b>b</b><c>c</c></a>','/a/c');
select extractValue('<a><b>b</b><c>c</c></a>','/a/child::b');
select extractValue('<a><b>b</b><c>c</c></a>','/a/child::c');
select extractValue('<a><b>b1</b><c>c1</c><b>b2</b><c>c2</c></a>','/a/b[1]');
select extractValue('<a><b>b1</b><c>c1</c><b>b2</b><c>c2</c></a>','/a/b[2]');
select extractValue('<a><b>b1</b><c>c1</c><b>b2</b><c>c2</c></a>','/a/c[1]');
select extractValue('<a><b>b1</b><c>c1</c><b>b2</b><c>c2</c></a>','/a/c[2]');
select extractValue('<a><b x="xb1" x="xb2"/><c x="xc1" x="xc2"/></a>','/a//@x');
select extractValue('<a><b x="xb1" x="xb2"/><c x="xc1" x="xc2"/></a>','/a//@x[1]');
select extractValue('<a><b x="xb1" x="xb2"/><c x="xc1" x="xc2"/></a>','/a//@x[2]');
select extractValue('<a><b>b1</b><b>b2</b><c><b>c1b1</b><b>c1b2</b></c><c><b>c2b1</c></b></a>','//b[1]');
select extractValue('<a><b>b1</b><b>b2</b><c><b>c1b1</b><b>c1b2</b></c><c><b>c2b1</c></b></a>','/descendant::b[1]');

select extractValue('<a><b>b1</b><b>b2</b></a>','/a/b[1+0]');
select extractValue('<a><b>b1</b><b>b2</b></a>','/a/b[1*1]');
select extractValue('<a><b>b1</b><b>b2</b></a>','/a/b[--1]');
select extractValue('<a><b>b1</b><b>b2</b></a>','/a/b[2*1-1]');
select extractValue('<a><b>b1</b><b>b2</b></a>','/a/b[1+1]');
select extractValue('<a><b>b1</b><b>b2</b></a>','/a/b[1*2]');
select extractValue('<a><b>b1</b><b>b2</b></a>','/a/b[--2]');
select extractValue('<a><b>b1</b><b>b2</b></a>','/a/b[1*(3-1)]');
select extractValue('<a><b>b1</b><b>b2</b></a>','//*[1=1]');
select extractValue('<a><b>b1</b><b>b2</b></a>','//*[1!=1]');
select extractValue('<a><b>b1</b><b>b2</b></a>','//*[1>1]');
select extractValue('<a><b>b1</b><b>b2</b></a>','//*[2>1]');
select extractValue('<a><b>b1</b><b>b2</b></a>','//*[1>2]');
select extractValue('<a><b>b1</b><b>b2</b></a>','//*[1>=1]');
select extractValue('<a><b>b1</b><b>b2</b></a>','//*[2>=1]');
select extractValue('<a><b>b1</b><b>b2</b></a>','//*[1>=2]');
select extractValue('<a><b>b1</b><b>b2</b></a>','//*[1<1]');
select extractValue('<a><b>b1</b><b>b2</b></a>','//*[2<1]');
select extractValue('<a><b>b1</b><b>b2</b></a>','//*[1<2]');
select extractValue('<a><b>b1</b><b>b2</b></a>','//*[1<=1]');
select extractValue('<a><b>b1</b><b>b2</b></a>','//*[2<=1]');
select extractValue('<a><b>b1</b><b>b2</b></a>','//*[1<=2]');
select extractValue('<a><b>b11<c>c11</c></b><b>b21<c>c21</c></b></a>','/a/b[c="c11"]');
select extractValue('<a><b>b11<c>c11</c></b><b>b21<c>c21</c></b></a>','/a/b[c="c21"]');
select extractValue('<a><b c="c11">b11</b><b c="c21">b21</b></a>','/a/b[@c="c11"]');
select extractValue('<a><b c="c11">b11</b><b c="c21">b21</b></a>','/a/b[@c="c21"]');
select extractValue('<a>a1<b c="c11">b11<d>d11</d></b><b c="c21">b21<d>d21</d></b></a>', '/a/b[@c="c11"]/d');
select extractValue('<a>a1<b c="c11">b11<d>d11</d></b><b c="c21">b21<d>d21</d></b></a>', '/a/b[@c="c21"]/d');
select extractValue('<a>a1<b c="c11">b11<d>d11</d></b><b c="c21">b21<d>d21</d></b></a>', '/a/b[d="d11"]/@c');
select extractValue('<a>a1<b c="c11">b11<d>d11</d></b><b c="c21">b21<d>d21</d></b></a>', '/a/b[d="d21"]/@c');
select extractValue('<a>a1<b c="c11">b11<d>d11</d></b><b c="c21">b21<d>d21</d></b></a>', '/a[b="b11"]');
select extractValue('<a>a1<b c="c11">b11<d>d11</d></b><b c="c21">b21<d>d21</d></b></a>', '/a[b/@c="c11"]');
select extractValue('<a>a1<b c="c11">b11<d>d11</d></b><b c="c21">b21<d>d21</d></b></a>', '/a[b/d="d11"]');
select extractValue('<a>a1<b c="c11">b11<d>d11</d></b><b c="c21">b21<d>d21</d></b></a>', '/a[/a/b="b11"]');
select extractValue('<a>a1<b c="c11">b11<d>d11</d></b><b c="c21">b21<d>d21</d></b></a>', '/a[/a/b/@c="c11"]');
select extractValue('<a>a1<b c="c11">b11<d>d11</d></b><b c="c21">b21<d>d21</d></b></a>', '/a[/a/b/d="d11"]');
select extractValue('<a>a</a>', '/a[false()]');
select extractValue('<a>a</a>', '/a[true()]');
select extractValue('<a>a</a>', '/a[not(false())]');
select extractValue('<a>a</a>', '/a[not(true())]');
select extractValue('<a>a</a>', '/a[true() and true()]');
select extractValue('<a>a</a>', '/a[true() and false()]');
select extractValue('<a>a</a>', '/a[false()and false()]');
select extractValue('<a>a</a>', '/a[false()and true()]');
select extractValue('<a>a</a>', '/a[true() or true()]');
select extractValue('<a>a</a>', '/a[true() or false()]');
select extractValue('<a>a</a>', '/a[false()or false()]');
select extractValue('<a>a</a>', '/a[false()or true()]');


select extractValue('<a>ab<b c="c" c="e">b1</b><b c="d">b2</b><b c="f" c="e">b3</b></a>','/a/b[@c="c"]');
select extractValue('<a>ab<b c="c" c="e">b1</b><b c="d">b2</b><b c="f" c="e">b3</b></a>','/a/b[@c="d"]');
select extractValue('<a>ab<b c="c" c="e">b1</b><b c="d">b2</b><b c="f" c="e">b3</b></a>','/a/b[@c="e"]');
select extractValue('<a>ab<b c="c" c="e">b1</b><b c="d">b2</b><b c="f" c="e">b3</b></a>','/a/b[not(@c="e")]');
select extractValue('<a>ab<b c="c" c="e">b1</b><b c="d">b2</b><b c="f" c="e">b3</b></a>','/a/b[@c!="e"]');
select extractValue('<a>ab<b c="c" c="e">b1</b><b c="d">b2</b><b c="f" c="e">b3</b></a>','/a/b[@c="c" or @c="d"]');
select extractValue('<a>ab<b c="c" c="e">b1</b><b c="d">b2</b><b c="f" c="e">b3</b></a>','/a/b[@c="c" and @c="e"]');
select extractValue('<a><b c="c" d="d">b1</b><b d="d" e="e">b2</b></a>','/a/b[@c]');
select extractValue('<a><b c="c" d="d">b1</b><b d="d" e="e">b2</b></a>','/a/b[@d]');
select extractValue('<a><b c="c" d="d">b1</b><b d="d" e="e">b2</b></a>','/a/b[@e]');
select extractValue('<a><b c="c" d="d">b1</b><b d="d" e="e">b2</b></a>','/a/b[not(@c)]');
select extractValue('<a><b c="c" d="d">b1</b><b d="d" e="e">b2</b></a>','/a/b[not(@d)]');
select extractValue('<a><b c="c" d="d">b1</b><b d="d" e="e">b2</b></a>','/a/b[not(@e)]');
select extractValue('<a><b c="c" d="d">b1</b><b d="d" e="e">b2</b></a>', '/a/b[boolean(@c) or boolean(@d)]');
select extractValue('<a><b c="c" d="d">b1</b><b d="d" e="e">b2</b></a>', '/a/b[boolean(@c) or boolean(@e)]');
select extractValue('<a><b c="c" d="d">b1</b><b d="d" e="e">b2</b></a>', '/a/b[boolean(@d) or boolean(@e)]');
select extractValue('<a><b c="c" d="d">b1</b><b d="d" e="e">b2</b></a>', '/a/b[boolean(@c) and boolean(@d)]');
select extractValue('<a><b c="c" d="d">b1</b><b d="d" e="e">b2</b></a>', '/a/b[boolean(@c) and boolean(@e)]');
select extractValue('<a><b c="c" d="d">b1</b><b d="d" e="e">b2</b></a>', '/a/b[boolean(@d) and boolean(@e)]');
select extractValue('<a><b c="c" d="d">b1</b><b d="d" e="e">b2</b></a>', '/a/b[@c or @d]');
select extractValue('<a><b c="c" d="d">b1</b><b d="d" e="e">b2</b></a>', '/a/b[@c or @e]');
select extractValue('<a><b c="c" d="d">b1</b><b d="d" e="e">b2</b></a>', '/a/b[@d or @e]');
select extractValue('<a><b c="c" d="d">b1</b><b d="d" e="e">b2</b></a>', '/a/b[@c and @d]');
select extractValue('<a><b c="c" d="d">b1</b><b d="d" e="e">b2</b></a>', '/a/b[@c and @e]');
select extractValue('<a><b c="c" d="d">b1</b><b d="d" e="e">b2</b></a>', '/a/b[@d and @e]');
select extractValue('<a><b c="c">b1</b><b>b2</b></a>','/a/b[@*]');
select extractValue('<a><b c="c">b1</b><b>b2</b></a>','/a/b[not(@*)]');
select extractValue('<a>a</a>', '/a[ceiling(3.1)=4]');
select extractValue('<a>a</a>', '/a[floor(3.1)=3]');
select extractValue('<a>a</a>', '/a[round(3.1)=3]');
select extractValue('<a>a</a>', '/a[round(3.8)=4]');
select extractValue('<a><b>b</b><c>c</c></a>', '/a/b | /a/c');
select extractValue('<a b="b1" b="b2" b="b3"/>','/a/@b[position()=1]');
select extractValue('<a b="b1" b="b2" b="b3"/>','/a/@b[position()=2]');
select extractValue('<a b="b1" b="b2" b="b3"/>','/a/@b[position()=3]');
select extractValue('<a b="b1" b="b2" b="b3"/>','/a/@b[1=position()]');
select extractValue('<a b="b1" b="b2" b="b3"/>','/a/@b[2=position()]');
select extractValue('<a b="b1" b="b2" b="b3"/>','/a/@b[3=position()]');
select extractValue('<a b="b1" b="b2" b="b3"/>','/a/@b[2>=position()]');
select extractValue('<a b="b1" b="b2" b="b3"/>','/a/@b[2<=position()]');
select extractValue('<a b="b1" b="b2" b="b3"/>','/a/@b[position()=3 or position()=2]');
select extractValue('<a>a<b>a1<c>c1</c></b><b>a2</b></a>','/a/b[count(c)=0]');
select extractValue('<a>a<b>a1<c>c1</c></b><b>a2</b></a>','/a/b[count(c)=1]');
select extractValue('<a>a1<b ba="1" ba="2">b1</b><b>b2</b>4</a>','/a/b[sum(@ba)=3]');
select extractValue('<a><b>b1</b><b>b2</b></a>','/a/b[1]');
select extractValue('<a><b>b1</b><b>b2</b></a>','/a/b[boolean(1)]');
select extractValue('<a><b>b1</b><b>b2</b></a>','/a/b[true()]');
select extractValue('<a><b>b1</b><b>b2</b></a>','/a/b[number(true())]');
select extractValue('<a>ab</a>','/a[contains("abc","b")]');
select extractValue('<a>ab</a>','/a[contains(.,"a")]');
select extractValue('<a>ab</a>','/a[contains(.,"b")]');
select extractValue('<a>ab</a>','/a[contains(.,"c")]');
select extractValue('<a b="1">ab</a>','/a[concat(@b,"2")="12"]');
select extractValue('<a b="11" b="12" b="21" b="22">ab</a>', '/a/@b[substring(.,2)="1"]');
select extractValue('<a b="11" b="12" b="21" b="22">ab</a>', '/a/@b[substring(.,2)="2"]');
select extractValue('<a b="11" b="12" b="21" b="22">ab</a>', '/a/@b[substring(.,1,1)="1"]');
select extractValue('<a b="11" b="12" b="21" b="22">ab</a>', '/a/@b[substring(.,1,1)="2"]');
select extractValue('<a b="11" b="12" b="21" b="22">ab</a>', '/a/@b[substring(.,2,1)="1"]');
select extractValue('<a b="11" b="12" b="21" b="22">ab</a>', '/a/@b[substring(.,2,1)="2"]');
select extractValue('<a><b>b1</b><b>b2</b></a>', '/a/b[string-length("x")=1]');
select extractValue('<a><b>b1</b><b>b2</b></a>', '/a/b[string-length("xx")=2]');
select extractValue('<a><b>b1</b><b>b2</b></a>', '/a/b[string-length("xxx")=2]');
select extractValue('<a><b>b1</b><b>b2</b></a>', '/a/b[string-length("x")]');
select extractValue('<a><b>b1</b><b>b2</b></a>', '/a/b[string-length("xx")]');
select extractValue('<a><b>b1</b><b>b2</b></a>', '/a/b[string-length()]');
select extractValue('<a><b>b1</b><b>b2</b></a>', 'string-length()');
select extractValue('<a><b>b1</b><b>b2</b></a>', 'string-length("x")');
select extractValue('<a b="b11" b="b12" b="b21" b="22"/>','/a/@b');
select extractValue('<a b="b11" b="b12" b="b21" b="22"/>','/a/@b[contains(.,"1")]');
select extractValue('<a b="b11" b="b12" b="b21" b="22"/>','/a/@b[contains(.,"1")][contains(.,"2")]');
select extractValue('<a b="b11" b="b12" b="b21" b="22"/>','/a/@b[contains(.,"1")][contains(.,"2")][2]');

select UpdateXML('<a>a1<b>b1<c>c1</c>b2</b>a2</a>','/a/b/c','+++++++++');
select UpdateXML('<a>a1<b>b1<c>c1</c>b2</b>a2</a>','/a/b/c','<c1>+++++++++</c1>');
select UpdateXML('<a>a1<b>b1<c>c1</c>b2</b>a2</a>','/a/b/c','<c1/>');
select UpdateXML('<a><b>bb</b></a>', '/a/b', '<b>ccc</b>');
select UpdateXML('<a aa1="aa1" aa2="aa2"><b bb1="bb1" bb2="bb2">bb</b></a>', '/a/b', '<b>ccc</b>');
select UpdateXML('<a aa1="aa1" aa2="aa2"><b bb1="bb1" bb2="bb2">bb</b></a>', '/a/@aa1', '');
select UpdateXML('<a aa1="aa1" aa2="aa2"><b bb1="bb1" bb2="bb2">bb</b></a>', '/a/@aa1', 'aa3="aa3"');
select UpdateXML('<a aa1="aa1" aa2="aa2"><b bb1="bb1" bb2="bb2">bb</b></a>', '/a/@aa2', '');
select UpdateXML('<a aa1="aa1" aa2="aa2"><b bb1="bb1" bb2="bb2">bb</b></a>', '/a/@aa2', 'aa3="aa3"');
select UpdateXML('<a aa1="aa1" aa2="aa2"><b bb1="bb1" bb2="bb2">bb</b></a>', '/a/b/@bb1', '');
select UpdateXML('<a aa1="aa1" aa2="aa2"><b bb1="bb1" bb2="bb2">bb</b></a>', '/a/b/@bb1', 'bb3="bb3"');
select UpdateXML('<a aa1="aa1" aa2="aa2"><b bb1="bb1" bb2="bb2">bb</b></a>', '/a/b/@bb2', '');
select UpdateXML('<a aa1="aa1" aa2="aa2"><b bb1="bb1" bb2="bb2">bb</b></a>', '/a/b/@bb2', 'bb3="bb3"');
select updatexml('<div><div><span>1</span><span>2</span></div></div>','/','<tr><td>1</td><td>2</td></tr>') as upd1;
select updatexml('', '/', '') as upd2;
select extractvalue('<order><clerk>lesser wombat</clerk></order>','order/clerk');
select extractvalue('<order><clerk>lesser wombat</clerk></order>','/order/clerk');
select extractvalue('<a><b>B</b></a>','/a|/b');
select extractvalue('<a><b>B</b></a>','/a|b');
select extractvalue('<a>a<b>B</b></a>','/a|/b');
select extractvalue('<a>a<b>B</b></a>','/a|b');
select extractvalue('<a>a<b>B</b></a>','a|/b');
select extractvalue('<a>A</a>','/<a>');
select extractvalue('<a><b>b</b><b!>b!</b!></a>','//b!');
select extractvalue('<a>A<b>B<c>C</c></b></a>','/a/descendant::*');
select extractvalue('<a>A<b>B<c>C</c></b></a>','/a/self::*');
select extractvalue('<a>A<b>B<c>C</c></b></a>','/a/descendant-or-self::*');
select extractvalue('<A_B>A</A_B>','/A_B');
select extractvalue('<a>A<b>B1</b><b>B2</b></a>','/a/b[position()]');
select extractvalue('<a>A<b>B1</b><b>B2</b></a>','/a/b[count(.)=last()]');
select extractvalue('<a>A<b>B1</b><b>B2</b></a>','/a/b[last()]');
select extractvalue('<a>A<b>B1</b><b>B2</b></a>','/a/b[last()-1]');
select extractvalue('<a>A<b>B1</b><b>B2</b></a>','/a/b[last()=1]');
select extractvalue('<a>A<b>B1</b><b>B2</b></a>','/a/b[last()=2]');
select extractvalue('<a>A<b>B1</b><b>B2</b></a>','/a/b[last()=position()]');
select extractvalue('<a>A<b>B1</b><b>B2</b></a>','/a/b[count(.)]');
select extractvalue('<a>A<b>B1</b><b>B2</b></a>','/a/b[count(.)-1]');
select extractvalue('<a>A<b>B1</b><b>B2</b></a>','/a/b[count(.)=1]');
select extractvalue('<a>A<b>B1</b><b>B2</b></a>','/a/b[count(.)=2]');
select extractvalue('<a>A<b>B1</b><b>B2</b></a>','/a/b[count(.)=position()]');
select extractvalue('<a>jack</a>','/a[contains(../a,"J")]');
select extractvalue('<a>jack</a>','/a[contains(../a,"j")]');
select ExtractValue('<tag1><![CDATA[test]]></tag1>','/tag1');
select extractValue('<a>a','/a');
select extractValue('<a>a<','/a');
select extractValue('<a>a</','/a');
select extractValue('<a>a</a','/a');
select extractValue('<a>a</a></b>','/a');
select extractValue('<a b=>a</a>','/a');
select extractValue('<e>1</e>','position()');
select extractValue('<e>1</e>','last()');
select extractValue('<e><a>1</a></e>','/e/');
set names utf8;
select extractValue('<Ñ><r>r</r></Ñ>','/Ñ/r');
select extractValue('<r><Ñ>Ñ</Ñ></r>','/r/Ñ');
select extractValue('<Ñ r="r"/>','/Ñ/@r');
select extractValue('<r Ñ="Ñ"/>','/r/@Ñ');
select extractValue('<ns:element xmlns:ns="myns"/>','count(ns:element)');
select extractValue('<ns:element xmlns:ns="myns">a</ns:element>','/ns:element');
select extractValue('<ns:element xmlns:ns="myns">a</ns:element>','/ns:element/@xmlns:ns');
select extractValue('<foo><foo.bar>Data</foo.bar><something>Otherdata</something></foo>','/foo/foo.bar');
select extractValue('<foo><foo.bar>Data</foo.bar><something>Otherdata</something></foo>','/foo/something');
select extractValue('<zot><tim0><01>10:39:15</01><02>140</02></tim0></zot>','/zot/tim0/02');
select extractValue('<zot><tim0><01>10:39:15</01><02>140</02></tim0></zot>','//*');
select extractValue('<.>test</.>','//*');
select extractValue('<->test</->','//*');
select extractValue('<:>test</:>','//*');
select extractValue('<_>test</_>','//*');
select extractValue('<x.-_:>test</x.-_:>','//*');
select ExtractValue('<a><b><Text>test</Text></b></a>','/a/b/Text');
select ExtractValue('<a><b><comment>test</comment></b></a>','/a/b/comment');
select ExtractValue('<a><b><node>test</node></b></a>','/a/b/node');
select ExtractValue('<a><b><processing-instruction>test</processing-instruction></b></a>','/a/b/processing-instruction');
select ExtractValue('<a><and>test</and></a>', '/a/and');
select ExtractValue('<a><or>test</or></a>', '/a/or');
select ExtractValue('<a><mod>test</mod></a>', '/a/mod');
select ExtractValue('<a><div>test</div></a>', '/a/div');
select ExtractValue('<a><and:and>test</and:and></a>', '/a/and:and');
select ExtractValue('<a><or:or>test</or:or></a>', '/a/or:or');
select ExtractValue('<a><mod:mod>test</mod:mod></a>', '/a/mod:mod');
select ExtractValue('<a><div:div>test</div:div></a>', '/a/div:div');
select ExtractValue('<a><ancestor>test</ancestor></a>', '/a/ancestor');
select ExtractValue('<a><ancestor-or-self>test</ancestor-or-self></a>', '/a/ancestor-or-self');
select ExtractValue('<a><attribute>test</attribute></a>', '/a/attribute');
select ExtractValue('<a><child>test</child></a>', '/a/child');
select ExtractValue('<a><descendant>test</descendant></a>', '/a/descendant');
select ExtractValue('<a><descendant-or-self>test</descendant-or-self></a>', '/a/descendant-or-self');
select ExtractValue('<a><following>test</following></a>', '/a/following');
select ExtractValue('<a><following-sibling>test</following-sibling></a>', '/a/following-sibling');
select ExtractValue('<a><namespace>test</namespace></a>', '/a/namespace');
select ExtractValue('<a><parent>test</parent></a>', '/a/parent');
select ExtractValue('<a><preceding>test</preceding></a>', '/a/preceding');
select ExtractValue('<a><preceding-sibling>test</preceding-sibling></a>', '/a/preceding-sibling');
select ExtractValue('<a><self>test</self></a>', '/a/self');
select ExtractValue('<a><b>b1</b><b>b2</b></a>','/a/b');
select ExtractValue('<a><b>b1</b><b>b2</b></a>','/a/c');
select ExtractValue('<a><b>b1</b><b>b2</b></a>','/a/b[-1]');
select ExtractValue('<a><b>b1</b><b>b2</b></a>','/a/b[10]');
select ExtractValue('<a><b>b1</b><b>b2</b></a>','/a/b["1"]');
select ExtractValue('<a><b>b1</b><b>b2</b></a>','/a/b["1 and string"]');
select ExtractValue('<a><b>b1</b><b>b2</b></a>','/a/b["string and 1"]');
select ExtractValue('<a><b>b1</b><b>b2</b></a>','/a/b["string"]');
select UpdateXML('<a>a</a>',repeat('a b ',1000),'');
select ExtractValue('<a>a</a>', '/a[@x=@y0123456789_0123456789_0123456789_0123456789]');
select ExtractValue('<a>a</a>', '/a[@x=$y0123456789_0123456789_0123456789_0123456789]');
select updatexml(NULL, 1, 1), updatexml(1, NULL, 1), updatexml(1, 1, NULL);
select updatexml(NULL, NULL, 1), updatexml(1, NULL, NULL), updatexml(NULL, 1, NULL);
select updatexml(NULL, NULL, NULL);

select ExtractValue('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html> 
 <head>
  <title> Title - document with document declaration</title>
 </head> 
  <body> Hi, Im a webpage with document a declaration </body> 
</html>', 'html/head/title');
select ExtractValue('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html> 
 <head>
  <title> Title - document with document declaration</title>
 </head> 
  <body> Hi, Im a webpage with document a declaration </body> 
</html>', 'html/body');

select ExtractValue('<xml "xxx" "yyy">CharData</xml>', '/xml');
select ExtractValue('<xml  xxx  "yyy">CharData</xml>', '/xml');

set @x=10;
select extractvalue('<a></a>','$@x/a');
select extractvalue('<a></a>','round(123.4)/a');
select extractvalue('<a></a>','1/a');
select extractvalue('<a></a>','"b"/a');
select extractvalue('<a></a>','(1)/a');


CREATE TABLE IF NOT EXISTS t1 (
  id int(10) unsigned NOT NULL AUTO_INCREMENT,
  xml text,
  PRIMARY KEY (id)
);

INSERT INTO t1 (id, xml) VALUES
(15, '<bla name="blubb"></bla>'),
(14, '<bla name="blubd"></bla>');

select
extractvalue( xml, '/bla/@name' ),
extractvalue( xml, '/bla/@name' )
FROM t1 ORDER BY t1.id;

select
UpdateXML(xml, '/bla/@name', 'test'),
UpdateXML(xml, '/bla/@name', 'test')
FROM t1 ORDER BY t1.id;

DROP TABLE t1;

select UPDATEXML(NULL, (LPAD(0.1111E-15, '2011', 1)), 1);
select EXTRACTVALUE('', LPAD(0.1111E-15, '2011', 1));


select UPDATEXML(CONVERT(_latin1'<' USING utf8),'1','1');
select UPDATEXML(CONVERT(_latin1'<!--' USING utf8),'1','1');

select ExtractValue(CONVERT('<\"', BINARY(10)), 1);

select UPDATEXML('','(a)/a','');
select UPDATEXML('<a><a>x</a></a>','(a)/a','<b />');
select UPDATEXML('<a><c><a>x</a></c></a>','(a)/a','<b />');
select UPDATEXML('<a><c><a>x</a></c></a>','(a)//a','<b />');
select ExtractValue('<a><a>aa</a><b>bb</b></a>','(a)/a|(a)/b');

select extractValue('<a>a1<b ba1="1" ba2="2">b1</b>4</a>','/a/b[sum(@ba1)=1]');

select extractValue('<a>text1<b>text2</b></a>', '/a[contains(.,"text")]');
select extractValue('<a>text1<b>text2</b></a>', '/a[CONTAINS(.,"text")]'); 
select extractValue('<a>text1<b>text2</b></a>', '/a[CoNtAiNs(.,"text")]');
select extractValue('<a>abc</a>', '/a[contains(.,"ABC")]');
select extractValue('<a>abc</a>', '/a[contains(.,"AbC")]');
select extractValue('<a><b>1</b><b>2</b></a>', 'count(/a/b)');
select extractValue('<a><b>1</b><b>2</b></a>', 'COUNT(/a/b)');
select extractValue('<a>text1<b>x</b><b>y</b></a>', '/a/b[position()=1]');
select extractValue('<a>text1<b>x</b><b>y</b></a>', '/a/b[position()=1 OR position()=2]');
select extractValue('<a>text1<b>x</b><b>y</b></a>', '/a/b[position()=1 or position()=2]');
select extractValue('<a><b><c>text</c></b></a>', '/a/descendant::c');
select extractValue('<a><b><c>text</c></b></a>', '/a/DESCENDANT::c');
select extractValue('<a><b><c>text</c></b></a>', '/a/descendant-or-self::c');
select extractValue('<a><b><c>text</c></b></a>', '/a/DESCENDANT-OR-SELF::c');
select extractValue('<A>text</A>', '/A');
select extractValue('<A>text</A>', '/a');
select extractValue('<a><b>text</b></a>', '/a/b');
select extractValue('<a><b>text</b></a>', '/a/B');
select extractValue('<a id="value">text</a>', '/a[@id]');
select extractValue('<a id="value">text</a>', '/a[@ID]');
select extractValue('<a ID="value">text</a>', '/a[@id]');
select extractValue('<a ID="value">text</a>', '/a[@ID]');
select extractvalue('<a>A<b>B1</b><b>B2</b></a>','/a/b[Last()]');
select extractValue('<a>a</a>', '/a[cEiling(3.1)=4]');
select extractValue('<a>a</a>', '/a[fLoor(3.1)=3]');
select extractValue('<a>a</a>', '/a[rounD(3.1)=3]');
select extractValue('<a>a</a>', '/a[roUnd(3.8)=4]');
select extractValue('<a>a</a>', '/a[faLse()]');
select extractValue('<a>a</a>', '/a[tRue()]');

