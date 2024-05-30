<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use Exception;
use IPP\Core\AbstractInterpreter;
use IPP\Core\Interface\OutputWriter;
use IPP\Core\ReturnCode;
use IPP\Core\Exception\InternalErrorException;
use IPP\Core\Exception\ParameterException;
use IPP\Core\Exception\InputFileException;
use IPP\Core\Interface\InputReader;
use IPP\Student\Exception\InvalidXmlException;
use IPP\Student\Exception\FrameAccessException;
use IPP\Student\Exception\VariableAccessException;
use IPP\Student\Exception\OperandTypeException;
use IPP\Student\Exception\OperandValueException;
use IPP\Student\Exception\InvalidSourceStructureException;
use IPP\Student\Exception\SemanticErrorException;
use IPP\Student\Exception\StringOperationException;
use IPP\Student\Exception\ValueErrorException;

class Interpreter extends AbstractInterpreter
{
    /**
     * @var array<mixed> Global frame storage
     */
    public array $gf;

    /**
     * @var array<mixed>|null Temporary frame storage
     */
    public ?array $tf;    
    
    /**
     * @var array<mixed> Local frame storage
     */
    public array $lf;

    /**
     * @var array<mixed> List of instructions
     */
    public array $instructions;
    public ?int $currentInstruction;

    /**
     * @var array<mixed> Stack for data storage
     */
    public array $stack;

    /**
     * @var array<mixed> Labels for jumps
     */
    private array $labels;

    /**
     * @var array<mixed> Call stack for function calls
     */
    public array $callStack;

    public function execute(): int
    {
        $this->gf = [];             
        $this->tf = null;           
        $this->lf = [];             
        $this->instructions = [];   
        $this->currentInstruction = 1;
        $this->stack = [];          
        $this->labels = [];         
        $this->callStack = [];

        try {
            // Instantiate InputSetup to manage command-line inputs
            $inputSetup = new InputSetup();
            $dom = $this->source->getDOMDocument();
            $domParser = new XMLParser($dom);
            $domParser->parseXML();

            $this->instructions = $domParser->getInstructions();
            $this->getLabels();

            $this->currentInstruction = $this->findFirstIndex();
            
            while ($this->currentInstruction !== null) {
                $instructionData = $this->instructions[$this->currentInstruction];
                $opcode = $instructionData['opcode'];
                $args = $instructionData['args'];
    
                // Depending on the opcode, instantiate the correct class
                switch ($opcode) {
                    case 'DEFVAR':
                        $instruction = new DEFVAR($opcode, $args, $this);
                        break;
                    case 'MOVE':
                        $instruction = new MOVE($opcode, $args, $this);
                        break;
                    case 'WRITE':
                        $instruction = new WRITE($opcode, $args, $this);
                        break;
                    case 'EXIT':
                        $instruction = new EXITI($opcode, $args, $this);
                        break;
                    case 'ADD':
                        $instruction = new ADD($opcode, $args, $this);
                        break;
                    case 'SUB':
                        $instruction = new SUB($opcode, $args, $this);
                        break;
                    case 'MUL':
                        $instruction = new MUL($opcode, $args, $this);
                        break;
                    case 'IDIV':
                        $instruction = new IDIV($opcode, $args, $this);
                        break;
                    case 'AND':
                        $instruction = new ANDI($opcode, $args, $this);
                        break;
                    case 'OR':
                        $instruction = new ORI($opcode, $args, $this);
                        break;
                    case 'NOT':
                        $instruction = new NOTI($opcode, $args, $this);
                        break;
                    case 'CONCAT':
                        $instruction = new CONCAT($opcode, $args, $this);
                        break;
                    case 'LT':
                        $instruction = new LT($opcode, $args, $this);
                        break;
                    case 'GT':
                        $instruction = new GT($opcode, $args, $this);
                        break;
                    case 'EQ':
                        $instruction = new EQ($opcode, $args, $this);
                        break;
                    case 'STRLEN':
                        $instruction = new STRLEN($opcode, $args, $this);
                        break;
                    case 'GETCHAR':
                        $instruction = new GETCHAR($opcode, $args, $this);
                        break;
                    case 'SETCHAR':
                        $instruction = new SETCHAR($opcode, $args, $this);
                        break;
                    case 'TYPE':
                        $instruction = new TYPE($opcode, $args, $this);
                        break;
                    case 'INT2CHAR':
                        $instruction = new INT2CHAR($opcode, $args, $this);
                        break;
                    case 'STRI2INT':
                        $instruction = new STRI2INT($opcode, $args, $this);
                        break;
                    case 'POPS':
                        $instruction = new POPS($opcode, $args, $this);
                        break;
                    case 'PUSHS':
                        $instruction = new PUSHS($opcode, $args, $this);
                        break;
                    case 'READ':
                        $instruction = new READ($opcode, $args, $this);
                        break;
                    case 'LABEL':
                        $instruction = new LABEL($opcode, $args, $this);
                        break;
                    case 'JUMP':
                        $instruction = new JUMP($opcode, $args, $this);
                        break;
                    case 'JUMPIFEQ':
                        $instruction = new JUMPIFEQ($opcode, $args, $this);
                        break;
                    case 'JUMPIFNEQ':
                        $instruction = new JUMPIFNEQ($opcode, $args, $this);
                        break;
                    case 'CALL':
                        $instruction = new CALL($opcode, $args, $this);
                        break;
                    case 'RETURN':
                        $instruction = new RETURNI($opcode, $args, $this);
                        break;
                    case 'CREATEFRAME':
                        $instruction = new CREATEFRAME($opcode, $args, $this);
                        break;
                    case 'POPFRAME':
                        $instruction = new POPFRAME($opcode, $args, $this);
                        break;
                    case 'PUSHFRAME':
                        $instruction = new PUSHFRAME($opcode, $args, $this);
                        break;
                    case 'BREAK':
                        $instruction = new BREAKI($opcode, $args, $this);
                        break;
                    case 'DPRINT':
                        $instruction = new DPRINT($opcode, $args, $this);
                        break;
                    default:
                        throw new Exception("Unknown instruction opcode: $opcode");
                        //break;
                }
                
                $instruction->execute($this);
                // Move to the next instruction index
                $this->currentInstruction = $this->findNextIndex($this->currentInstruction);
            }

            return ReturnCode::OK;
        } catch (ParameterException $e) {
            throw new ParameterException($e->getMessage());
        } catch (ValueErrorException $e) {
            throw new ValueErrorException($e->getMessage());
        } catch (StringOperationException $e) {
            throw new StringOperationException($e->getMessage());
        } catch (OperandValueException $e) {
            throw new OperandValueException($e->getMessage());
        } catch (VariableAccessException $e) {
            throw new VariableAccessException($e->getMessage());
        } catch (FrameAccessException $e) {
            throw new FrameAccessException($e->getMessage());
        } catch (OperandTypeException $e) {
            throw new OperandTypeException($e->getMessage());
        } catch (InputFileException $e) {
            throw new InputFileException($e->getMessage());
        } catch (SemanticErrorException $e) {
            throw new SemanticErrorException($e->getMessage());
        } catch (InvalidXmlException $e) {
            throw new InvalidXmlException($e->getMessage());
        } catch (InvalidSourceStructureException $e) {
            throw new InvalidSourceStructureException($e->getMessage());
        } catch (Exception $e) {
            // Handle exceptions from InputSetup or processing
            if($e->getMessage() === 'Invalid source XML format') {
                throw new InvalidXmlException($e->getMessage());
            }
            throw new InternalErrorException($e->getMessage());
        }
    }

    /**
    * Finds the first index in the instructions array.
    *
    * @return int|null Returns the first index or null if the array is empty.
    */
    public function findFirstIndex() {
        if (empty($this->instructions)) {
            return null;
        }
        reset($this->instructions);
        return key($this->instructions);
    }

    /**
    * Finds the next index in the instructions array given the current index.
    *
    * @param int $currentIndex The current index in the instructions array.
    * @return int|null Returns the next index or null if there is no next index.
    */
    public function findNextIndex($currentIndex) {
        $keys = array_keys($this->instructions);
        $position = array_search($currentIndex, $keys);
        if ($position === false || $position + 1 >= count($keys)) {
            return null;
        }
        return $keys[$position + 1];
    }

    public function getStdOutWriter(): OutputWriter {
        return $this->stdout;
    }

    public function getStdErrWriter(): OutputWriter {
        return $this->stderr;
    }

    public function getInputReader(): InputReader {
        return $this->input;
    }

    /**
    * Collect all labels from the instructions to ensure no duplicates.
    */
    public function getLabels(): void
    {
        foreach ($this->instructions as $instruction) {
            if ($instruction['opcode'] === 'LABEL') {
                $labelName = $instruction['args']['arg1']['value'];
                if (in_array($labelName, $this->labels)) {
                    throw new SemanticErrorException("Label '{$labelName}' already defined.");
                }
                $this->labels[] = $labelName;
            }
        }
    }

    /**
    * Find label index in the program for jumps and calls.
    *
    * @param string $label The label to find.
    * @return int The index of the label if found, otherwise -1.
    */
    public function findLabelIndex(string $label): int
    {
        foreach ($this->instructions as $index => $instruction) {
            if ($instruction['opcode'] === 'LABEL' && $instruction['args']['arg1']['value'] === $label) {
                return $index;
            }
        }
        return -1;  // Return -1 if label is not found
    }

    /**
    * Checks if a variable is defined in its respective frame.
    *
    * @param array<mixed> $var Associative array representing the variable with keys 'frame' and 'name'.
    * @return bool True if the variable is defined, false otherwise.
    */
    public function isVarDefined(array $var): bool {
        $frame = $var['frame'];
        $name = $var['name'];
    
        switch ($frame) {
            case 'GF':
                return isset($this->gf[$name]);
    
            case 'TF':
                if ($this->tf !== null) {
                    return isset($this->tf[$name]);
                }
                break;
    
            case 'LF':
                if (!empty($this->lf) && isset($this->lf[0][$name])) {
                    return true;
                }
                break;
        }
    
        return false;
    }

    /**
    * Define empty variable and check if it is not already defined.
    *
    * @param array<mixed> $var Associative array representing the argument (variable) with 'frame' and 'name'.
    */
    public function defineVar(array $var): void {
        if ($this->isVarDefined($var)) {
            throw new SemanticErrorException("Variable already defined");
        }

        switch ($var['frame']) {
            case 'GF':
                $this->gf[$var['name']] = ['type' => 'const', 'value' => null];
                break;
            case 'TF':
                if ($this->tf !== null) {
                    $this->tf[$var['name']] = ['type' => 'const', 'value' => null];
                } else {
                    throw new FrameAccessException("Temporary Frame not defined");
                }
                break;
            case 'LF':
                if (count($this->lf) > 0) {
                    $this->lf[0][$var['name']] = ['type' => 'const', 'value' => null];
                } else {
                    throw new FrameAccessException("Local Frame not defined");
                }
                break;
            default:
                throw new FrameAccessException("Frame not defined");
        }
    }

    /**
    * Reads data from a variable located in a frame.
    *
    * @param array<mixed> $var Associative array representing the variable.
    */
    public function readFromVar(array $var): mixed {
        $frame = $var['frame'];
        $name = $var['name'];

        switch ($frame) {
            case 'GF':
                if (isset($this->gf[$name])) {
                    return $this->gf[$name]['value'];
                }
                break;

            case 'TF':
                if ($this->tf !== null && isset($this->tf[$name])) {
                    return $this->tf[$name]['value'];
                }
                break;

            case 'LF':
                if (!empty($this->lf) && isset($this->lf[0][$name])) {
                    return $this->lf[0][$name]['value'];
                }
                break;
        }

        if ($frame !== 'GF' && $frame !== 'TF' && $frame !== 'LF') {
            throw new FrameAccessException("Frame not defined.");
        }

        throw new VariableAccessException("Variable not defined in frame.");
    }

    /**
    * Reads the entire variable object from a frame.
    *
    * @param array<mixed> $var Associative array representing the variable.
    * @return array<mixed> Associated attributes to the $var, e.g. value, type
    */
    public function readVarObj(array $var): array {
        $frame = $var['frame'];
        $name = $var['name'];

        switch ($frame) {
            case 'GF':
                if (isset($this->gf[$name])) {
                    return $this->gf[$name];
                }
                break;

            case 'TF':
                if ($this->tf !== null && isset($this->tf[$name])) {
                    return $this->tf[$name];
                }
                if ($this->tf === null) {
                    throw new FrameAccessException("Temporary frame not defined.");
                }
                break;

            case 'LF':
                if (!empty($this->lf) && isset($this->lf[0][$name])) {
                    return $this->lf[0][$name];
                }
                if (empty($this->lf)) {
                    throw new FrameAccessException("Local frame stack is empty.");
                }
                break;

            default:
                throw new FrameAccessException("Invalid frame specified.");
        }

        throw new VariableAccessException("Variable not defined.");
    }
    
    /**
    * Writes data to a variable in the specified frame.
    *
    * @param array<mixed> $var Associative array representing the variable with 'frame' and 'name'.
    * @param mixed $data The data to write to the variable.
    * @param bool $override Defines if the caller wants to grand the method ability to override type of variable.
    */
    public function writeToVar(array $var, $data, $override): void {
        $frame = $var['frame'];
        $name = $var['name'];

        switch ($frame) {
            case 'GF':
                if (isset($this->gf[$name])) {
                    if($data === 'nil' && $override){
                        $this->gf[$name]['type'] = 'nil';
                    }
                    $this->gf[$name]['value'] = $data;
                    return;
                }
                break;

            case 'TF':
                if ($this->tf !== null) {
                    if (isset($this->tf[$name])) {
                        if($data === 'nil' && $override){
                            $this->tf[$name]['type'] = 'nil';
                        }
                        $this->tf[$name]['value'] = $data;
                        return;
                    }
                } else {
                    throw new FrameAccessException("Temporary frame not defined.");
                }
                break;

            case 'LF':
                if (!empty($this->lf)) {
                    $topFrame = &$this->lf[0];
                    if (isset($topFrame[$name])) {
                        if($data === 'nil' && $override){
                            $topFrame[$name]['type'] = 'nil';
                        }
                        $topFrame[$name]['value'] = $data;
                        return;
                    }
                } else {
                    throw new FrameAccessException("Local frame stack is empty.");
                }
                break;

            default:
                throw new FrameAccessException("Invalid frame specified.");
        }

        throw new VariableAccessException("Variable not defined.");
    }

    /**
    * Retrieves the symbol which could be a variable or a constant.
    *
    * @param array<mixed> $symb Associative array representing the symbol with 'type' and potentially other keys.
    */
    public function getSymb(array $symb): mixed {
        if ($symb['type'] === 'var') {
            return $this->readVarObj($symb);
        } elseif ($this->isConstType($symb['type'])) {
            return $symb; // Assuming const structure contains 'value'
        } else {
            throw new OperandTypeException("Wrong argument type for symbol processing.");
        }
    }

    /**
    * Checks if a type is a valid constant type.
    *
    * @param string $type Type of the symbol.
    */
    private function isConstType(string $type): bool {
        // List of all valid constant types
        $constTypes = ['int', 'string', 'bool', 'nil', 'type'];
        return in_array($type, $constTypes);
    }


    /**
     * Gets the Global Frame array.
     *
     * @return array<mixed> The current state of the Global Frame.
     */
    public function getGlobalFrame(): array {
        return $this->gf;
    }

    /**
     * Gets the Temporary Frame array.
     *
     * @return array<mixed>|null The current state of the Temporary Frame.
     */
    public function getTemporaryFrame(): ?array {
        return $this->tf;
    }

    /**
     * Gets the Local Frame array.
     *
     * @return array<mixed> The current state of the Local Frame.
     */
    public function getLocalFrame(): array {
        return $this->lf;
    }

    /**
     * Gets the list of instructions.
     *
     * @return array<mixed> The instructions loaded into the program.
     */
    public function getInstructions(): array {
        return $this->instructions;
    }

    /**
     * Gets the current instruction pointer/index.
     *
     * @return int The current position in the instruction list.
     */
    public function getCurrentInstruction(): int {
        return $this->currentInstruction;
    }

    /**
     * Gets the Data Stack.
     *
     * @return array<mixed> The stack used for data storage during execution.
     */
    public function getStack(): array {
        return $this->stack;
    }

    /**
     * Gets the Call Stack.
     *
     * @return array<mixed> The call stack used for managing function calls.
     */
    public function getCallStack(): array {
        return $this->callStack;
    }
}
