FROM ubuntu:24.04

WORKDIR /app

RUN apt-get update && \
    apt-get install -y dpkg libc6 libstdc++6 && \
    rm -rf /var/lib/apt/lists/*

COPY build/*.deb /app/

RUN dpkg -i /app/*.deb || (apt-get update && apt-get install -f -y)

EXPOSE 8080

CMD ["prime-checker"]