<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student;

use IPP\Core\Exception\ParameterException;
use IPP\Core\Exception\InputFileException;

class InputSetup {
    private ?string $sourceFilename = null;
    private ?string $inputFilename = null;

    public function __construct() {
        global $argv; // Use the global variable $argv to access command line arguments
        
        array_shift($argv); // Remove the script name from the arguments

        $opts = [
            'source' => null,
            'input' => null
        ];

        foreach ($argv as $arg) {
            if (preg_match('/^--(source|input)=(.*)$/', $arg, $matches)) {
                if (trim($matches[2]) === "") {
                    throw new ParameterException("The value for --{$matches[1]} cannot be empty.");
                }
                $opts[$matches[1]] = $matches[2];
            } else if ($arg === "--input" || $arg === "--source") {
                throw new ParameterException("Wrong use of --source or --input parameter.");
            }
        }

        if (!$opts['source'] && !$opts['input']) {
            throw new ParameterException("At least one of --source or --input must be provided.");
        }

        $this->sourceFilename = $opts['source'] ?? "php://stdin";
        $this->inputFilename = $opts['input'] ?? "php://stdin";
    }

    public function getSourceFilename(): ?string {
        return $this->sourceFilename;
    }

    public function getInputFilename(): ?string {
        return $this->inputFilename;
    }
}
