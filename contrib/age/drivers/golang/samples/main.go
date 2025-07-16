/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
package main

import (
	"fmt"
	"encoding/json"
	"io/ioutil"
	"log"

	_ "github.com/lib/pq"
)

type Config struct {
	DBHost     string `json:"db_host"`
	DBPort     int    `json:"db_port"`
	DBName     string `json:"db_name"`
	DBUser     string `json:"db_user"`
	DBPassword string `json:"db_password"`
	GraphName  string `json:"graph_name"`
}

func loadConfig() Config {
	file, err := ioutil.ReadFile("./config.json")
	if err != nil {
			log.Fatal("Failed to read config file:", err)
	}

	var config Config
	if err := json.Unmarshal(file, &config); err != nil {
			log.Fatal("Failed to parse config:", err)
	}
	return config
}

var config Config = loadConfig()

// var dsn string = "host={host} port={port} dbname={dbname} user={username} password={password} sslmode=disable"
var dsn string = fmt.Sprintf(
	"host=%s port=%d dbname=%s user=%s password=%s sslmode=disable",
	config.DBHost, config.DBPort, config.DBName, config.DBUser, config.DBPassword,
)

// var graphName string = "{graph_path}"
var graphName string = "testGraph"

func main() {

	// Do cypher query to AGE with database/sql Tx API transaction conrol
	fmt.Println("# Do cypher query with SQL API")
	doWithSqlAPI(dsn, graphName)

	// Do cypher query to AGE with Age API
	fmt.Println("# Do cypher query with Age API")
	doWithAgeWrapper(dsn, graphName)
}
