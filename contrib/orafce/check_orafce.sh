#!/bin/bash

function delete()
{
    rm -rf test_orafce
}

function database_install()
{
    echo "init Opengauss database"
    gs_initdb -D test_orafce/dn1 --nodename=single_node1 -w Test@123 > init.log 2>&1
    if [ $? -ne 0 ]
    then
        echo "init failed, see init.log for detail information"
    delete
        exit 1
    else
        echo "init success, begin to start"
    fi
    echo "port = 25632" >> test_orafce/dn1/postgresql.conf
    echo "support_extended_features = on" >> test_orafce/dn1/postgresql.conf
    echo "enable_thread_pool = on" >> test_orafce/dn1/postgresql.conf
    echo "thread_pool_attr = '0, 0, (allbind)'" >> test_orafce/dn1/postgresql.conf
    gs_ctl start -D test_orafce/dn1 > start.log 2>&1
    if [ $? -ne 0 ]
    then
        echo "start failed，see start.log for detail information"
    delete
        exit 1
    else
        echo "openGauss start success，the port is 25632"
    fi
}

function run_check()
{
    #gsql -d postgres -p 25632 -c "create database regression dbcompatibility 'A';"
    gsql -d postgres -p 25632 -c "create database regression;"
    if [ $? -ne 0 ]
    then
        echo "create database failed"
	delete
        exit 1
    fi
    if [ ! -d "result" ]; then
        mkdir result
    fi

    # for sql_flle in $( ls sql/|awk -F '.' '{print $1;}')
    # do
    #     gsql -d regression -p 25632 -a <  sql/$sql_flle.sql > result/$sql_flle.out 2>&1
    # done

    username=$(whoami)
    sed -i "s/USER_NAME/$username/g" expected/dbcompatibility_pg_test.out
    sed -i "s/USER_NAME/$username/g" expected/nlssort.out

    gsql -d regression -p 25632 -a <  sql/init.sql > result/init.out 2>&1
    gsql -d regression -p 25632 -a <  sql/aggregates.sql > result/aggregates.out 2>&1
    gsql -d regression -p 25632 -a <  sql/files.sql > result/files.out 2>&1
    gsql -d regression -p 25632 -a <  sql/dbms_output.sql > result/dbms_output.out 2>&1
    gsql -d regression -p 25632 -a <  sql/dbms_random.sql > result/dbms_random.out 2>&1
    gsql -d regression -p 25632 -a <  sql/dbms_utility.sql > result/dbms_utility.out 2>&1
    gsql -d regression -p 25632 -a <  sql/nlssort.sql > result/nlssort.out 2>&1
    gsql -d regression -p 25632 -a <  sql/varchar2.sql > result/varchar2.out 2>&1
    gsql -d regression -p 25632 -a <  sql/nvarchar2.sql > result/nvarchar2.out 2>&1
    gsql -d regression -p 25632 -a <  sql/orafce.sql > result/orafce.out 2>&1
    gsql -d regression -p 25632 -a <  sql/orafce2.sql > result/orafce2.out 2>&1
    gsql -d regression -p 25632 -a <  sql/regexp_func.sql > result/regexp_func.out 2>&1
    gsql -d regression -p 25632 -a <  sql/dbcompatibility_pg_test.sql > result/dbcompatibility_pg_test.out 2>&1
    gsql -d regression -p 25632 -a <  sql/charlen.sql > result/charlen.out 2>&1
    gsql -d regression -p 25632 -a <  sql/to_date_with_timezone.sql > result/to_date_with_timezone.out 2>&1
    gsql -d regression -p 25632 -a <  sql/dbms_pipe.sql > result/dbms_pipe.out 2>&1
    gsql -d regression -p 25632 -a <  sql/dbms_pipe_session_A.sql > result/dbms_pipe_session_A.out 2>&1
    gsql -d regression -p 25632 -a <  sql/dbms_pipe_session_B.sql > result/dbms_pipe_session_B.out 2>&1
#    gsql -d regression -p 25632 -a <  sql/dbms_alert_session_A.sql > result/dbms_alert_session_A.out 2>&1 
#    gsql -d regression -p 25632 -a <  sql/dbms_alert_session_B.sql > result/dbms_alert_session_B.out 2>&1 
#    gsql -d regression -p 25632 -a <  sql/dbms_alert_session_C.sql > result/dbms_alert_session_C.out 2>&1
#    wait

    if [ -e "diff.log" ]; then
        rm diff.log
    fi

    # for out_file in $(ls expected)
    # do
    #     diff -u expected/$out_file result/$out_file >> diff.log
    # done

    user=$(whoami)
    sed -i "s/hlv/$user/g" expected/dbcompatibility_pg_test.out
    sed -i "s/hlv/$user/g" expected/nlssort.out

    diff -u expected/init.out result/init.out >> diff.log
    diff -u expected/aggregates.out result/aggregates.out >> diff.log
    diff -u expected/files.out result/files.out >> diff.log
    diff -u expected/dbms_output.out result/dbms_output.out >> diff.log
    diff -u expected/dbms_random.out result/dbms_random.out >> diff.log
    diff -u expected/dbms_utility.out result/dbms_utility.out >> diff.log
    diff -u expected/nlssort.out result/nlssort.out >> diff.log
    diff -u expected/varchar2.out result/varchar2.out >> diff.log
    diff -u expected/nvarchar2.out result/nvarchar2.out >> diff.log
    diff -u expected/orafce.out result/orafce.out >> diff.log
    diff -u expected/orafce2.out result/orafce2.out >> diff.log
    diff -u expected/regexp_func.out result/regexp_func.out >> diff.log
    diff -u expected/dbcompatibility_pg_test.out result/dbcompatibility_pg_test.out >> diff.log
    diff -u expected/dbms_pipe.out result/dbms_pipe.out >> diff.log
    diff -u expected/dbms_pipe_session_A.out result/dbms_pipe_session_A.out >> diff.log
    diff -u expected/dbms_pipe_session_B.out result/dbms_pipe_session_B.out >> diff.log
#    diff -u expected/dbms_alert_session_A.out result/dbms_alert_session_A.out >> diff.log
#    diff -u expected/dbms_alert_session_B.out result/dbms_alert_session_B.out >> diff.log
#    diff -u expected/dbms_alert_session_C.out result/dbms_alert_session_C.out >> diff.log

    if [[ `cat diff.log |wc -l` -eq 0 ]]
    then
        echo -e "\033[32m OK \033[0m"
    else
        echo -e "\033[31m FAILED \033[0m"
    fi
    pid=$(ps ux | grep "test_orafce" | grep -v "grep" | tr -s ' ' | cut -d ' ' -f 2)
    kill -9 $pid
    delete
    rm init.log
    rm start.log
}

function main()
{
    database_install
    run_check
}

main $@
