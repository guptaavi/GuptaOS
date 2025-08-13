// Dummy __stack_chk_fail_local function
// This is a workaround for GCC's stack protector when building bare-metal kernels.

void __stack_chk_fail_local(void) {
    // For now, just halt the CPU.
    // In a real OS, this would trigger a kernel panic or a more robust error handling.
    __asm__ __volatile__ ("cli\nhlt");
}
