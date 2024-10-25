.align 2
.globl  main

.text
main:
    addi $sp, $sp, -16
    sw $ra, 12($sp)
    sw $s0, 8($sp)
    sw $s1, 4($sp)
    sw $s2, 0($sp)
    li $t4, 0           # as indicator that blue devil was seen already
    li $t5, 0           # for reference to root

read_in_again:
                            # PART 1: store patient node
    li $v0, 4                   # prompt patient name
    la $a0, str_prompt1
    syscall
    li $v0, 8                   # read patient name from console
    la $a0, name1                # load address of name to $a0
    li $a1, 16                   # 16 bytes in $a1 to store name1
    syscall

    li $a0, 24              # $a0 = tell sys that 24 bytes dynamically allocated
    li $v0, 9         
    syscall                 # $v0 = address of first byte of allocated block

    move $s5, $v0           # store address of name1
    sw $0, 16($s5)          # set this node's left and right fields to null
    sw $0, 20($s5)

    move $a0, $s5           # address of dyn.memory = destination for str to copy
    la $a1, name1          # address of name1 = source 
    jal strcpy

    la $a0, name1
    la $a1, done
    jal strcmp
    beqz $v0, end_of_inputs    # name1 = "DONE", then exit program but if no entry then need to handle $a0 not 0 for dfs

second_person:
    li $v0, 4                        # prompt for unfector
    la $a0, str_prompt2
    syscall
    li $v0, 8                # read from console
    la $a0, name2            # load the address of name on to $a0
    li $a1, 16              # 16 bytes in $a1 to store name1
    syscall

consider_skipping_root_setup:
    bnez $t4, skip_root_setup       # don't bother with str comparison ---(*)
    la $a0, name2               # $t4 is 0 -> hasn't been reset yet
    la $a1, bluedevil       
    jal strcmp
    bnez $v0, skip_root_setup       # not bluedevil

    li $t4, 1               # reset $t4 = 0 to 1 to satisfy condition in (*)
    li $a0, 24              # $a0 = ask bytes dynamically allocated
    li $v0, 9         
    syscall                 # $v0 = address of first byte of allocated block
    move $t5, $v0           # dedicated register to store bluedevil

    move $a0, $t5           # $v0 = address of 1st byte of memory = destination for str to be copied
    la $a1, name2           # $a1 = address of name2 = source of str
    jal strcpy

    sw $s5, 16($t5)          # store address of name1 
    sw $0, 20($t5)          # store 32 bits of 0 in right field (2 person prog. only)

    j read_in_again

skip_root_setup:
    move $a0, $t5           # root in $t5
    la $a1, name2           # compare name2 with names branching from root
    move $s6, $0            # initialize $s6 to $0 -> used as a check in traverse

    jal traverse_and_find     # $v0 = add. of infector
    move $s0, $v0

check_right_left:
    lw $a0, 16($s0)             # current left of infector in $a0 (as arg)
    bne $a0, $0, left_not_null

    sw $s5, 16($s0)          # else, left_is_null; simply store name1 in left
    # sw $0, 20($s0)          # store 0 in right - redundant; this would have already been done

    la $a0, name1
    jal strclr
    la $a0, name2
    jal strclr
    j read_in_again

left_not_null:
    move $a1, $s5           # $a0 is current left, compare with name1 to be inserted
    jal strcmp             # does left need to be swapped to right?
    bgtz $v0, needs_swap 
    sw $s5, 20($s0)         # doesn't_need_swap, so just store name1 in right 

    la $a0, name1
    jal strclr
    la $a0, name2
    jal strclr

    j read_in_again

needs_swap:
    lw $s2, 16($s0)             # temporary storage for left
    sw $s5, 16($s0)             # name1 stored in left
    sw $s2, 20($s0)             # temp/old left now stored in right

    la $a0, name1
    jal strclr
    la $a0, name2
    jal strclr
    j read_in_again

end_of_inputs:
    move $a0, $t5
    jal dfs_and_print

just_ra:
    lw $s2, 0($sp)
    lw $s1, 4($sp)
    lw $s0, 8($sp)
    lw $ra, 12($sp)
    addu $sp, $sp, 16
    jr $ra

traverse_and_find:               # $a0 = address(root), $a1 = address(name2)
    subu $sp, $sp, 20
    sw $s4, 16($sp)
    sw $s3, 12($sp)
    sw $ra, 8($sp)
    sw $s2, 4($sp)
    sw $s1, 0($sp)
    beqz $a0, end_traverse         # this root == null
    move $s3, $a0                   # save root in $s3
    move $s4, $a1                   # save name2 in $s4
    jal strcmp                     # else compare root name with name2 in $a1
    beqz $v0, found_infector       # root.name == name2 in $a1

    lw $s1, 16($s3)                    # left of root
    lw $s2, 20($s3)                   # right of root
    
    beqz $s1, right_traverse             # left is null
    move $a0, $s1
    move $a1, $s4
    jal traverse_and_find               # check left subtree
    beqz $s6, right_traverse     # $s6 was not reset; no infector on left; check right
    j end_traverse      # otherwise, $s6 == 1; with $v0 updated to $a0 at some node in left subtree
    
right_traverse:
    beqz $s2, end_traverse
    move $a0, $s2             # change root to right, and traverse again
    move $a1, $s4
    jal traverse_and_find
    bnez $s6, end_traverse          # found infector on right

found_infector:                # otherwise, print found infector
    li $s6, 1                   # reset $s6 (can only happen once)
    move $v0, $s3               # save address of infector (saved in $s4) in $v0
    
    lw $ra, 8($sp)
    lw $s1, 0($sp)
    lw $s2, 4($sp)
    lw $s3, 12($sp)
    lw $s4, 16($sp)
    addu $sp, $sp, 20
    jr $ra
end_traverse:
    lw $ra, 8($sp)
    lw $s1, 0($sp)
    lw $s2, 4($sp)
    lw $s3, 12($sp)
    lw $s4, 16($sp)
    addu $sp, $sp, 20
    jr $ra

dfs_and_print:
    subu $sp, $sp, 12
    sw $ra, 0($sp)
    sw $s1, 4($sp)
    sw $s2, 8($sp)
    beqz $a0, end_of_dfs           # root null so return to caller
    li $v0, 4                       # else print $a0 =  root       
    syscall

    lw $s1, 16($a0)                    # left of root
    lw $s2, 20($a0)                   # right of root

    beqz $s1, skip_to_right             # left is null
    move $a0, $s1                       # root = root.left
    jal dfs_and_print                    # left was not null - more to print 

skip_to_right:
    beqz $s2, end_of_dfs             # right is null
    move $a0, $s2                    # root = root.right
    jal dfs_and_print                
                                     # nothing more to print from right
end_of_dfs:
    lw $ra, 0($sp)
    lw $s1, 4($sp)
    lw $s2, 8($sp)
    addu $sp, $sp, 12
    jr $ra

strcmp:                 # $v0 = result of strcmp($a0, $a1)
	lb $t0, 0($a0)
	lb $t1, 0($a1)

	bne $t0, $t1, done_with_strcmp_loop
	addi $a0, $a0, 1
	addi $a1, $a1, 1
	bnez $t0, strcmp
	li $v0, 0
	jr $ra
		
	done_with_strcmp_loop:
	sub $v0, $t0, $t1
	jr $ra

strcpy:
	lb $t0, 0($a1)
	beq $t0, $zero, done_copying
	sb $t0, 0($a0)
	addi $a0, $a0, 1
	addi $a1, $a1, 1
	j strcpy

	done_copying:
	jr $ra

strclr:                        # $a0 = string buffer to be zeroed out
	lb $t0, 0($a0)
	beq $t0, $zero, done_clearing
	sb $zero, 0($a0)
	addi $a0, $a0, 1
	j strclr

	done_clearing:
	jr $ra


.align 2
.data
str_prompt1: .asciiz "Please enter the patient's name:"
.align 2
str_prompt2: .asciiz "Please enter the name of infector:"

skipped_msg: .asciiz "skipped root set up\n"
dfs_exit_msg: .asciiz "dfs exited correctly\n"
success_reading_msg: .asciiz "read 2 names and found infector and swapped correctly!\n"
bluedevil: .asciiz "bluedevil\n"
end_of_inputs_msg: .asciiz "Finished reading inputs\n"
done: .asciiz "done\n"
name1: .space 16
name2: .space 16