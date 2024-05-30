<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Student\Exception\OperandTypeException;

class STRLEN extends Instruction
{
    /**
     * Executes the 'STRLEN' instruction which returns the length of a string argument.
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

        if(is_string($val1) && $val1 !== 'nil') {
            $result = (int)(strlen($val1));
        } else {
            throw new OperandTypeException("Wrong operands, unexpected operand types.");
        }
        
        $interpreter->writeToVar($var, $result, true);
    }
}
