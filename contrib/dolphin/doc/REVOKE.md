
<!DOCTYPE html
  PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html lang="zh-cn" xml:lang="zh-cn">
<head>
      <meta http-equiv="Content-Type" content="text/html; charset=gb2312">
   
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="DC.Type" content="topic">
<meta name="DC.Title" content="REVOKE">
<meta name="product" content="">
<meta name="DC.Relation" scheme="URI" content="zh-cn_topic_0289900416.html">
<meta name="prodname" content="">
<meta name="version" content="3.0.0">
<meta name="brand" content="openGauss">
<meta name="DC.Publisher" content="20220331">
<meta name="DC.Format" content="XHTML">
<meta name="DC.Identifier" content="ZH-CN_TOPIC_0289900263">
<meta name="DC.Language" content="zh-cn">
<link rel="stylesheet" type="text/css" href="public_sys-resources/commonltr.css">
<link rel="stylesheet" type="text/css" href="public_sys-resources/pygments.css">
<title>REVOKE</title>
</head>
<body style="clear:both; padding-left:10px; padding-top:5px; padding-right:5px; padding-bottom:5px"><a name="ZH-CN_TOPIC_0289900263"></a><a name="ZH-CN_TOPIC_0289900263"></a>

<h1 class="topictitle1">REVOKE</h1>
<div id="body2586631"><div class="section" id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_sda1d739a0a8c460c93bc099fb8208944"><h4 class="sectiontitle">功能描述</h4><p id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_ad15e5a0d6fcc4ad687216718a1d08e00">REVOKE用于撤销一个或多个角色的权限。</p>
</div>
<div class="section" id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_sf1580b93b5664a7db2c08cf69806faa5"><h4 class="sectiontitle">注意事项</h4><p id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_a70fa2247e94a4d69b3dd0e5ab890416c">非对象所有者试图在对象上REVOKE权限，命令按照以下规则执行：</p>
<ul id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_u3b457081b16742e68081c066fc2054b0"><li id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_l2cdf8891fa45468e80f96229fa174ea6">如果授权用户没有该对象上的权限，则命令立即失败。</li><li id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_l335c48c5ad744728b781c51ccfad2a9b">如果授权用户有部分权限，则只撤销那些有授权选项的权限。</li><li id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_l2a3506a3296347c799105978c90e655b">如果授权用户没有授权选项，REVOKE ALL PRIVILEGES形式将发出一个错误信息，而对于其他形式的命令而言，如果是命令中指定名称的权限没有相应的授权选项，该命令将发出一个警告。</li><li id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_l420a46ddf0aa4f13bf955e20c7726f3d">不允许对表分区进行REVOKE操作，对分区表进行REVOKE操作会引起告警。</li></ul>
</div>
<div class="section" id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_s5eb0513470714ccbbd425944c1d73c8e"><h4 class="sectiontitle">语法格式</h4><ul id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_ud03ae9484bd7492d9b3ce4bcf0fcd92d"><li id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_lfe3d43e1c6ed447599b427c3e8e118ec">回收指定表或视图上权限。<pre class="screen" id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_s1cbae7208ff641c4b35830fa7abff0cf">REVOKE [ GRANT OPTION FOR ]
    { { SELECT | INSERT | UPDATE | DELETE | TRUNCATE | REFERENCES | ALTER | DROP | COMMENT | INDEX | VACUUM }[, ...] 
    | ALL [ PRIVILEGES ] }
    ON { [ TABLE ] table_name [, ...]
       | ALL TABLES IN SCHEMA schema_name [, ...] }
    FROM { [ GROUP ] role_name | PUBLIC } [, ...]
    [ CASCADE | RESTRICT ];</pre>
</li><li id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_l2b31beb884684822a2472c6f900d8a7b">回收表上指定字段权限。<pre class="screen" id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_sdde234eebf3b4143bc1fd8ebd8f6f7f1">REVOKE [ GRANT OPTION FOR ]
    { {{ SELECT | INSERT | UPDATE | REFERENCES | COMMENT } ( column_name [, ...] )}[, ...] 
    | ALL [ PRIVILEGES ] ( column_name [, ...] ) }
    ON [ TABLE ] table_name [, ...]
    FROM { [ GROUP ] role_name | PUBLIC } [, ...]
    [ CASCADE | RESTRICT ];</pre>
</li><li id="ZH-CN_TOPIC_0289900263__li18645931124417">回收指定序列上权限，LARGE字段属性可选，回收语句不区分序列是否为LARGE。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900263__screen176453315446"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5
6
7</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">REVOKE</span> <span class="p">[</span> <span class="k">GRANT</span> <span class="k">OPTION</span> <span class="k">FOR</span> <span class="p">]</span>
    <span class="err">{</span> <span class="err">{</span> <span class="k">SELECT</span> <span class="o">|</span> <span class="k">UPDATE</span> <span class="o">|</span> <span class="k">ALTER</span> <span class="o">|</span> <span class="k">DROP</span> <span class="o">|</span> <span class="k">COMMENT</span> <span class="err">}</span><span class="p">[,</span> <span class="p">...]</span> 
    <span class="o">|</span> <span class="k">ALL</span> <span class="p">[</span> <span class="k">PRIVILEGES</span> <span class="p">]</span> <span class="err">}</span>
    <span class="k">ON</span> <span class="err">{</span> <span class="p">[</span> <span class="p">[</span> <span class="k">LARGE</span> <span class="p">]</span> <span class="n">SEQUENCE</span> <span class="p">]</span> <span class="n">sequence_name</span> <span class="p">[,</span> <span class="p">...]</span>
       <span class="o">|</span> <span class="k">ALL</span> <span class="n">SEQUENCES</span> <span class="k">IN</span> <span class="k">SCHEMA</span> <span class="k">schema_name</span> <span class="p">[,</span> <span class="p">...]</span> <span class="err">}</span>
    <span class="k">FROM</span> <span class="err">{</span> <span class="p">[</span> <span class="k">GROUP</span> <span class="p">]</span> <span class="n">role_name</span> <span class="o">|</span> <span class="k">PUBLIC</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="p">[</span> <span class="k">CASCADE</span> <span class="o">|</span> <span class="k">RESTRICT</span> <span class="p">];</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_lbee825b5e4d545a490823d3571b6834a">回收指定数据库上权限。<pre class="screen" id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_s2fd98399195347ae8cc74a8485ed97cb">REVOKE [ GRANT OPTION FOR ]
    { { CREATE | CONNECT | TEMPORARY | TEMP | CREATE TEMPORARY TABLES | ALTER | DROP | COMMENT } [, ...] 
    | ALL [ PRIVILEGES ] }
    ON { DATABASE database_name [, ...] | database_name.* }
    FROM { [ GROUP ] role_name | PUBLIC } [, ...]
    [ CASCADE | RESTRICT ];</pre>
</li><li id="ZH-CN_TOPIC_0289900263__li05441224152516">回收指定域上权限。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900263__screen1415684772520"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">REVOKE</span> <span class="p">[</span> <span class="k">GRANT</span> <span class="k">OPTION</span> <span class="k">FOR</span> <span class="p">]</span>
    <span class="err">{</span> <span class="k">USAGE</span> <span class="o">|</span> <span class="k">ALL</span> <span class="p">[</span> <span class="k">PRIVILEGES</span> <span class="p">]</span> <span class="err">}</span>
    <span class="k">ON</span> <span class="k">DOMAIN</span> <span class="n">domain_name</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="k">FROM</span> <span class="err">{</span> <span class="p">[</span> <span class="k">GROUP</span> <span class="p">]</span> <span class="n">role_name</span> <span class="o">|</span> <span class="k">PUBLIC</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="p">[</span> <span class="k">CASCADE</span> <span class="o">|</span> <span class="k">RESTRICT</span> <span class="p">];</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900263__li1392616243276">回收指定客户端加密主密钥上的权限。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900263__screen179261224202720"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">REVOKE</span> <span class="p">[</span> <span class="k">GRANT</span> <span class="k">OPTION</span> <span class="k">FOR</span> <span class="p">]</span>
    <span class="err">{</span> <span class="err">{</span> <span class="k">USAGE</span> <span class="o">|</span> <span class="k">DROP</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span> <span class="o">|</span> <span class="k">ALL</span> <span class="p">[</span><span class="k">PRIVILEGES</span><span class="p">]</span> <span class="err">}</span>
    <span class="k">ON</span> <span class="n">CLIENT_MASTER_KEYS</span> <span class="n">client_master_keys_name</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="k">FROM</span> <span class="err">{</span> <span class="p">[</span> <span class="k">GROUP</span> <span class="p">]</span> <span class="n">role_name</span> <span class="o">|</span> <span class="k">PUBLIC</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="p">[</span> <span class="k">CASCADE</span> <span class="o">|</span> <span class="k">RESTRICT</span> <span class="p">];</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900263__li1926224182714">回收指定列加密密钥上的权限。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900263__screen1192612419272"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">REVOKE</span> <span class="p">[</span> <span class="k">GRANT</span> <span class="k">OPTION</span> <span class="k">FOR</span> <span class="p">]</span>
    <span class="err">{</span> <span class="err">{</span> <span class="k">USAGE</span> <span class="o">|</span> <span class="k">DROP</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span> <span class="o">|</span> <span class="k">ALL</span> <span class="p">[</span><span class="k">PRIVILEGES</span><span class="p">]</span><span class="err">}</span>
    <span class="k">ON</span> <span class="n">COLUMN_ENCRYPTION_KEYS</span> <span class="n">column_encryption_keys_name</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="k">FROM</span> <span class="err">{</span> <span class="p">[</span> <span class="k">GROUP</span> <span class="p">]</span> <span class="n">role_name</span> <span class="o">|</span> <span class="k">PUBLIC</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="p">[</span> <span class="k">CASCADE</span> <span class="o">|</span> <span class="k">RESTRICT</span> <span class="p">];</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900263__li6351112318">回收指定目录上权限。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900263__screen622117299318"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">REVOKE</span> <span class="p">[</span> <span class="k">GRANT</span> <span class="k">OPTION</span> <span class="k">FOR</span> <span class="p">]</span>
    <span class="err">{</span> <span class="err">{</span> <span class="k">READ</span> <span class="o">|</span> <span class="k">WRITE</span> <span class="o">|</span> <span class="k">ALTER</span> <span class="o">|</span> <span class="k">DROP</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span> <span class="o">|</span> <span class="k">ALL</span> <span class="p">[</span> <span class="k">PRIVILEGES</span> <span class="p">]</span> <span class="err">}</span>
    <span class="k">ON</span> <span class="n">DIRECTORY</span> <span class="n">directory_name</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="k">FROM</span> <span class="err">{</span> <span class="p">[</span> <span class="k">GROUP</span> <span class="p">]</span> <span class="n">role_name</span> <span class="o">|</span> <span class="k">PUBLIC</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="p">[</span> <span class="k">CASCADE</span> <span class="o">|</span> <span class="k">RESTRICT</span> <span class="p">];</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900263__li166871919123512">回收指定外部数据源上权限。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900263__screen9687919143516"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">REVOKE</span> <span class="p">[</span> <span class="k">GRANT</span> <span class="k">OPTION</span> <span class="k">FOR</span> <span class="p">]</span>
   <span class="err">{</span> <span class="k">USAGE</span> <span class="o">|</span> <span class="k">ALL</span> <span class="p">[</span> <span class="k">PRIVILEGES</span> <span class="p">]</span> <span class="err">}</span>
    <span class="k">ON</span> <span class="k">FOREIGN</span> <span class="k">DATA</span> <span class="n">WRAPPER</span> <span class="n">fdw_name</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="k">FROM</span> <span class="err">{</span> <span class="p">[</span> <span class="k">GROUP</span> <span class="p">]</span> <span class="n">role_name</span> <span class="o">|</span> <span class="k">PUBLIC</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="p">[</span> <span class="k">CASCADE</span> <span class="o">|</span> <span class="k">RESTRICT</span> <span class="p">];</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900263__li169819518362">回收指定外部服务器上权限。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900263__screen189813516363"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">REVOKE</span> <span class="p">[</span> <span class="k">GRANT</span> <span class="k">OPTION</span> <span class="k">FOR</span> <span class="p">]</span>
    <span class="err">{</span> <span class="err">{</span> <span class="k">USAGE</span> <span class="o">|</span> <span class="k">ALTER</span> <span class="o">|</span> <span class="k">DROP</span> <span class="o">|</span> <span class="k">COMMENT</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span> <span class="o">|</span> <span class="k">ALL</span> <span class="p">[</span> <span class="k">PRIVILEGES</span> <span class="p">]</span> <span class="err">}</span>
    <span class="k">ON</span> <span class="k">FOREIGN</span> <span class="n">SERVER</span> <span class="k">server_name</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="k">FROM</span> <span class="err">{</span> <span class="p">[</span> <span class="k">GROUP</span> <span class="p">]</span> <span class="n">role_name</span> <span class="o">|</span> <span class="k">PUBLIC</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="p">[</span> <span class="k">CASCADE</span> <span class="o">|</span> <span class="k">RESTRICT</span> <span class="p">];</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_l2c20827f9ded4dce8fe4dfde19f5b328">回收指定函数上权限。<pre class="screen" id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_s0c39abf7e8e948d083c3ab25ea599909">REVOKE [ GRANT OPTION FOR ]
    { { EXECUTE | ALTER | ALTER ROUTINE | DROP | COMMENT } [, ...] | ALL [ PRIVILEGES ] }
    ON { FUNCTION {function_name ( [ {[ argmode ] [ arg_name ] arg_type} [, ...] ] )} [, ...]
       | ALL FUNCTIONS IN SCHEMA schema_name [, ...] | schema_name.* }
    FROM { [ GROUP ] role_name | PUBLIC } [, ...]
    [ CASCADE | RESTRICT ];</pre>
</li><li id="ZH-CN_TOPIC_0289900263__li18353104633510">回收指定存储过程上权限。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900263__screen14488481357"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5
6</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">REVOKE</span> <span class="p">[</span> <span class="k">GRANT</span> <span class="k">OPTION</span> <span class="k">FOR</span> <span class="p">]</span>
    <span class="err">{</span> <span class="err">{</span> <span class="k">EXECUTE</span> <span class="o">|</span> <span class="k">ALTER</span> <span class="o">|</span> <span class="k">ALTER ROUTINE</span> <span class="o">|</span> <span class="k">DROP</span> <span class="o">|</span> <span class="k">COMMENT</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span> <span class="o">|</span> <span class="k">ALL</span> <span class="p">[</span> <span class="k">PRIVILEGES</span> <span class="p">]</span> <span class="err">}</span>
    <span class="k">ON</span> <span class="err">{</span> <span class="k">PROCEDURE</span> <span class="err">{</span><span class="n">proc_name</span> <span class="p">(</span> <span class="p">[</span> <span class="err">{</span><span class="p">[</span> <span class="n">argmode</span> <span class="p">]</span> <span class="p">[</span> <span class="n">arg_name</span> <span class="p">]</span> <span class="n">arg_type</span><span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span> <span class="p">]</span> <span class="p">)</span><span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
       <span class="o">|</span> <span class="k">ALL</span> <span class="k">PROCEDURE</span> <span class="k">IN</span> <span class="k">SCHEMA</span> <span class="k">schema_name</span> <span class="p">[,</span> <span class="p">...]</span> <span class="o">|</span> <span class="k">schema_name.*</span> <span class="err">}</span>
    <span class="k">FROM</span> <span class="err">{</span> <span class="p">[</span> <span class="k">GROUP</span> <span class="p">]</span> <span class="n">role_name</span> <span class="o">|</span> <span class="k">PUBLIC</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="p">[</span> <span class="k">CASCADE</span> <span class="o">|</span> <span class="k">RESTRICT</span> <span class="p">];</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900263__li387133815378">回收指定过程语言上权限。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900263__screen58743863713"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">REVOKE</span> <span class="p">[</span> <span class="k">GRANT</span> <span class="k">OPTION</span> <span class="k">FOR</span> <span class="p">]</span>
   <span class="err">{</span> <span class="k">USAGE</span> <span class="o">|</span> <span class="k">ALL</span> <span class="p">[</span> <span class="k">PRIVILEGES</span> <span class="p">]</span> <span class="err">}</span>
    <span class="k">ON</span> <span class="k">LANGUAGE</span> <span class="n">lang_name</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="k">FROM</span> <span class="err">{</span> <span class="p">[</span> <span class="k">GROUP</span> <span class="p">]</span> <span class="n">role_name</span> <span class="o">|</span> <span class="k">PUBLIC</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="p">[</span> <span class="k">CASCADE</span> <span class="o">|</span> <span class="k">RESTRICT</span> <span class="p">];</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_la71991d977dd4e50b6e84d061fde54e7">回收指定大对象上权限。<pre class="screen" id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_sf144c87206f44f1b85e5ea818c12a2fa">REVOKE [ GRANT OPTION FOR ]
    { { SELECT | UPDATE } [, ...] | ALL [ PRIVILEGES ] }
    ON LARGE OBJECT loid [, ...]
    FROM { [ GROUP ] role_name | PUBLIC } [, ...]
    [ CASCADE | RESTRICT ];</pre>
</li><li id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_lc250133173514957b0b6f1660df756f1">回收指定模式上权限。<pre class="screen" id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_sd7f43ae1546747e58281ed20d7c800ac">REVOKE [ GRANT OPTION FOR ]
    { { CREATE | USAGE | ALTER | DROP | COMMENT } [, ...] | ALL [ PRIVILEGES ] }
    ON SCHEMA schema_name [, ...]
    FROM { [ GROUP ] role_name | PUBLIC } [, ...]
    [ CASCADE | RESTRICT ];</pre>
</li><li id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_l3024d43e3a204daebfc69e6ad0456059">回收指定表空间上权限。<pre class="screen" id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_s426207c57a0e46f0876961acc100ce2e">REVOKE [ GRANT OPTION FOR ]
    { { CREATE | ALTER | DROP | COMMENT } [, ...] | ALL [ PRIVILEGES ] }
    ON TABLESPACE tablespace_name [, ...]
    FROM { [ GROUP ] role_name | PUBLIC } [, ...]
    [ CASCADE | RESTRICT ];</pre>
</li><li id="ZH-CN_TOPIC_0289900263__li130114218396">回收指定类型上权限。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900263__screen730152183918"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">REVOKE</span> <span class="p">[</span> <span class="k">GRANT</span> <span class="k">OPTION</span> <span class="k">FOR</span> <span class="p">]</span>
   <span class="err">{</span> <span class="err">{</span> <span class="k">USAGE</span> <span class="o">|</span> <span class="k">ALTER</span> <span class="o">|</span> <span class="k">DROP</span> <span class="o">|</span> <span class="k">COMMENT</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span> <span class="o">|</span> <span class="k">ALL</span> <span class="p">[</span> <span class="k">PRIVILEGES</span> <span class="p">]</span> <span class="err">}</span>
    <span class="k">ON</span> <span class="k">TYPE</span> <span class="n">type_name</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="k">FROM</span> <span class="err">{</span> <span class="p">[</span> <span class="k">GROUP</span> <span class="p">]</span> <span class="n">role_name</span> <span class="o">|</span> <span class="k">PUBLIC</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="p">[</span> <span class="k">CASCADE</span> <span class="o">|</span> <span class="k">RESTRICT</span> <span class="p">];</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900263__li17867157204017">回收Data Source对象上的权限。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900263__screen1731620984012"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">REVOKE</span> <span class="p">[</span> <span class="k">GRANT</span> <span class="k">OPTION</span> <span class="k">FOR</span> <span class="p">]</span>
   <span class="err">{</span> <span class="k">USAGE</span> <span class="o">|</span> <span class="k">ALL</span> <span class="p">[</span><span class="k">PRIVILEGES</span><span class="p">]</span> <span class="err">}</span>
    <span class="k">ON</span> <span class="k">DATA</span> <span class="k">SOURCE</span> <span class="n">src_name</span> <span class="p">[,</span> <span class="p">...]</span>
    <span class="k">FROM</span> <span class="err">{</span><span class="p">[</span><span class="k">GROUP</span><span class="p">]</span> <span class="n">role_name</span> <span class="o">|</span> <span class="k">PUBLIC</span><span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
   <span class="p">[</span> <span class="k">CASCADE</span> <span class="o">|</span> <span class="k">RESTRICT</span> <span class="p">];</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900263__li1753801972810">回收package对象的权限。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900263__screen14133104742917"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">REVOKE</span> <span class="p">[</span> <span class="k">GRANT</span> <span class="k">OPTION</span> <span class="k">FOR</span> <span class="p">]</span>
   <span class="err">{</span> <span class="err">{</span> <span class="k">EXECUTE</span> <span class="o">|</span> <span class="k">ALTER</span> <span class="o">|</span> <span class="k">DROP</span> <span class="o">|</span> <span class="k">COMMENT</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span> <span class="o">|</span> <span class="k">ALL</span> <span class="p">[</span><span class="k">PRIVILEGES</span><span class="p">]</span> <span class="err">}</span>
   <span class="k">ON</span> <span class="n">PACKAGE</span> <span class="n">package_name</span> <span class="p">[,</span> <span class="p">...]</span>
   <span class="k">FROM</span> <span class="err">{</span><span class="p">[</span><span class="k">GROUP</span><span class="p">]</span> <span class="n">role_name</span> <span class="o">|</span> <span class="k">PUBLIC</span><span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
   <span class="p">[</span> <span class="k">CASCADE</span> <span class="o">|</span> <span class="k">RESTRICT</span> <span class="p">];</span>
</pre></div>
</td></tr></table></div>
</li><li id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_l74bafdb55a76493d887cf968a5a02637">按角色回收角色上的权限。<pre class="screen" id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_s9a99a423dfd543e6bc380d45f5180a9d">REVOKE [ ADMIN OPTION FOR ]
    role_name [, ...] FROM role_name [, ...]
    [ CASCADE | RESTRICT ];</pre>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l909b6acf69c046c38146fab22d824fd6">将创建角色的权限回收的语法。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s8e34d3f10d234dbab830fabcb5963234">REVOKE CREATE USER ON *.* FROM ROLE_NAME;</pre>
</li><li id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_l909b6acf69c046c38146fab22d824fd6">将创建表空间的权限回收的语法。<pre class="screen" id="ZH-CN_TOPIC_0289900312__zh-cn_topic_0283137177_zh-cn_topic_0237122166_zh-cn_topic_0059778755_s8e34d3f10d234dbab830fabcb5963234">REVOKE CREATE TABLESPACE ON *.* FROM ROLE_NAME;</pre>
</li><li id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_l799cab40aedb435ebd897fa953cefb68">回收角色上的sysadmin权限。<pre class="screen" id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_saae89a8dd9f7460e8c298af0753a28b0">REVOKE ALL { PRIVILEGES | PRIVILEGE } FROM role_name;</pre>
</li></ul>
</div>
<ul id="ZH-CN_TOPIC_0289900263__ul7613142895910"><li id="ZH-CN_TOPIC_0289900263__li1642144111387">回收ANY权限。<div class="codecoloring" codetype="Sql" id="ZH-CN_TOPIC_0289900263__screen12642441193817"><table class="highlighttable"><tr><td class="linenos"><div class="linenodiv"><pre>1
2
3
4
5</pre></div></td><td class="code"><div class="highlight"><pre><span></span><span class="k">REVOKE</span> <span class="p">[</span> <span class="k">ADMIN</span> <span class="k">OPTION</span> <span class="k">FOR</span> <span class="p">]</span>
  <span class="err">{</span> <span class="k">CREATE</span> <span class="k">ANY</span> <span class="k">TABLE</span> <span class="o">|</span> <span class="k">ALTER</span> <span class="k">ANY</span> <span class="k">TABLE</span> <span class="o">|</span> <span class="k">DROP</span> <span class="k">ANY</span> <span class="k">TABLE</span> <span class="o">|</span> <span class="k">SELECT</span> <span class="k">ANY</span> <span class="k">TABLE</span> <span class="o">|</span> <span class="k">INSERT</span> <span class="k">ANY</span> <span class="k">TABLE</span> <span class="o">|</span> <span class="k">UPDATE</span> <span class="k">ANY</span> <span class="k">TABLE</span> <span class="o">|</span>
  <span class="k">DELETE</span> <span class="k">ANY</span> <span class="k">TABLE</span> <span class="o">|</span> <span class="k">CREATE</span> <span class="k">ANY</span> <span class="n">SEQUENCE</span> <span class="o">|</span> <span class="k">CREATE</span> <span class="k">ANY</span> <span class="k">INDEX</span> <span class="o">|</span> <span class="k">INDEX</span> <span class="o">|</span> <span class="k">CREATE</span> <span class="k">ANY</span> <span class="k">FUNCTION</span> <span class="o">|</span> <span class="k">CREATE</span> <span class="k">ROUTINE</span> <span class="o">|</span> <span class="k">EXECUTE</span> <span class="k">ANY</span> <span class="k">FUNCTION</span> <span class="o">|</span>
  <span class="k">CREATE</span> <span class="k">ANY</span> <span class="n">PACKAGE</span> <span class="o">|</span> <span class="k">EXECUTE</span> <span class="k">ANY</span> <span class="n">PACKAGE</span> <span class="o">|</span> <span class="k">CREATE</span> <span class="k">ANY</span> <span class="k">TYPE</span> <span class="err">}</span> <span class="p">[,</span> <span class="p">...]</span>
  <span class="p">[</span> <span class="k">ON</span>  <span class="k">*.*</span> <span class="p">]</span>
  <span class="k">FROM</span> <span class="p">[</span> <span class="k">GROUP</span> <span class="p">]</span> <span class="n">role_name</span> <span class="p">[,</span> <span class="p">...];</span>
</pre></div>
</td></tr></table></div>
</li></ul>
<div class="section" id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_s54fe58f3f55f4965a6b9370f9edebfdf"><h4 class="sectiontitle">参数说明</h4><p id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_a359d05ba131e403b9db97f13c21a419d">关键字PUBLIC表示一个隐式定义的拥有所有角色的组。</p>
<p id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_a5628bbd205a14a979b3c6ec694cfdf1c">权限类别和参数说明，请参见GRANT的参数说明。</p>
<p id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_p1856754015244">任何特定角色拥有的特权包括直接授予该角色的特权、从该角色作为其成员的角色中得到的权限以及授予给PUBLIC的权限。因此，从PUBLIC收回SELECT特权并不一定会意味着所有角色都会失去在该对象上的SELECT特权，那些直接被授予的或者通过另一个角色被授予的角色仍然会拥有它。类似地，从一个用户收回SELECT后，如果PUBLIC仍有SELECT权限，该用户还是可以使用SELECT。</p>
<p id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_a40df548c53314780be0d8f4b5ebc9b1d">指定GRANT OPTION FOR时，只撤销对该权限授权的权力，而不撤销该权限本身。</p>
<p id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_a80a31065a9794a9a843aa07bc2eb9b9b">如用户A拥有某个表的UPDATE权限，及WITH GRANT OPTION选项，同时A把这个权限赋予了用户B，则用户B持有的权限称为依赖性权限。当用户A持有的权限或者授权选项被撤销时，必须声明CASCADE，将所有依赖性权限都撤销。</p>
<p id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_ac5c28c26710b4713a2c4086f00bfb53c">一个用户只能撤销由它自己直接赋予的权限。例如，如果用户A被指定授权（WITH ADMIN OPTION）选项，且把一个权限赋予了用户B，然后用户B又赋予了用户C，则用户A不能直接将C的权限撤销。但是，用户A可以撤销用户B的授权选项，并且使用CASCADE。这样，用户C的权限就会自动被撤销。另外一个例子：如果A和B都赋予了C同样的权限，则A可以撤销他自己的授权选项，但是不能撤销B的，因此C仍然拥有该权限。</p>
<p id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_afa0c38b5c916451e85ba47962234270f">如果执行REVOKE的角色持有的权限是通过多层成员关系获得的，则具体是哪个包含的角色执行的该命令是不确定的。在这种场合下，最好的方法是使用SET ROLE成为特定角色，然后执行REVOKE，否则可能导致删除了不想删除的权限，或者是任何权限都没有删除。</p>
</div>
<div class="section" id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_s82ec0652acdd4e6091abc851b909926d"><h4 class="sectiontitle">示例</h4><p id="ZH-CN_TOPIC_0289900263__zh-cn_topic_0283137669_zh-cn_topic_0237122179_zh-cn_topic_0059779274_ad905dd85e0cb42fda73382aac2a3cedc">请参考GRANT的示例。</p>
</div>
</div>
</body>
</html>