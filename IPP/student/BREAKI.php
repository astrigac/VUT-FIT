<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

class BREAKI extends Instruction
{
    /**
     *
     * @param Interpreter $interpreter An instance of the interpreter context.
     */
    public function execute(Interpreter $interpreter): void
    {
        $interpreter->getStdErrWriter()->writeString("BREAK: \n Called from: " . $interpreter->instructions[$interpreter->currentInstruction]['opcode'] . " on the index " . $interpreter->currentInstruction);
        ob_start();
        var_dump($interpreter->gf);
        $dumpOutput = ob_get_clean();
        $interpreter->getStdErrWriter()->writeString("\nGF contains: \n" . $dumpOutput);
        ob_start();
        var_dump($interpreter->tf);
        $dumpOutput = ob_get_clean();
        $interpreter->getStdErrWriter()->writeString("\nTF contains: \n" . $dumpOutput);
        ob_start();
        var_dump($interpreter->lf);
        $dumpOutput = ob_get_clean();
        $interpreter->getStdErrWriter()->writeString("\nLF contains: \n" . $dumpOutput);
    }
}
