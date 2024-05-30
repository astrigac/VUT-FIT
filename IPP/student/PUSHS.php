<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

class PUSHS extends Instruction
{
    /**
     * Executes the 'PUSHS' instruction which appends the value of symbol to the interpreter stack.
     *
     * @param Interpreter $interpreter An instance of the interpreter context.
     */
    public function execute(Interpreter $interpreter): void
    {
        $var = $this->getArg1();
        
        if($var['type'] === 'var') {
            $val = $interpreter->readFromVar($var);
        } else {
            $val = $var['value'];
        }

        $interpreter->stack[] = $val;
    }
}
