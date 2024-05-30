<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Student\Exception\OperandTypeException;
use IPP\Student\Exception\StringOperationException;

class DPRINT extends Instruction
{
    /**
     *
     * @param Interpreter $interpreter An instance of the interpreter context.
     */
    public function execute(Interpreter $interpreter): void
    {
        $symb = $this->getArg1();

        $operand = $interpreter->getSymb($symb);

        $val = $operand['value'];

        if ($val !== null && $val !== '') {
            if ($operand['type'] === 'nil' && $operand['value'] === 'nil') {
                $val = '';
            } elseif (($operand['type'] === 'const' && is_bool($val) || $operand['type'] === 'bool')) {
                $val = $val ? 'true' : 'false';
            }
            $interpreter->getStdErrWriter()->writeString($val);
        }
        
    }
}
