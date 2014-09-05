enum clientMessage_t {
  GET = 1,
  QUIT = 0
};

/*struct fileName_t {
  unsigned int size;
  string bytes<>;
};*/

struct clientQuery_t {
  clientMessage_t message;
  string fileName<>;
};

enum serverMessage_t {
  OK = 1,
  ERR = 0
};

struct serverResponse_t {
  serverMessage_t serverMessage;
  opaque fileData_t<>;
};