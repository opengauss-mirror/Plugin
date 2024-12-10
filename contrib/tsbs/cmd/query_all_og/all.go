package main

import (
	"database/sql"
	"fmt"
	"log"
	"time"

	_ "github.com/jackc/pgx/v4/stdlib"
	_ "github.com/lib/pq"
)

const (
	connStr = "user=lhy dbname=postgres sslmode=disable password=Test@123"
)

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
