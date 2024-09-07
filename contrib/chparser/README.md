chparser
========

chparser is an openGauss extension for full-text search of Chinese language (Mandarin Chinese). It implements a Chinese language parser base on 
the [Simple Chinese Word Segmentation(SCWS)](https://github.com/hightman/scws).

Project home page: http://blog.amutu.com/chparser/

**注意**：对于分词结果不满意的，或者需要调试分词结果的，可以在这个页面调试：http://www.xunsearch.com/scws/demo/v48.php

INSTALL
-------
1. 安装SCWS

```
 wget -q -O - http://www.xunsearch.com/scws/down/scws-1.2.3.tar.bz2 | tar xfj -

 cd scws-1.2.3 ; ./configure --prefix=/home/omm/scws; make install 
 
"/home/omm/scws"为安装scws时制定prefix路径，用户可自行制定

注意:在FreeBSD release 10及以上版本上运行configure时，需要增加--with-pic选项。

如果是从github上下载的scws源码需要先运行以下命令生成configure文件： 

 git clone https://github.com/hightman/scws.git
 cd scws; touch README;aclocal;autoconf;autoheader;libtoolize;automake --add-missing
```
2. 下载openGauss/Plugin源码

```
 git clone https://gitee.com/opengauss/Plugin.git
```
3. 设置scws环境变量：

```
 export PATH=/home/omm/scws/bin:$PATH 
 export LD_LIBRARY_PATH=/home/omm/scws/lib:$PATH
 
"/home/omm/scws"为configure时通过prefix指定的安装路径，通过具体情况修改
```

4. 编译和安装chparser

```
 cp -r Plugin/contrib/chparser openGauss-server/contrib/
 cd openGauss-server/contrib/chparser
 make -j; make -j install
```



5. 创建extension

```
 gsql -d dbname -p port -U superuser -W password -c 'CREATE EXTENSION chparser'
```

CONFIGURATION
-------

忽略所有的标点等特殊符号: 
chparser.punctuation_ignore = f 

闲散文字自动以二字分词法聚合: 
chparser.seg_with_duality = f 

将词典全部加载到内存里: 
chparser.dict_in_memory = f 

短词复合: 
chparser.multi_short = f 

散字二元复合: 
chparser.multi_duality = f 

重要单字复合: 
chparser.multi_zmain = f 

全部单字复合: 
chparser.multi_zall = f 

除了chparser自带的词典，用户可以增加自定义词典，自定义词典的优先级高于自带的词典。自定义词典的文件必须放在share/tsearch_data目录中,chparser根据文件扩展名确定词典的格式类型，.txt扩展名表示词典是文本格式，.xdb扩展名表示这个词典是xdb格式，多个文件使用逗号分隔,词典的分词优先级由低到高,如：  

chparser.extra_dicts = 'dict_extra.txt,mydict.xdb' 

注意：chparser.extra_dicts和chparser.dict_in_memory两个选项需要在backend启动前设置（可以在配置文件中修改然后reload，之后新建连接会生效）,其他选项可以随时在session中设置生效。chparser的选项与scws相关的选项对应，关于这些选项的含义，可以参考scws的文档：http://www.xunsearch.com/scws/docs.php#libscws  

EXAMPLE
-------
```
-- create the extension

CREATE EXTENSION chparser;

-- make test configuration using parser

CREATE TEXT SEARCH CONFIGURATION testchcfg (PARSER = chparser);

-- add token mapping

ALTER TEXT SEARCH CONFIGURATION testchcfg ADD MAPPING FOR n,v,a,i,e,l WITH simple;

-- ts_parse

SELECT * FROM ts_parse('chparser', 'hello world! 2010年保障房建设在全国范围内获全面启动，从中央到地方纷纷加大 了保障房的建设和投入力度 。2011年，保障房进入了更大规模的建设阶段。住房城乡建设部党组书记、部长姜伟新去年底在全国住房城乡建设工作会议上表示，要继续推进保障性安居工程建设。');

-- test to_tsvector

SELECT to_tsvector('testchcfg','“今年保障房新开工数量虽然有所下调，但实际的年度在建规模以及竣工规模会超以往年份，相对应的对资金的需求也会创历>史纪录。”陈国强说。在他看来，与2011年相比，2012年的保障房建设在资金配套上的压力将更为严峻。');

-- test to_tsquery

SELECT to_tsquery('testchcfg', '保障房资金压力');
```

自定义词库
-------
** 详解 TXT 词库的写法 (TXT词库目前已兼容 cli/scws_gen_dict 所用的文本词库) ** 

1) 每行一条记录，以 # 或 分号开头的相当于注释，忽略跳过 

2) 每行由4个字段组成，依次为“词语"(由中文字或3个以下的字母合成), "TF", "IDF", "词性"，字段使用空格或制表符分开，数量不限，可自行对齐以美化 

3) 除“词语”外，其它字段可忽略不写。若忽略，TF和IDF默认值为 1.0 而 词性为 "@" 

4) 由于 TXT 库动态加载（内部监测文件修改时间自动转换成 xdb 存于系统临时目录），故建议TXT词库不要过大 

5) 删除词做法，请将词性设为“!“，则表示该词设为无效，即使在其它核心库中存在该词也视为无效 

注意：1.自定义词典的格式可以是文本TXT，也可以是二进制的XDB格式。XDB格式效率更高，适合大辞典使用。可以使用scws自带的工具scws-gen-dict将文本词典转换为XDB格式；2.chparser默认的词典是简体中文，如果需要繁体中文，可以在[这里](http://www.xunsearch.com/scws/download.php)下载已经生成好的XDB格式此词典。3.自定义词典的例子可以参考[dict_extra.txt](https://github.com/amutu/chparser/blob/master/dict_extra.txt)。更多信息参见[SCWS官方文档](http://www.xunsearch.com/scws/docs.php#utilscws)。

自定义词库 2.1
-------
** 自定义词库2.1 增加自定义词库的易容性, 并兼容1.0提供的功能 **


自定义词库需要superuser权限, 自定义库是数据库级别的(不是实例),每个数据库拥有自己的自定义分词, 并存储在data目录下base/数据库ID下(2.0 版本存储在share/tsearch_data下)

生成环境版本升级(新环境直接安装就可以)：
	alter extension chparser update ;
```
test=# SELECT * FROM ts_parse('chparser', '保障房资金压力');
 tokid | token
-------+-------
   118 | 保障
   110 | 房
   110 | 资金
   110 | 压力

test=# insert into chparser.chprs_custom_word values('资金压力');
--删除词insert into chprs_custom_word(word, attr) values('word', '!');
--\d chprs_custom_word 查看其表结构，支持TD, IDF
test=# select sync_chprs_custom_word();
 sync_chprs_custom_word
------------------------

(1 row)

test=# \q --sync 后重新建立连接
[omm@localhost-pc bin]$ ./gsql -U omm -d test -p 1600
test=# SELECT * FROM ts_parse('chparser', '保障房资金压力');
 tokid |  token
-------+----------
   118 | 保障
   110 | 房
   120 | 资金压力
```


COPYRITE
--------

zhparser

Portions Copyright (c) 2012-2017, Jov(amutu@amutu.com)

Permission to use, copy, modify, and distribute this software and its documentation
for any purpose, without fee, and without a written agreement is hereby granted,
provided that the above copyright notice and this paragraph and the following 
two paragraphs appear in all copies.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
