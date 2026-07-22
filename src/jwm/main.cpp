
#include <jx.hpp>
#include <gdi/gdi.hpp>

const char message[] = "Hello, World!\r\n";

PROCEDURAL int main(int argc, char **argv, char **envp)
{       (void)argc; (void)argv; (void)envp;
        JX::stdout.write(message, sizeof(message));
        return 0;
}
