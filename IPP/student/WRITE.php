<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

class WRITE extends Instruction
{
    /**
     * Executes the 'WRITE' instruction which outputs the value of a symbol.
     *
     * @param Interpreter $interpreter An instance of the interpreter context.
     * @return void
     */
    public function execute(Interpreter $interpreter): void
    {
        $symb = $this->getArg1();  // Retrieve the first argument which should be a variable or constant

        $operand = $interpreter->getSymb($symb);  // Get the operand's value

        $val = $operand['value'];  // Assuming $operand1 is an array with a 'value' key

        if ($val !== null && $val !== '') {
            if ($operand['type'] === 'nil' && $operand['value'] === 'nil') {
                $val = '';
            } elseif (($operand['type'] === 'const' && is_bool($val) || $operand['type'] === 'bool')) {
                $val = $val ? 'true' : 'false';
            }
            $interpreter->getStdOutWriter()->writeString($val);
        }
    }
}