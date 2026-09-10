
#include <gdi/gdi.hpp>
#include <jx.hpp>

PROCEDURAL int main(int argc, char **argv, char **envp) {
  (void)argc;
  (void)argv;
  (void)envp;
  Display disp;
  JX::stdout.write("Hello, World!\r\n", 15);
  return 0;
}
