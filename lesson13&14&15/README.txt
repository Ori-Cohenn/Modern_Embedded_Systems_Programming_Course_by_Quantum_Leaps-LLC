 In Keil IDE, the linker script is typically a default one that's used for all projects, and it's controlled through settings in the IDE rather than individual linker script files. In contrast, IAR IDE allows for custom linker files for each project, giving more control over the memory layout and other linker-specific settings.

if im still want to change  the linker file i need to create a scatter file

why in C we want to use extern? 

Accessing Global Variables Across Files: extern allows you to declare a global variable in one file so it can be used in other files without redefining it.

Function Prototypes: Using extern in header files clarifies that functions declared there are defined elsewhere and are available for use in multiple files.

Avoiding Multiple Definitions: extern in a header file prevents multiple definitions of a global variable, ensuring only one definition exists across the program.

Linking with Assembly Code: extern links C code with assembly code by declaring symbols (variables or functions) defined in assembly files.

Indicating External Linkage: extern explicitly indicates that a function or variable has external linkage, improving code readability and clarity about the symbol's linkage.

lesson 15

Stack Pointer Initialization:

The first entry in the vector table should be the initial value of the Stack Pointer.
Use the symbol CSTACK$$Limit to initialize the stack pointer, compatible with the IAR toolset.
Declare CSTACK$$Limit as an extern int to inform the compiler of its existence.
Apply an explicit cast to int to resolve the symbol correctly.
Validate the stack pointer initialization by checking the SP register and the linker map file.
Exception and Interrupt Handlers:

The Reset handler address should be initialized with __iar_program_start.
Declare function prototypes for exception handlers using the tm4c CMSIS header file.
Use the assert_failed() function for error handling in exception handlers.
Provide weak aliases for non-fault exception handlers to allow optional definitions in other parts of the program.
Custom Board Support Package (BSP):

Create a new file bsp.c for board-specific implementations.
Include tm4c_cmsis.h for relevant definitions.
Implement the assert_failed() function to handle errors and reset the system using NVIC_SystemReset.
Validation:

Ensure the project builds without errors.
Check the vector table and handlers in the disassembly view to confirm correct initialization and functionality.


Miro Samek's "Modern Embedded Systems Programming" course lesson focuses on understanding and customizing the startup code for embedded systems. The lesson involves copying a previous project, renaming it to "lesson14," and using the IAR toolset. It revisits the concept of the vector table, a critical data structure at address 0 in ROM for ARM Cortex-M processors.

The lesson explains the embedded software build process, emphasizing cross-development, where tools like the IAR Workbench compile and link code on a host machine for a target embedded system. Source files are compiled into object files, then linked into a final program, resolving addresses and dependencies. The ELF (Executable and Linkable Format) is used for object files and final images, which can be inspected with tools like ielfdumparm.exe or objdump.

The course highlights the role of the linker in resolving symbols and addresses, showcasing examples of how it handles functions and variables. The lesson underscores the importance of understanding object files and linker behavior for effective embedded programming.

Finally, the lesson guides students on how to replace the generic vector table from the IAR library with a custom one. This involves creating a new C file, startup_tm4c.c, to define the vector table array, ensuring its correct memory placement, and addressing initialization challenges. This customization demonstrates the process of tailoring startup code to specific microcontroller requirements, reducing dependency on default library code.







Here are the problems and their solutions:


Problem 1: Generic Vector Table from IAR Library
Problem: The default startup code from the IAR library includes a generic vector table that is incomplete compared to the vector table described in the datasheet for the specific TM4C microcontroller.
Solution: Replace the generic vector table with a custom one by understanding and modifying the embedded software build process.

Problem 2: Understanding the Embedded Software Build Process
Problem: Lack of understanding of the steps involved in the embedded software build process and how they relate to replacing the vector table.
Solution:

Build Process Steps: Explain the steps of the build process, including how source files are compiled into object files and then linked into the final program.
Cross-Development: Emphasize the concept of cross-development, where tools on a host machine build software for a target embedded system.
Problem 3: ELF File Format and Object Files
Problem: Object files generated by the compiler contain relocatable code and symbolic information that need to be understood to replace the vector table.
Solution:

ELF File Format: Introduce the ELF (Executable and Linkable Format) and explain its sections.
Tools for Inspection: Use tools like ielfdumparm.exe and objdump to inspect ELF files and understand their content.
Problem 4: Relocatable Code in Object Files
Problem: Understanding what "relocatable code" means and how the linker resolves addresses and references.
Solution:

Relocation by Linker: Explain how the linker resolves addresses and cross-module references by combining object files and fixing addresses.
Example Analysis: Provide examples of how instructions and addresses are fixed by the linker, such as the BL instruction and constant addresses in the code.
Problem 5: Cross-Module References and Linking Process
Problem: The process of resolving cross-module references to functions and global variables during linking.
Solution:

Exported and Imported Symbols: Describe how the linker matches imported symbols to exported symbols, using lists of undefined and defined symbols.
Library Linking Rules: Explain how libraries are linked, emphasizing the fine granularity of object files in libraries to avoid bloating the final image.
Problem 6: Custom Vector Table Placement
Problem: Placing the custom vector table correctly in memory during the startup process.
Solution:

Define Custom Vector Table: Plan to define the custom vector table in a new C file (startup_tm4c.c) and ensure it is linked before the standard IAR libraries.
Startup Code Considerations: Address the challenges of writing startup code in C for ARM Cortex-M, including careful handling of initialization sections like .data and .bss.