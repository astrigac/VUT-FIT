# minitask 5

# Define function add_question with 3 parameters - index of the question that should be added, list of the pairs (question, list of possible answers), and the current quiz. The function selects the question with the index from the pool, adds it as the last item of the input quiz and returns the resulting quiz as the result. If no input quiz is provided when calling the add_question function, a new quiz with just the selected question is returned - the default of the third parameter is thus an empty quiz.

def add_question(index, question_pool, current_quiz=None):
    current_quiz = current_quiz if current_quiz is not None else []
    current_quiz.append(question_pool[index])
    return current_quiz

funcqpool = [('If return statement is not used inside the function, the function will return:',
          ['0',
           'None object',
           'an arbitrary integer',
           'Error! Functions in Python must have a return statement.'
          ]),
         ('Which of the following function calls can be used to invoke function definition:\n def test(a, b, c, d):?',          
          ['test(1, 2, 3, 4)',
           'test(a = 1, 2, 3, 4)',
           'test(a = 1, b = 2, c = 3, 4)',
           'test(a = 1, b = 2, c = 3, d = 4)',
           'test(1, 2, 3, d = 4)])'])
        ]
funcquiz1 = [('Which of the following keywords marks the beginning of the function block?',
         ['func',
          'define',
          'def',
          'func',
         ])]


print(add_question(0, funcqpool, funcquiz1))
print(funcquiz1)
print(add_question(0, funcqpool))
