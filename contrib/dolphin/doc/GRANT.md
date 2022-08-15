
<!DOCTYPE html
  PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html lang="zh-cn" xml:lang="zh-cn">
<head>
      <meta http-equiv="Content-Type" content="text/html; charset=gb2312">
   
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="DC.Type" content="topic">
<meta name="DC.Title" content="GRANT">
<meta name="product" content="">
<meta name="DC.Relation" scheme="URI" content="zh-cn_topic_0289900416.html">
<meta name="prodname" content="">
<meta name="version" content="3.0.0">
<meta name="brand" content="openGauss">
<meta name="DC.Publisher" content="20220331">
<meta name="DC.Format" content="XHTML">
<meta name="DC.Identifier" content="ZH-CN_TOPIC_0289900312">
<meta name="DC.Language" content="zh-cn">
<link rel="stylesheet" type="text/css" href="public_sys-resources/commonltr.css">
<link rel="stylesheet" type="text/css" href="public_sys-resources/pygments.css">
<title>GRANT</title>
</head>
<body style="clear:both; padding-left:10px; padding-top:5px; padding-right:5px; padding-bottom:5px"><a name="ZH-CN_TOPIC_0289900312"></a><a name="ZH-CN_TOPIC_0289900312"></a>

<h1 class="topictitle1">GRANT</h1>
<div id="body2586631"><div class="section" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s4bc6f47f2f9e45c18707d7219f3987ee"><h4 class="sectiontitle">功能描述</h4><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_p1821110131201">对角色和用户进行授权操作。</p>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a1af4b134e10a40a4819cf52822723a01">使用GRANT命令进行用户授权包括以下场景：</p>
<ul id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_u9914f016f6d947f580d3b9ef56a9450b"><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l3d6974f0e8034ca7929801cb929f270a"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a57c8420b40664d00b5393fab6b0dd647">将系统权限授权给角色或用户</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_aa24ec417526945a2a8ec487c89b0257f">系统权限又称为用户属性，包括SYSADMIN、CREATEDB、CREATEROLE、AUDITADMIN、MONADMIN、OPRADMIN、POLADMIN、INHERIT、REPLICATION、VCADMIN和LOGIN等。</p>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a2a8d90db8ca146cbb8a2e8ba8277fb5f">系统权限一般通过CREATE/ALTER ROLE语法来指定。其中，SYSADMIN权限可以通过GRANT/REVOKE ALL PRIVILEGE授予或撤销。但系统权限无法通过ROLE和USER的权限被继承，也无法授予PUBLIC。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l0b5cd8e1889241088682f8b29b51a740"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ab18c3ba37a0e4a1b8142192f91cf3d15">将数据库对象授权给角色或用户</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a1fbd6d9bc7f348e9b0938e84247354f1">将数据库对象（表和视图、指定字段、数据库、函数、模式、表空间等）的相关权限授予特定角色或用户；</p>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_zh-cn_topic_0058965813_p370990115811">GRANT命令将数据库对象的特定权限授予一个或多个角色。这些权限会追加到已有的权限上。</p>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_af94f79d581244223ad246812be0a2e34">关键字PUBLIC表示该权限要赋予所有角色，包括以后创建的用户。PUBLIC可以看做是一个隐含定义好的组，它总是包括所有角色。任何角色或用户都将拥有通过GRANT直接赋予的权限和所属的权限，再加上PUBLIC的权限。</p>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a7a9cf169da1a4efc8743a442f1021f09">如果声明了WITH GRANT OPTION，则被授权的用户也可以将此权限赋予他人，否则就不能授权给他人。这个选项不能赋予PUBLIC，这是<span id="ZH-CN_TOPIC_0289900312__text1798254510538">openGauss</span>特有的属性。</p>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_p113955825012"><span id="ZH-CN_TOPIC_0289900312__text264253102918">openGauss</span>会将某些类型的对象上的权限授予PUBLIC。默认情况下，对表、表字段、序列、外部数据源、外部服务器、模式或表空间对象的权限不会授予PUBLIC，而以下这些对象的权限会授予PUBLIC：数据库的CONNECT权限和CREATE TEMP TABLE权限、函数的EXECUTE特权、语言和数据类型（包括域）的USAGE特权。当然，对象拥有者可以撤销默认授予PUBLIC的权限并专门授予权限给其他用户。为了更安全，建议在同一个事务中创建对象并设置权限，这样其他用户就没有时间窗口使用该对象。另外可参考安全加固指南的权限控制章节，对PUBLIC用户组的权限进行限制。这些初始的默认权限可以使用ALTER DEFAULT PRIVILEGES命令修改。</p>
<p id="ZH-CN_TOPIC_0289900312__p1576104516111">对象的所有者缺省具有该对象上的所有权限，出于安全考虑所有者可以舍弃部分权限，但ALTER、DROP、COMMENT、INDEX、VACUUM以及对象的可再授予权限属于所有者固有的权限，隐式拥有。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_leaa2f1ebaaed4b51854e8d672bdbb3e9"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_aa8141f52d6e64b9c81fda2cf86a8ac85">将角色或用户的权限授权给其他角色或用户</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a2517627e842b4530aaf5b8265c77111c">将一个角色或用户的权限授予一个或多个其他角色或用户。在这种情况下，每个角色或用户都可视为拥有一个或多个数据库权限的集合。</p>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a730cbd38e3b34073b87d21b30ec659f5">当声明了WITH ADMIN OPTION，被授权的用户可以将该权限再次授予其他角色或用户，以及撤销所有由该角色或用户继承到的权限。当授权的角色或用户发生变更或被撤销时，所有继承该角色或用户权限的用户拥有的权限都会随之发生变更。</p>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ac68dc52b07614d4baf7e080b2f4d0ddc">数据库系统管理员可以给任何角色或用户授予/撤销任何权限。拥有CREATEROLE权限的角色可以赋予或者撤销任何非系统管理员角色的权限。</p>
</li><li id="ZH-CN_TOPIC_0289900312__li621082125315"><strong id="ZH-CN_TOPIC_0289900312__b877813811256">将ANY权限授予给角色或用户</strong><p id="ZH-CN_TOPIC_0289900312__p1923511417533">将ANY权限授予特定的角色和用户，ANY权限的取值范围参见语法格式。当声明了WITH ADMIN OPTION，被授权的用户可以将该ANY权限再次授予其他角色/用户，或从其他角色/用户处回收该ANY权限。ANY权限可以通过角色被继承，但不能赋予PUBLIC。初始用户和三权分立关闭时的系统管理员用户可以给任何角色/用户授予或撤销ANY权限。</p>
<p id="ZH-CN_TOPIC_0289900312__p122730915617">目前支持以下ANY权限：CREATE ANY TABLE、ALTER ANY TABLE、DROP ANY TABLE、SELECT ANY TABLE、INSERT ANY TABLE、UPDATE ANY TABLE、DELETE ANY TABLE、CREATE ANY SEQUENCE、CREATE ANY INDEX、CREATE ANY FUNCTION、EXECUTE ANY FUNCTION、 CREATE ANY PACKAGE、EXECUTE ANY PACKAGE、CREATE ANY TYPE。详细的ANY权限范围描述参考<a href="#ZH-CN_TOPIC_0289900312__table1360121832117">表1</a></p>
</li></ul>
</div>
<div class="section" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_section1780116145345"><h4 class="sectiontitle">注意事项</h4><ul id="ZH-CN_TOPIC_0289900312__ul1526015616163"><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0059778442_lc894783be3fc4f37a7c330c52658f851">不允许将ANY权限授予PUBLIC，也不允许从PUBLIC回收ANY权限。</li><li id="ZH-CN_TOPIC_0289900312__li146349190165">ANY权限属于数据库内的权限，只对授予该权限的数据库内的对象有效，例如SELECT ANY TABLE只允许用户查看当前数据库内的所有用户表数据，对其他数据库内的用户表无查看权限。</li><li id="ZH-CN_TOPIC_0289900312__li1921123131718">即使用户被授予ANY权限，也不能对私有用户下的对象进行访问操作（INSERT、DELETE、UPDATE、SELECT）。</li><li id="ZH-CN_TOPIC_0289900312__li1066684810177">ANY权限与原有的权限相互无影响。</li><li id="ZH-CN_TOPIC_0289900312__li1973825191815">如果用户被授予CREATE ANY TABLE权限，在同名schema下创建表的属主是该schema的创建者，用户对表进行其他操作时，需要授予相应的操作权限。</li></ul>
</div>
<div class="section" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s9b21365068e9482782f400457afa8a01"><h4 class="sectiontitle">语法格式</h4><ul id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_u486412225f9c45168b249bd41faa5363"><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_la6e58034572346a3a04ee75809c775d3">将表或视图的访问权限赋予指定的用户或角色。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s99752dc2c9f342569f69981eaebab01c">GRANT { { SELECT | INSERT | UPDATE | DELETE | TRUNCATE | REFERENCES | ALTER | DROP | COMMENT | INDEX | VACUUM } [, ...] 
      | ALL [ PRIVILEGES ] }
    ON { [ TABLE ] table_name [, ...]
       | ALL TABLES IN SCHEMA schema_name [, ...] }
    TO { [ GROUP ] role_name | PUBLIC } [, ...] 
    [ WITH GRANT OPTION ];
</pre>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_la4223281c2ff425d87ed7803a6a61cf5">将表中字段的访问权限赋予指定的用户或角色。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s86c6f055a85a4171a4efdbe9e7a18ea6">GRANT { {{ SELECT | INSERT | UPDATE | REFERENCES | COMMENT } ( column_name [, ...] )} [, ...] 
      | ALL [ PRIVILEGES ] ( column_name [, ...] ) }
    ON [ TABLE ] table_name [, ...]
    TO { [ GROUP ] role_name | PUBLIC } [, ...]
    [ WITH GRANT OPTION ];</pre>
</li><li id="ZH-CN_TOPIC_0289900312__li18626130133712">将序列的访问权限赋予指定的用户或角色，LARGE字段属性可选，赋权语句不区分序列是否为LARGE。<pre class="screen" id="ZH-CN_TOPIC_0289900312__screen862611019378">GRANT { { SELECT | UPDATE | USAGE | ALTER | DROP | COMMENT } [, ...] 
      | ALL [ PRIVILEGES ] }
    ON { [ [ LARGE ] SEQUENCE ] sequence_name [, ...]
       | ALL SEQUENCES IN SCHEMA schema_name [, ...] }
    TO { [ GROUP ] role_name | PUBLIC } [, ...] 
    [ WITH GRANT OPTION ];</pre>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l9e7af9bd57df48a38da181b48eeecb05">将数据库的访问权限赋予指定的用户或角色。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s694c266ce33c4f588938acaba54cb164">GRANT { { CREATE | CONNECT | TEMPORARY | TEMP | CREATE TEMPORARY TABLES | ALTER | DROP | COMMENT } [, ...]
      | ALL [ PRIVILEGES ] }
    ON { DATABASE database_name [, ...] | database_name.* }
    TO { [ GROUP ] role_name | PUBLIC } [, ...]
    [ WITH GRANT OPTION ];</pre>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_la09ed3d2ec274e41b877123e99ca6db2">将域的访问权限赋予指定的用户或角色。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s79a3473ec4ba4dbaa07c9cae74e5c3ec">GRANT { USAGE | ALL [ PRIVILEGES ] }
    ON DOMAIN domain_name [, ...]
    TO { [ GROUP ] role_name | PUBLIC } [, ...]
    [ WITH GRANT OPTION ];</pre>
<div class="note" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_n7bceb61b4dbb4b30870b9a86e7a1a274"><span class="notetitle"> </span><div class="notebody"><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_acc32c93750ca441c8b763f51b452a618">本版本暂时不支持赋予域的访问权限。</p>
</div></div>
</li><li id="ZH-CN_TOPIC_0289900312__li37021840142413">将客户端加密主密钥CMK的访问权限赋予指定的用户或角色。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900312__screen670219403245"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">GRANT</span> <span class="err">{</span> <span class="err">{</span> <span class="k">USAGE</span> <span class="o">|</span> <span class="k">DROP</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span> <span class="o">|</span> <span class="k">ALL</span> <span class="p">[</span> <span class="k">PRIVILEGES</span> <span class="p">]</span> <span class="err">}</span>
    <span class="k">ON</span> <span class="n">CLIENT_MASTER_KEY</span> <span class="n">client_master_key</span> <span class="p">[,</span> <span class="p">...]</span> 
    <span class="k">TO</span> <span class="err">{</span> <span class="p">[</span> <span class="k">GROUP</span> <span class="p">]</span> <span class="n">role_name</span> <span class="o">|</span> <span class="k">PUBLIC</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span> 
    <span class="p">[</span> <span class="k">WITH</span> <span class="k">GRANT</span> <span class="k">OPTION</span> <span class="p">];</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900312__li11567192910282">将列加密密钥CEK的访问权限赋予指定的用户或角色。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900312__screen656782982818"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">GRANT</span> <span class="err">{</span> <span class="err">{</span> <span class="k">USAGE</span> <span class="o">|</span> <span class="k">DROP</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span> <span class="o">|</span> <span class="k">ALL</span> <span class="p">[</span> <span class="k">PRIVILEGES</span> <span class="p">]</span> <span class="err">}</span>
    <span class="k">ON</span> <span class="n">COLUMN_ENCRYPTION_KEY</span> <span class="n">column_encryption_key</span> <span class="p">[,</span> <span class="p">...]</span> 
    <span class="k">TO</span> <span class="err">{</span> <span class="p">[</span> <span class="k">GROUP</span> <span class="p">]</span> <span class="n">role_name</span> <span class="o">|</span> <span class="k">PUBLIC</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span> 
    <span class="p">[</span> <span class="k">WITH</span> <span class="k">GRANT</span> <span class="k">OPTION</span> <span class="p">];</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_la72696894620487f845cfc7e8f1ba2cd">将外部数据源的访问权限赋予给指定的用户或角色。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_sd02f30e1f4d14b128fff64e380b02343">GRANT { USAGE | ALL [ PRIVILEGES ] }
    ON FOREIGN DATA WRAPPER fdw_name [, ...]
    TO { [ GROUP ] role_name | PUBLIC } [, ...]
    [ WITH GRANT OPTION ];</pre>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ld50a23786c454a5f9022156c1340570d">将外部服务器的访问权限赋予给指定的用户或角色。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s4e2d6abc087a40e2bdd0d9d8a43c4537">GRANT { { USAGE | ALTER | DROP | COMMENT } [, ...] | ALL [ PRIVILEGES ] }
    ON FOREIGN SERVER server_name [, ...]
    TO { [ GROUP ] role_name | PUBLIC } [, ...]
    [ WITH GRANT OPTION ];</pre>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_leb18afec01124132bf815ef7c2e40b9e">将函数的访问权限赋予给指定的用户或角色。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s237ad05ae9ea49149f068109eafa1339">GRANT { { EXECUTE | ALTER | ALTER ROUTINE | DROP | COMMENT } [, ...] | ALL [ PRIVILEGES ] }
    ON { FUNCTION {function_name ( [ {[ argmode ] [ arg_name ] arg_type} [, ...] ] )} [, ...]
       | ALL FUNCTIONS IN SCHEMA schema_name [, ...]  | schema_name.* }
    TO { [ GROUP ] role_name | PUBLIC } [, ...]
    [ WITH GRANT OPTION ];</pre>
</li><li id="ZH-CN_TOPIC_0289900312__li916911244231">将存储过程的访问权限赋予给指定的用户或角色。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900312__screen17630916174911"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">GRANT</span> <span class="err">{</span> <span class="err">{</span> <span class="k">EXECUTE</span> <span class="o">|</span> <span class="k">ALTER</span> <span class="o">|</span> <span class="k">ALTER ROUTINE</span> <span class="o">|</span> <span class="k">DROP</span> <span class="o">|</span> <span class="k">COMMENT</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span> <span class="o">|</span> <span class="k">ALL</span> <span class="p">[</span> <span class="k">PRIVILEGES</span> <span class="p">]</span> <span class="err">}</span>
    <span class="k">ON</span> <span class="err">{</span> <span class="k">PROCEDURE</span> <span class="err">{</span><span class="n">proc_name</span> <span class="p">(</span> <span class="p">[</span> <span class="err">{</span><span class="p">[</span> <span class="n">argmode</span> <span class="p">]</span> <span class="p">[</span> <span class="n">arg_name</span> <span class="p">]</span> <span class="n">arg_type</span><span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span> <span class="p">]</span> <span class="p">)</span><span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
       <span class="o">|</span> <span class="k">ALL</span> <span class="k">PROCEDURE</span> <span class="k">IN</span> <span class="k">SCHEMA</span> <span class="k">schema_name</span> <span class="p">[,</span> <span class="p">...]</span> <span class="o">|</span> <span class="k">schema_name.*</span> <span class="err">}</span>
    <span class="k">TO</span> <span class="err">{</span> <span class="p">[</span> <span class="k">GROUP</span> <span class="p">]</span> <span class="n">role_name</span> <span class="o">|</span> <span class="k">PUBLIC</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="p">[</span> <span class="k">WITH</span> <span class="k">GRANT</span> <span class="k">OPTION</span> <span class="p">];</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ldf0eb02117b849618bd158d14ad689fd">将过程语言的访问权限赋予给指定的用户或角色。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_sfd53fca2f10e4134adcc1974958e635c">GRANT { USAGE | ALL [ PRIVILEGES ] }
    ON LANGUAGE lang_name [, ...]
    TO { [ GROUP ] role_name | PUBLIC } [, ...]
    [ WITH GRANT OPTION ];</pre>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_lddbafa614df248b6b11e0cc0343550b5">将大对象的访问权限赋予指定的用户或角色。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_se0c7e82345b24fb2a50a734ceaa953ee">GRANT { { SELECT | UPDATE } [, ...] | ALL [ PRIVILEGES ] }
    ON LARGE OBJECT loid [, ...]
    TO { [ GROUP ] role_name | PUBLIC } [, ...]
    [ WITH GRANT OPTION ];</pre>
<div class="note" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_n16f6eda6e6b64915b5b8766a2e9806c4"><span class="notetitle"> </span><div class="notebody"><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ac043b6b5737c4a93b990d93261893c9f">本版本暂时不支持大对象。</p>
</div></div>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_labe4483cb4534c8ca561b7d52ecd2fd9">将模式的访问权限赋予指定的用户或角色。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_saa1dd5bff30f4f9d98feb56127825ef8">GRANT { { CREATE | USAGE | ALTER | DROP | COMMENT } [, ...] | ALL [ PRIVILEGES ] }
    ON SCHEMA schema_name [, ...]
    TO { [ GROUP ] role_name | PUBLIC } [, ...]
    [ WITH GRANT OPTION ];</pre>
<div class="note" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_n27d7a0552c2644b48f1b7fc1dec5b2e1"><span class="notetitle"> </span><div class="notebody"><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_zh-cn_topic_0058965813_p61234714018">将模式中的表或者视图对象授权给其他用户时，需要将表或视图所属的模式的USAGE权限同时授予该用户，若没有该权限，则只能看到这些对象的名称，并不能实际进行对象访问。 同名模式下创建表的权限无法通过此语法赋予，可以通过将角色的权限赋予其他用户或角色的语法，赋予同名模式下创建表的权限。</p>
</div></div>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l1555ff84126d4982b671333ce94191b7">将表空间的访问权限赋予指定的用户或角色。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s064bb2adf29c420cb924dbb90ab0807f">GRANT { { CREATE | ALTER | DROP | COMMENT } [, ...] | ALL [ PRIVILEGES ] }
    ON TABLESPACE tablespace_name [, ...]
    TO { [ GROUP ] role_name | PUBLIC } [, ...]
    [ WITH GRANT OPTION ];</pre>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l15351614d0b5426cb308a35699337ebd">将类型的访问权限赋予指定的用户或角色。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_se6a8305f7e7348e88bb42b48587eaf52">GRANT { { USAGE | ALTER | DROP | COMMENT } [, ...] | ALL [ PRIVILEGES ] }
    ON TYPE type_name [, ...]
    TO { [ GROUP ] role_name | PUBLIC } [, ...]
    [ WITH GRANT OPTION ];</pre>
<div class="note" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_nfd3e706d5ad4411cb4f1a78bed53cee0"><span class="notetitle"> </span><div class="notebody"><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a29d57d5b8c204c348e35b0961d407054">本版本暂时不支持赋予类型的访问权限。</p>
</div></div>
</li><li id="ZH-CN_TOPIC_0289900312__li132851965186">将Data Source对象的权限赋予指定的角色。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900312__screen1159361071812"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">GRANT</span> <span class="err">{</span> <span class="k">USAGE</span> <span class="o">|</span> <span class="k">ALL</span> <span class="p">[</span><span class="k">PRIVILEGES</span><span class="p">]</span><span class="err">}</span>
   <span class="k">ON</span> <span class="k">DATA</span> <span class="k">SOURCE</span> <span class="n">src_name</span> <span class="p">[,</span> <span class="p">...]</span>
   <span class="k">TO</span> <span class="err">{</span> <span class="p">[</span><span class="k">GROUP</span><span class="p">]</span> <span class="n">role_name</span> <span class="o">|</span> <span class="k">PUBLIC</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
   <span class="p">[</span><span class="k">WITH</span> <span class="k">GRANT</span> <span class="k">OPTION</span><span class="p">];</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900312__li18327151911184">将directory对象的权限赋予指定的角色。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900312__screen1859318107182"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">GRANT</span> <span class="err">{</span> <span class="err">{</span> <span class="k">READ</span> <span class="o">|</span> <span class="k">WRITE</span> <span class="o">|</span> <span class="k">ALTER</span> <span class="o">|</span> <span class="k">DROP</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span> <span class="o">|</span> <span class="k">ALL</span> <span class="p">[</span><span class="k">PRIVILEGES</span><span class="p">]</span> <span class="err">}</span>
   <span class="k">ON</span> <span class="n">DIRECTORY</span> <span class="n">directory_name</span> <span class="p">[,</span> <span class="p">...]</span>
   <span class="k">TO</span> <span class="err">{</span> <span class="p">[</span><span class="k">GROUP</span><span class="p">]</span> <span class="n">role_name</span> <span class="o">|</span> <span class="k">PUBLIC</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
   <span class="p">[</span><span class="k">WITH</span> <span class="k">GRANT</span> <span class="k">OPTION</span><span class="p">];</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900312__li81880421244">将package对象的权限赋予指定的角色。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900312__screen1818874218241"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">GRANT</span> <span class="err">{</span> <span class="err">{</span> <span class="k">EXECUTE</span> <span class="o">|</span> <span class="k">ALTER</span> <span class="o">|</span> <span class="k">DROP</span> <span class="o">|</span> <span class="k">COMMENT</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span> <span class="o">|</span> <span class="k">ALL</span> <span class="p">[</span><span class="k">PRIVILEGES</span><span class="p">]</span> <span class="err">}</span>
   <span class="k">ON</span> <span class="err">{</span> <span class="n">PACKAGE</span> <span class="n">package_name</span> <span class="p">[,</span> <span class="p">...]</span>
      <span class="o">|</span> <span class="k">ALL</span> <span class="n">PACKAGES</span> <span class="k">IN</span> <span class="k">SCHEMA</span> <span class="k">schema_name</span> <span class="p">[,</span> <span class="p">...]</span> <span class="err">}</span>
   <span class="k">TO</span> <span class="err">{</span> <span class="p">[</span><span class="k">GROUP</span><span class="p">]</span> <span class="n">role_name</span> <span class="o">|</span> <span class="k">PUBLIC</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
   <span class="p">[</span><span class="k">WITH</span> <span class="k">GRANT</span> <span class="k">OPTION</span><span class="p">];</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l909b6acf69c046c38146fab22d824fd6">将角色的权限赋予其他用户或角色的语法。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s8e34d3f10d234dbab830fabcb5963234">GRANT role_name [, ...]
   TO role_name [, ...]
   [ WITH ADMIN OPTION ];</pre>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l909b6acf69c046c38146fab22d824fd6">将创建角色的权限赋予其他用户或角色的语法。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s8e34d3f10d234dbab830fabcb5963234">GRANT CREATE USER ON *.* TO ROLE_NAME;</pre>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l909b6acf69c046c38146fab22d824fd6">将创建表空间的权限赋予其他用户或角色的语法。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s8e34d3f10d234dbab830fabcb5963234">GRANT CREATE TABLESPACE ON *.* TO ROLE_NAME;</pre>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l185b60900eaa4793b4e6c30498687483">将sysadmin权限赋予指定的角色。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s39aaa23544924df6ae27181828c2342f">GRANT ALL { PRIVILEGES | PRIVILEGE }
   TO role_name;</pre>
</li></ul>
</div>
<ul id="ZH-CN_TOPIC_0289900312__ul5844554172010"><li id="ZH-CN_TOPIC_0289900312__li1042118569207">将ANY权限赋予其他用户或角色的语法。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900312__screen1042125682018"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">GRANT</span> <span class="err">{</span> <span class="k">CREATE</span> <span class="k">ANY</span> <span class="k">TABLE</span> <span class="o">|</span> <span class="k">ALTER</span> <span class="k">ANY</span> <span class="k">TABLE</span> <span class="o">|</span> <span class="k">DROP</span> <span class="k">ANY</span> <span class="k">TABLE</span> <span class="o">|</span> <span class="k">SELECT</span> <span class="k">ANY</span> <span class="k">TABLE</span> <span class="o">|</span> <span class="k">INSERT</span> <span class="k">ANY</span> <span class="k">TABLE</span> <span class="o">|</span> <span class="k">UPDATE</span> <span class="k">ANY</span> <span class="k">TABLE</span> <span class="o">|</span>
  <span class="k">DELETE</span> <span class="k">ANY</span> <span class="k">TABLE</span> <span class="o">|</span> <span class="k">CREATE</span> <span class="k">ANY</span> <span class="n">SEQUENCE</span> <span class="o">|</span> <span class="k">CREATE</span> <span class="k">ANY</span> <span class="k">INDEX</span> <span class="o">|</span> <span class="k">INDEX</span> <span class="o">|</span> <span class="k">CREATE</span> <span class="k">ANY</span> <span class="k">FUNCTION</span> <span class="o">|</span> <span class="k">CREATE</span> <span class="k">ROUTINE</span> <span class="o">|</span> <span class="k">EXECUTE</span> <span class="k">ANY</span> <span class="k">FUNCTION</span> <span class="o">|</span>
  <span class="k">CREATE</span> <span class="k">ANY</span> <span class="n">PACKAGE</span> <span class="o">|</span> <span class="k">EXECUTE</span> <span class="k">ANY</span> <span class="n">PACKAGE</span> <span class="o">|</span> <span class="k">CREATE</span> <span class="k">ANY</span> <span class="k">TYPE</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
  <span class="p">[</span> <span class="k">ON</span>  <span class="k">*.*</span> <span class="p">]</span>
  <span class="k">TO</span> <span class="p">[</span> <span class="k">GROUP</span> <span class="p">]</span> <span class="n">role_name</span> <span class="p">[,</span> <span class="p">...]</span>
  <span class="p">[</span> <span class="k">WITH</span> <span class="k">ADMIN</span> <span class="k">OPTION</span> <span class="p">];</span>
</pre></div>
</td></tr></table></div>
</li></ul>
<div class="section" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s3557d45c54124d86bc3f619358d806f8"><a name="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s3557d45c54124d86bc3f619358d806f8"></a><a name="zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s3557d45c54124d86bc3f619358d806f8"></a><h4 class="sectiontitle">参数说明</h4><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a1b5daae897a4428494749aaedc083fd5">GRANT的权限分类如下所示。</p>
<ul id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_u029ee3a8542a40e0b98c46347df3469a"><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_la8cdadf8efc543cf8f85ceef3d0d93cd"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a4992eaaefc644b5aa009ed8a87d7363a">SELECT</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_af2878880c6574fe298069013686d4636">允许对指定的表、视图、序列执行SELECT命令，update或delete时也需要对应字段上的select权限。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_lc4e046638e8a42b2a3d3c407e44f002f"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ad5b0a16494a04f3f9d1defc7d3312bc4">INSERT</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a7f18f8696534460a9c0f7222e625fc1b">允许对指定的表执行INSERT命令。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_lb2347e44442a47e0956c5f5c7a0ac96b"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ae520f18bb128484fac99f9e4465d7668">UPDATE</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a3470d68253d74c1e82010d1ff29701ee">允许对声明的表中任意字段执行UPDATE命令。通常，update命令也需要select权限来查询出哪些行需要更新。SELECT… FOR UPDATE和SELECT… FOR SHARE除了需要SELECT权限外，还需要UPDATE权限。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_lbe1bd9ac138541e1922a1ac52dd7baa8"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a295c5208a0a043f7adf100f0ec17f062">DELETE</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ade4d76835c884ea895f440f9b809a327">允许执行DELETE命令删除指定表中的数据。通常，delete命令也需要select权限来查询出哪些行需要删除。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l801fb2ef6a4c4b9f995404df1dc2a645"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_aeab398b400834e95b8406630169f41ac">TRUNCATE</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_zh-cn_topic_0058965813_p693168719133">允许执行TRUNCATE语句删除指定表中的所有记录。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_lc5c309b0542f43cf9670aab27d157323"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_afd91019ff86445f28b35493086380e17">REFERENCES</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_zh-cn_topic_0058965813_p215045819139">创建一个外键约束，必须拥有参考表和被参考表的REFERENCES权限。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ldcd4ca10a1e64d83a10f0835a339ad44"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_acbba72d09c1c432eb5125f7df9d482fb">CREATE</strong><ul id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ue2119a6acc264d3585b5db1dad04aad1"><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l6958656d57aa4b838db30e7613fac68d">对于数据库，允许在数据库里创建新的模式。</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l1444846f14b34bc0bf64d85413305fdf">对于模式，允许在模式中创建新的对象。如果要重命名一个对象，用户除了必须是该对象的所有者外，还必须拥有该对象所在模式的CREATE权限。</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l27f59263b9c745018ec1894d0f3fc4c4">对于表空间，允许在表空间中创建表，允许在创建数据库和模式的时候把该表空间指定为缺省表空间。</li></ul>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l0b17d22cc61d42a690399d516c26dc1b"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a12cc964fbf2145c1867bb83e2283014b">CONNECT</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_af8eaa31d8c16454f95096141e0aad327">允许用户连接到指定的数据库。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l36b288b4ed024594a48550f7f8025dba"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a0caec6a141f942888b48acdc175f2a6f">EXECUTE</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a9f391a50c7cb4e77b243f88ed9e0a333">允许使用指定的函数，以及利用这些函数实现的操作符。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ldb99d26c34694ce7ba5bd6b9a08f9678"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ad346185278684dbda43c6476fe1a2963">USAGE</strong><ul id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_u9e2be9e3b11f482ab69fda6bb1115131"><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l2c51d4dae23d4f84b575a17ab229703e">对于过程语言，允许用户在创建函数的时候指定过程语言。</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l248c3550b8db46dda03aea6debb51b10">对于模式，USAGE允许访问包含在指定模式中的对象，若没有该权限，则只能看到这些对象的名称。</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_lcc9f30d2fdb946f68e4f09a8b905284e">对于序列，USAGE允许使用nextval函数。</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_li75810362380">对于Data Source对象，USAGE是指访问权限，也是可赋予的所有权限，即USAGE与ALL PRIVILEGES等价。</li></ul>
</li><li id="ZH-CN_TOPIC_0289900312__li16334013316"><strong id="ZH-CN_TOPIC_0289900312__b1533809316">ALTER</strong><p id="ZH-CN_TOPIC_0289900312__p33370163116">允许用户修改指定对象的属性，但不包括修改对象的所有者和修改对象所在的模式。</p>
</li><li id="ZH-CN_TOPIC_0289900312__li16334013316"><strong id="ZH-CN_TOPIC_0289900312__b1533809316">ALTER ROUTINE</strong><p id="ZH-CN_TOPIC_0289900312__p33370163116">只能对function和procedure对象使用，和ALTER权限等价</p>
</li><li id="ZH-CN_TOPIC_0289900312__li260162163114"><strong id="ZH-CN_TOPIC_0289900312__b19601202153112">DROP</strong><p id="ZH-CN_TOPIC_0289900312__p1760182173110">允许用户删除指定的对象。</p>
</li><li id="ZH-CN_TOPIC_0289900312__li14540193173110"><strong id="ZH-CN_TOPIC_0289900312__b85400319312">COMMENT</strong><p id="ZH-CN_TOPIC_0289900312__p15401538318">允许用户定义或修改指定对象的注释。</p>
</li><li id="ZH-CN_TOPIC_0289900312__li14350144318"><strong id="ZH-CN_TOPIC_0289900312__b11121446113120">INDEX</strong><p id="ZH-CN_TOPIC_0289900312__p19350549313">允许用户在指定表上创建索引，并管理指定表上的索引，还允许用户对指定表执行REINDEX和CLUSTER操作。</p>
</li><li id="ZH-CN_TOPIC_0289900312__li51642619310"><strong id="ZH-CN_TOPIC_0289900312__b171647615313">VACUUM</strong><p id="ZH-CN_TOPIC_0289900312__p171646617311">允许用户对指定的表执行ANALYZE和VACUUM操作。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ld97a83be17c548bfa69167040bb1a1f7"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a5ef986d1622a4900ad333751b5a5748a">ALL PRIVILEGES</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_adba2687ee138469d98c81eb44bd0372b">一次性给指定用户/角色赋予所有可赋予的权限。只有系统管理员有权执行GRANT ALL PRIVILEGES。</p>
</li></ul>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a88ca52c5e85f4fdfb7efb88286287124">GRANT的参数说明如下所示。</p>
<ul id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_u482272de7a064e24b12d144824c83071"><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l30d3e513f316454ba11ddc4affc9b3cf"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a9927592555ac4f36bc36030305d63200">role_name</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ae2ffbfdc06684f60b60727eb0923d716">已存在用户名称。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l45fcf0aee866439db79932be9f78a36c"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a889accc9e99c481aa259b60e7ef6ae77">table_name</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a31435de293f6426ab94d007084d107ee">已存在表名称。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l10b14419e228487ea83de3b6517843ed"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a05d075b247a44e1ea18cfdc1736ca386">column_name</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a248925cc99a44d02910f8e800426dc0b">已存在字段名称。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l8046d9edb9d54e32a6eccaa00aa6b35c"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a66c8c64b576148e4a16f7d15fb0f6357">schema_name</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a168a1f33decd46709c85a552c49883e4">已存在模式名称。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ld797abd38f8648e689bd44e2eecb0bd2"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_aef57c3d94f3b4d60b9bcc26334d2e524">database_name</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a16a1636a9c3f4e979415d7d19d0516b1">已存在数据库名称。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_lc1405c468b26491d86521030a8dda96c"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ab6d65170b6504075a6a4f32c4064235d">function_name</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_aa57fdd12839340b5965506e3ce730bd6">已存在函数名称。</p>
</li><li id="ZH-CN_TOPIC_0289900312__li6790125914280"><strong id="ZH-CN_TOPIC_0289900312__b1079020598280">procedure_name</strong><p id="ZH-CN_TOPIC_0289900312__p187901559142818">已存在存储过程名称。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l22d8f80096bf48d09755a03c7c822f49"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_afdaff996d9294eafa9e02eccbfe195ce">sequence_name</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_zh-cn_topic_0058965813_p954458319166">已存在序列名称。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_lfbb07bd8ac6f407496f08a2fd5049ff0"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a46bcb481c00b47a18d12ab5037ce32f1">domain_name</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_af8949e748a264c10be82a7f431726880">已存在域类型名称。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l604155ab77bd4716843afc1a98492985"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a5b7108c542534fc5bc503fa8187d099c">fdw_name</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_aebda197c6c134a5bb7921650c8a46dd8">已存在外部数据包名称。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l1b2975118e0c42199b787c4b6d873c9e"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_add6b83cd0115401382d3ed9729a23d64">lang_name</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_acd53cf209ea043a7a2566585b52c231a">已存在语言名称。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l57e229365f354ff0bb19d9cd7a850d73"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ab897b2a4416d48258a5578b5eab09273">type_name</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ae408daa0ee404afe90ff4e1c367126f8">已存在类型名称。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_li8892131624720"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_b16811741191019">src_name</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_p1442152544713">已存在的Data Source对象名称。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_le37a44a59e69420a90700cacc3bf534f"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a63a7f265a5604485be7d045636fdcf3e">argmode</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a5bf91853ac054c6299d21dea9fce7b0e">参数模式。</p>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a0de243a938c64eb0b43857f538579a50">取值范围：字符串，要符合标识符命名规范。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l72920001ac634210b11ece8f1591db78"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a063647d8dfd74f96af53005d5120eacc">arg_name</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_abf1db9890e0244998c5d1cc0f45e85fa">参数名称。</p>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ae0de4ef8d05943f8abab200f825db13b">取值范围：字符串，要符合标识符命名规范。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l9ef56febef41450bbc182e41a435c93c"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_acc07042f50a94b878e58d65111687f87">arg_type</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a7c63d37afe03413395128f7e9f0972fd">参数类型。</p>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a3df3fd432cb046d3af74612dedc70a80">取值范围：字符串，要符合标识符命名规范。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l380a369859b4441392497ed414cb0fda"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_add8c903d707c4abb852d5893757fba81">loid</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_aa46be1ec3e9841c3835ece2d04287bb7">包含本页的大对象的标识符。</p>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ac0944ed764b94d2aba0d02f75d3b27b4">取值范围：字符串，要符合标识符命名规范。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l8262029979e44034b6b713e53352ae80"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a1fd4a142a8c24e62aa4ad576ef5baccc">tablespace_name</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a089a6358efa049a490fcd8ef390a5321">表空间名称。</p>
</li><li id="ZH-CN_TOPIC_0289900312__li11690184519510">client_master_key<p id="ZH-CN_TOPIC_0289900312__p81149201618"><a name="ZH-CN_TOPIC_0289900312__li11690184519510"></a><a name="li11690184519510"></a>客户端加密主密钥的名称。</p>
<p id="ZH-CN_TOPIC_0289900312__p587920495919">取值范围：字符串，要符合标识符命名规范。</p>
</li><li id="ZH-CN_TOPIC_0289900312__li146118236617">column_encryption_key<p id="ZH-CN_TOPIC_0289900312__p175751630094"><a name="ZH-CN_TOPIC_0289900312__li146118236617"></a><a name="li146118236617"></a>列加密密钥的名称。</p>
<p id="ZH-CN_TOPIC_0289900312__p6223125213910">取值范围：字符串，要符合标识符命名规范。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_li1435116194510"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_b63513624512">directory_name</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_p1135863459">目录名称。</p>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_p19354614453">取值范围：字符串，要符合标识符命名规范。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_le199e18b44234da4b328d60dd494032f"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a1c378504691149dda22e013975c25a28">WITH GRANT OPTION</strong><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_aade4e36f143e46649e7457ec5ce8763f">如果声明了WITH GRANT OPTION，则被授权的用户也可以将此权限赋予他人，否则就不能授权给他人。这个选项不能赋予PUBLIC。</p>
</li></ul>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a9cfc5bdd285e40caac508d42ba229b6a">非对象所有者给其他用户授予对象权限时，命令按照以下规则执行：</p>
<ul id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ue46bba1826ab4dec84a62a4ab4f47fb6"><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l88b5a7ab383e464987b034b01f051775">如果用户没有该对象上指定的权限，命令立即失败。</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_lbb162116f00244fc9b8a8fbc696549ca">如果用户有该对象上的部分权限，则GRANT命令只授予他有授权选项的权限。</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_la980fc60689444cf9fba1ed670b7313a">如果用户没有可用的授权选项，GRANT ALL PRIVILEGES形式将发出一个警告信息，其他命令形式将发出在命令中提到的且没有授权选项的相关警告信息。</li></ul>
<div class="note" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ndd2f2dbce0b64340b5a2d511d1c42ba5"><span class="notetitle"> </span><div class="notebody"><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a6bc68fe9bc544449937b202fe14790b8">数据库系统管理员可以访问所有对象，而不会受对象的权限设置影响。这个特点类似Unix系统的root的权限。和root一样，除了必要的情况外，建议不要总是以系统管理员身份进行操作。</p>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_a2298f929f38d4f46894084f510171f7a">不允许对表分区进行GRANT操作，对分区表进行GRANT操作会引起告警。</p>
</div></div>
</div>
<ul id="ZH-CN_TOPIC_0289900312__ul3154650102719"><li id="ZH-CN_TOPIC_0289900312__li1115495002712">WITH ADMIN OPTION<p id="ZH-CN_TOPIC_0289900312__p18774194152814"><a name="ZH-CN_TOPIC_0289900312__li1115495002712"></a><a name="li1115495002712"></a>对于角色，当声明了WITH ADMIN OPTION，被授权的用户可以将该角色再授予其他角色/用户，或从其他角色/用户回收该角色。</p>
<p id="ZH-CN_TOPIC_0289900312__p146061220193216">对于ANY权限，当声明了WITH ADMIN OPTION，被授权的用户可以将该ANY权限再授予其他角色/用户，或从其他角色/用户回收该ANY权限。</p>
</li></ul>

<div class="tablenoborder"><a name="ZH-CN_TOPIC_0289900312__table1360121832117"></a><a name="table1360121832117"></a><table cellpadding="4" cellspacing="0" summary="" id="ZH-CN_TOPIC_0289900312__table1360121832117" frame="border" border="1" rules="all"><caption><b>表1 </b>ANY权限列表</caption><thead align="left"><tr id="ZH-CN_TOPIC_0289900312__row116015189214"><th align="left" class="cellrowborder" valign="top" width="22.509999999999998%" id="mcps1.3.7.2.3.1.1"><p id="ZH-CN_TOPIC_0289900312__p6601181862115">系统权限名称</p>
</th>
<th align="left" class="cellrowborder" valign="top" width="77.49000000000001%" id="mcps1.3.7.2.3.1.2"><p id="ZH-CN_TOPIC_0289900312__p26011318192119">描述</p>
</th>
</tr>
</thead>
<tbody><tr id="ZH-CN_TOPIC_0289900312__row5601171810211"><td class="cellrowborder" valign="top" width="22.509999999999998%" headers="mcps1.3.7.2.3.1.1 "><p id="ZH-CN_TOPIC_0289900312__p196011187211">CREATE ANY TABLE</p>
</td>
<td class="cellrowborder" valign="top" width="77.49000000000001%" headers="mcps1.3.7.2.3.1.2 "><p id="ZH-CN_TOPIC_0289900312__p96013188214">用户能够在public模式和用户模式下创建表或视图。如果想要创建serial类型列的表，还需要授予创建序列的权限。</p>
</td>
</tr>
<tr id="ZH-CN_TOPIC_0289900312__row8601131892110"><td class="cellrowborder" valign="top" width="22.509999999999998%" headers="mcps1.3.7.2.3.1.1 "><p id="ZH-CN_TOPIC_0289900312__p9601201818210">ALTER ANY TABLE</p>
</td>
<td class="cellrowborder" valign="top" width="77.49000000000001%" headers="mcps1.3.7.2.3.1.2 "><p id="ZH-CN_TOPIC_0289900312__p4601518112115">用户拥有对public模式和用户模式下表或视图的ALTER权限。如果想要修改表的唯一索引为表增加主键约束或唯一约束，还需要授予该表的索引权限。</p>
</td>
</tr>
<tr id="ZH-CN_TOPIC_0289900312__row960101852112"><td class="cellrowborder" valign="top" width="22.509999999999998%" headers="mcps1.3.7.2.3.1.1 "><p id="ZH-CN_TOPIC_0289900312__p11601111814216">DROP ANY TABLE</p>
</td>
<td class="cellrowborder" valign="top" width="77.49000000000001%" headers="mcps1.3.7.2.3.1.2 "><p id="ZH-CN_TOPIC_0289900312__p117212437315">用户拥有对public模式和用户模式下表或视图的DROP权限。</p>
</td>
</tr>
<tr id="ZH-CN_TOPIC_0289900312__row2601171822114"><td class="cellrowborder" valign="top" width="22.509999999999998%" headers="mcps1.3.7.2.3.1.1 "><p id="ZH-CN_TOPIC_0289900312__p11601121822110">SELECT ANY TABLE</p>
</td>
<td class="cellrowborder" valign="top" width="77.49000000000001%" headers="mcps1.3.7.2.3.1.2 "><p id="ZH-CN_TOPIC_0289900312__p8713439315">用户拥有对public模式和用户模式下表或视图的SELETCT权限，仍然受行级访问控制限制。</p>
</td>
</tr>
<tr id="ZH-CN_TOPIC_0289900312__row1960171812214"><td class="cellrowborder" valign="top" width="22.509999999999998%" headers="mcps1.3.7.2.3.1.1 "><p id="ZH-CN_TOPIC_0289900312__p2601218192119">UPDATE ANY TABLE</p>
</td>
<td class="cellrowborder" valign="top" width="77.49000000000001%" headers="mcps1.3.7.2.3.1.2 "><p id="ZH-CN_TOPIC_0289900312__p1770144313316">用户拥有对public模式和用户模式下表或视图的UPDATE权限，仍然受行级访问控制限制。</p>
</td>
</tr>
<tr id="ZH-CN_TOPIC_0289900312__row1960141815214"><td class="cellrowborder" valign="top" width="22.509999999999998%" headers="mcps1.3.7.2.3.1.1 "><p id="ZH-CN_TOPIC_0289900312__p56010186214">INSERT ANY TABLE</p>
</td>
<td class="cellrowborder" valign="top" width="77.49000000000001%" headers="mcps1.3.7.2.3.1.2 "><p id="ZH-CN_TOPIC_0289900312__p186911435319">用户拥有对public模式和用户模式下表或视图的INSERT权限。</p>
</td>
</tr>
<tr id="ZH-CN_TOPIC_0289900312__row186016187218"><td class="cellrowborder" valign="top" width="22.509999999999998%" headers="mcps1.3.7.2.3.1.1 "><p id="ZH-CN_TOPIC_0289900312__p2060191816213">DELETE ANY TABLE</p>
</td>
<td class="cellrowborder" valign="top" width="77.49000000000001%" headers="mcps1.3.7.2.3.1.2 "><p id="ZH-CN_TOPIC_0289900312__p76816437315">用户拥有对public模式和用户模式下表或视图的DELETE权限，仍然受行级访问控制限制。</p>
</td>
</tr>
<tr id="ZH-CN_TOPIC_0289900312__row7827488255"><td class="cellrowborder" valign="top" width="22.509999999999998%" headers="mcps1.3.7.2.3.1.1 "><p id="ZH-CN_TOPIC_0289900312__p1382104882516">CREATE ANY FUNCTION</p>
</td>
<td class="cellrowborder" valign="top" width="77.49000000000001%" headers="mcps1.3.7.2.3.1.2 "><p id="ZH-CN_TOPIC_0289900312__p2799144511319">用户能够在用户模式下创建函数或存储过程。</p>
</td>
</tr>
<tr id="ZH-CN_TOPIC_0289900312__row7827488255"><td class="cellrowborder" valign="top" width="22.509999999999998%" headers="mcps1.3.7.2.3.1.1 "><p id="ZH-CN_TOPIC_0289900312__p1382104882516">CREATE ROUTINE</p>
</td>
<td class="cellrowborder" valign="top" width="77.49000000000001%" headers="mcps1.3.7.2.3.1.2 "><p id="ZH-CN_TOPIC_0289900312__p2799144511319">用户能够在用户模式下创建函数或存储过程。</p>
</td>
</tr>
<tr id="ZH-CN_TOPIC_0289900312__row1466925310257"><td class="cellrowborder" valign="top" width="22.509999999999998%" headers="mcps1.3.7.2.3.1.1 "><p id="ZH-CN_TOPIC_0289900312__p8669135372515">EXECUTE ANY FUNCTION</p>
</td>
<td class="cellrowborder" valign="top" width="77.49000000000001%" headers="mcps1.3.7.2.3.1.2 "><p id="ZH-CN_TOPIC_0289900312__p9669135311252">用户拥有在public模式和用户模式下函数或存储过程的EXECUTE权限。</p>
</td>
</tr>
<tr id="ZH-CN_TOPIC_0289900312__row9568146102610"><td class="cellrowborder" valign="top" width="22.509999999999998%" headers="mcps1.3.7.2.3.1.1 "><p id="ZH-CN_TOPIC_0289900312__p14568184619264">CREATE ANY PACKAGE</p>
</td>
<td class="cellrowborder" valign="top" width="77.49000000000001%" headers="mcps1.3.7.2.3.1.2 "><p id="ZH-CN_TOPIC_0289900312__p14990113362">用户能够在public模式和用户模式下创建PACKAGE。</p>
</td>
</tr>
<tr id="ZH-CN_TOPIC_0289900312__row47031450142617"><td class="cellrowborder" valign="top" width="22.509999999999998%" headers="mcps1.3.7.2.3.1.1 "><p id="ZH-CN_TOPIC_0289900312__p27031350102618">EXECUTE ANY PACKAGE</p>
</td>
<td class="cellrowborder" valign="top" width="77.49000000000001%" headers="mcps1.3.7.2.3.1.2 "><p id="ZH-CN_TOPIC_0289900312__p37961145938">用户拥有在public模式和用户模式下PACKAGE的EXECUTE权限。</p>
</td>
</tr>
<tr id="ZH-CN_TOPIC_0289900312__row1654415246293"><td class="cellrowborder" valign="top" width="22.509999999999998%" headers="mcps1.3.7.2.3.1.1 "><p id="ZH-CN_TOPIC_0289900312__p254416244293">CREATE ANY TYPE</p>
</td>
<td class="cellrowborder" valign="top" width="77.49000000000001%" headers="mcps1.3.7.2.3.1.2 "><p id="ZH-CN_TOPIC_0289900312__p175443247299">用户能够在public模式和用户模式下创建类型。</p>
</td>
</tr>
<tr id="ZH-CN_TOPIC_0289900312__row1565211281297"><td class="cellrowborder" valign="top" width="22.509999999999998%" headers="mcps1.3.7.2.3.1.1 "><p id="ZH-CN_TOPIC_0289900312__p1765242862910">CREATE ANY SEQUENCE</p>
</td>
<td class="cellrowborder" valign="top" width="77.49000000000001%" headers="mcps1.3.7.2.3.1.2 "><p id="ZH-CN_TOPIC_0289900312__p2079315451731">用户能够在public模式和用户模式下创建序列。</p>
</td>
</tr>
<tr id="ZH-CN_TOPIC_0289900312__row87515532292"><td class="cellrowborder" valign="top" width="22.509999999999998%" headers="mcps1.3.7.2.3.1.1 "><p id="ZH-CN_TOPIC_0289900312__p14752155317297">CREATE ANY INDEX</p>
</td>
<td class="cellrowborder" valign="top" width="77.49000000000001%" headers="mcps1.3.7.2.3.1.2 "><p id="ZH-CN_TOPIC_0289900312__p37921145332">用户能够在public模式和用户模式下创建索引。如果在某表空间创建分区表索引，需要授予用户该表空间的创建权限。</p>
</td>
</tr>
<tr id="ZH-CN_TOPIC_0289900312__row87515532292"><td class="cellrowborder" valign="top" width="22.509999999999998%" headers="mcps1.3.7.2.3.1.1 "><p id="ZH-CN_TOPIC_0289900312__p14752155317297">INDEX</p>
</td>
<td class="cellrowborder" valign="top" width="77.49000000000001%" headers="mcps1.3.7.2.3.1.2 "><p id="ZH-CN_TOPIC_0289900312__p37921145332">用户能够在public模式和用户模式下创建索引。如果在某表空间创建分区表索引，需要授予用户该表空间的创建权限。</p>
</td>
</tr>
</tbody>
</table>
</div>
<div class="note" id="ZH-CN_TOPIC_0289900312__note213711711217"><span class="notetitle"> </span><div class="notebody"><p id="ZH-CN_TOPIC_0289900312__p813717720127">用户被授予任何一种ANY权限后，用户对public模式和用户模式具有USAGE权限，对<a href="zh-cn_topic_0000001190922647.html#ZH-CN_TOPIC_0000001190922647__table167371825175015">表1</a>中除public之外的系统模式没有USAGE权限。</p>
</div></div>
<div class="section" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s724dfb1c8978412b95cb308b64dfa447"><a name="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s724dfb1c8978412b95cb308b64dfa447"></a><a name="zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s724dfb1c8978412b95cb308b64dfa447"></a><h4 class="sectiontitle">示例</h4><p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_p13712915141740"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_b61396388141720">示例：将系统权限授权给用户或者角色。</strong></p>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_p17824766141134">创建名为joe的用户，并将sysadmin权限授权给他。</p>
<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_screen52712157141640"><span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text2083675121020">openGauss=# </span>CREATE USER joe PASSWORD '<span id="ZH-CN_TOPIC_0289900312__text10266195685919">xxxxxxxxx</span>';
<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text18021046111118">openGauss=# </span>GRANT ALL PRIVILEGES TO joe;</pre>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_p44654699141134">授权成功后，用户joe会拥有sysadmin的所有权限。</p>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_p62094381141742"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_b50075555141730">示例：将对象权限授权给用户或者角色</strong>。</p>
<ol id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ol8170248141748"><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_li10937659141748">撤销joe用户的sysadmin权限，然后将模式tpcds的使用权限和表<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text63901427102617">tpcds.</span>reason的所有权限授权给用户joe。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_screen57499683142041"><span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text15408753181013">openGauss=# </span>REVOKE ALL PRIVILEGES FROM joe;
<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text77451838171117">openGauss=# </span>GRANT USAGE ON SCHEMA tpcds TO joe;
<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text1976725419106">openGauss=# </span>GRANT ALL PRIVILEGES ON <span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text62472952614">tpcds.</span>reason TO joe;</pre>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_p6750360142019">授权成功后，joe用户就拥有了<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text69578315264">tpcds.</span>reason表的所有权限，包括增删改查等权限。</p>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_li25767916141924">将<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text733733311263">tpcds.</span>reason表中r_reason_sk、r_reason_id、r_reason_desc列的查询权限，r_reason_desc的更新权限授权给joe。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_screen15372644102339"><span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text2520455151010">openGauss=# </span>GRANT select (r_reason_sk,r_reason_id,r_reason_desc),update (r_reason_desc) ON <span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text153471034202611">tpcds.</span>reason TO joe;</pre>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_p57913020102330">授权成功后，用户joe对<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text32574359267">tpcds.</span>reason表中r_reason_sk，r_reason_id的查询权限会立即生效。如果joe用户需要拥有将这些权限授权给其他用户的权限，可以通过以下语法对joe用户进行授权。</p>
<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_screen42992689102346"><span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text3769145619102">openGauss=# </span>GRANT select (r_reason_sk, r_reason_id) ON <span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text5483336192619">tpcds.</span>reason TO joe WITH GRANT OPTION;</pre>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_p55396272102330">将数据库<span id="ZH-CN_TOPIC_0289900312__text114413213911">openGauss</span>的连接权限授权给用户joe，并给予其在<span id="ZH-CN_TOPIC_0289900312__text8355116153914">openGauss</span>中创建schema的权限，而且允许joe将此权限授权给其他用户。</p>
<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_screen27339006102350"><span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text28157575104">openGauss=# </span>GRANT create,connect on database <span id="ZH-CN_TOPIC_0289900312__text8125135663817">openGauss</span> TO joe WITH GRANT OPTION;</pre>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_p13111472102330">创建角色tpcds_manager，将模式tpcds的访问权限授权给角色tpcds_manager，并授予该角色在tpcds下创建对象的权限，不允许该角色中的用户将权限授权给其他人。</p>
<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_screen3500373910242"><span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text16816205815108">openGauss=# </span>CREATE ROLE tpcds_manager PASSWORD '<span id="ZH-CN_TOPIC_0289900312__text1332835845912">xxxxxxxxx</span>';
<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text1393615051110">openGauss=# </span>GRANT USAGE,CREATE ON SCHEMA tpcds TO tpcds_manager;</pre>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_p36752821102330">将表空间tpcds_tbspc的所有权限授权给用户joe，但用户joe无法将权限继续授予其他用户。</p>
<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_screen1778532102410"><span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text1384017291116">openGauss=# </span>CREATE TABLESPACE tpcds_tbspc RELATIVE LOCATION 'tablespace/tablespace_1';
<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text22793531110">openGauss=# </span>GRANT ALL ON TABLESPACE tpcds_tbspc TO joe;</pre>
</li></ol>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_p11165308141134"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_b3523050514213">示例：将用户或者角色的权限授权给其他用户或角色。</strong></p>
<ol id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_ol47048151142150"><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_li14521142150">创建角色manager，将joe的权限授权给manager，并允许该角色将权限授权给其他人。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_screen4529445114240"><span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text155016610111">openGauss=# </span>CREATE ROLE manager PASSWORD '<span id="ZH-CN_TOPIC_0289900312__text1137620013016">xxxxxxxxx</span>';
<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text14403197121115">openGauss=# </span>GRANT joe TO manager WITH ADMIN OPTION;</pre>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_li355872142251">创建用户senior_manager，将用户manager的权限授权给该用户。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_screen863909314246"><span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text762018121115">openGauss=# </span>CREATE ROLE senior_manager PASSWORD '<span id="ZH-CN_TOPIC_0289900312__text2554521805">xxxxxxxxx</span>';
<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text969219915119">openGauss=# </span>GRANT manager TO senior_manager;</pre>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_li20309668142325">撤销权限，并清理用户。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_screen26059070142414"><span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text2953181081115">openGauss=# </span>REVOKE manager FROM joe;
<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text1376551241120">openGauss=# </span>REVOKE senior_manager FROM manager;
<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text31721414101114">openGauss=# </span>DROP USER manager;</pre>
</li></ol>
<p id="ZH-CN_TOPIC_0289900312__p14449928182418"><strong id="ZH-CN_TOPIC_0289900312__b19449152842419">示例：将CMK或者CEK的权限授权给其他用户或角色。</strong></p>
<ol id="ZH-CN_TOPIC_0289900312__ol194491928152413"><li id="ZH-CN_TOPIC_0289900312__li3887111112428">连接密态数据库<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900312__screen021415424423"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="n">gsql</span> <span class="o">-</span><span class="n">p</span> <span class="mi">57101</span> <span class="n">openGauss</span> <span class="o">-</span><span class="n">r</span> <span class="o">-</span><span class="k">C</span>
<span class="n">openGauss</span><span class="o">=#</span>  <span class="k">CREATE</span> <span class="n">CLIENT</span> <span class="n">MASTER</span> <span class="k">KEY</span> <span class="n">MyCMK1</span> <span class="k">WITH</span> <span class="p">(</span> <span class="n">KEY_STORE</span> <span class="o">=</span> <span class="n">localkms</span> <span class="p">,</span> <span class="n">KEY_PATH</span> <span class="o">=</span> <span class="ss">&quot;key_path_value&quot;</span> <span class="p">,</span> <span class="n">ALGORITHM</span> <span class="o">=</span> <span class="n">RSA_2048</span><span class="p">);</span>
<span class="k">CREATE</span> <span class="n">CLIENT</span> <span class="n">MASTER</span> <span class="k">KEY</span>
<span class="n">openGauss</span><span class="o">=#</span> <span class="k">CREATE</span> <span class="k">COLUMN</span> <span class="n">ENCRYPTION</span> <span class="k">KEY</span> <span class="n">MyCEK1</span> <span class="k">WITH</span> <span class="k">VALUES</span> <span class="p">(</span><span class="n">CLIENT_MASTER_KEY</span> <span class="o">=</span> <span class="n">MyCMK1</span><span class="p">,</span> <span class="n">ALGORITHM</span> <span class="o">=</span> <span class="n">AEAD_AES_256_CBC_HMAC_SHA256</span><span class="p">);</span>
<span class="k">CREATE</span> <span class="k">COLUMN</span> <span class="n">ENCRYPTION</span> <span class="k">KEY</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900312__li5449728182414">创建角色newuser，将密钥的权限授权给newuser。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900312__screen144491028102419"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5
6
7
8</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="n">openGauss</span><span class="o">=#</span> <span class="k">CREATE</span> <span class="k">USER</span> <span class="n">newuser</span> <span class="n">PASSWORD</span> <span class="s1">'xxxxxxxxx'</span><span class="p">;</span>
<span class="k">CREATE</span> <span class="k">ROLE</span>
<span class="n">openGauss</span><span class="o">=#</span> <span class="k">GRANT</span> <span class="k">ALL</span> <span class="k">ON</span> <span class="k">SCHEMA</span> <span class="k">public</span> <span class="k">TO</span> <span class="n">newuser</span><span class="p">;</span>
<span class="k">GRANT</span>
<span class="n">openGauss</span><span class="o">=#</span> <span class="k">GRANT</span> <span class="k">USAGE</span> <span class="k">ON</span> <span class="n">COLUMN_ENCRYPTION_KEY</span> <span class="n">MyCEK1</span> <span class="k">to</span> <span class="n">newuser</span><span class="p">;</span>
<span class="k">GRANT</span>
<span class="n">openGauss</span><span class="o">=#</span> <span class="k">GRANT</span> <span class="k">USAGE</span> <span class="k">ON</span> <span class="n">CLIENT_MASTER_KEY</span> <span class="n">MyCMK1</span> <span class="k">to</span> <span class="n">newuser</span><span class="p">;</span>
<span class="k">GRANT</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900312__li13449122819244">设置该用户连接数据库,使用该CEK创建加密表。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900312__screen8449142810241"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5
6
7
8</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="n">openGauss</span><span class="o">=#</span> <span class="k">SET</span> <span class="k">SESSION</span> <span class="k">AUTHORIZATION</span> <span class="n">newuser</span> <span class="n">PASSWORD</span> <span class="s1">'xxxxxxxxx'</span><span class="p">;</span>
<span class="n">openGauss</span><span class="o">=&gt;</span>  <span class="k">CREATE</span> <span class="k">TABLE</span> <span class="n">acltest1</span> <span class="p">(</span><span class="n">x</span> <span class="nb">int</span><span class="p">,</span> <span class="n">x2</span> <span class="nb">varchar</span><span class="p">(</span><span class="mi">50</span><span class="p">)</span> <span class="k">ENCRYPTED</span> <span class="k">WITH</span> <span class="p">(</span><span class="n">COLUMN_ENCRYPTION_KEY</span> <span class="o">=</span> <span class="n">MyCEK1</span><span class="p">,</span> <span class="n">ENCRYPTION_TYPE</span> <span class="o">=</span> <span class="k">DETERMINISTIC</span><span class="p">));</span>
<span class="k">CREATE</span> <span class="k">TABLE</span>
<span class="n">openGauss</span><span class="o">=&gt;</span> <span class="k">SELECT</span> <span class="n">has_cek_privilege</span><span class="p">(</span><span class="s1">'newuser'</span><span class="p">,</span> <span class="s1">'MyCEK1'</span><span class="p">,</span> <span class="s1">'USAGE'</span><span class="p">);</span>
 <span class="n">has_cek_privilege</span>
<span class="c1">-------------------</span>
 <span class="n">t</span>
<span class="p">(</span><span class="mi">1</span> <span class="k">row</span><span class="p">)</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900312__li1644932802417">撤销权限，并清理用户。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900312__screen184494284245"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5
6
7
8</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="n">openGauss</span><span class="o">=#</span> <span class="k">REVOKE</span> <span class="k">USAGE</span> <span class="k">ON</span> <span class="n">COLUMN_ENCRYPTION_KEY</span> <span class="n">MyCEK1</span> <span class="k">FROM</span> <span class="n">newuser</span><span class="p">;</span>
<span class="n">openGauss</span><span class="o">=#</span> <span class="k">REVOKE</span> <span class="k">USAGE</span> <span class="k">ON</span> <span class="n">CLIENT_MASTER_KEY</span> <span class="n">MyCMK1</span> <span class="k">FROM</span> <span class="n">newuser</span><span class="p">;</span>
<span class="n">openGauss</span><span class="o">=#</span> <span class="k">DROP</span> <span class="k">TABLE</span> <span class="n">newuser</span><span class="p">.</span><span class="n">acltest1</span><span class="p">;</span>
<span class="n">openGauss</span><span class="o">=#</span> <span class="k">DROP</span> <span class="k">COLUMN</span> <span class="n">ENCRYPTION</span> <span class="k">KEY</span> <span class="n">MyCEK1</span><span class="p">;</span>
<span class="n">openGauss</span><span class="o">=#</span> <span class="k">DROP</span> <span class="n">CLIENT</span> <span class="n">MASTER</span> <span class="k">KEY</span> <span class="n">MyCMK1</span><span class="p">;</span>
<span class="n">openGauss</span><span class="o">=#</span> <span class="k">DROP</span> <span class="k">SCHEMA</span> <span class="k">IF</span> <span class="k">EXISTS</span> <span class="n">newuser</span> <span class="k">CASCADE</span><span class="p">;</span>
<span class="n">openGauss</span><span class="o">=#</span> <span class="k">REVOKE</span> <span class="k">ALL</span> <span class="k">ON</span> <span class="k">SCHEMA</span> <span class="k">public</span> <span class="k">FROM</span> <span class="n">newuser</span><span class="p">;</span>
<span class="n">openGauss</span><span class="o">=#</span> <span class="k">DROP</span> <span class="k">ROLE</span> <span class="k">IF</span> <span class="k">EXISTS</span> <span class="n">newuser</span><span class="p">;</span>
</pre></div>
</td></tr></table></div>
</li></ol>
<p id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_p22667284141134"><strong id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_b40601625142119">示例：撤销上述授予的权限，并清理角色和用户。</strong></p>
<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_screen49586471142135"><span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text02641211114614">openGauss=# </span>REVOKE ALL PRIVILEGES ON <span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text7398123752618">tpcds.</span>reason FROM joe;
<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text6821191315464">openGauss=# </span>REVOKE ALL PRIVILEGES ON SCHEMA tpcds FROM joe;
<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text19126616174614">openGauss=# </span>REVOKE ALL ON TABLESPACE tpcds_tbspc FROM joe;
<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text197128188469">openGauss=# </span>DROP TABLESPACE tpcds_tbspc;
<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text0111111991114">openGauss=# </span>REVOKE USAGE,CREATE ON SCHEMA tpcds FROM tpcds_manager;
<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text984731951114">openGauss=# </span>DROP ROLE tpcds_manager;
<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text1661715208111">openGauss=# </span>DROP ROLE senior_manager;
<span id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_text173695216111">openGauss=# </span>DROP USER joe CASCADE;</pre>
</div>
</div>
</body>
</html>