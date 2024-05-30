<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Student\Exception\FrameAccessException;

class PUSHFRAME extends Instruction
{
    /**
     * Executes the 'PUSHFRAME' instruction which moves the temporary frame to the local frame stack.
     *
     * @param Interpreter $interpreter An instance of the interpreter context.
     * @throws FrameAccessException If there is no temporary frame to push.
     */
    public function execute(Interpreter $interpreter): void
    {
        if ($interpreter->tf !== null) {
            array_unshift($interpreter->lf, $interpreter->tf);
            $interpreter->tf = null;
        } else {
            throw new FrameAccessException("Temporary frame is empty");
        }
    }
}