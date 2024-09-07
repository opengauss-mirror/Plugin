pid=$$
docker run --rm --name testpgchparser-$pid -p 5432:5432 -d -e POSTGRES_PASSWORD=somepassword@debian-16 chparser/chparser:bookworm-16
sleep 5
export PGPASSWORD=somepassword@debian-16
psql -h 127.0.0.1 -X -a -q postgres postgres -f sql/chparser.sql | diff expected/chparser-debian.out -

if [ $? -eq 0 ]
then
    echo "pass!"
else
    echo "do not pass!"
fi
docker stop testpgchparser-$pid
