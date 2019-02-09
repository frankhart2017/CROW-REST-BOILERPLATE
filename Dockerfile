FROM bbox:latest

WORKDIR /usr/src/cpp-rest/
COPY . .

WORKDIR /usr/src/cpp-rest/
RUN cmake .
RUN make
CMD ["./cpp-rest"]
