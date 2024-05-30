# minitask 2
# change the last du to DU
import re
pattern = re.compile(r'(du)(?!.*du.*)')
text = ['du du du',
        'du po ledu',
        'dopředu du',
        'i dozadu du',
        'dudu dupl',
        'Rammstein du hast']

# expected output:
# du du DU
# du po leDU
# dopředu DU
# i dozadu DU
# dudu DUpl
# Rammstein DU hast

for row in text:
  print(re.sub(pattern, 'DU', row))

