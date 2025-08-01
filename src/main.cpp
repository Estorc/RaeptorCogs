#include <app.hpp>

const char* CLIENT_ID = "oimks47i3t5pnlb2vm7hy3rdfkwh6j";
const char* CLIENT_SECRET = "6jlc8bcesyj1cukiga7aji44cba78i";

int main() {
    RaeptorApplication app = RaeptorApplication();

    int8_t ret_value = 0;
    while (!(ret_value = app.update()));

    return ret_value;
}
