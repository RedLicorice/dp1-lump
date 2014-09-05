enum operation {
  QUIT = 0,
  GET = 1
};

struct Request {
  string data<>; /* file name */
  operation op;
};

struct Response {
  bool success;
  opaque data<>; /* file contents */
};