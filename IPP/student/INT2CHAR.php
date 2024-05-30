<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Student\Exception\OperandTypeException;
use IPP\Student\Exception\StringOperationException;

class INT2CHAR extends Instruction
{
    /**
     * Executes the 'INT2CHAR' instruction which performs translation of int value to char according to ascii value.
     *
     * @param Interpreter $interpreter An instance of the interpreter context.
     */
    public function execute(Interpreter $interpreter): void
    {
        $var = $this->getArg1();
        $symb1 = $this->getArg2();

        $operand1 = $interpreter->getSymb($symb1);
        
        if($operand1['type'] === 'var') {
            $val1 = $interpreter->readFromVar($operand1);
        } else {
            $val1 = $operand1['value'];
        }

        if (is_int($val1)) {
            if ($val1 >= 0 && $val1 <= 255) {
                $result = chr($val1);
            } else {
                throw new StringOperationException("Invalid ASCII code, value must be between 0 and 255.");
            }
        } else {
            throw new OperandTypeException("Wrong operands, unexpected operand types, expected integer.");
        }
        
        $interpreter->writeToVar($var, (string)($result), false);
    }
}
