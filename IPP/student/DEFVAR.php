<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

class DEFVAR extends Instruction
{
    /**
     * Executes the 'DEFVAR' instruction which defines a new variable.
     *
     * @param Interpreter $interpreter An instance of the interpreter context.
     * @return void
     */
    public function execute(Interpreter $interpreter): void
    {
        $var = $this->getArg1();
        $interpreter->defineVar($var);
    }
}