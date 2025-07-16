package main

import (
	"bufio"
	"database/sql"
	"fmt"
	"log"
	"os"
	"strings"
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

	err = createTables(db)
	if err != nil {
		log.Fatal(err)
	}

	file, err := os.Open("/tmp/timescaledb-data")
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	var tagsInserted, cpuInserted int
	startTime := time.Now()

	var count int
	for scanner.Scan() {
		if count < 3 {
			count++
			continue
		}
		line := scanner.Text()
		if strings.HasPrefix(line, "tags") {
			tagData := parseTagsLine(line)
			err = insertTags(db, tagData)
			if err != nil {
				log.Fatal(err)
			}
			tagsInserted++
		} else if strings.HasPrefix(line, "cpu") {
			cpuData := parseCpuLine(line)
			err = insertCpu(db, cpuData)
			if err != nil {
				log.Fatal(err)
			}
			cpuInserted++
		}
	}

	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}

	duration := time.Since(startTime)
	fmt.Printf("Inserted %d tags and %d cpu entries in %v\n", tagsInserted, cpuInserted, duration)
}

func createTables(db *sql.DB) error {
	tagsTable := `
	CREATE TABLE IF NOT EXISTS tags (
		hostname TEXT,
		region TEXT,
		datacenter TEXT,
		rack TEXT,
		os TEXT,
		arch TEXT,
		team TEXT,
		service TEXT,
		service_version TEXT,
		service_environment TEXT
	);`

	cpuTable := `
	CREATE TABLE IF NOT EXISTS cpu (
		timestamp BIGINT,
		usage_user INT,
		usage_system INT,
		usage_idle INT,
		usage_nice INT,
		usage_iowait INT,
		usage_irq INT,
		usage_softirq INT,
		usage_steal INT,
		usage_guest INT,
		usage_guest_nice INT
	);`

	_, err := db.Exec(tagsTable)
	if err != nil {
		return err
	}

	_, err = db.Exec(cpuTable)
	return err
}

func parseTagsLine(line string) map[string]string {
	data := strings.Split(line, ",")
	tagData := make(map[string]string)
	for _, item := range data[1:] {
		parts := strings.Split(item, "=")
		tagData[parts[0]] = parts[1]
	}
	return tagData
}

func parseCpuLine(line string) []interface{} {
	data := strings.Split(line, ",")
	var cpuData []interface{}
	for _, item := range data[1:] {
		cpuData = append(cpuData, item)
	}
	return cpuData
}

func insertTags(db *sql.DB, data map[string]string) error {
	query := `
	INSERT INTO tags (hostname, region, datacenter, rack, os, arch, team, service, service_version, service_environment)
	VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10);`
	_, err := db.Exec(query, data["hostname"], data["region"], data["datacenter"], data["rack"], data["os"], data["arch"], data["team"], data["service"], data["service_version"], data["service_environment"])
	//if err == nil {
	//	log.Printf("Inserted tag: %v\n", data)
	//}
	return err
}

func insertCpu(db *sql.DB, data []interface{}) error {
	query := `
	INSERT INTO cpu (timestamp, usage_user, usage_system, usage_idle, usage_nice, usage_iowait, usage_irq, usage_softirq, usage_steal, usage_guest, usage_guest_nice)
	VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11);`
	_, err := db.Exec(query, data...)
	//if err == nil {
	//	log.Printf("Inserted CPU data: %v\n", data)
	//}
	return err
}
