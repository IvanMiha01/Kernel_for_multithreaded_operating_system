.extern handleSupervisorTrap
.global supervisorTrap
.align 4

supervisorTrap:
    addi sp, sp, -256
    .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr

    csrw sscratch, sp #save sp in sscratch

    call handleSupervisorTrap;

    .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31 #bez 10
    ld x\index, \index * 8(sp)
    .endr
    addi sp, sp, 256

    sret