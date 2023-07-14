typedef enum {P_BOOL, P_CHAR, P_UCHAR, P_INT, P_UINT, P_LONG, P_ULONG, P_DOUBLE} packet_t;

const char* packet_type_to_string(packet_t t) {
  switch (t) {
  case P_BOOL:
    return "P_BOOL";
  case P_CHAR:
    return "P_CHAR";
  case P_UCHAR:
    return "P_UCHAR";
  case P_INT:
    return "P_INT";
  case P_UINT:
    return "P_UINT";
  case P_LONG:
    return "P_LONG";
  case P_ULONG:
    return "P_ULONG";
  case P_DOUBLE:
    return "P_DOUBLE";
  default:
    return "Unknown packet type";
  }
}
