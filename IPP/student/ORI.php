<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Student\Exception\OperandTypeException;

class ORI extends Instruction
{
    /**
     * Executes the 'OR' instruction which performs a logical OR operation on two boolean operands.
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
        
        // Ensure both operands are boolean
        if (!is_bool($val1) || !is_bool($val2)) {
            throw new OperandTypeException("Wrong operands, expected boolean types.");
        }
        
        $result = $val1 || $val2;
        $interpreter->writeToVar($var, $result, true);
    }
}
