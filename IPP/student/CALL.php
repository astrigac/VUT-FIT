<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Student\Exception\SemanticErrorException; // Assuming you have a SemanticException class for semantic errors
use IPP\Student\Exception\OperandTypeException; // Assuming OperandTypeException is for type errors

class CALL extends Instruction
{
    /**
     * Executes the 'CALL' instruction which saves the current instruction position and
     * sets the instruction pointer to the index of a specified label.
     *
     * @param Interpreter $interpreter An instance of the interpreter context.
     */
    public function execute(Interpreter $interpreter): void
    {
        $label = $this->getArg1();

        if ($label['type'] === 'label') {
            $index = $interpreter->findLabelIndex($label['value']);
            if ($index !== -1) {
                // Push the current instruction index onto the call stack
                array_push($interpreter->callStack, $interpreter->currentInstruction);
                // Set the instruction pointer to the new index
                $interpreter->currentInstruction = $index;
            } else {
                throw new SemanticErrorException("Label not found: " . $label['value']);
            }
        } else {
            throw new OperandTypeException("Expected a label in CALL");
        }
    }
}
