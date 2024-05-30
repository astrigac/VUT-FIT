<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Student\Exception\OperandTypeException;

class LT extends Instruction
{
    /**
     * Executes the 'LT' instruction which performs a logical comparison between two operands.
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
        if ($val1 === 'nil' || $val2 === 'nil') {
            throw new OperandTypeException("Wrong operands, nil used in LT.");
        }

        if(is_bool($val1) && is_bool($val2)) {
            $valTmp1 = (int) ($val1);
            $valTmp2 = (int) ($val2);
            $result = (bool)($valTmp1 < $valTmp2);
        } elseif((is_string($val1) && is_string($val2)) || (is_int($val1) && is_int($val2))) {
            $result = (bool)($val1 < $val2);
        } else {
            throw new OperandTypeException("Wrong operands, unexpected operand types.");
        }
        
        $interpreter->writeToVar($var, $result, true);
    }
}
