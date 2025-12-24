#include "helpers/AppConfiguration.h"
#include "helpers/StructureHelper.h"

using namespace std;

int main(const int argc, const char *argv[]) {
    int port = 5000;
    if (argc > 1) port = stoi(argv[1]);
    return AppConfiguration::bootstrap(port) ? 0 : -1;
}
