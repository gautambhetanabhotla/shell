#include "src/parser.h"
#include "src/prompt.h"
#include "src/hop.h"

int main() {
    init_shell();
    while(true) prompt();
    return 0;
}