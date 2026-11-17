FROM gcr.io/oss-fuzz-base/base-builder-go@sha256:1dbadefbb28c6bfd796c64d9c2115977deb8f19494533cc5a3b724c3441df23d

COPY contrib/age/drivers/golang $SRC/Plugin/contrib/age/drivers/golang
WORKDIR $SRC/Plugin/contrib/age/drivers/golang
RUN go mod download all
COPY .clusterfuzzlite/build.sh $SRC/build.sh
