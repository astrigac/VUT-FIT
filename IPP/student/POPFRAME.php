<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Student\Exception\FrameAccessException;

class POPFRAME extends Instruction
{
    /**
     * Executes the 'POPFRAME' instruction which pops the top local frame to become the new temporary frame.
     *
     * @param Interpreter $interpreter An instance of the interpreter context.
     */
    public function execute(Interpreter $interpreter): void
    {
        if (count($interpreter->lf) > 0) {
            $interpreter->tf = array_shift($interpreter->lf);
        } else {
            throw new FrameAccessException("Local frame is empty");
        }
    }
}