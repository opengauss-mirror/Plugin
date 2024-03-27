ARG PG_MAJOR=15
FROM postgres:$PG_MAJOR
ARG PG_MAJOR

COPY . /tmp/datavec

RUN apt-get update && \
		apt-get install -y --no-install-recommends build-essential postgresql-server-dev-$PG_MAJOR && \
		cd /tmp/datavec && \
		make clean && \
		make OPTFLAGS="" && \
		make install && \
		mkdir /usr/share/doc/datavec && \
		cp LICENSE README.md /usr/share/doc/datavec && \
		rm -r /tmp/datavec && \
		apt-get remove -y build-essential postgresql-server-dev-$PG_MAJOR && \
		apt-get autoremove -y && \
		rm -rf /var/lib/apt/lists/*
