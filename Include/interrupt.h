// Interrupt handlers must be C function.
// For a project compiled as a C project this is not necessary.

#ifdef _cplusplus
extern "C"
{
#endif
    void SysTick_Handler();
    
#ifdef _cplusplus
}
#endif