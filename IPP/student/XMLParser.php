<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use DOMDocument;
use DOMXPath;
use Exception;
use IPP\Student\Exception\InvalidXmlException;
use IPP\Student\Exception\InvalidSourceStructureException;

class XMLParser {
    /**
     * @var array<mixed> Instruction storage
     */
    private array $instructions = [];
    private DOMDocument $dom;

    /**
     * Constructor for XMLParser.
     * 
     * @param DOMDocument $domDocument The file path to the XML input file.
     */
    public function __construct(DOMDocument $domDocument) {
        $this->dom = $domDocument;
    }

    /**
     * Parse the XML file.
     */
    public function parseXML(): void {
        $xpath = new DOMXPath($this->dom);
        $programNode = $xpath->query("/program")->item(0);

        if (!$programNode) { 
            throw new InvalidSourceStructureException("Unexpected XML structure");
        }

        $languageAttribute = $programNode->attributes->getNamedItem('language');
        if ($languageAttribute && $languageAttribute->nodeValue !== 'IPPcode24') {
            throw new InvalidSourceStructureException("Wrong language");
        }

        // Check for any nodes directly under <program> which is not allowed
        $invalidElements = $xpath->query("/program/*[not(self::instruction)]");
        if ($invalidElements->length > 0) {
            throw new InvalidSourceStructureException("Arguments must not be outside of an instruction context");
        }

        $allowedArgNames = ['arg1', 'arg2', 'arg3']; // The allowed argument names

        $instructionNodes = $xpath->query("instruction", $programNode);
        foreach ($instructionNodes as $node) {
            if (!$node instanceof \DOMElement) {
                throw new InvalidSourceStructureException("Unknown element");
            }

            $orderAttribute = $node->attributes->getNamedItem('order');
            if (!$orderAttribute || !is_numeric($orderAttribute->nodeValue) || intval($orderAttribute->nodeValue) < 1) {
                throw new InvalidSourceStructureException("Instruction order '{$orderAttribute->nodeValue}' is not valid.");
            }
            $order = intval($orderAttribute->nodeValue);

            if (isset($this->instructions[$order])) {
                throw new InvalidSourceStructureException("Duplicate instruction order detected.");
            }

            $opcodeAttribute = $node->attributes->getNamedItem('opcode');
            if (!$opcodeAttribute) {
                throw new InvalidSourceStructureException("Unknown opcode");
            }

            if(!in_array(strtoupper($opcodeAttribute->nodeValue), InterpreterUtils::$opcodes)) {
                throw new InvalidSourceStructureException("Invalid argument name.");
            }
            $opcode = strtoupper($opcodeAttribute->nodeValue);

            $expectedArgs = InterpreterUtils::$expectedArgs[$opcode];
            $args = [];
            $argNodes = $xpath->query("./*[starts-with(name(), 'arg')]", $node);

            foreach ($argNodes as $argNode) {
                if (!$argNode instanceof \DOMElement) {
                    continue; // Skip if node is not an element
                }

                $argIndex = $argNode->nodeName; // 'arg1', 'arg2', 'arg3'

                // Check if the nodeName is one of the allowed argument names
                if (!in_array($argIndex, $allowedArgNames)) {
                    throw new InvalidSourceStructureException("Invalid argument name.");
                }

                if (isset($args[$argIndex])) {
                    throw new InvalidSourceStructureException("Argument is already defined in instruction.");
                }

                $type = $argNode->attributes->getNamedItem('type')->nodeValue;
                $value = trim($argNode->textContent);

    
                // Handling based on type
                switch ($type) {
                    case 'var':
                        list($frame, $varName) = explode('@', $value);
                        if (!in_array($frame, ['GF', 'LF', 'TF'])) {
                            throw new InvalidSourceStructureException("Invalid frame specification in variable");
                        }
                        $args[$argIndex] = ['type' => 'var', 'frame' => $frame, 'name' => $varName];
                        break;
                    case 'label':
                        $args[$argIndex] = ['type' => 'label', 'value' => $value];
                        break;
                    case 'type':
                        $args[$argIndex] = ['type' => 'type', 'value' => $value];
                        break;
                    case 'int':
                        if(!is_numeric($value)){
                            throw new InvalidSourceStructureException("Int is filled with non numerical symbols.");
                        }
                        $args[$argIndex] = ['type' => 'int', 'value' => intval($value)];
                        break;
                    case 'bool':
                        $args[$argIndex] = ['type' => 'bool', 'value' => strtolower($value) === 'true'];
                        break;
                    case 'string':
                        $value = InterpreterUtils::stringEscape($value);
                        $args[$argIndex] = ['type' => 'string', 'value' => $value];
                        break;
                    case 'nil':
                        $args[$argIndex] = ['type' => 'nil', 'value' => $value]; // nil usually has no value
                        break;
                    default:
                        throw new InvalidSourceStructureException("Invalid argument type");
                    }
            }
    
            // Validate argument count
            if (count($args) !== count($expectedArgs)) {
                throw new InvalidSourceStructureException("Incorrect number of arguments for opcode {$opcode}");
            }

            // Mapping between class names and argument types
            $classTypeMapping = [
                MyBool::class => 'bool',
                MyConst::class => 'const',
                MyInt::class => 'int',
                MyLabel::class => 'label',
                MyNil::class => 'nil',
                MyString::class => 'string',
                MySymb::class => 'symb',
                MyVar::class => 'var',
                MyType::class => 'type'
            ];

            // Validate argument types
            foreach ($args as $index => $arg) {
                $realIndex = (int)substr($index, 3) - 1; // Convert 'arg1' to 0, 'arg2' to 1, etc.
                $expectedClass = $expectedArgs[$realIndex];
                $expectedType = $classTypeMapping[$expectedClass];
                if ($arg['type'] !== $expectedType) {
                    if($expectedType !== 'symb') { // symb can be anything
                        throw new InvalidSourceStructureException("Wrong operand type");
                    }
                }
            }
    
            $this->instructions[$order] = $this->createInstruction($opcode, $args);

        }
        ksort($this->instructions); // Sort the instructions by their order keys
    }

    /**
    * Creates an instruction based on opcode and args.
    * 
    * @param string $opcode The opcode for the instruction.
    * @param array<mixed> $args Array of Argument objects for the instruction.
    * @return array<mixed> Array containing the opcode and associated arguments.
    */
    private function createInstruction(string $opcode,array $args): array
    {
        return ['opcode' => $opcode, 'args' => $args];
    }

    /**
     * Returns the list of parsed instructions.
     *
     * @return array<mixed> The instructions extracted from the XML.
     */
    public function getInstructions(): array
    {
        return $this->instructions;
    }

    /**
     * Provides a simple way to view the loaded XML document.
     *
     * @return string XML content of the document.
     */
    public function viewXML(): string
    {
        return $this->dom->saveXML();
    }
}