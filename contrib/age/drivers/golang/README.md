# incubator-age AGType parser and driver support for Golang 

AGType parser and driver support for [Apache AGE](https://age.apache.org/), graph extention for PostgreSQL.

### Features
* Unmarshal AGE result data(AGType) to Vertex, Edge, Path
* Cypher query support for 3rd. Party sql driver (enables to use cypher queries directly)

### Prerequisites
* over Go 1.16
* This module runs on golang standard api [database/sql](https://golang.org/pkg/database/sql/) and [antlr4-python3](https://github.com/antlr/antlr4/tree/master/runtime/Go/antlr)


### Go get  
``` 
go get github.com/apache/incubator-age/drivers/golang
```
### gomod
``` 
require  github.com/apache/incubator-age/drivers/golang {version}
```


Check [latest version](https://github.com/apache/incubator-age/releases)

### For more information about [Apache AGE](https://age.apache.org/)
* Apache Incubator Age : https://age.apache.org/
* Github : https://github.com/apache/incubator-age
* Document : https://age.incubator.apache.org/docs/

### Check AGE loaded on your PostgreSQL
Connect to your containerized Postgres instance and then run the following commands:
```(sql)
# psql 
CREATE EXTENSION age;
LOAD 'age';
SET search_path = ag_catalog, "$user", public;
```

### Test
Check out and rewrite DSN in incubator-age/drivers/golang/age/age_test.go
```
cd incubator-age/drivers/golang/age
go test . -v

```

### Samples
* Usage 1: using database/sql API and Cypher execution function 'ExecCypher' 
  Sample : [samples/sql_api_sample.go](samples/sql_api_sample.go)

* Usage 2: using Age Wrapper 
  Sample : [samples/age_wrapper_sample.go](samples/age_wrapper_sample.go)

* Run Samples : [samples/main.go](samples/main.go)


### License
Apache-2.0 License