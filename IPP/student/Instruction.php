<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

class Instruction {
    private string $opcode;
    /**
     * @var array<mixed> Argument storage
     */
    private array $args;
    private Interpreter $interpreter;

    /**
     * Constructs an Instruction object.
     * 
     * @param string $opcode The opcode of the instruction.
     * @param mixed[] $args Arguments of the instruction.
     * @param Interpreter $interpreter An instance of the interpreter context.
     */
    public function __construct(string $opcode, array $args, Interpreter $interpreter) {
        $this->opcode = $opcode;
        $this->args = $args;
        $this->interpreter = $interpreter;
    }

    /**
     * Get the opcode of the instruction.
     *
     * @return string The opcode.
     */
    public function getOpcode(): string {
        return $this->opcode;
    }

    /**
     * Get the interpreter context.
     *
     * @return Interpreter The interpreter context.
     */
    public function getInterpreter(): Interpreter {
        return $this->interpreter;
    }

    /**
     * Returns the first argument of the instruction.
     * 
     * @return mixed The first argument.
     */
    public function getArg1() {
        return $this->args['arg1'] ?? null; // Safely return the first argument or null if not set.
    }

    /**
     * Returns the second argument of the instruction.
     * 
     * @return mixed The second argument.
     */
    public function getArg2() {
        return $this->args['arg2'] ?? null; // Safely return the second argument or null if not set.
    }

    /**
     * Returns the third argument of the instruction.
     * 
     * @return mixed The third argument.
     */
    public function getArg3() {
        return $this->args['arg3'] ?? null; // Safely return the third argument or null if not set.
    }

    /**
     * Executes the instruction within the context of the given interpreter.
     * 
     * @param Interpreter $interpreter The interpreter context in which the instruction is executed.
     */
    public function execute(Interpreter $interpreter): void {
        // Implementation should be specific to the type of instruction.
    }
}