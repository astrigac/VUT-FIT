<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Student\Exception\ValueErrorException;

class POPS extends Instruction
{
    /**
     * Executes the 'POPS' instruction which takes the value from the top of the stack and stores it into var.
     *
     * @param Interpreter $interpreter An instance of the interpreter context.
     */
    public function execute(Interpreter $interpreter): void
    {
        $var = $this->getArg1();
        
        if (count($interpreter->stack) > 0) {
            $result = array_pop($interpreter->stack);
        } else {
            throw new ValueErrorException("Empty stack");
        }

        $interpreter->writeToVar($var, $result, true);
    }
}
