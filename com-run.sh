docker build --rm --squash --no-cache -t restcpp:latest .
docker run -p 8080:8080 -e PORT=8080 -e MONGODB_URI="mongodb://heroku_nqv061hq:t86if4klo126d7sar1hohl40ut@ds131621.mlab.com:31621/heroku_nqv061hq" restcpp:latest
