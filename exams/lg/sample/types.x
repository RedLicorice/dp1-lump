enum clientMessage_t {
  GET = 1,
  QUIT = 0
};

struct clientReq_t {
  clientMessage_t message;
  string fileName<>;
};

enum serverMessage_t {
  OK = 1,
  ERR = 0
};

struct serverRes_t {
  serverMessage_t message;
  opaque fileData_t<>;
};