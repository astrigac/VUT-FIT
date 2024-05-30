<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Student\Exception\OperandTypeException;
use IPP\Student\Exception\StringOperationException;

class STRI2INT extends Instruction
{
    /**
     * Executes the 'STRI2INT' instruction which performs translation from char to integer .
     *
     * @param Interpreter $interpreter An instance of the interpreter context.
     */
    public function execute(Interpreter $interpreter): void
    {
        $var = $this->getArg1();
        $symb1 = $this->getArg2();
        $symb2 = $this->getArg3();

        $operand1 = $interpreter->getSymb($symb1);
        $operand2 = $interpreter->getSymb($symb2);
        
        if($operand1['type'] === 'var') {
            $val1 = $interpreter->readFromVar($operand1);
        } else {
            $val1 = $operand1['value'];
        }
        if($operand2['type'] === 'var') {
            $val2 = $interpreter->readFromVar($operand2);
        } else {
            $val2 = $operand2['value'];
        }

        if (is_string($val1) && is_int($val2) && $operand1['type'] !== 'nil') {
            if ($val2 < 0 || $val2 >= strlen($val1)) {
                throw new StringOperationException("Invalid index.");
            } else {
                $character = $val1[$val2];
                $asciiValue = ord($character);
                $result = $asciiValue;
            }
        } else {
            throw new OperandTypeException("Wrong operands.");
        }
        
        $interpreter->writeToVar($var, $result, true);
    }
}
