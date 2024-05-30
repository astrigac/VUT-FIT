<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Student\Exception\OperandTypeException;

class TYPE extends Instruction
{
    /**
     * Executes the 'TYPE' instruction which determines the type of a symbol and exports it to the variable.
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

        if($val1 === 'nil') {
            $result = 'nil';
        } else if(is_int($val1)) {
            $result = 'int';
        } else if(is_string($val1)) {
            $result = 'string';
        } else if(is_bool($val1)) {
            $result = 'bool';
        } else if(!$interpreter->isVarDefined($operand1)) {
            $result = '';   
        } else {
            throw new OperandTypeException("Wrong operands, unexpected operand types.");
        }
        
        $interpreter->writeToVar($var, $result, false);
    }
}
