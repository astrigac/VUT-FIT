<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Student\Exception\SemanticErrorException;
use IPP\Student\Exception\OperandTypeException; 

class JUMPIFNEQ extends Instruction
{
    /**
     * Executes the 'JUMPIFNEQ' instruction which sets the instruction pointer to the index of a specified label if two operands are not equal.
     *
     * @param Interpreter $interpreter An instance of the interpreter context.
     */
    public function execute(Interpreter $interpreter): void
    {
        $label = $this->getArg1();
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
        
        if($val1 === 'nil' || $val2 === 'nil') {
            if($val1 !== 'nil' && $val2 === 'nil') {
                $index = $interpreter->findLabelIndex($label['value']);
                    if ($index !== -1) {
                        $interpreter->currentInstruction = $index;
                    } else {
                        throw new SemanticErrorException("Label not found: " . $label['value']);
                    }
            } else {
                throw new OperandTypeException("Nil in comparison");
            }
        }

        if ($label['type'] === 'label') {
            // Check if both operands are of the same type or are null (considering PHP's loose type comparison)
            if (gettype($val1) === gettype($val2) || is_null($val1) || is_null($val2)) {
                if ($val1 !== $val2) {
                    $index = $interpreter->findLabelIndex($label['value']);
                    if ($index !== -1) {
                        $interpreter->currentInstruction = $index;
                    } else {
                        throw new SemanticErrorException("Label not found: " . $label['value']);
                    }
                }
            } else {
                throw new OperandTypeException("Operands must be of the same type for equality check");
            }
        } else {
            throw new OperandTypeException("Expected a Label, got another type");
        }
    }
}
