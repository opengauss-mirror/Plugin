#! /bin/sh

CURRENT_DIR=$(cd `dirname $0`; pwd)

connectorJ="$CURRENT_DIR/class/mysql-connector-java-5.1.47.jar"
if [ ! -f "$connectorJ" ]; then
     wget http://dev.mysql.com/get/Downloads/Connector-J/mysql-connector-java-5.1.47.tar.gz
     tar -zxf mysql-connector-java-5.1.47.tar.gz 
     cp mysql-connector-java-5.1.47/mysql-connector-java-5.1.47.jar $CURRENT_DIR/class/
     rm -rf mysql-connector-java-5.1.47* 
fi

connectorJ="$CURRENT_DIR/class/mysql-connector-java-8.0.28.jar"
if [ ! -f "$connectorJ" ]; then
     wget http://dev.mysql.com/get/Downloads/Connector-J/mysql-connector-java-8.0.28.tar.gz
     tar -zxf mysql-connector-java-8.0.28.tar.gz 
     cp mysql-connector-java-8.0.28/mysql-connector-java-8.0.28.jar $CURRENT_DIR/class/
     rm -rf mysql-connector-java-8.0.28* 
fi