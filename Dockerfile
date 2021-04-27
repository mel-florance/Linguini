FROM ubuntu
RUN apt update
RUN apt -y install\
	bash\
	make\
	g++\
	libmysqlclient-dev\
	libmysqlcppconn-dev\
	libuv1-dev\
	openssl
COPY . /app
WORKDIR /app

RUN /app/Tools/premake/premake5 gmake
RUN make
CMD ["/bin/bash", "-c", "/app/Binaries/Debug-linux-x86_64/Server/Server"]
