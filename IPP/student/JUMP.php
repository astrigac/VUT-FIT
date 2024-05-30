<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Student\Exception\SemanticErrorException;
use IPP\Student\Exception\OperandTypeException;

class JUMP extends Instruction
{
    /**
     * Executes the 'JUMP' instruction which sets the instruction pointer to the index of a specified label.
     *
     * @param Interpreter $interpreter An instance of the interpreter context.
     */
    public function execute(Interpreter $interpreter): void
    {
        $label = $this->getArg1();

        if ($label['type'] === 'label') {
            $index = $interpreter->findLabelIndex($label['value']);
            if ($index !== -1) {
                $interpreter->currentInstruction = $index;
            } else {
                throw new SemanticErrorException("Label not found");
            }
        } else {
            throw new OperandTypeException("Expected a Label, got another type");
        }
    }
}
