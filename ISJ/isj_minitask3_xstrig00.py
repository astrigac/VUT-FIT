# minitask 3
# change the code lines following the comment lines below to correspond to
# the comments

def complex_func(x,y):
    return 2 ** x ** y
def first(x):
    return 2 ** x - 1
def second(x):
    return 3 ** x - 2
def third(x):
    return 5 ** x - 3

# list of functions first, second, third, complex_func
funlist = [first, second, third, complex_func]
a = 2
b = 3
# print the result of the last function in the list with parameters a and b
# consider that the funlist could change in the meantime so _the last_ does
# not necessarily mean the fourth one, and it is not necessarily complex_func
print(funlist[-1](a, b))
# for the slice starting with the second function and including the third one
for func in funlist[1:3]:
    # print the result of the function with parameter a
    print(func(a))

# expected output:
# 256
# 7
# 22
