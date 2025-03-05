#! /bin/sh

CURRENT_DIR=$(cd `dirname $0`; pwd)

connectorJ="$CURRENT_DIR/class/mysql-connector-java-5.1.47.jar"
localZip="$CURRENT_DIR/mysql-connector-java-5.1.47.tar.gz"
if [ ! -f "$connectorJ" ]; then
     if [ ! -f "$localZip" ]; then
          wget http://dev.mysql.com/get/Downloads/Connector-J/mysql-connector-java-5.1.47.tar.gz 
     fi
     tar -zxf $localZip 
     cp mysql-connector-java-5.1.47/mysql-connector-java-5.1.47.jar $CURRENT_DIR/class/
     rm -rf mysql-connector-java-5.1.47/
fi

connectorJ="$CURRENT_DIR/class/mysql-connector-java-8.0.28.jar"
localZip="$CURRENT_DIR/mysql-connector-java-8.0.28.tar.gz"
if [ ! -f "$connectorJ" ]; then
     if [ ! -f "$localZip" ]; then
          wget http://dev.mysql.com/get/Downloads/Connector-J/mysql-connector-java-8.0.28.tar.gz
     fi
     tar -zxf $localZip 
     cp mysql-connector-java-8.0.28/mysql-connector-java-8.0.28.jar $CURRENT_DIR/class/
     rm -rf mysql-connector-java-8.0.28/
fi
