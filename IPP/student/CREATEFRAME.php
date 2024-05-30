<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;


class CREATEFRAME extends Instruction
{
    /**
     * Executes the 'CREATEFRAME' instruction which initializes a new temporary frame.
     *
     * @param Interpreter $interpreter An instance of the interpreter context.
     */
    public function execute(Interpreter $interpreter): void
    {
        $interpreter->tf = [];
    }
}