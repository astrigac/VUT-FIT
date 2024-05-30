<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Student\Exception\OperandTypeException;
use IPP\Student\Exception\OperandValueException;

class EXITI extends Instruction
{
    /**
     * Executes the 'EXIT' instruction which terminates the program with a given exit code.
     * 
     * @param Interpreter $interpreter An instance of the interpreter context.
     */
    public function execute(Interpreter $interpreter): void
    {
        $symb1 = $this->getArg1();

        $operand1 = $interpreter->getSymb($symb1);

        $val1 = $operand1['value'];

        if ($operand1['type'] === 'int') {
            if ($val1 < 0 || $val1 > 9) {
                throw new OperandValueException("Wrong exit code");
            } else {
                exit($val1);
            }
        } else {
            throw new OperandTypeException("Wrong operands");
        }
    }
}