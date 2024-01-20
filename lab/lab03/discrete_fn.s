.globl f # this allows other files to find the function f

# f takes in two arguments:
# a0 is the value we want to evaluate f at
# a1 is the address of the "output" array (defined above).
# The return value should be stored in a0
f:

    # This is how you return from a function. You'll learn more about this later.
    # This should be the last line in your program.
    jr ra  