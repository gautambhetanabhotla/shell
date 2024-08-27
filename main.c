#include "src/prompt.h"

int main() {
    init_shell();
    while(true) prompt();
    return 0;
}