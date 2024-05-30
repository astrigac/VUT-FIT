<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Student\Exception\ValueErrorException;

class RETURNI extends Instruction
{
    /**
     * Executes the 'RETURN' instruction which sets the instruction pointer back to the position stored in the call stack.
     *
     * @param Interpreter $interpreter An instance of the interpreter context.
     */
    public function execute(Interpreter $interpreter): void
    {
        // Check if the call stack has any entries
        if (count($interpreter->callStack) > 0) {
            $index = array_pop($interpreter->callStack);
            $interpreter->currentInstruction = $index;
        } else {
            throw new ValueErrorException("Empty callstack");
        }
    }
}