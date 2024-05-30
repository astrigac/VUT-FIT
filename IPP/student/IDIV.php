<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Student\Exception\OperandTypeException;
use IPP\Student\Exception\OperandValueException;

class IDIV extends Instruction
{
    /**
     * Executes the 'IDIV' instruction which divides two integers.
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

        $val1 = $operand1['value'];
        $val2 = $operand2['value'];
        
        if (!$this->isNumericType($operand1) || !$this->isNumericType($operand2)) {
            throw new OperandTypeException("Wrong operands, expected numeric types.");
        }

        // Check for division by zero
        if ($val2 == 0) {
            throw new OperandValueException("Division by zero");
        }

        // Perform integer division
        $result = intdiv($val1, $val2);
        $interpreter->writeToVar($var, $result, true);
    }

    /**
     * Check if the operand type is 'int' or 'const' with a numeric value.
     *
     * @param array{type: string, value: mixed} $operand The operand array containing type and value.
     * @return bool True if the operand is of a numeric type.
     */
    private function isNumericType(array $operand): bool
    {
        return ($operand['type'] === 'int') || 
               ($operand['type'] === 'const' && is_numeric($operand['value']));
    }
}