# Introduction

- Problem: How to run different operating systems on a machine at the same time?
- Solution: IBM introduced another level of indirection in the form of a **virtual machine monitor (VMM)** (also called a **hypervisor**).

# Motivation

- Enable an administrator to **consolidate** multiple OSes onto fewer hardware platforms, thus lower costs and ease administration.
- Many users wish to run 1 OS but still have access to native applications on another platform.
- Developers write code on 1 platform, but they often want to debug and test it on many different platforms.

# Virtualizing the CPU

- To run a **virtual machine** _(an OS and its applications)_ on top of a virtual machine monitor, the basic technique that is used is **limited direct execution**.
- When we wish to boot a new OS on top of the VMM, we jump to the address of the first instruction and let the OS begin running.
- A virtual machine monitor must perform a **machine switch** between running virtual machines:
  - Save the entire machine state of 1 OS: registers, PC, privileged hardware state.
  - Restore the machine state of the to-be-run VM.
  - Jump to the PC of the to-be-run VM -> complete the switch.
- When a running application or OS tries to perform a **privileged operation**, the VMM must intercept the attempt and retain control of the machine.
- System call without virtualization:

```
Process                 Hardware                    OS
----------------------------------------------------------------------
1. Execute instructions
2. System call:
Trap to OS
                        3. Switch to kernel mode;
                        Jump to trap handler
                                                    4. Handle system call;
                                                    Return from trap
                        5. Switch to user mode;
                        Return to user code
6. Resume execution
(@PC after trap)
```

- System call without virtualization (leave out hardware operations):

```
Process                 OS
----------------------------------------------------------------------
1. System call:
Trap to OS
                        2. OS trap handler:
                        Decode trap and execute syscall routine;
                        Return from trap when done.
3. Resume execution:
(@PC after trap)
```

- System call with virtualization:

```
Process                 OS                      VMM
----------------------------------------------------------------------
1. System call:
Trap to OS
                                                2. Process trapped:
                                                Call OS trap handler
                                                (at reduced privilege)
                        3. OS trap handler:
                        - Decode trap and
                        execute syscall;
                        - When done: issue
                        return-from-trap
                                                4. OS tried to return from trap:
                                                Do real return from trap
5. Resume execution:
(@PC after trap)
```

- Because of the extra jumping around, virtualization might slow down the system calls and hurt performance.
- What mode should the OS run in? In can't run in kernel mode, because then it would have unrestricted access to the hardware.
  - In the Disco work, they use the **supervisor mode** provided by the MIPS hardware: One still doesn't have access to privileged instruction, but can access a little more memory than when in user mode. The OS can use this memory for its data structures.
  - On hardware that doesn't have such a mode, one has to run the OS in user mode and use memory protection _(page tables and TLBs)_ to protect OS data structures _(remove the ability to read and write OS data structures when switching back to running application)_.

## System call in a system without virtualization

- A system call is achieved with a special instruction:
  - On MIPS: `trap` .
  - On x86: `int` (interrupt).
- Place arguments and a number onto the stack before calling the instruction.
- When a trap instruction is executed:
  - Transfer control to a **trap handler** within the OS. The OS establishes the address of such routine with the hardware (a privileged operation) on boot time.
  - The hardware also changes the mode of the processor from **user mode** to **kernel mode**.

## System call on virtualized platforms

- When an application running on an OS wishes to make a system call, it does the exact same thing: execute the trap instruction with the arguments placed on the stack or registers.
- The VMM doesn't know **how** to handle the call. But it knows **where** the OS's trap handler is.
  - When the OS booted up, it tries to install the trap handler.
  - It's a privileged operation, so it trapped into the VMM.
  - The VMM then recorded necessary information _(where this OS's trap handler is in memory)_.
- The VMM jumps into the OS's trap handler and let the OS handle the system call.
- When the OS is finished, it execute a privileged instruction to return from trap (`rett` on MIPS, `iret` on x86).
- The VMM performs a real return from trap, returns control to the user and puts the machine back in user mode.

# Virtualizing memory

- With VMM, "physical" memory of OS is a virtualization on top of **machine memory** _(the real physical memory of the system)_.
- An extra layer of indirection:
  - Each OS maps virtual-to-physical addresses via its per-process page tables.
  - The VMM maps the physical addresses to the underlying machine addresses via its per-OS page tables.

## Address translation without virtualization

- Processes generate virtual addresses, which consist of 2 parts: the virtual page number, and the offset.
- The OS, with help from hardware TLB, translates the virtual page number into a valid physical frame number, then produces the physical address.
- When a TLB miss occur, the OS must get involved to service the miss _(on a system with software-managed TLB)_.
  - A TLB miss causes a trap into the OS, which handles the fault by looking up the VPN in the page table and installing the translation in the TLB.

- Flow: TLB miss without virtualization

```
Process                         OS
----------------------------------------------------------------------
1. Load from memory
TLB miss: Trap
                                2. OS TLB miss handler:
                                - Extract VPN from VA
                                - Do page table lookup
                                - If present and valid,
                                  get PFN and update TLB
                                - Return from trap
3. Resume execution
(@PC of trapping instruction)
Instruction is retried,
results in TLB hit
```

## Address translation with virtualization

- When a TLB miss occurs, it is not the OS TLB miss handler that runs. It is the VMM miss handler.
- The VMM TLB handler doesn't know how to handle the TLB miss, so it immediately jumps into the OS TLB miss handler.
  - The VMM knows the location of the handler because the OS tried to install its own trap handler during boot.
- The OS TLB miss handler then runs, does a page table lookup for the VPN, and tries to install the VPN to PFN mapping in the TLB.
- However, that's a privileged operation and causes another trap into the VMM. Instead of installing the OS's VPN to PFN mapping, the VMM installs its VPN to MFN mapping.
- The system eventually get back to the user-level code, which retries the instruction, and results in a TLB hit.

- Flow: TLB miss with virtualization

```
Process                 OS                          VMM
----------------------------------------------------------------------
1. Load from memory
TLB miss: Trap
                                                2. VMM TLB miss handler:
                                                Call into OS TLB miss handler
                                                (reducing privilege)
                    3. OS TLB miss handler:
                    Extract VPN from VA;
                    Do page table lookup;
                    If present and valid,
                    get PFN, update TLB
                                                4. Trap handler:
                                                OS is trying to install VPN-PFN mapping;
                                                Update TLB instead with VPN-MFN mapping;
                                                Jump back to OS
                                                (reducing privilege)
                    5. Return from trap
                                                6. Trap handler:
                                                Actual return from trap.
7. Resume execution
(@PC of trapping instruction)
Instruction is retried,
results in TLB hit
```

- The VMM must track the physical-to-machine mappings for each virtual machine. These per-machine page tables are consulted by the VMM TLB miss handler.
- TLB miss on a virtualized system is expensive -> The designers of Disco added a VMM-level "software TLB":
  - The VMM records every virtual-to-physical that it sees the OS try to install.
  - On a TLB miss, the VMM first consults its software TLB to see if it has seen this virtual-to-physical mapping before.

# The information gap

- The OS doesn't know too much about what application programs really want, and thus often must make general policies that hopefully work for all programs.
- The VMM doesn't know too much about what the OS is doing or wanting -> This lack of knowledge (called the **information gap** between the VMM and the OS) can lead to various inefficiencies.
- Example 1: An OS, when it has nothing to do, will sometimes go into an **idle loop** just spinning and waiting for the next interrupt to occur.
  - It makes sense to spin if the OS is in charge of the entire machine and knows there is nothing else to run.
  - However, it would be useful for the VMM to know that 1 OS is idle so it can give more CPU time to the other OSes.
- Example 2: demand zeroing of pages
  - Most operating systems zero a physical frame before mapping it into a process's address space. The reason is **security** _(prevent information leak across processes)_.
  - For the same reason, the VMM must zero pages that it gives to each OS, thus a page will be zeroed twice.
  - Solution: The authors of Disco changed the OS to not zero pages that it knew had been zeroed by the underlying VMM.
- The VMM can use inference (a form of **implicit information**) to overcome the problem:
  - Example: A VMM can detect the idle loop by noticing that the OS switched to low-power mode.
- A different approach, seen in **para-virtualized** systems, requires the OS to be changed.
  - This is harder to deploy but can be quite effective.

# Other virtualization topics

- I/O
- ...
