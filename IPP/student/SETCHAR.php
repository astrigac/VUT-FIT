<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Student\Exception\OperandTypeException;
use IPP\Student\Exception\StringOperationException;

class SETCHAR extends Instruction
{
    /**
     * Executes the 'SETCHAR' instruction which sets one character on demanded index in particular string.
     *
     * @param Interpreter $interpreter An instance of the interpreter context.
     */
    public function execute(Interpreter $interpreter): void
    {
        $var = $this->getArg1();
        $symb1 = $this->getArg2();
        $symb2 = $this->getArg3();
        
        $currentString = $interpreter->readFromVar($var);

        $operand1 = $interpreter->getSymb($symb1);
        $operand2 = $interpreter->getSymb($symb2);
        
        $index = ($operand1['type'] === 'var') ? $interpreter->readFromVar($operand1) : $operand1['value'];
        $newChar = ($operand2['type'] === 'var') ? $interpreter->readFromVar($operand2) : $operand2['value'];

        if (!is_string($currentString) || !is_int($index) || !is_string($newChar) || $currentString === 'nil' || $newChar === 'nil') {
            throw new OperandTypeException("Wrong operands or operand types.");
        }
        if ($index < 0 || $index >= strlen($currentString) || strlen($newChar) === 0) {
            throw new StringOperationException("Wrong index or empty new character.");
        }

        // Set the character at the specified index
        $currentString[$index] = $newChar[0];
        $interpreter->writeToVar($var, $currentString, false);
    }
}
