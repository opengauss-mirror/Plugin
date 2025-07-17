read -s -p "Please enter password for container to start: " user_password
echo

if [ -z "$user_password" ]; then
    echo "password is required for the test"
    exit 1
fi

pid=$$
docker run --rm --name testpgchparser-$pid -p 5432:5432 -d -e POSTGRES_PASSWORD="$user_password" chparser/chparser:alpine-16
sleep 5
export PGPASSWORD="$user_password"
psql -h 127.0.0.1 -X -a -q postgres postgres -f sql/chparser.sql | diff expected/chparser-alpine.out -

if [ $? -eq 0 ]
then
    echo "pass!"
else
    echo "do not pass!"
fi
docker stop testpgchparser-$pid
