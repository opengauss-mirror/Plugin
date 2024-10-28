pid=$$
docker run --rm --name testpgchparser-$pid -p 5432:5432 -d -e POSTGRES_PASSWORD=somepassword@alpine chparser/chparser:alpine-16
sleep 5
export PGPASSWORD=somepassword@alpine
psql -h 127.0.0.1 -X -a -q postgres postgres -f sql/chparser.sql | diff expected/chparser-alpine.out -

if [ $? -eq 0 ]
then
    echo "pass!"
else
    echo "do not pass!"
fi
docker stop testpgchparser-$pid
