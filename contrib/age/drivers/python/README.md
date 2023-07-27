# incubator-age AGType parser and driver support for Python 
AGType parser and driver support for [Apache AGE](https://age.apache.org/), graph extention for PostgreSQL.

### Features
* Unmarshal AGE result data(AGType) to Vertex, Edge, Path
* Cypher query support for Psycopg2 PostreSQL driver (enables to use cypher queries directly)

### Prerequisites
* over Python 3.9
* This module runs on [psycopg2](https://www.psycopg.org/) and [antlr4-python3](https://pypi.org/project/antlr4-python3-runtime/)
```
sudo apt-get update
sudo apt-get install python3-dev libpq-dev
pip install --no-binary :all: psycopg2
pip install antlr4-python3-runtime

```
### Test
```
python -m unittest -v test_age_py.py
python -m unittest -v test_agtypes.py
```

### Build from source
``` 
git clone https://github.com/apache/incubator-age.git
cd incubator-age/dirivers/python

python setup.py install

```

### Install from PyPi

``` 
pip install apache-age-python

```

### For more information about [Apache AGE](https://age.apache.org/)
* Apache Incubator Age : https://age.apache.org/
* Github : https://github.com/apache/incubator-age
* Document : https://age.incubator.apache.org/docs/
* apache-age-python GitHub : https://github.com/rhizome-ai/apache-age-python

### Check AGE loaded on your PostgreSQL
Connect to your containerized Postgres instance and then run the following commands:
```
# psql 
CREATE EXTENSION age;
LOAD 'age';
SET search_path = ag_catalog, "$user", public;
```

### Usage
* If you are familiar with Psycopg2 driver : Go to [Jupyter Notebook : Basic Sample](samples/apache-age-basic.ipynb) 
* Simpler way to access Apache AGE [AGE Sample](samples/apache-age-note.ipynb) in Samples.
* Agtype converting samples: [Agtype Sample](samples/apache-age-agtypes.ipynb) in Samples.

### License
Apache-2.0 License