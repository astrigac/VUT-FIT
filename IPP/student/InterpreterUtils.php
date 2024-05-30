<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

/**
 * Helper class with static method useful everywhere and declaration of instruction formats
 */
class InterpreterUtils
{
    /**
     * Escapes string sequences into their actual characters (especially for backslashed sequences).
     * 
     * @param string $string The string to change.
     * @return string The new string.
     */
    public static function stringEscape(string $string): string
    {
        $index = 0;
        while (($index = strpos($string, '\\', $index)) !== false) {
            if (ctype_digit(substr($string, $index + 1, 3))) {
                // If the following three characters are digits, treat them as an ASCII code.
                $asciiCode = intval(substr($string, $index + 1, 3));
                $string = substr_replace($string, chr($asciiCode), $index, 4);
            } else {
                $index++; // Move past this backslash if not a numeric sequence
            }
        }
        return $string;
    }

    /**
     * List of all opcodes used in the interpreter.
     * 
     * @var string[] instruction names
     */
    public static array $opcodes = [
        "MOVE", "CREATEFRAME", "PUSHFRAME", "POPFRAME", "DEFVAR", "CALL", "RETURN",
        "PUSHS", "POPS", "ADD", "SUB", "MUL", "IDIV", "LT", "GT", "EQ", "AND",
        "OR", "NOT", "INT2CHAR", "STRI2INT", "READ", "WRITE", "CONCAT", "STRLEN",
        "GETCHAR", "SETCHAR", "TYPE", "LABEL", "JUMP", "JUMPIFEQ", "JUMPIFNEQ",
        "EXIT", "DPRINT", "BREAK"
    ];

    /**
     * Mapping of opcodes to their expected argument types.
     * 
     * @var array<string, array<class-string>>
     */
    public static array $expectedArgs = [
        "MOVE" => [MyVar::class, MySymb::class],
        "CREATEFRAME" => [],
        "PUSHFRAME" => [],
        "POPFRAME" => [],
        "DEFVAR" => [MyVar::class],
        "CALL" => [MyLabel::class],
        "RETURN" => [],
        "PUSHS" => [MySymb::class],
        "POPS" => [MyVar::class],
        "ADD" => [MyVar::class, MySymb::class, MySymb::class],
        "SUB" => [MyVar::class, MySymb::class, MySymb::class],
        "MUL" => [MyVar::class, MySymb::class, MySymb::class],
        "IDIV" => [MyVar::class, MySymb::class, MySymb::class],
        "LT" => [MyVar::class, MySymb::class, MySymb::class],
        "GT" => [MyVar::class, MySymb::class, MySymb::class],
        "EQ" => [MyVar::class, MySymb::class, MySymb::class],
        "AND" => [MyVar::class, MySymb::class, MySymb::class],
        "OR" => [MyVar::class, MySymb::class, MySymb::class],
        "NOT" => [MyVar::class, MySymb::class],
        "INT2CHAR" => [MyVar::class, MySymb::class],
        "STRI2INT" => [MyVar::class, MySymb::class, MySymb::class],
        "READ" => [MyVar::class, MyType::class],
        "WRITE" => [MySymb::class],
        "CONCAT" => [MyVar::class, MySymb::class, MySymb::class],
        "STRLEN" => [MyVar::class, MySymb::class],
        "GETCHAR" => [MyVar::class, MySymb::class, MySymb::class],
        "SETCHAR" => [MyVar::class, MySymb::class, MySymb::class],
        "TYPE" => [MyVar::class, MySymb::class],
        "LABEL" => [MyLabel::class],
        "JUMP" => [MyLabel::class],
        "JUMPIFEQ" => [MyLabel::class, MySymb::class, MySymb::class],
        "JUMPIFNEQ" => [MyLabel::class, MySymb::class, MySymb::class],
        "EXIT" => [MySymb::class],
        "DPRINT" => [MySymb::class],
        "BREAK" => []
    ];
}
