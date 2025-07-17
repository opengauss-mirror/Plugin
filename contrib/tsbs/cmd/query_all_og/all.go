package main

import (
	"database/sql"
	"fmt"
	"log"
	"time"
	"io/ioutil"
	"encoding/json"

	_ "github.com/jackc/pgx/v4/stdlib"
	_ "github.com/lib/pq"
)

type Config struct {
	User     string `json:"user"`
	DBName   string `json:"dbname"`
	SSLMode  string `json:"sslmode"`
	Password string `json:"password"`
	// This is a vacant value by default, fill it within the tsbs/cmd/config.json, \
	// you can also refer to tsbs/scripts/load_timescaledb.sh for a default configuration
}

func loadConfig() string {
	config := Config{
			SSLMode: "disable",
	}

	file, err := ioutil.ReadFile("../config.json")
	if err != nil {
			if os.IsNotExist(err) {
					log.Fatal("config.json does not exist, please create tsbs/cmd/config.json")
			}
			log.Fatal("failed to read tsbs/cmd/config.json:", err)
	}

	if err := json.Unmarshal(file, &config); err != nil {
			log.Fatal("failed to parse tsbs/cmd/config.json:", err)
	}

	if config.User == "" || config.DBName == "" || config.Password == "" {
			log.Fatal("user, dbname and password must be provided in tsbs/cmd/config.json")
	}

	return buildConnStr(config)
}

func buildConnStr(c Config) string {
	return "user=" + c.User +
		   " dbname=" + c.DBName +
		   " sslmode=" + c.SSLMode +
		   " password=" + c.Password
}

var connStr = loadConfig()

func main() {
	db, err := sql.Open("pgx", connStr)
	if err != nil {
		log.Fatal(err)
	}
	defer db.Close()

	query := `
	SELECT * FROM cpu WHERE usage_user > 80 AND usage_system < 20;
	`

	startTime := time.Now()
	rows, err := db.Query(query)
	if err != nil {
		log.Fatal(err)
	}
	defer rows.Close()

	duration := time.Since(startTime)
	fmt.Printf("Query all from cpu entries in %v\n", duration)

	columns, err := rows.Columns()
	if err != nil {
		log.Fatal(err)
	}

	// Create a slice of interface{}'s to represent each column, and a second slice to contain pointers to each item in the columns slice.
	values := make([]interface{}, len(columns))
	valuePtrs := make([]interface{}, len(columns))
	for i := range values {
		valuePtrs[i] = &values[i]
	}

	count := 0
	for rows.Next() {
		err := rows.Scan(valuePtrs...)
		if err != nil {
			log.Fatal(err)
		}

		// Print each row's data
		for i, col := range columns {
			val := values[i]

			// Convert the value to a string
			var v interface{}
			if b, ok := val.([]byte); ok {
				v = string(b)
			} else {
				v = val
			}
			fmt.Printf("%s: %v ", col, v)
		}
		fmt.Println()

		count++
		if count >= 5 {
			break
		}
	}

	if err := rows.Err(); err != nil {
		log.Fatal(err)
	}
}
