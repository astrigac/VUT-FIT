<?php
/**
 * IPP Project 2 - Interpreter of XML code representation
 * @author Aurel Strigac
 * 
 */
namespace IPP\Student\Exception;

use IPP\Core\Exception\IPPException;
use IPP\Core\ReturnCode;
use Throwable;

/**
 * Exception for semantic errors
 */
class SemanticErrorException extends IPPException
{
    public function __construct(string $message = "Semantic error", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::SEMANTIC_ERROR, $previous);
    }
}