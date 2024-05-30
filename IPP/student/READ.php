<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Student\Exception\OperandTypeException;

class READ extends Instruction
{
    /**
     * Executes the 'READ' instruction which reads a value from input based on the specified type and stores it in a variable.
     *
     * @param Interpreter $interpreter An instance of the interpreter context.
     */
    public function execute(Interpreter $interpreter): void
    {
        $var = $this->getArg1();
        $typeSymbol = $this->getArg2();

        // Determine the type of the value to read based on typeSymbol's provided type
        $typeInfo = $interpreter->getSymb($typeSymbol);
        if ($typeInfo === null || !isset($typeInfo['type'], $typeInfo['value']) || $typeInfo['type'] !== 'type') {
            throw new OperandTypeException("Wrong argument type for READ operation");
        }

        $valType = $typeInfo['value'];

        // Read value based on the specified type
        $inputReader = $interpreter->getInputReader();
        $val = null;

        switch ($valType) {
            case 'int':
                $val = $inputReader->readInt();
                if ($val === null) {
                    $val = null;
                }
                break;
            case 'bool':
                $read = $inputReader->readString();
                $read = strtolower(trim($read));
                if ($read === 'true' || $read === 'false') {
                    $val = $read === 'true';
                } else {
                    $val = null;
                }
                break;
            case 'string':
                $read = $inputReader->readString();
                $val = trim($read);
                break;
            default:
                throw new OperandTypeException("Unsupported type for READ operation");
        }
        
        $interpreter->writeToVar($var, $val, true);
    }
}