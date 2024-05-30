<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Student\Exception\OperandTypeException;

class NOTI extends Instruction
{
    /**
     * Executes the 'NOT' instruction which performs a logical NOT operation on boolean operand.
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
        
        // Ensure both operands are boolean
        if (!is_bool($val1)) {
            throw new OperandTypeException("Wrong operands, expected boolean types.");
        }
        
        $result = (bool)(!$val1);
        $interpreter->writeToVar($var, $result, true);
    }
}
