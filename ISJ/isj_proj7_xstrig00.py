#!/usr/bin/env python3
import collections

def log_and_count(key=None, counts=None):   
    
    if counts is None:
        raise TypeError('Missing argument: \'counts\'')  
     
    def outer(func):      
        # Use the provided key or default to the function's name
        counter_key = key if key is not None else func.__name__
        
        def inner(*args, **kwargs):
            # Prepare the args string: special case for single argument to add comma
            if len(args) == 1:
                arg_str = repr(args[0]) + ','  # Trailing comma for a single argument
            else:
                arg_str = ', '.join(repr(arg) for arg in args)
            
            # Prepare the kwargs string
            kwarg_str = ', '.join(f"'{k}':{v!r}" for k, v in kwargs.items())
            
            # Log the function call as specified
            print(f"called {func.__name__} with ({arg_str}) and {{{kwarg_str}}}")
            
            # Count the function call in the provided Counter
            counts[counter_key] += 1
            
            ret_val = func(*args, **kwargs)
            return ret_val
        
        return inner
    return outer

if __name__ == '__main__':
    # Usage example

    # Define a counter to track function calls
    my_counter = collections.Counter()

    @log_and_count(key='basic functions', counts=my_counter)
    def f1(a, b=2):
        return a ** b

    @log_and_count(key='basic functions', counts=my_counter)
    def f2(a, b=3):
        return a ** 2 + b

    @log_and_count(counts=my_counter)
    def f3(a, b=5):
        return a ** 3 - b

    # Test calls
    f1(2)
    f2(2, b=4)
    f1(a=2, b=4)
    f2(4)
    f2(5)
    f3(5)
    f3(5, 4)

    # Output the counter
    print(my_counter)
