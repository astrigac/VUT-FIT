<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

class MOVE extends Instruction
{
    /**
     * Executes the 'MOVE' instruction which assigns a value to a variable.
     *
     * @param Interpreter $interpreter An instance of the interpreter context.
     * @return void
     */
    public function execute(Interpreter $interpreter): void
    {
        $var = $this->getArg1();  
        $symb = $this->getArg2(); 

        $symbtmp = $interpreter->getSymb($symb); 
        $val = $symbtmp['value'];

        $interpreter->writeToVar($var, $val, true);
    }
}